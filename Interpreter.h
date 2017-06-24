#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

class Interpreter{
struct cmd_line{
	string header, instruction, operation;
	int line_numb;
};

//cmd_line* program_code = new cmd_line[32];
	public:
		Interpreter();
		Interpreter(string cmd_name, string data_name);
		cmd_line cmd_lines[32];
		int num_line;
};
#endif