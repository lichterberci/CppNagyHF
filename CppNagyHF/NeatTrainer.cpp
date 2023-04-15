#include "NeatTrainer.hpp"
#include "Game.hpp"
#include <execution>

namespace model {

	void NeatTrainer::ConstructInitialGeneration() {

		cstd::Vector<NeatModel> initialGeneration;
		initialGeneration.reserve_and_copy(populationCount);

		for (size_t i = 0; i < populationCount; i++) {
			initialGeneration += NeatModel(NUM_SENSORS, NUM_OUTPUTS, activationFunction, innovationNumberTable);
		}

		organismsByGenerations += initialGeneration;

		representativesOfThePrevGeneration += &organismsByGenerations[0][0];
	}

	double NeatTrainer::TrainIndividual(NeatModel& neatModel) {

		auto game = game::Game(false, game::GameControlType::AI, gameWidth, gameHeight, 800, 800, neatModel, numMaxIdleSteps, placeFirstAppleInFrontOfSnake);

		game.Start();

		auto report = game.GenerateReport();

		double fitness = fitnessFunction->operator()(report);

		return fitness;
	}

	void NeatTrainer::TrainCurrentGeneration() {

		auto& currentGeneration = organismsByGenerations.last();

		cstd::Vector<double> fitnessScores(currentGeneration.size());

#if MULTI_THREAD_TRAINING == 0
		for (auto& organism : currentGeneration)
			fitnessScores += TrainIndividual(organism);
#else
		std::for_each(std::execution::par, currentGeneration.begin(), currentGeneration.end(), [this, &fitnessScores](NeatModel& organism) {
			fitnessScores += TrainIndividual(organism);
		});
#endif

		// speciation

		const cstd::Vector<int> speciesIndicies = Speciate(currentGeneration);

		// sum up species sizes

		std::unordered_map<int, int> speciesSizes;

		int numSpecies = 0;

		for (int speciesIndex : speciesIndicies) {

			if (speciesSizes.find(speciesIndex) == speciesSizes.end()) {

				speciesSizes[speciesIndex] = 1;
				numSpecies++;

				continue;
			}

			speciesSizes[speciesIndex]++;
		}

		// allocate places in the new generation in proportion to the adjusted fitness scores
		// this means we divide the sum of the fitness score by the size of the species

		cstd::Vector<double> sumOfAdjustedFitnessForEachSpecies;
		sumOfAdjustedFitnessForEachSpecies.reserve_and_copy(numSpecies);

		for (int i = 0; i < numSpecies; i++)
			sumOfAdjustedFitnessForEachSpecies += 0;

		// sum up fitness scores by species

		for (int i = 0; i < populationCount; i++) {

			int speciesIndex = speciesIndicies[i];

			currentGeneration[i].rawFitness = fitnessScores[i];

			const double adjustedFitness = fitnessScores[i] / speciesSizes[speciesIndex];

			currentGeneration[i].adjustedFitness = adjustedFitness;

			sumOfAdjustedFitnessForEachSpecies[speciesIndex] += adjustedFitness;
		}

		// save avg. fitness

		double avgFitness = 0;

		for (int i = 0; i < populationCount; i++)
			avgFitness += fitnessScores[i];

		avgFitness /= populationCount;

		avgFitnessOfGenerations += avgFitness;

		// allocate places accordingly 

		auto placesAllocatedForSpecies = AllocatePlacesForSpecies(sumOfAdjustedFitnessForEachSpecies);

		// if sum of raw fitness does not rise for more than X generations, only keep the top Y species
		if (avgFitnessOfGenerations.size() >= numGenerationsWithSameFitnessBeforeOnlyLookingAtTopSpecies) {

			bool hasThereBeenImprovement = false;

			double firstFitnessInObservedRange = avgFitnessOfGenerations[avgFitnessOfGenerations.size() - numGenerationsWithSameFitnessBeforeOnlyLookingAtTopSpecies];

			for (
				unsigned int generationIndex = avgFitnessOfGenerations.size() - numGenerationsWithSameFitnessBeforeOnlyLookingAtTopSpecies + 1; 
				generationIndex < avgFitnessOfGenerations.size(); 
				generationIndex++
			) {
				if (firstFitnessInObservedRange + minImprovementOfAvgFitnessToConsiderItAnImprovement <= avgFitnessOfGenerations[generationIndex]) {
					hasThereBeenImprovement = true;
					break;
				}
			}

			if (hasThereBeenImprovement == false) {

				// only keep top Y species

				cstd::Vector<int> speciesOrderedByFitness;

				for (int i = 0; i < numSpecies; i++)
					speciesOrderedByFitness += i;

				std::sort(speciesOrderedByFitness.begin(), speciesOrderedByFitness.end(), [&sumOfAdjustedFitnessForEachSpecies](int a, int b) {
					return sumOfAdjustedFitnessForEachSpecies[a] < sumOfAdjustedFitnessForEachSpecies[b];
				});

				for (int i = numSpecies - 1; i >= numberOfTopSpeciesToLookAtIfFitnessIsStableForTooLong; i--) {
					sumOfAdjustedFitnessForEachSpecies[i] = 0; // dont allocate anything for this species
				}

				//std::cout << "Pruned all species except top 2!" << std::endl;

				placesAllocatedForSpecies = AllocatePlacesForSpecies(sumOfAdjustedFitnessForEachSpecies);
			}
		}


		// reproduction

		auto newGeneration = ProduceNewGenerationByReproduction(
			currentGeneration, 
			speciesIndicies, 
			placesAllocatedForSpecies, 
			fitnessScores
		);

		for (auto& organism : newGeneration)
			organism.Mutate(
				innovationNumberTable, 
				chanceOfDentritInsertion, 
				chanceOfNeuronInsertion,
				chanceOfDentritMutation, 
				chanceOfMutationBeingNewValue, 
				chanceOfDisabling, 
				weightSetMin, 
				weightSetMax, 
				weightAdjustMin, 
				weightAdjustMax
			);

		//std::cout << "Species: (" << speciesSizes.size() << ")" << std::endl;

		//for (int i = 0; i < speciesSizes.size(); i++) {
		//	std::cout << "size: " << speciesSizes[i] << " avg. fitness: " << sumOfAdjustedFitnessForEachSpecies[i] << std::endl;
		//}


		organismsByGenerations += newGeneration;

		if (organismsByGenerations.size() > 2) {
			// remove unwanted organisms from earlier gen (that we dont use)
			auto& gen = organismsByGenerations[organismsByGenerations.size() - 3];

			std::sort(gen.begin(), gen.end(), [](const NeatModel& a, const NeatModel& b) {
				return a.rawFitness > b.rawFitness;
			});

			for (int i = populationCount - 1; i >= numBestOrganismsToKeepFromPrevGenerations; i--)
				gen.pop();
		}
	}

