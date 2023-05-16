#include "GameReport.hpp"

namespace game {

	std::ostream& operator<< (std::ostream& os, const GameReport& report) {
		os << "GameReport(" << (report.won ? "won" : "lost") << ", points=" << report.points << ", steps=" << report.numStepsTaken << ")";
		return os;
	}

}