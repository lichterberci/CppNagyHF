#pragma once

#include <math.h>

#define LEAKY_RELU_ALPHA 0.1

namespace model {

	struct ActivationFunction {

		virtual double operator()(double x) const {
			return x;
		};
	};

	struct Sigmoid : public ActivationFunction {

		double operator()(double x) const override {
			return 1 / (1 + exp(-x));
		}
	};

	struct Tanh : public ActivationFunction {

		double operator()(double x) const override {
			return tanh(x);
		}
	};

	struct ReLU : public ActivationFunction {

		double operator()(double x) const override {
			return x >= 0 ? x : 0;
		}
	};

	struct LReLU : public ActivationFunction {

		double operator()(double x) {
			return x >= 0 ? x : x * LEAKY_RELU_ALPHA;
		}
	};

}