	cstd::Vector<int> NeatTrainer::Speciate(const cstd::Vector<NeatModel>& organisms) {

		/*
		The distance measure δ allows us to speciate using a compatibility threshold δt.
		An ordered list of species is maintained. In each generation, genomes are sequentially
		placed into species. Each existing species is represented by a random genome inside
		the species from the previous generation. A given genome g in the current generation is
		placed in the first species in which g is compatible with the representative genome of
		that species. This way, species do not overlap.1
		If g is not compatible with any existing
		species, a new species is created with g as its representative.
		*/
		

		cstd::Vector<int> result;

		cstd::Vector<const NeatModel*> representativesOfSpeciesInNewGeneration;

		cstd::Vector<std::tuple<int, const NeatModel*>> representativeCandidatesFromTheNewGeneration;

		// these are only for debug purposes
		int inNewSpecies = 0, inOldSpecies = 0, generateSpecies = 0;

		for (auto& organism : organisms) {

			bool didFindSpecies = false;

			// we first search in those species that are present in the new generation
			for (int newSpeciesIndex = 0; newSpeciesIndex < (int)representativesOfSpeciesInNewGeneration.size(); newSpeciesIndex++) {

				const double delta = GetSpeciesDifferenceDelta(organism, *representativesOfSpeciesInNewGeneration[newSpeciesIndex]);

				if (delta <= neatDeltaSubT) {
					didFindSpecies = true;
					result += newSpeciesIndex;
					inNewSpecies++;
					break;
				}

			}

			// we then search for species in the previous generation
			if (didFindSpecies == false) {

				for (int speciesIndex = 0; speciesIndex < (int)representativesOfThePrevGeneration.size(); speciesIndex++) {

					const double delta = GetSpeciesDifferenceDelta(organism, *representativesOfThePrevGeneration[speciesIndex]);

					if (delta <= neatDeltaSubT) {

						didFindSpecies = true;

						// if we find someone from the old generation, we move the species' representative to the new vector
						// this means that in the new vector, there will still remain organisms from the old one
						// --> at the end, we will have to swap them out for new organisms
						representativesOfSpeciesInNewGeneration += representativesOfThePrevGeneration[speciesIndex];

						// the new index, this species is inserted in
						const int newIndexOfSpecies = representativesOfSpeciesInNewGeneration.size() - 1;

						// we mark the current organism as a candidate, so after speciation, we can swap the old ones to these
						representativeCandidatesFromTheNewGeneration += std::make_tuple(newIndexOfSpecies, &organism);

						// the species index will be set according to the new vector
						result += newIndexOfSpecies; 

						// we have succesfully moved this from the old to the new generation, now we can remove it
						representativesOfThePrevGeneration.removeAt(speciesIndex); 

						inOldSpecies++;

						break;
					}
				}
			}

			if (didFindSpecies == false) {

				// we create a new species

				generateSpecies++;

				representativesOfSpeciesInNewGeneration += &organism;
				result += (int)representativesOfSpeciesInNewGeneration.size() - 1;

				didFindSpecies = true;
			}
		}

		// remove old representatives, and replace them with new ones
		for (const auto [speciesIndex, representative] : representativeCandidatesFromTheNewGeneration)
			representativesOfSpeciesInNewGeneration[speciesIndex] = representative;

		// replace old representatives with new ones
		representativesOfThePrevGeneration = representativesOfSpeciesInNewGeneration;

		return result;
	}

