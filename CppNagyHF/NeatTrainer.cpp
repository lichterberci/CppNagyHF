#include "NeatTrainer.hpp"
#include "Game.hpp"

namespace model {

	void NeatTrainer::ConstructInitialGeneration() {

		cstd::Vector<NeatModel> initialGeneration(populationCount);

		for (size_t i = 0; i < populationCount; i++) {
			initialGeneration[i] = NeatModel(NUM_SENSORS, NUM_OUTPUTS, activationFunction, innovationNumberTable);
		}

		organismsByGenerations += initialGeneration;

		representativesOfThePrevGeneration += initialGeneration[0];
	}

	double NeatTrainer::TrainIndividual(NeatModel& neatModel) {

		auto game = game::Game(false, game::GameControlType::AI, gameWidth, gameHeight, -1, -1, neatModel, numMaxIdleSteps);

		game.Start();

		auto report = game.GenerateReport();

		double fitness = fitnessFunction(report);

		return fitness;
	}

	void NeatTrainer::TrainGeneration() {

		auto& currentGeneration = organismsByGenerations.last();

		cstd::Vector<double> fitnessScores(currentGeneration.size());

		for (auto& organism : currentGeneration)
			fitnessScores += TrainIndividual(organism);

		// speciation

		cstd::Vector<int> speciesIndicies = Speciate(currentGeneration);
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

		cstd::Vector<NeatModel&> representativesOfSpeciesInNewGeneration;

		for (const auto& organism : organisms) {

			bool didFindSpecies = false;

			for (size_t speciesIndex = 0; speciesIndex < representativesOfThePrevGeneration.size(); speciesIndex++) {

				const double delta = GetSpeciesDifferenceDelta(organism, representativesOfThePrevGeneration[speciesIndex]);

				if (delta <= neatDeltaSubT) {
					didFindSpecies = true;
					result += speciesIndex;
					break;
				}
			}

			if (didFindSpecies == false) {

				for (size_t newSpeciesIndex = 0; newSpeciesIndex < representativesOfSpeciesInNewGeneration.size(); newSpeciesIndex++) {

					const double delta = GetSpeciesDifferenceDelta(organism, representativesOfSpeciesInNewGeneration[newSpeciesIndex]);

					if (delta <= neatDeltaSubT) {
						didFindSpecies = true;
						result += 
					}

				}

			}

		}

		return result;
	}

	double NeatTrainer::GetSpeciesDifferenceDelta(const NeatModel& a, const NeatModel& b) {

		const unsigned int N = std::max(a.Genes().size(), b.Genes().size());

		unsigned int numDisjointGenes = 0;
		unsigned int numExcessGenes = 0;

		size_t i = 0, j = 0; // indicies of the vectors

		const size_t sizeOfSmaller = std::min(a.Genes().size(), b.Genes().size());

		int i = 0, j = 0;

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

			if (aGene < bGene) {
				i++;

				if (aGene.innovationNumber < innovationNumberInGenomWithSmallerMaxInnovationNumber)
					numDisjointGenes++;
				else
					numExcessGenes++;

				continue;
			}

			if (bGene < aGene) {
				j++;

				if (bGene.innovationNumber < innovationNumberInGenomWithSmallerMaxInnovationNumber)
					numDisjointGenes++;
				else
					numExcessGenes++;

				continue;
			}
		}

		numDisjointGenes += genesOfA.size() - i + genesOfB.size() - j;

		const double wBar = sumDifferenceOfWeights / numAllignedGenes;

		const double delta = neatC1 * numExcessGenes / N + neatC2 * numDisjointGenes / N + neatC3 * wBar;

		return delta;
	}
}
