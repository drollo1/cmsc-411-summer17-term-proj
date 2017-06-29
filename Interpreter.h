#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctype.h>
#include <algorithm>

using namespace std;

class Interpreter{
struct cmd_line{
	string header, instruction, operation;
	int line_numb;
	string ops[5];  //0=reg1 1=reg2 2=reg3 3= imediate 4=header
};

//cmd_line* program_code = new cmd_line[32];
	public:
		Interpreter();
		Interpreter(string cmd_name, string data_name);
		void parse_instructions(string cmd_name);
		void build_data(string data_name);
		void print_code();
		void run();
		int pop_instruction_cache(int pc, int cycle);
		int buff_move(int pos);
		int isDone();
		int got_instruction(int pc);
		void decode(cmd_line *line);
		int if_control(int pc, int cycle);
		void id_control(int cycle);
		void ex1_control(int cycle);
		void ex2_control(int cycle);
		void ex3_control(int cycle);
		void mem_control(int cycle);
		void wb_control(int cycle);
		void clear_control(int cycle);

		cmd_line cmd_lines[32];
		int num_line;
		int registers[32];
		string clock_cycles[32];
		cmd_line reg_buff[7];//0=if 1=id 2=ex1 3=ex2 4=ex3 5=mem 6=wb
		string memory_data[32];
		cmd_line i_memory[2][8];
		int i_cache_acess;
		int i_cache_misses;
		int d_cache_acess;
		int d_cache_misses;
};
#endif