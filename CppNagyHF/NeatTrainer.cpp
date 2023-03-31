#include "NeatTrainer.hpp"
#include "Game.hpp"

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

		auto game = game::Game(false, game::GameControlType::AI, gameWidth, gameHeight, -1, -1, neatModel, numMaxIdleSteps);

		game.Start();

		auto report = game.GenerateReport();

		double fitness = fitnessFunction(report);

		return fitness;
	}

	void NeatTrainer::TrainCurrentGeneration() {

		auto& currentGeneration = organismsByGenerations.last();

		cstd::Vector<double> fitnessScores(currentGeneration.size());

		for (auto& organism : currentGeneration)
			fitnessScores += TrainIndividual(organism);

		// speciation

		cstd::Vector<int> speciesIndicies = Speciate(currentGeneration);

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

		for (int i = 0; i < numSpecies; i++) {

			int speciesIndex = speciesIndicies[i];

			currentGeneration[speciesIndex].rawFitness = fitnessScores[i];
			currentGeneration[speciesIndex].adjustedFitness = fitnessScores[i] / speciesSizes[speciesIndicies[i]];

			sumOfAdjustedFitnessForEachSpecies[speciesIndex] += fitnessScores[i];
		}

		// divide them by their respective sizes

		for (int i = 0; i < numSpecies; i++)
			sumOfAdjustedFitnessForEachSpecies[i] /= speciesSizes[i];

		// allocate places accordingly

		double totalSum = 0;

		for (double adjustedFitness : sumOfAdjustedFitnessForEachSpecies)
			totalSum += adjustedFitness;

		int numTotalPlacesAllocated = 0;

		cstd::Vector<int> placesAllocatedForSpecies;
		placesAllocatedForSpecies.reserve_and_copy(numSpecies);

		for (int i = 0; i < numSpecies; i++) {

			int places = std::floor<int>(sumOfAdjustedFitnessForEachSpecies[i] / totalSum);

			placesAllocatedForSpecies += places;

			numTotalPlacesAllocated += places;
		}

		// if we have rounding errors, we just give all places to the first species by default
		if (numTotalPlacesAllocated < populationCount)
			placesAllocatedForSpecies[0] += populationCount - numTotalPlacesAllocated;

		// TODO: if sum of raw fitness does not rise for more than 20 generations, only keep the top 2 species

		// reproduction

		auto newGeneration = ProduceNewGenerationByReproduction(currentGeneration, speciesIndicies, placesAllocatedForSpecies, fitnessScores);

		for (auto& organism : newGeneration)
			organism.Mutate(
				innovationNumberTable, 
				chanceOfDentritInsertion, 
				chanceOfNeuronInsertion,
				chanceOfMutation, 
				chanceOfMutationBeingNewValue, 
				chanceOfDisabling, 
				weightSetMin, 
				weightSetMax, 
				weightAdjustMin, 
				weightAdjustMax
			);

		organismsByGenerations += newGeneration;
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

		for (auto& organism : organisms) {

			bool didFindSpecies = false;

			// we first search in those species that are present in the new generation
			for (int newSpeciesIndex = 0; newSpeciesIndex < (int)representativesOfSpeciesInNewGeneration.size(); newSpeciesIndex++) {

				const double delta = GetSpeciesDifferenceDelta(organism, *representativesOfSpeciesInNewGeneration[newSpeciesIndex]);

				if (delta <= neatDeltaSubT) {
					didFindSpecies = true;
					result += newSpeciesIndex;
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
						const size_t newIndexOfSpecies = representativesOfSpeciesInNewGeneration.size() - 1;

						// we mark the current organism as a candidate, so after speciation, we can swap the old ones to these
						representativeCandidatesFromTheNewGeneration += std::make_tuple(speciesIndex, &organism);

						// the species index will be set according to the new vector
						result += newIndexOfSpecies; 

						// we have succesfully moved this from the old to the new generation, now we can remove it
						representativesOfThePrevGeneration.removeAt(speciesIndex); 

						break;
					}
				}
			}

			if (didFindSpecies == false) {

				// we create a new species

				representativesOfSpeciesInNewGeneration += &organism;
				result += (int)representativesOfSpeciesInNewGeneration.size() - 1;
			}
		}

		// remove old representatives, and replace them with new ones
		for (const auto& newRepresentative : representativeCandidatesFromTheNewGeneration) {

			const int speciesIndex = std::get<0>(newRepresentative);
			const NeatModel* representative = std::get<1>(newRepresentative);

			representativesOfSpeciesInNewGeneration[speciesIndex] = representative;
		}

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
		const cstd::Vector<int>& numPlacesAllocatedForSpecies,
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

			while (species.size() > std::floor(portionOfSpeciesToKeepForReproduction * numPlacesAllocatedForSpecies[speciesIndex]))
				species.pop();
		}

		// from every species, select couples at random to reproduce
		// if a species only contains 1 organism, just copy it
		
		cstd::Vector<NeatModel> newGeneration;
		newGeneration.reserve_and_copy(populationCount);

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
}
