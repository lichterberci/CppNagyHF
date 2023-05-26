
#include <chrono>
#include "Manager.hpp"
#include "Tests.hpp"

#ifdef CPORTA
#define MEMTRACE
#include "memtrace.h"
#endif

int main(int argc, char* argv[])
{

    srand((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());

#ifndef CPORTA
    Manager::MainWithUI(argc, argv);
#else
    //Manager::MainWithoutUI(argc, argv);
    if (tests::ConvergenceTest("testparams.json"))
        std::cout << "TEST PASSED!" << std::endl;
    else
        std::cout << "TEST FAILED!" << std::endl;
#endif

    return 0;
}