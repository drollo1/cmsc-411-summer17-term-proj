#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctype.h>
#include <algorithm>
#include <math.h>

using namespace std;

class Interpreter{
struct cmd_line{
	string header, instruction, operation;
	int line_numb;
	string ops[5];  //0=reg1 1=reg2 2=reg3 3=imediate 4=header
	int proc_done;
	int solution;
};

struct r_register{
	string name;
	int value;
};

struct data_memory{
	int value;
	int address;
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
		int pop_data_cache();
		int buff_move(int pos);
		int pipe_line(int pc, int cycle, int pop_cycles);
		int isDone();
		int reg_empty();
		int got_instruction(int pc);
		int got_data(cmd_line cmd);
		void decode(cmd_line *line);
		int if_control(int pc, int cycle);
		void id_control(int cycle);
		int ex1_control(int cycle, int pc);
		void ex2_control(int cycle);
		void ex3_control(int cycle);
		void mem_control(int cycle);
		void wb_control(int cycle);
		void clear_control(int cycle);
		int arth_forward();
		int branch_forward();
		void reg_store(string reg_name, int value);
		void setup_registers();
		int compare_registers();
		int branch(string header);
		int to_decimal(string binary);
		int arth(string reg1, string reg2, string instruction);

		cmd_line cmd_lines[32];
		int num_line;
		string clock_cycles[32];
		cmd_line reg_buff[7];//0=if 1=id 2=ex1 3=ex2 4=ex3 5=mem 6=wb
		int memory_data[32];
		cmd_line i_memory[2][8];
		data_memory d_memory[4][4];
		r_register registers[32];
		int i_cache_acess;
		int i_cache_misses;
		int d_cache_acess;
		int d_cache_misses;
		int pop_d_cycles;
		int bus_use;
};
#endif