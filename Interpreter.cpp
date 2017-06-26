#include "Interpreter.h"

Interpreter::Interpreter(){
}

Interpreter::Interpreter(string cmd_name, string data_name ){
	
	for(int i=0;i<7;i++)
		reg_buff[i].line_numb=00;

	parse_instructions(cmd_name);
	run();
	print_code();
} 

void Interpreter::parse_instructions(string cmd_name){

	num_line=0;
	string line;
	ifstream cmd_file;
	cmd_file.open(cmd_name.c_str());
	while(getline(cmd_file, line)){
		string token="";
		int inst_token=0;
		for(int i=0;i<line.length();i++){
			if (line[i]!=' '&&line[i]!='	'){
				token+=line[i];
				if (line[i]==':'){
					cmd_lines[num_line].header=token;
					token="";
				}
				else if((line[i+1]==' '||line[i+1]=='	'||line[i+1]==00)&&!inst_token){
					cmd_lines[num_line].instruction=token;
					cmd_lines[num_line].line_numb=num_line;
					inst_token =1;
					token="";
					num_line++;
					cmd_lines[num_line-1].line_numb = num_line;
				}
				else if(inst_token&&line[i]!=00){
					cmd_lines[num_line-1].operation=line.substr(i,line.length());
					i=line.length()-1;
					inst_token=0;
				}
			}			
			
		}
	}
}

void Interpreter::print_code(){
	for(int i=0; i< num_line;i++){
		cout<<cmd_lines[i].line_numb <<"# ";
		cout<<cmd_lines[i].header<<"  instruction: ";
		cout<<cmd_lines[i].instruction<<"  operation: ";
		cout<<cmd_lines[i].operation<<"\n";
	}
}

void Interpreter::run(){
	int pc=0;
	reg_buff[0]=cmd_lines[0];
	while(isDone()){
		buff_control(7);
		buff_control(6);
		buff_control(5);
		buff_control(4);
		buff_control(3);
		buff_control(2);
		buff_control(1);
		pc++;
		reg_buff[0]=cmd_lines[pc];
	}	
}

int Interpreter::isDone(){
	int result =0;
	for(int i=0;i<7;i++){
		cout<<"buff: " << reg_buff[i].line_numb <<"  ";
		if(reg_buff[i].line_numb!=00)
			result++;
	}
	cout <<"\n\n";
	return result;
}

int Interpreter::buff_control(int pos){
	if(pos==7)
		reg_buff[6].line_numb=00;
	if(reg_buff[pos].line_numb==00&&reg_buff[pos-1].line_numb!=00){
		reg_buff[pos]=reg_buff[pos-1];
		reg_buff[pos-1].line_numb=00;
		return 1;
	}
	return 0;
}

void Interpreter::decode(cmd_line *line){

	if(line->instruction=="LI"){

	}
}