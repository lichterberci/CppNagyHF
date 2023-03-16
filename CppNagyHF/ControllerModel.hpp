#pragma once

#include "vector.hpp"
#include "ModelParams.hpp"
#include <SFML/Graphics.hpp>

namespace model {

	class ControllerModel {
	public:
		cstd::Vector<double> Predict(const ModelParams& modelParams);
		void GetKeyPresses(const ModelParams& modelParams, cstd::Vector<sf::Keyboard::Key>& out_keyPresses);
	};

	class RandomModel : ControllerModel {
		cstd::Vector<double> Predict(const ModelParams& modelParams) {
			cstd::Vector<double> result;

			result += (double)(rand() % 100 - 50);
			result += (double)(rand() % 100 - 50);
			result += (double)(rand() % 100 - 50);
			result += (double)(rand() % 100 - 50);

			return result;
		}

		void GetKeyPresses(const ModelParams& modelParams, cstd::Vector<sf::Keyboard::Key>& out_keyPresses) {
			cstd::Vector<double> result = Predict(modelParams);

			// find max value
			int choice = 0;
			double maxValue = result[0];
			for (int i = 1; i < std::min<int>(result.size(), 4); i++) {
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
	};

}