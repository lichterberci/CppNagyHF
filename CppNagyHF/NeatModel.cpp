#include "NeatModel.hpp"

#include <algorithm>

namespace model {

	void NeatModel::GenerateLookUp() {

		for (int i = 0; i < genes.size(); i++)
			geneIndexLookupByOutputNeuron[genes[i].to] += i;
	}

	void NeatModel::ConstructSimplestModelForInputOutputNeurons() {

		cstd::Vector<ConnectionGene> result(NUM_SENSORS * NUM_OUTPUTS);

		for (int i = 0; i < NUM_SENSORS; i++)
			for (int j = 0; j < NUM_OUTPUTS; j++)
				result.push(ConnectionGene(i, NUM_SENSORS + j));

		genes = result;
	}

	cstd::Vector<double> NeatModel::Predict(const ModelParams& modelParams) {

		const auto inputs = modelParams.GetInputVector();

		cstd::Vector<double> result(NUM_OUTPUTS);

		std::unordered_map<int, double> valueMap;

		for (int i = 0; i < NUM_OUTPUTS; i++)
			result += ComputeValueOfNeuron(inputs, i, valueMap);

		return result;
	}

	double NeatModel::ComputeValueOfNeuron(const cstd::Vector<double>& inputs, int neuronId, std::unordered_map<int, double>& valueMap) const {

		if (neuronId < NUM_SENSORS)
			return inputs[neuronId];

		if (valueMap.find(neuronId) != valueMap.end())
			return valueMap.at(neuronId);

		double sum = 0;

		if (geneIndexLookupByOutputNeuron.find(neuronId) == geneIndexLookupByOutputNeuron.end()) {
			std::cout << "ERROR: neuron id not found in lookup!" << std::endl;
			throw std::out_of_range("Neuron id not found in lookup!");
		}

		const auto& connections = geneIndexLookupByOutputNeuron.at(neuronId);

		for (int inputIndex : connections) {

			const ConnectionGene& gene = genes[inputIndex];
			
			sum += gene.weight * ComputeValueOfNeuron(inputs, inputIndex, valueMap);
		}

		const double result = activationFunction.operator()(sum);

		valueMap[neuronId] = result;

		return result;
	}

	void NeatModel::GetKeyPresses(const ModelParams& modelParams, cstd::Vector<sf::Keyboard::Key>& out_keyPresses) {

		cstd::Vector<double> result = Predict(modelParams);

		// find max value
		int choice = 0;
		double maxValue = result[0];

		/*std::cout << "------------------------------" << std::endl;

		std::cout << "Output[" << 0 << "] = " << result[0] << std::endl;*/

		for (size_t i = 1; i < std::min<int>(result.size(), NUM_OUTPUTS); i++) {

			//std::cout << "Output[" << i << "] = " << result[i] << std::endl;

			if (result[i] > maxValue) {
				maxValue = result[i];
				choice = i;
			}
		}

		sf::Keyboard::Key keys[] = {
			sf::Keyboard::Up,
			sf::Keyboard::Down,
			sf::Keyboard::Left,
			sf::Keyboard::Right
		};

		out_keyPresses.push(keys[choice]);
	}
}