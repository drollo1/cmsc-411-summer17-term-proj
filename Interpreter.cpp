#include "Interpreter.h"

Interpreter::Interpreter(){
}

Interpreter::Interpreter(string cmd_name, string data_name ){
	
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
	IF =&cmd_lines[0];
	while(IF->get_number()!=num_line){
		int temp=IF->get_number();
		cout << temp << "\n";
		IF=&cmd_lines[temp];


	}
}