#include "NeatModel.hpp"

#include <algorithm>

namespace model {

	void NeatModel::GenerateLookUp() {

		for (int i = 0; i < genes.size(); i++)
			geneIndexLookupByOutputNeuron[genes[i].to] += i;
	}

	cstd::Vector<ConnectionGene> NeatModel::ConstructSimplestModelForInputOutputNeurons() {

		cstd::Vector<ConnectionGene> result(NUM_SENSORS * NUM_OUTPUS);

		for (size_t i = 0; i < NUM_SENSORS; i++)
			for (size_t j = 0; j < NUM_OUTPUS; j++)
				result.push(ConnectionGene(i, NUM_SENSORS + j));

		return result;
	}

	cstd::Vector<double> NeatModel::Predict(const ModelParams& modelParams) {

		const auto inputs = modelParams.GetInputVector();

		cstd::Vector<double> result(NUM_OUTPUS);

		return result;
	}

	double NeatModel::ComputeValueOfNeuron(const cstd::Vector<double>& inputs, const int neuronId) const {

		if (neuronId < NUM_SENSORS)
			return inputs[neuronId];

		double sum = 0;

		if (geneIndexLookupByOutputNeuron.find(neuronId) == geneIndexLookupByOutputNeuron.end()) {
			std::cout << "ERROR: neuron id not found in lookup!" << std::endl;
			throw std::out_of_range("Neuron id not found in lookup!");
		}

		for (int inputIndex : geneIndexLookupByOutputNeuron.at(neuronId)) {

			const ConnectionGene& gene = genes[inputIndex];


		}

		return activationFunction.operator()(sum);
	}

	void NeatModel::GetKeyPresses(const ModelParams& modelParams, cstd::Vector<sf::Keyboard::Key>& out_keyPresses) {

	}
}