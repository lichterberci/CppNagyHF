#pragma once

#include <iostream>

namespace game {

	struct GameReport {

		bool won = false;
		int points = 0;
		int numStepsTaken = 0;

		GameReport() {}
		GameReport(bool didWin, int points, int numStepsTaken) 
			: won(didWin), points(points), numStepsTaken(numStepsTaken)
		{}

		friend std::ostream& operator<< (std::ostream& os, const GameReport& report);
	};	

}