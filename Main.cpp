
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Interpreter.h"

using namespace std;

int main(int argc, char* argv[]) {
	
Interpreter interpreterer(argv[1], argv[2]);

	return EXIT_SUCCESS;
}