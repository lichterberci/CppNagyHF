#pragma once

#include "vector.hpp"
#include "ConnectionGene.hpp"
#include "ActivationFunction.hpp"
#include "ControllerModel.hpp"
#include "ModelParams.hpp"
#include <unordered_map>

#define NUM_SENSORS 25
#define NUM_OUTPUTS 4
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
		cstd::Vector<int> neuronLayerNumbers; // 0 for output, 1 if it is connected to output, etc.

		std::unordered_map<int, cstd::Vector<int>> geneIndexLookupByOutputNeuron; // this helps reduce the time-complexity of the forwarding

		ActivationFunction activationFunction;

		double fitness;

		void GenerateNeuronIndiciesList();
		void GenerateLookUp();
		void ConstructSimplestModelForInputOutputNeurons(std::unordered_map<long long, int>& innovationNumberTable);

		void OrderNeuronsByLayer();
		void SetNeuronOrder(int neuronId, cstd::Vector<int>& orders, int depth = 0);

		void InsertNewDentrit(std::unordered_map<long long, int>& innovationNumberTable);
		void InsertNewNeuron(std::unordered_map<long long, int>& innovationNumberTable);
	public:
		NeatModel() 
			: genes(cstd::Vector<ConnectionGene>()), activationFunction(Sigmoid()), geneIndexLookupByOutputNeuron(std::unordered_map<int, cstd::Vector<int>>())
		{ }
	
		NeatModel(cstd::Vector<ConnectionGene> genes, int numSensors, int numOutputs, ActivationFunction activationFunction = Sigmoid())
			: genes(genes), activationFunction(activationFunction)
		{ 
			GenerateLookUp();
		}

		NeatModel(const NeatModel& other)
			: genes(other.genes), activationFunction(other.activationFunction)
		{ 
			GenerateLookUp();
		}

		NeatModel(int sensorNeurons, int outputNeurons, ActivationFunction activationFunction, std::unordered_map<long long, int>& innovationNumberTable)
			: activationFunction(activationFunction)
		{
			ConstructSimplestModelForInputOutputNeurons(innovationNumberTable);
			GenerateLookUp();
		}
		
		const cstd::Vector<ConnectionGene>& Genes() const {
			return genes;
		}

		cstd::Vector<ConnectionGene>& Genes() {
			return genes;
		}

		double ComputeValueOfNeuron(const cstd::Vector<double>& inputs, int neuronId, std::unordered_map<int, double>& valueMap) const;
		
		cstd::Vector<double> Predict(const ModelParams& modelParams);
		void GetKeyPresses(const ModelParams& modelParams, cstd::Vector<sf::Keyboard::Key>& out_keyPresses);

		void Mutate(
			std::unordered_map<long long, int>& innovationNumberTable, 
			double chanceOfDentritInsertion, 
			double chanceOfNeuronInsertion, 
			double chanceOfMutation, 
			double chanceOfMutationBeingNewValue, 
			double chanceOfDisabling,
			double weightSetMin,
			double weightSetMax,
			double weightAdjustMin,
			double weightAdjustMax
		);

		static void ResetGlobalNeuronCount() {
			s_globalNeuronCount = 0;
		}
	};

}
