#pragma once

//#define XOR 1

#include "vector.hpp"
#include "ConnectionGene.hpp"
#include "ActivationFunction.hpp"
#include "ControllerModel.hpp"
#include "ModelParams.hpp"
#include <unordered_map>
#include <iostream>
#include "Direction.hpp"


#if XOR == 1
	#define NUM_SENSORS 3
	#define NUM_OUTPUTS 1
#else
	#if USE_RELATIVE_DIRECTION == true
		#define NUM_SENSORS 9
		#define NUM_OUTPUTS 3
	#else
		#define NUM_SENSORS 25
		#define NUM_OUTPUTS 4
	#endif
#endif

#define MAX_ATTEMPTS_AT_INSERTING_DENTRIT 30

namespace model {

	/*
	
	Note:

	The order of the neurons is hard-coded: SENSORS (# = numSensors), OUTPUTS (# = numOutputs), HIDDEN (# = defined by genes)
	
	*/

	class NeatModel : public ControllerModel {

		static int s_globalNeuronCount;

		cstd::Vector<int> neuronIndicies;
		cstd::Vector<ConnectionGene> genes;
		std::unordered_map<int, int> topologicalOrderOfNeurons; // 0 for output, 1 if it is connected to output, etc.

		std::unordered_map<int, cstd::Vector<int>> geneIndexLookupByOutputNeuronOfAllDentrits; // this helps reduce the time-complexity of the forwarding

		std::shared_ptr<const ActivationFunction> activationFunction;

		void GenerateNeuronIndiciesList();
		void GenerateLookUp();
		void ConstructSimplestModelForInputOutputNeurons(std::unordered_map<long long, int>& innovationNumberTable);

		void OrderNeuronsByLayer();

		void InsertNewDentrit(std::unordered_map<long long, int>& innovationNumberTable);
		void InsertNewNeuron(std::unordered_map<long long, int>& innovationNumberTable);
	public:

		double rawFitness = 0;
		double adjustedFitness = 0;

		NeatModel()
		{ }

		// load from serialized file
		NeatModel(std::istream& is)
		{
			DesrializeAndSetUp(is);
		}

		NeatModel(const NeatModel& other) 
			: neuronIndicies(other.neuronIndicies), 
			genes(other.genes), 
			topologicalOrderOfNeurons(other.topologicalOrderOfNeurons), 
			geneIndexLookupByOutputNeuronOfAllDentrits(other.geneIndexLookupByOutputNeuronOfAllDentrits),
			activationFunction(other.activationFunction),
			rawFitness(other.rawFitness),
			adjustedFitness(other.adjustedFitness)
		{ }

		NeatModel(NeatModel&& other) noexcept
			: neuronIndicies(std::move(other.neuronIndicies)),
			genes(std::move(other.genes)),
			topologicalOrderOfNeurons(std::move(other.topologicalOrderOfNeurons)),
			geneIndexLookupByOutputNeuronOfAllDentrits(std::move(other.geneIndexLookupByOutputNeuronOfAllDentrits)),
			activationFunction(other.activationFunction),
			rawFitness(other.rawFitness),
			adjustedFitness(other.adjustedFitness)
		{ }

		NeatModel& operator= (const NeatModel& other) {
			neuronIndicies = other.neuronIndicies;
			genes = other.genes;
			topologicalOrderOfNeurons = other.topologicalOrderOfNeurons;
			geneIndexLookupByOutputNeuronOfAllDentrits = other.geneIndexLookupByOutputNeuronOfAllDentrits;
			activationFunction = other.activationFunction;
			rawFitness = other.rawFitness;
			adjustedFitness = other.adjustedFitness;

			return *this;
		}

		NeatModel& operator= (NeatModel&& other) noexcept {
			neuronIndicies = std::move(other.neuronIndicies);
			genes = std::move(other.genes);
			topologicalOrderOfNeurons = std::move(other.topologicalOrderOfNeurons);
			geneIndexLookupByOutputNeuronOfAllDentrits = std::move(other.geneIndexLookupByOutputNeuronOfAllDentrits);

			activationFunction = other.activationFunction;
			rawFitness = other.rawFitness;
			adjustedFitness = other.adjustedFitness;

			other.activationFunction = nullptr;
			other.rawFitness = 0;
			other.adjustedFitness = 0;

			return *this;
		}
	
		NeatModel(cstd::Vector<ConnectionGene> genes, const std::shared_ptr<const ActivationFunction>& activationFunction)
			: genes(genes), activationFunction(activationFunction)
		{ 
			GenerateLookUp();
			GenerateNeuronIndiciesList();
			OrderNeuronsByLayer();
		}

		NeatModel(const std::shared_ptr<const ActivationFunction>& activationFunction, std::unordered_map<long long, int>& innovationNumberTable)
			: activationFunction(activationFunction)
		{
			ConstructSimplestModelForInputOutputNeurons(innovationNumberTable);
			GenerateLookUp();
			GenerateNeuronIndiciesList();
			OrderNeuronsByLayer();
		}
				
		const cstd::Vector<ConnectionGene>& Genes() const {
			return genes;
		}

		cstd::Vector<ConnectionGene>& Genes() {
			return genes;
		}

		double ComputeValueOfNeuron(const cstd::Vector<double>& inputs, int neuronId, std::unordered_map<int, double>& valueMap) const;
		
		cstd::Vector<double> Predict(const ModelParams& modelParams) const;
		void GetKeyPresses(const ModelParams& modelParams, cstd::Vector<Direction>& out_keyPresses) const;

		void Mutate(
			std::unordered_map<long long, int>& innovationNumberTable, 
			double chanceOfDentritInsertion, 
			double chanceOfNeuronInsertion, 
			double chanceOfDentritMutation, 
			double chanceOfMutationBeingNewValue, 
			double chanceOfDisabling,
			double weightSetMin,
			double weightSetMax,
			double weightAdjustMin,
			double weightAdjustMax
		);

		static void ResetGlobalNeuronCount(int to = 0) {
			s_globalNeuronCount = to;
		}

		static int GetGlobalNeuronCount() {
			return s_globalNeuronCount;
		}

		friend std::ostream& operator<< (std::ostream& os, const NeatModel& model);

		std::ostream& Serialize(std::ostream& os) const;
		void DesrializeAndSetUp(std::istream& is);
	};

	
}
