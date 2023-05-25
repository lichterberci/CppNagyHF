#pragma once

#include <math.h>
#include <memory>

#define LEAKY_RELU_ALPHA 0.1

namespace model {

	struct ActivationFunction {

		virtual double operator()(double x) const = 0;
		virtual uint8_t GetTypeIndex() const = 0;
	};

	struct Sigmoid : public ActivationFunction {

		double operator()(double x) const override {
			return 1 / (1 + exp(-x));
		}

		uint8_t GetTypeIndex() const override {
			return 1;
		}
	};

	struct SteepSigmoid : public ActivationFunction {

		double coeff;

		double operator()(double x) const override {
			x *= coeff;
			return 1 / (1 + exp(-x));
		}

		uint8_t GetTypeIndex() const override {
			// steep sigmoid might with the given params might not be compiled --> it will be converted into sigmoid
			return 1; 
		}
	};

	struct Tanh : public ActivationFunction {

		double operator()(double x) const override {
			return tanh(x);
		}

		uint8_t GetTypeIndex() const override {
			return 2;
		}
	};

	struct ReLU : public ActivationFunction {

		double operator()(double x) const override {
			return x >= 0 ? x : 0;
		}

		uint8_t GetTypeIndex() const override {
			return 3;
		}
	};

	struct LReLU : public ActivationFunction {

		double operator()(double x) const override {
			return x >= 0 ? x : x * LEAKY_RELU_ALPHA;
		}

		uint8_t GetTypeIndex() const override {
			return 4;
		}
	};

}