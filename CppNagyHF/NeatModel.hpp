#pragma once

#include "vector.hpp"
#include "ConnectionGene.hpp"
#include "ActivationFunction.hpp"
#include "ControllerModel.hpp"
#include "ModelParams.hpp"
#include <unordered_map>
#include <iostream>

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
		std::unordered_map<int, int> neuronLayerNumbers; // 0 for output, 1 if it is connected to output, etc.

		std::unordered_map<int, cstd::Vector<int>> geneIndexLookupByOutputNeuronIfDentritIsActive; // this helps reduce the time-complexity of the forwarding

		const ActivationFunction* activationFunction;

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
			: genes(cstd::Vector<ConnectionGene>()), geneIndexLookupByOutputNeuronIfDentritIsActive(std::unordered_map<int, cstd::Vector<int>>())
		{ }
	
		NeatModel(cstd::Vector<ConnectionGene> genes, int numSensors, int numOutputs, const ActivationFunction* activationFunction)
			: genes(genes), activationFunction(activationFunction)
		{ 
			GenerateLookUp();
			GenerateNeuronIndiciesList();
			OrderNeuronsByLayer();
		}

		NeatModel(const NeatModel& other)
			: genes(other.genes), activationFunction(other.activationFunction)
		{ 
			GenerateLookUp();
			GenerateNeuronIndiciesList();
			OrderNeuronsByLayer();
		}

		NeatModel(int sensorNeurons, int outputNeurons, const ActivationFunction* activationFunction, std::unordered_map<long long, int>& innovationNumberTable)
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

		static void ResetGlobalNeuronCount(int to = 0) {
			s_globalNeuronCount = to;
		}

		friend std::ostream& operator<< (std::ostream& os, const NeatModel& model);
	};

	
}
