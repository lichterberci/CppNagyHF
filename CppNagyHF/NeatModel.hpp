#pragma once

#include "vector.hpp"
#include "ConnectionGene.hpp"
#include "ActivationFunction.hpp"
#include "ControllerModel.hpp"
#include "ModelParams.hpp"
#include <unordered_map>

#define NUM_SENSORS 25
#define NUM_OUTPUTS 4

namespace model {

	/*
	
	Note:

	The order of the neurons is hard-coded: SENSORS (# = numSensors), OUTPUTS (# = numOutputs), HIDDEN (# = defined by genes)
	
	*/

	class NeatModel : public ControllerModel {

		cstd::Vector<ConnectionGene> genes;
		std::unordered_map<int, cstd::Vector<int>> geneIndexLookupByOutputNeuron; // this helps reduce the time-complexity of the forwarding

		ActivationFunction activationFunction;

		void GenerateLookUp();
		void ConstructSimplestModelForInputOutputNeurons();

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

		NeatModel(int sensorNeurons, int outputNeurons, ActivationFunction activationFunction = Sigmoid())
			: activationFunction(activationFunction)
		{
			ConstructSimplestModelForInputOutputNeurons();
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
	};

}
