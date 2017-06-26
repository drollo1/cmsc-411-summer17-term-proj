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
	string get_header(){return header;}
	string get_instruction(){return instruction;}
	string get_operation(){return operation;}
	int get_number(){return line_numb;}
};

//cmd_line* program_code = new cmd_line[32];
	public:
		Interpreter();
		Interpreter(string cmd_name, string data_name);
		void parse_instructions(string cmd_name);
		void print_code();
		void run();
		int if_control();
		int id_control();
		int ex1_control();
		int ex2_control();
		int ex3_control();
		int mem_control();
		int wb_control();

		cmd_line cmd_lines[32];
		int num_line;
		int registers[32];

		cmd_line * IF, * ID, * EX1, * EX2, * EX3, * MEM, * WB;
};
#endif