	double NeatTrainer::GetSpeciesDifferenceDelta(const NeatModel& a, const NeatModel& b) {

		const unsigned int N = std::max<const unsigned int>(a.Genes().size(), b.Genes().size());

		unsigned int numDisjointGenes = 0;
		unsigned int numExcessGenes = 0;

		size_t i = 0, j = 0; // indicies of the vectors

		const size_t sizeOfSmaller = std::min(a.Genes().size(), b.Genes().size());

		const auto& genesOfA = a.Genes();
		const auto& genesOfB = b.Genes();

		double sumDifferenceOfWeights = 0;
		size_t numAllignedGenes = 0;

		const int innovationNumberInGenomWithSmallerMaxInnovationNumber = std::min(
			genesOfA.last().innovationNumber, 
			genesOfB.last().innovationNumber
		);

		while (i < sizeOfSmaller && j < sizeOfSmaller) {

			const auto& aGene = genesOfA[i];
			const auto& bGene = genesOfB[j];

			if (aGene == bGene) {
				i++; j++;

				sumDifferenceOfWeights += std::abs(aGene.weight - bGene.weight);
				numAllignedGenes++;

				continue;
			}

			// cannot be excess, because it is smaller than another gene in the genom of the other organism
			if (aGene < bGene) { 
				i++;

				numDisjointGenes++;

				continue;
			}

			// cannot be excess, because it is smaller than another gene in the genom of the other organism
			if (bGene < aGene) {
				j++;

				numDisjointGenes++;
				
				continue;
			}
		}

		numExcessGenes += (int)genesOfA.size() - i + (int)genesOfB.size() - j;

		const double wBar = sumDifferenceOfWeights / numAllignedGenes;

		const double delta = neatC1 * numExcessGenes / N + neatC2 * numDisjointGenes / N + neatC3 * wBar;

		return delta;
	}

