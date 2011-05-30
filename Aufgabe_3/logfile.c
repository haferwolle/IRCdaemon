#include <iostream>
#include <fstream>

extern "C" void log(char* t)
{
    std::ofstream logfile;
    logfile.open("log.txt", std::ios::out | std::ios::app);
    logfile << t << std::endl;
    logfile.close();
}
