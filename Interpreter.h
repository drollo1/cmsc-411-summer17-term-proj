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
	//string get_header(){return header;}
	string get_instruction(){return instruction;}
	//string get_operation(){return operation;}
	//int get_number(){return line_numb;}
	string ops[5];  //0=reg1 1=reg2 2=reg3 3= imediate 4=header
};

//cmd_line* program_code = new cmd_line[32];
	public:
		Interpreter();
		Interpreter(string cmd_name, string data_name);
		void parse_instructions(string cmd_name);
		void print_code();
		void run();
		int buff_move(int pos);
		int isDone();
		void decode(cmd_line *line);
		int if_control(int pc);
		void id_control(int buf);

		cmd_line cmd_lines[32];
		int num_line;
		int registers[32];
		cmd_line reg_buff[7];//0=if 1=id 2=ex1 3=ex2 4=ex3 5=mem 6=wb
		//cmd_line * IF, * ID, * EX1, * EX2, * EX3, * MEM, * WB;
};
#endif