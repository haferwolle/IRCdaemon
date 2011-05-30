#include <iostream>

extern "C" void log(char* t)
{
    std::cout << "Log: " << t << std::endl;
}
