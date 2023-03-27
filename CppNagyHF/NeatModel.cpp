#include "NeatModel.hpp"

#include <algorithm>
#include "ModelUtils.hpp"
#include <set>

namespace model {

	int NeatModel::s_globalNeuronCount = 0;

	void NeatModel::GenerateLookUp() {

		for (int i = 0; i < genes.size(); i++)
			geneIndexLookupByOutputNeuron[genes[i].to] += i;
	}

	void NeatModel::ConstructSimplestModelForInputOutputNeurons(std::unordered_map<long long, int>& innovationNumberTable) {

		cstd::Vector<ConnectionGene> result(NUM_SENSORS * NUM_OUTPUTS);

		for (int i = 0; i < NUM_SENSORS; i++)
			for (int j = 0; j < NUM_OUTPUTS; j++)
				result.push(ConnectionGene(i, NUM_SENSORS + j, innovationNumberTable));

		s_globalNeuronCount += NUM_SENSORS * NUM_OUTPUTS;

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

		for (int i = 1; i < std::min<int>(result.size(), NUM_OUTPUTS); i++) {

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

	void NeatModel::InsertNewDentrit(std::unordered_map<long long, int>& innovationNumberTable) {

		// we choose 2 inficies that are in order (to avoid any loops)
		// --> we choose a separator index at random and then 2 indicies from its left and right

		int from, to;

		int attempts = 0;

		do {

			from = utils::RandomInt(0, neuronIndicies.size());
			to = utils::RandomInt(0, neuronIndicies.size());

		} while (neuronLayerNumbers[from] <= neuronLayerNumbers[to] && attempts++ <= MAX_ATTEMPTS_AT_INSERTING_DENTRIT);

		if (attempts >= MAX_ATTEMPTS_AT_INSERTING_DENTRIT)
			return;

		ConnectionGene newGene;

		if (innovationNumberTable.find(utils::MakeHashKeyFromPair(from, to)) == innovationNumberTable.end()) {

			newGene = ConnectionGene(from, to, innovationNumberTable);

			innovationNumberTable[utils::MakeHashKeyFromPair(from, to)] = newGene.innovationNumber;
		}
		else {
			newGene = ConnectionGene(from, to, innovationNumberTable[utils::MakeHashKeyFromPair(from, to)]);
		}

		genes.push(newGene);
	}

	void NeatModel::InsertNewNeuron(std::unordered_map<long long, int>& innovationNumberTable) {

		int dentritToInsertNeuronIn = utils::RandomInt(0, genes.size());

		auto& oldGene = genes[dentritToInsertNeuronIn];

		int fromIndex = oldGene.from;
		int toIndex = oldGene.to;
		int throughIndex = s_globalNeuronCount++;

		ConnectionGene newFromThroughGene(fromIndex, throughIndex, innovationNumberTable);
		ConnectionGene newThroughToGene(throughIndex, toIndex, innovationNumberTable);

		newFromThroughGene.weight = 1.0; // in order for new mutations not to have too much an effect

		oldGene.Disable();
	}

	void NeatModel::GenerateNeuronIndiciesList() {

		neuronIndicies = cstd::Vector<int>();

		std::set<int> indexSet;

		for (const auto& gene : genes) {
			indexSet.insert(gene.from);
			indexSet.insert(gene.to);
		}

		for (int index : indexSet)
			neuronIndicies.push(index);
	}

	void NeatModel::Mutate(
		std::unordered_map<long long, int>& innovationNumberTable, 
		double chanceOfNeuronInsertion, 
		double chanceOfDentritInsertion,
		double chanceOfMutation, 
		double chanceOfMutationBeingNewValue, 
		double chanceOfDisabling,
		double weightSetMin, 
		double weightSetMax, 
		double weightAdjustMin, 
		double weightAdjustMax
	) {

		if (utils::RandomDouble(0, 1) < chanceOfNeuronInsertion)
			InsertNewNeuron(innovationNumberTable);

		if (utils::RandomDouble(0, 1) < chanceOfDentritInsertion)
			InsertNewDentrit(innovationNumberTable);

		for (auto& gene : genes)
			if (utils::RandomDouble(0, 1) < chanceOfMutation)
				gene.MutateWeight(chanceOfMutationBeingNewValue, weightSetMin, weightSetMax, weightAdjustMin, weightAdjustMax);
	}

	void NeatModel::OrderNeuronsByLayer() {

		neuronLayerNumbers = cstd::Vector<int>();
		neuronLayerNumbers.reserve_and_copy(neuronIndicies.size());

		for (int i = 0; i < neuronLayerNumbers.size(); i++)
			neuronLayerNumbers += 99999;

		if (geneIndexLookupByOutputNeuron.size() == 0)
			GenerateLookUp();

		for (int i = NUM_SENSORS; i < NUM_SENSORS + NUM_OUTPUTS; i++)
			SetNeuronOrder(i, neuronLayerNumbers);
	}

	void NeatModel::SetNeuronOrder(int neuronId, cstd::Vector<int>& orders, int depth) {

		// implements DFS with always setting to the min

		if (orders[neuronId] == -1)
			orders[neuronId] = depth;
		else 
			orders[neuronId] = std::min(depth, orders[neuronId]);

		if (neuronId < NUM_SENSORS)
			return;

		const auto& connections = geneIndexLookupByOutputNeuron.at(neuronId);

		for (int inputIndex : connections)
			SetNeuronOrder(inputIndex, orders, depth + 1);
	}
}