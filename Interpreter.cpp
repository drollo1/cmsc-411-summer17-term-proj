#include "Interpreter.h"

Interpreter::Interpreter(){

}
Interpreter::Interpreter(string cmd_name, string data_name ){
	num_line=0;
	string line;
	ifstream cmd_file;
	cmd_file.open(cmd_name.c_str());

	while(getline(cmd_file, line)){

		string token="";
		int inst_token=0;
		int op_token=0;
		for(int i=0;i<line.length()&&!op_token;i++){
			if (line[i]!=' '){
				token+=line[i];
				if (line[i]==':'){
					cmd_lines[num_line].header=token;
					token="";
				}
				if(inst_token){
					cmd_lines[num_line].operation=line.substr(i,line.length());
					op_token=1;
				}
			}
			else if(line[i-1]!=' '){
				cmd_lines[num_line].instruction=token;
				num_line ++;
				cmd_lines[num_line].line_numb=num_line;
				inst_token =1;
				token="";
			}
		}
	}

	for(int i=0; i< num_line;i++)
		cout<<cmd_lines[i].header<<"  instruction: "<<cmd_lines[i].instruction<<"  operation: "<<cmd_lines[i].operation<<"\n";
} 