	cstd::Vector<NeatModel> NeatTrainer::ProduceNewGenerationByReproduction(
		const cstd::Vector<NeatModel>& currentGeneration, 
		const cstd::Vector<int>& speciesIndicies, 
		cstd::Vector<int>& numPlacesAllocatedForSpecies,
		const cstd::Vector<double>& rawFitnessScores
	) {

		/*
		Species then reproduce by first eliminating the lowest performing members from the population. 
		The entire population is then replaced by the offspring of the remaining organisms in each species.
		*/

		cstd::Vector<cstd::Vector<int>> organismIndiciesBySpecies;

		for (int i = 0; i < numPlacesAllocatedForSpecies.size(); i++)
			organismIndiciesBySpecies += cstd::Vector<int>();

		// generate lookup
		for (int i = 0; i < populationCount; i++)
			organismIndiciesBySpecies[speciesIndicies[i]] += i;

		// sort according to fitness scores
		for (auto& species : organismIndiciesBySpecies)
			std::sort(species.begin(), species.end(), [&rawFitnessScores](const int a, const int b) {
				return rawFitnessScores[a] > rawFitnessScores[b]; // sort in desc. order
			});

		// chop off those deemed useless
		for (int speciesIndex = 0; speciesIndex < organismIndiciesBySpecies.size(); speciesIndex++) {

			auto& species = organismIndiciesBySpecies[speciesIndex];

			while (species.size() > std::ceil(portionOfSpeciesToKeepForReproduction * numPlacesAllocatedForSpecies[speciesIndex]))
				species.pop();
		}

		// from every species, select couples at random to reproduce
		// if a species only contains 1 organism, just copy it
		
		cstd::Vector<NeatModel> newGeneration;
		newGeneration.reserve_and_copy(populationCount);

		int recievingSpeciesIndex = 0;

		// if a species' organisms were killed off in the decimation part, but places were allocated, 
		// we want to give them away for another species
		for (int speciesIndex = 0; speciesIndex < organismIndiciesBySpecies.size(); speciesIndex++) {

			const auto& species = organismIndiciesBySpecies[speciesIndex];

			if (species.size() != 0)
				continue;

			if (numPlacesAllocatedForSpecies[speciesIndex] == 0)
				continue;

			while (recievingSpeciesIndex < numPlacesAllocatedForSpecies.size()) {
				if (organismIndiciesBySpecies[recievingSpeciesIndex].size() > 0)
					break;

				recievingSpeciesIndex = (recievingSpeciesIndex + 1) % numPlacesAllocatedForSpecies.size();
			}

			if (recievingSpeciesIndex == numPlacesAllocatedForSpecies.size()) {
				std::cout << "ERROR: all species got lost in the decimation process!" << std::endl;
				throw std::out_of_range("All species got lost in the decimation process!");
			}

			numPlacesAllocatedForSpecies[recievingSpeciesIndex] += numPlacesAllocatedForSpecies[speciesIndex];

			numPlacesAllocatedForSpecies[speciesIndex] = 0;

			recievingSpeciesIndex = (recievingSpeciesIndex + 1) % numPlacesAllocatedForSpecies.size();
		}

		for (int speciesIndex = 0; speciesIndex < organismIndiciesBySpecies.size(); speciesIndex++) {

			const auto& species = organismIndiciesBySpecies[speciesIndex];

			// in the prev generation, there was a corresponding species, but it got extinct
			if (species.size() == 0)
				continue;

			if (species.size() == 1) {
				for (int i = 0; i < numPlacesAllocatedForSpecies[speciesIndex]; i++)
					newGeneration += currentGeneration[species[0]];

				continue;
			}

			for (int i = 0; i < numPlacesAllocatedForSpecies[speciesIndex]; i++) {

				int indexInSpeciesOfParentA = (int)utils::RandomInt(0, species.size());
				int indexInSpeciesOfParentB = (int)utils::RandomInt(0, species.size());

				int indexOfParentA = species[indexInSpeciesOfParentA];
				int indexOfParentB = species[indexInSpeciesOfParentB];

				NeatModel offspring = GenerateOffSpring(
					currentGeneration[indexOfParentA], 
					currentGeneration[indexOfParentB], 
					rawFitnessScores[indexOfParentA], 
					rawFitnessScores[indexOfParentB]
				);

				newGeneration += offspring;
			}
		}

		if (newGeneration.size() != populationCount) {
			std::cout << "ERROR: new generation has invalid length!" << std::endl;
			throw std::exception("New generation has invalid length!");
		}


		return newGeneration;
	}

