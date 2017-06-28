#include "Interpreter.h"

Interpreter::Interpreter(){
}

Interpreter::Interpreter(string cmd_name, string data_name ){
	
	for(int i=0;i<7;i++)
		reg_buff[i].line_numb=00;

	parse_instructions(cmd_name);
	//build_data(data_name);
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
	cmd_file.close();
}

void Interpreter::build_data(string data_name){
	int temp=0;
	string line;
	ifstream data_file;
	data_file.open(data_name.c_str());
	while(getline(data_file, line)){
		memory_data[temp]=line;
		temp++;
	}
}

void Interpreter::print_code(){
	for(int i=0; i< num_line;i++){
		cout<<cmd_lines[i].line_numb <<"# ";
		if(cmd_lines[i].header=="")
			cout<<"\t\t";
		else
			cout<<cmd_lines[i].header<<"\t";
		cout<<cmd_lines[i].instruction<<"\t";
		if(cmd_lines[i].instruction=="HLT")
			cout<<"\t\t";
		else
			cout<<cmd_lines[i].operation<<"   \t";
		cout<<clock_cycles[i]<<"\n";
	}
}

void Interpreter::run(){
	int pc=0;
	int cycle=0;
	while(pc==0||isDone()){
		clear_control(cycle);
		wb_control(cycle);
		mem_control(cycle);
		ex3_control(cycle);
		ex2_control(cycle);
		ex1_control(cycle);
		id_control(cycle);
		pc=if_control(pc, cycle);
		cycle++;
	}	
}

int Interpreter::isDone(){
	int result =0;
	for(int i=0;i<7;i++){
		//cout<<"buff: " << reg_buff[i].line_numb <<"  ";
		if(reg_buff[i].line_numb!=00)
			result++;
	}
	//cout <<"\n\n";
	return result;
}

int Interpreter::buff_move(int pos){
	if(pos==7){
		reg_buff[6].line_numb=00;
		return 1;
	}
	if(reg_buff[pos].line_numb==00&&reg_buff[pos-1].line_numb!=00){
		reg_buff[pos]=reg_buff[pos-1];
		reg_buff[pos-1].line_numb=00;
		return 1;
	}
	return 0;
}

void Interpreter::decode(cmd_line *line){

	if(line->instruction=="LI"){
		line->ops[0]=line->operation.substr(0,2);
		line->ops[3]=line->operation.substr(4);
		//cout<<line->instruction<<" "<<line->ops[0]<<" "<<line->ops[3]<<"\n";
	}else if(line->instruction=="ADDI"||line->instruction=="SUBI"||line->instruction=="ANDI"||line->instruction=="ORI"||line->instruction=="BEQ"||line->instruction=="BNE"||line->instruction=="MULTI"){
		line->ops[0]=line->operation.substr(0,2);
		line->ops[1]=line->operation.substr(4,2);
		line->ops[3]=line->operation.substr(8);
		//cout<<line->instruction<<" "<<line->ops[0]<<" "<<line->ops[1]<<" "<<line->ops[3]<<"\n";
	}else if(line->instruction=="LW"||line->instruction=="SW"){
		line->ops[0]=line->operation.substr(0,2);
		int temp=line->operation.find("(");
		line->ops[3]=line->operation.substr(4,temp-4);
		line->ops[1]=line->operation.substr(temp+1,2);
		//cout<<line->instruction<<" "<<line->ops[0]<<" "<<line->ops[3]<<" "<<line->ops[1]<<"\n";
	}else if(line->instruction=="ADD"||line->instruction=="SUB"||line->instruction=="OR"||line->instruction=="AND"||line->instruction=="MULT"){
		line->ops[0]=line->operation.substr(0,2);
		line->ops[1]=line->operation.substr(4,2);
		line->ops[2]=line->operation.substr(8);
		//cout<<line->instruction<<" "<<line->ops[0]<<" "<<line->ops[1]<<" "<<line->ops[2]<<"\n";
	}else if(line->instruction=="J"){
		line->ops[4]=line->operation;
		//cout<<line->instruction<<" "<<line->ops[4]<<"\n";
	}
}

int Interpreter::if_control(int pc, int cycle){
	if(reg_buff[0].line_numb==00){
		reg_buff[0]=cmd_lines[pc];
		pc++;
	}
	return pc;
}

void Interpreter::id_control(int cycle){
	if(buff_move(1)){
		decode(&reg_buff[1]);
		stringstream ss;
		ss << cycle;
		clock_cycles[reg_buff[1].line_numb-1]+=ss.str();
		clock_cycles[reg_buff[1].line_numb-1]+="\t";
	}
}

void Interpreter::ex1_control(int cycle){
	if(buff_move(2)){

	}
}

void Interpreter::ex2_control(int cycle){
	if(buff_move(3)){

	}
}

void Interpreter::ex3_control(int cycle){
	if(buff_move(4)){
		stringstream ss;
		ss << cycle;
		clock_cycles[reg_buff[4].line_numb-1]+=ss.str();
		clock_cycles[reg_buff[4].line_numb-1]+="\t";	
	}
}

void Interpreter::mem_control(int cycle){
	if(buff_move(5)){
		stringstream ss;
		ss << cycle;
		clock_cycles[reg_buff[5].line_numb-1]+=ss.str();
		clock_cycles[reg_buff[5].line_numb-1]+="\t";
	}
}

void Interpreter::wb_control(int cycle){
	if(buff_move(6)){
		stringstream ss;
		ss << cycle;
		clock_cycles[reg_buff[6].line_numb-1]+=ss.str();
		clock_cycles[reg_buff[6].line_numb-1]+="\t";
	}
}

void Interpreter::clear_control(int cycle){
	if(reg_buff[6].line_numb!=00){
		stringstream ss;
		ss << cycle;
		clock_cycles[reg_buff[6].line_numb-1]+=ss.str();
		clock_cycles[reg_buff[6].line_numb-1]+="\t";
		reg_buff[6].line_numb=00;
	}
}