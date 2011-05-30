#include <cstdlib>
#include <iostream>
#include <fstream>
#include <dlfcn.h> 

using namespace std;


extern "C"
{
	void log(char*);
	double calcNewton(double, double, double);
	double calcCircleArea(double);
	double calcRectArea(double, double);
} 

void* loghandle;

#define log(a) log((char*)a)
typedef void (*Tlog)(char*);

void loadLogPlugin()
{
	string filename;
	
	ifstream configfile;
    configfile.open("modules.conf", std::ios::in);
	if(configfile.is_open())
		getline(configfile, filename);
	else
		exit(1);
    configfile.close();

	loghandle = dlopen(("./"+filename).c_str() , RTLD_LAZY);
										
    if (!loghandle)
	{
        cerr << dlerror() << endl;
        exit(1);
    }
}


int main(int argc, char *argv[])
{
	loadLogPlugin();
					
	Tlog log = (Tlog) dlsym(loghandle, "log");
    if(!log)
	{
        cerr << dlerror() << endl;
        exit(1);
    }
	
	
	char ausgabe[20];
	
	log("calcCircleArea(5)");
	sprintf(ausgabe, "%f", calcCircleArea(5));
	log(ausgabe);
	
	log("calcRectArea(4, 6)");
	sprintf(ausgabe, "%f", calcRectArea(4, 6));
	log(ausgabe);
	
	log("calcNewton(4, 6, 2)");
	sprintf(ausgabe, "%f", calcNewton(4, 6, 2));
	log(ausgabe);
	
	
	
	
    cin.get();
	
	dlclose(loghandle);
    return EXIT_SUCCESS;
}
