
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Interpreter.h"

using namespace std;

int main(int argc, char* argv[]) {
	
Interpreter interpreterer(argv[1], argv[2]);

//cout<<"Program ended\n";
	return EXIT_SUCCESS;
}