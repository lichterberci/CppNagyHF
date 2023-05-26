#pragma once

#include <string>
#include <unordered_map>

class Manager{

public:
#ifndef  CPORTA
	static void MainWithUI(int argc, char* argv[]);
#endif 
	static void MainWithoutUI(int argc, char* argv[]);

private:
	static std::unordered_map<std::string, std::string>  ProcessCmdArguments(int argc, char* argv[]);

};
