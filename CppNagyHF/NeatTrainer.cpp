#include "NeatTrainer.hpp"
#include "Game.hpp"
#include "ModelUtils.hpp"

#ifndef CPORTA
#include <execution>
#endif
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <atomic>
#include <iomanip>

namespace model {

	NeatTrainer* NeatTrainer::instance = nullptr;

#ifndef CPORTA
	std::atomic<bool> g_stopLoop = false;
#else
	bool g_stopLoop = false;
#endif

	void NeatTrainer::ConstructInitialGenerationFromFile(std::ifstream& file) {

		fitnessFunction = utils::GenerateFitnessFunctionFromTypeIndex((uint8_t)file.get());
		fitnessFunction->ReadAndSetUpCoeffs(file);

		int globalNeuronCount;
		file.read((char*)&globalNeuronCount, sizeof(int));
		NeatModel::ResetGlobalNeuronCount(globalNeuronCount);

		file.read((char*)&populationCount, sizeof(int));

		if (populationCount <= 0) {
			std::cerr << "ERROR: Population count is 0 or negative in file!" << std::endl;
			throw "Population count is 0 or negative in file!";
		}

		size_t innovationNumberTableSize;
		file.read((char*)&innovationNumberTableSize, sizeof(size_t));

		innovationNumberTable.clear();

		for (size_t i = 0; i < innovationNumberTableSize; i++) {
			long long key;
			int value;
			file.read((char*)&key, sizeof(long long));
			file.read((char*)&value, sizeof(int));
			innovationNumberTable[key] = value;
		}

		for (int i = 0; i < populationCount; i++) {
			int speciesIndex;
			file.read((char*)&speciesIndex, sizeof(int));
			speciesIndiciesOfOrganisms += speciesIndex;
		}

		// count species <==> max species index + 1

		size_t speciesCount = *std::min_element(speciesIndiciesOfOrganisms.begin(), speciesIndiciesOfOrganisms.end(), std::greater<int>()) + 1;

		speciesData = cstd::Vector<SpeciesData>();

		for (size_t i = 0; i < speciesCount; i++) {
			SpeciesData data;

			file.read((char*)&data, sizeof(SpeciesData));

			speciesData += data;
		}

		cstd::Vector<NeatModel> initialGeneration;

		for (int i = 0; i < populationCount; i++) {
			NeatModel model;
			model.DesrializeAndSetUp(file);
			initialGeneration += model;
		}

		organismsByGenerations += std::move(initialGeneration);

		// representatives

		for (size_t i = 0; i < speciesData.size(); i++)
			representativesOfThePrevGeneration += nullptr;

		for (int i = 0; i < populationCount; i++) {
			const int speciesIndex = speciesIndiciesOfOrganisms[i];

			if (representativesOfThePrevGeneration[speciesIndex] == nullptr)
				representativesOfThePrevGeneration[speciesIndex] = &organismsByGenerations[0][i];
		}

		file.close();
	}

	void NeatTrainer::ConstructInitialGeneration() {

		cstd::Vector<NeatModel> initialGeneration;
		initialGeneration.reserve_and_copy(populationCount);

		for (int i = 0; i < populationCount; i++) {
			initialGeneration += NeatModel(activationFunction, innovationNumberTable);
		}

		organismsByGenerations += initialGeneration;

		speciesData += { 0, 0, 0 };

		representativesOfThePrevGeneration += &organismsByGenerations[0][0];
	}

	double NeatTrainer::EvaluateIndividual(const NeatModel& neatModel) {

#if XOR == 1

		ModelParams params;
		params.SetToRandom();

		double avgLoss = 0;

		for (int i = 0; i < numberOfEvaluationSteps; i++) {

			double randVal = utils::RandomDouble(0, 1);

			double solution = 0;

			if (randVal > 0.75) {
				params.distancesToWall[0] = 0;
				params.distancesToWall[1] = 0;
				solution = 0;
			}
			else if (randVal > 0.5) {
				params.distancesToWall[0] = 0;
				params.distancesToWall[1] = 1;
				solution = 1;
			}
			else if (randVal > 0.25) {
				params.distancesToWall[0] = 1;
				params.distancesToWall[1] = 0;
				solution = 1;
			}
			else {
				params.distancesToWall[0] = 1;
				params.distancesToWall[1] = 1;
				solution = 0;
			}

			params.distancesToWall[2] = 1;

			auto res = neatModel.Predict(params);

			double loss = powl(res[0] - solution, 2);

			avgLoss += loss / numberOfEvaluationSteps;
		}

		return 1 - avgLoss;

#else
		double avgFitness = 0;

		for (size_t i = 0; i < numberOfEvaluationSteps; i++) {

			auto game = game::Game(false, game::GameControlType::AI, gameWidth, gameHeight, 800, 800, neatModel, numMaxIdleSteps, placeFirstAppleInFrontOfSnake);

			game.Start();

			auto report = game.GenerateReport();

			double fitness = fitnessFunction->operator()(report);

			avgFitness += fitness / numberOfEvaluationSteps;
		}

		return avgFitness;
#endif
	}