	NeatModel NeatTrainer::GenerateOffSpring(const NeatModel& a, const NeatModel& b, double fitnessOfA, double fitnessOfB) {

		/*
		In composing the offspring, genes are randomly chosen from either parent at matching genes, 
		whereas all excess or disjoint genes are always included from the more fit parent.
		*/

		const auto& genesOfA = a.Genes();
		const auto& genesOfB = b.Genes();

		cstd::Vector<ConnectionGene> newGenes;

		const int innovationNumberInGenomWithSmallerMaxInnovationNumber = std::min(
			genesOfA.last().innovationNumber,
			genesOfB.last().innovationNumber
		);

		size_t i = 0, j = 0; // indicies of the vectors

		const size_t sizeOfSmaller = std::min(a.Genes().size(), b.Genes().size());

		const bool isParentAFitter = fitnessOfA > fitnessOfB;

		while (i < sizeOfSmaller && j < sizeOfSmaller) {

			const auto& aGene = genesOfA[i];
			const auto& bGene = genesOfB[j];

			if (aGene == bGene) {
				i++; j++;

				// alligned

				if (utils::RandomDouble(0, 1) < 0.5)
					newGenes += aGene;
				else
					newGenes += bGene;

				if (aGene.disabled || bGene.disabled)
					if (utils::RandomDouble(0, 1) < chanceOfGeneDisablingIfEitherGeneIsDisabled)
						newGenes.last().Disable();

				continue;
			}

			if (aGene < bGene) {
				i++;

				// disjoint

				if (isParentAFitter)
					newGenes += aGene;

				continue;
			}

			if (bGene < aGene) {
				j++;

				// disjoint

				if (isParentAFitter == false)
					newGenes += bGene;

				continue;
			}
		}

		// handle excess genes

		if (isParentAFitter) {
			while (i < genesOfA.size())
				newGenes += genesOfA[i++];
		}
		else {
			while (j < genesOfB.size())
				newGenes += genesOfB[j++];
		}

		return NeatModel(newGenes, NUM_SENSORS, NUM_OUTPUTS, activationFunction);
	}

	cstd::Vector<int> NeatTrainer::AllocatePlacesForSpecies(const cstd::Vector<double>& sumOfAdjustedFitnessForEachSpecies) {

 		const int numSpecies = (int)sumOfAdjustedFitnessForEachSpecies.size();
		double totalSum = 1e-10; // prevents divide by zero exception

		for (double adjustedFitness : sumOfAdjustedFitnessForEachSpecies)
			totalSum += adjustedFitness;

		int numTotalPlacesAllocated = 0;

		cstd::Vector<int> placesAllocatedForSpecies;
		placesAllocatedForSpecies.reserve_and_copy(numSpecies);

		for (int i = 0; i < numSpecies; i++) {

			double placesDouble = populationCount * sumOfAdjustedFitnessForEachSpecies[i] / totalSum;

			int places = std::floor<int>(placesDouble);

			placesAllocatedForSpecies += places;

			numTotalPlacesAllocated += places;
		}

		// if we have rounding errors, we just give them away in increasing order for non-extinct species
		if (numTotalPlacesAllocated < populationCount) {
			int recieveingIndex = 0;

			bool isThereNonExtinctSpecies = numTotalPlacesAllocated > 0;

			while (numTotalPlacesAllocated < populationCount) {
				if (isThereNonExtinctSpecies == false || placesAllocatedForSpecies[recieveingIndex] > 0) {
					placesAllocatedForSpecies[recieveingIndex]++;
					numTotalPlacesAllocated++;
				}
				recieveingIndex = (recieveingIndex + 1) % numSpecies;
			}
		}

		return placesAllocatedForSpecies;
	}

	void NeatTrainer::Train() {

		std::cout << "Training " << numGenerations << " generation with " << populationCount << " organisms in each..." << std::endl;

		for (size_t generationIndex = 0; generationIndex < numGenerations; generationIndex++) {

			std::cout << 
				"\33[2K\rTraining generation " << std::setfill(' ') << std::setw(4) <<
				generationIndex << "/" << numGenerations
				<< " (" << std::setw(3) << std::fixed << std::setprecision(1) 
				<< (100.0 * generationIndex / numGenerations) << "%)";

			// slider 

			const int sliderLength = 40;

			std::cout << "   \u001b[42;1m";

			for (int i = 0; i < sliderLength * generationIndex / numGenerations; i++)
				std::cout << ' ';

			std::cout << "\u001b[47;1m";

			for (int i = sliderLength * generationIndex / numGenerations; i < sliderLength; i++)
				std::cout << ' ';

			std::cout << "|\u001b[40;1m";

			if (generationIndex > 0)
				std::cout << "   avg. fitness: " << std::fixed << std::setprecision(4) << avgFitnessOfGenerations.last();
		
			TrainCurrentGeneration();
		}

		std::cout << "\33[2K\rTraining done!" << std::endl;
	}
}