	void NeatTrainer::TrainCurrentGeneration() {

		auto& currentGeneration = organismsByGenerations.last();

		cstd::Vector<double> fitnessScores(currentGeneration.size());
#ifndef CPORTA
		std::vector<double> fitnessScoresAsyncResults;
		
		for (int i = 0; i < populationCount; i++)
			fitnessScoresAsyncResults.push_back(0);

		std::for_each(
			std::execution::par_unseq, 
			currentGeneration.begin(), 
			currentGeneration.end(),
			[&](const NeatModel& organism) {
				size_t index = (&organism - currentGeneration.begin());
				fitnessScoresAsyncResults[index] = EvaluateIndividual(organism);
				//std::cout << "index: " << index << " fitness: " << fitnessScoresAsyncResults[index] << std::endl;
			}
		);

		for (const double fitness : fitnessScoresAsyncResults)
			fitnessScores += fitness;
#else
		for (auto& organism : currentGeneration)
			fitnessScores += EvaluateIndividual(organism);
#endif
		// speciation

		const cstd::Vector<int> speciesIndicies = Speciate(currentGeneration, speciesData);

		speciesIndiciesOfOrganisms = speciesIndicies;

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

		// for new species, set the start fitness
		for (int speciesIndex = 0; speciesIndex < numSpecies; speciesIndex++) {

			speciesData[speciesIndex].lastFitness = sumOfAdjustedFitnessForEachSpecies[speciesIndex];

			if (speciesData[speciesIndex].age > 0)
				continue;

			speciesData[speciesIndex].startFitness = sumOfAdjustedFitnessForEachSpecies[speciesIndex];
		}

		// cut off old species that are not progressing
		for (int speciesIndex = 0; speciesIndex < numSpecies; speciesIndex++) {
			
			if (speciesData[speciesIndex].age < speciesDropOffAge)
				continue;

			const double fitnessDelta = speciesData[speciesIndex].lastFitness - speciesData[speciesIndex].startFitness;

			// if the fitness did not increase sufficiently, kill the species
			if (fitnessDelta < speciesDropOffFitnessThreshold * speciesData[speciesIndex].age / speciesDropOffAge)
				sumOfAdjustedFitnessForEachSpecies[speciesIndex] = 0;
		}

		// if sum of raw fitness does not rise for more than X generations, only keep the top Y species
		if (avgFitnessOfGenerations.size() % numGenerationsWithSameFitnessBeforeOnlyLookingAtTopSpecies == 0) {

			bool hasThereBeenImprovement = false;

			double firstFitnessInObservedRange = avgFitnessOfGenerations[avgFitnessOfGenerations.size() - numGenerationsWithSameFitnessBeforeOnlyLookingAtTopSpecies];
			
			// THIS CODE CHECKS IF THERE HAS BEEN ANY IMPROVEMENT (TOO GENEROUS)
			// 
			//for (
			//	unsigned int generationIndex = avgFitnessOfGenerations.size() - numGenerationsWithSameFitnessBeforeOnlyLookingAtTopSpecies + 1; 
			//	generationIndex < avgFitnessOfGenerations.size(); 
			//	generationIndex++
			//) {
			//	if (firstFitnessInObservedRange + minImprovementOfAvgFitnessToConsiderItAnImprovement <= avgFitnessOfGenerations[generationIndex]) {
			//		hasThereBeenImprovement = true;
			//		break;
			//	}
			//}

			hasThereBeenImprovement = avgFitnessOfGenerations.last() - firstFitnessInObservedRange >= minImprovementOfAvgFitnessToConsiderItAnImprovement;

			if (hasThereBeenImprovement == false) {

				// only keep top Y species

				cstd::Vector<int> speciesOrderedByFitness;

				for (int i = 0; i < numSpecies; i++)
					speciesOrderedByFitness += i;

				std::sort(speciesOrderedByFitness.begin(), speciesOrderedByFitness.end(), [&sumOfAdjustedFitnessForEachSpecies](int a, int b) {
					return sumOfAdjustedFitnessForEachSpecies[a] < sumOfAdjustedFitnessForEachSpecies[b];
				});

				for (int i = 0; i < std::min<int>(numberOfTopSpeciesToLookAtIfFitnessIsStableForTooLong, numSpecies); i++) {
					sumOfAdjustedFitnessForEachSpecies[i] = 0; // dont allocate anything for this species
				}
			}
		}

		// allocate places accordingly 

		auto placesAllocatedForSpecies = AllocatePlacesForSpecies(sumOfAdjustedFitnessForEachSpecies);

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

			/*for (int i = populationCount - 1; i >= numBestOrganismsToKeepFromPrevGenerations; i--)
				gen.pop();*/

			gen = cstd::Vector<NeatModel>();
		}
	}

	cstd::Vector<int> NeatTrainer::Speciate(const cstd::Vector<NeatModel>& organisms, cstd::Vector<SpeciesData>& speciesAgeData) {

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

		std::unordered_map<int, int> newToOldSpeciesMap;

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
						const int newIndexOfSpecies = (int)representativesOfSpeciesInNewGeneration.size() - 1;

						// we mark the current organism as a candidate, so after speciation, we can swap the old ones to these
						representativeCandidatesFromTheNewGeneration += std::make_tuple(newIndexOfSpecies, &organism);

						// the species index will be set according to the new vector
						result += newIndexOfSpecies; 

						// we have succesfully moved this from the old to the new generation, now we can remove it
						representativesOfThePrevGeneration.removeAt(speciesIndex); 

						// record the species it came from
						newToOldSpeciesMap[(int)representativeCandidatesFromTheNewGeneration.size() - 1] = speciesIndex;

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
		for (const auto representative : representativeCandidatesFromTheNewGeneration) {
			const int speciesIndex = std::get<0>(representative);
			const auto candidate = std::get<1>(representative);
			representativesOfSpeciesInNewGeneration[speciesIndex] = candidate;
		}

		// replace old representatives with new ones
		representativesOfThePrevGeneration = representativesOfSpeciesInNewGeneration;

		// handle speciesAge
		cstd::Vector<SpeciesData> newSpeciesAgeData;

		for (int newSpeciesIndex = 0; newSpeciesIndex < (int)representativesOfSpeciesInNewGeneration.size(); newSpeciesIndex++) {

			if (newToOldSpeciesMap.find(newSpeciesIndex) == newToOldSpeciesMap.end()) {
				newSpeciesAgeData += { 0, 0, 0 }; // it is new
				continue;
			}

			auto dataFromLastGenerationsSpecies = speciesAgeData[newToOldSpeciesMap.at(newSpeciesIndex)];
			dataFromLastGenerationsSpecies.age++;
			newSpeciesAgeData += dataFromLastGenerationsSpecies;
		}

		speciesAgeData = newSpeciesAgeData;

		return result;
	}

	double NeatTrainer::GetSpeciesDifferenceDelta(const NeatModel& a, const NeatModel& b) {

		const size_t N = std::max<const size_t>(a.Genes().size(), b.Genes().size());

		unsigned int numDisjointGenes = 0;
		unsigned int numExcessGenes = 0;

		size_t i = 0, j = 0; // indicies of the vectors

		const size_t sizeOfSmaller = std::min(a.Genes().size(), b.Genes().size());

		const auto& genesOfA = a.Genes();
		const auto& genesOfB = b.Genes();

		double sumDifferenceOfWeights = 0;
		size_t numAllignedGenes = 0;

		/*const int innovationNumberInGenomWithSmallerMaxInnovationNumber = std::min(
			genesOfA.last().innovationNumber, 
			genesOfB.last().innovationNumber
		);*/

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

		numExcessGenes += (uint32_t)genesOfA.size() - (uint32_t)i + (uint32_t)genesOfB.size() - (uint32_t)j;

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

		for (size_t i = 0; i < numPlacesAllocatedForSpecies.size(); i++)
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
		for (int speciesIndex = 0; speciesIndex < (int)organismIndiciesBySpecies.size(); speciesIndex++) {

			auto& species = organismIndiciesBySpecies[speciesIndex];

			while (species.size() > std::ceil(portionOfSpeciesToKeepForReproduction * numPlacesAllocatedForSpecies[speciesIndex]))
				species.pop();
		}

		// from every species, select couples at random to reproduce
		// if a species only contains 1 organism, just copy it
		
		cstd::Vector<NeatModel> newGeneration;
		newGeneration.reserve_and_copy(populationCount);

		size_t recievingSpeciesIndex = 0;

		// if a species' organisms were killed off in the decimation part, but places were allocated, 
		// we want to give them away for another species
		for (int speciesIndex = 0; speciesIndex < (int)organismIndiciesBySpecies.size(); speciesIndex++) {

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

		for (size_t speciesIndex = 0; speciesIndex < organismIndiciesBySpecies.size(); speciesIndex++) {

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

				int indexInSpeciesOfParentA = (int)utils::RandomInt(0, (int)species.size());
				int indexInSpeciesOfParentB = (int)utils::RandomInt(0, (int)species.size());

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

		if ((int)newGeneration.size() != populationCount) {
			std::cout << "ERROR: new generation has invalid length!" << std::endl;
			throw "New generation has invalid length!";
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

		/*const int innovationNumberInGenomWithSmallerMaxInnovationNumber = std::min(
			genesOfA.last().innovationNumber,
			genesOfB.last().innovationNumber
		);*/

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

		return NeatModel(newGenes, activationFunction);
	}

	void NeatTrainer::KeyInterruptHandler(int code) const
	{
		g_stopLoop = true;

		std::cout << std::endl << "Do you want to exit? (y/n) ";

		if (tolower(std::cin.get()) != 'y') {
			std::cout << "\33[2K\r"; // delete everything that might be on the new line
			std::cout << "\033[1A\33[2K\r"; // delete the question with the answer
			std::cout << "\033[1A\33[2K\r"; // delete the previous line
			signal(SIGINT, [](int code) { NeatTrainer::instance->KeyInterruptHandler(code); });
			g_stopLoop = false;
			return;
		}

		std::cin.get(); // '\n'

		std::cout << "Do you want to save your progress? (y/n) ";

		if (tolower(std::cin.get()) != 'y')
			exit(code);

		std::cin.get(); // '\n'

		std::cout << "Filename:" << std::endl;

		std::string fileName;
		std::getline(std::cin, fileName);
		
		try {
			SaveProgress(fileName);
		}
		catch (...) {
			std::cerr << "ERROR: Could not save file!" << std::endl;
		}

		exit(code);
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

			int places = (int)std::floor<int>((int)placesDouble);

			placesAllocatedForSpecies += places;

			numTotalPlacesAllocated += places;
		}

		// if we have rounding errors, we give them away in the order of fitnesses
		if (numTotalPlacesAllocated < populationCount) {

			cstd::Vector<int> speciesIndiciesOderedByFitness;

			for (int i = 0; i < (int)sumOfAdjustedFitnessForEachSpecies.size(); i++)
				speciesIndiciesOderedByFitness += i;

			std::sort(
				speciesIndiciesOderedByFitness.begin(), 
				speciesIndiciesOderedByFitness.end(), 
				[&sumOfAdjustedFitnessForEachSpecies] (const int a, const int b) { 
					return sumOfAdjustedFitnessForEachSpecies[a] < sumOfAdjustedFitnessForEachSpecies[b]; 
				}
			);

			int recieveingIndexInSortedArray = 0;

			bool isThereNonExtinctSpecies = numTotalPlacesAllocated > 0;

			while (numTotalPlacesAllocated < populationCount) {

				const int speciesIndex = speciesIndiciesOderedByFitness[recieveingIndexInSortedArray];

				if (isThereNonExtinctSpecies == false || placesAllocatedForSpecies[speciesIndex] > 0) {
					placesAllocatedForSpecies[speciesIndex]++;
					numTotalPlacesAllocated++;
				}
				recieveingIndexInSortedArray = (recieveingIndexInSortedArray + 1) % numSpecies;
			}
		}

		return placesAllocatedForSpecies;
	}

	bool NeatTrainer::Train() {

		std::cout << "Training " << numGenerations << " generation with " << populationCount << " organisms in each..." << std::endl;

		signal(SIGINT, [](int code) { NeatTrainer::instance->KeyInterruptHandler(code); });

		for (int generationIndex = 0; generationIndex < numGenerations; generationIndex++) {

			while (g_stopLoop) {}

			//std::cout << "\33[2K\r";

			std::cout <<
				"\33[2K\rTraining generation " << std::setfill(' ') << std::setw(4) 
				<< generationIndex << "/" << numGenerations
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

			std::cout << "\033[0m";

			if (generationIndex > 0) {
				std::cout << "   avg. fitness: " << std::fixed << std::setprecision(4) << avgFitnessOfGenerations.last();
				std::cout << ", # species: " << speciesData.size();

				double avgSpeciesAge = 0;

				for (const auto& speciesAge : speciesData)
					avgSpeciesAge += speciesAge.age;

				avgSpeciesAge /= speciesData.size();

				std::cout << ", avg. species age: " << avgSpeciesAge;
			}
		
			TrainCurrentGeneration();

			//if (avgFitnessOfGenerations.last() >= targetFitness) {
			//	std::cout << "\33[2K\rTarget fitness reached, evolution stopped! (generation: " << generationIndex << ", fitness: " << avgFitnessOfGenerations.last() << ")" << std::endl;
			//	return;
			//}

			SpeciesData* dataOfBestSpecies = std::max_element(
				speciesData.begin(), 
				speciesData.end(), 
				[&](const SpeciesData a, const SpeciesData b) {
					return a.lastFitness < b.lastFitness;
				});

			if (dataOfBestSpecies->lastFitness >= targetFitness) {

				std::cout << "\33[2K\rTarget fitness reached, evolution stopped! (generation: " << generationIndex << ", current fitness of a good species: " << dataOfBestSpecies->lastFitness << ")" << std::endl;

				int specIndex = 0; 

				for (size_t i = 0; i < speciesData.size(); i++)
					if (&speciesData[i] == dataOfBestSpecies)
						specIndex = i;

				NeatModel* bestModel = const_cast<NeatModel*>(representativesOfThePrevGeneration[specIndex]);

				std::cout << "Fitness: " << bestModel->rawFitness << std::endl;

				for (int i = 0; i < 5; i++)
					std::cout << "Score: " << EvaluateIndividual(*bestModel) << std::endl;

				return true;
			}

			chanceOfNeuronInsertion *= 0.99;
			chanceOfDentritInsertion *= 0.99;
		}

		std::cout << "\33[2K\rTraining done! Avg. fitness of the last generation was " << avgFitnessOfGenerations.last() << std::endl;

		signal(SIGINT, exit);

		return false;
	}

	void NeatTrainer::SaveProgress(const std::string& fileName) const {
		
		std::ofstream file(fileName, std::ios::binary | std::ios::out);

		// fitness function

		uint8_t fitnessFunctionType = fitnessFunction->GetTypeIndex();
		file.write((char*) & fitnessFunctionType, 1);
		fitnessFunction->WriteCoeffs(file);

		// global neuron count

		int globalNeuronCount = NeatModel::GetGlobalNeuronCount();
		file.write((char*)&globalNeuronCount, sizeof(int));

		// pop count

		file.write((char*)&populationCount, sizeof(int));

		// innovation number table

		size_t innovationNumberTableSize = innovationNumberTable.size();
		file.write((char*)&innovationNumberTableSize, sizeof(size_t));

		for (const auto item : innovationNumberTable) {
			const auto key = item.first;
			const auto value = item.second;

			file.write((char*)&key, sizeof(long long));
			file.write((char*)&value, sizeof(int));
		}

		// species indicies

		for (const int speciesIndex : speciesIndiciesOfOrganisms)
			file.write((char*)&speciesIndex, sizeof(int));

		// species data

		for (const SpeciesData& data : speciesData)
			file.write((char*)&data, sizeof(SpeciesData));

		// models

		for (const auto& model : organismsByGenerations.last())
			model.Serialize(file);

		file.close();

		std::cout << "Progress saved to [" << fileName << "]" << std::endl;
	}

	const NeatModel* NeatTrainer::GetModelFromBestSpeciesInLastGeneration() const
	{
		const SpeciesData* dataOfBestSpecies = std::max_element(
			speciesData.begin(),
			speciesData.end(),
			[&](const SpeciesData a, const SpeciesData b) {
				return a.lastFitness < b.lastFitness;
			});

		int specIndex = 0;

		for (size_t i = 0; i < speciesData.size(); i++)
			if (&speciesData[i] == dataOfBestSpecies)
				specIndex = i;

		const NeatModel* bestModel = const_cast<NeatModel*>(representativesOfThePrevGeneration[specIndex]);

		return bestModel;
	}
}
