#include "Interpreter.h"

Interpreter::Interpreter(){
}

Interpreter::Interpreter(string cmd_name, string data_name ){
	
	i_cache_acess = i_cache_misses = d_cache_acess = d_cache_misses = 0;

	for(int i=0;i<7;i++)
		reg_buff[i].line_numb=00;
	for(int i=0;i<2;i++)
		for(int j=0;j<8;j++)
			i_memory[i][j].line_numb=00;

	setup_registers();

	parse_instructions(cmd_name);
	build_data(data_name);
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
		memory_data[temp]=to_decimal(line);
		temp++;
	}
	data_file.close();
}

int Interpreter::to_decimal(string binary){
	int result=0;
	for(int i=31;i>0;i--){
		if(binary[i]=='1')
			result+=pow(2, (31-i));
	}
	return result;
}

void Interpreter::setup_registers(){
	string temp;
	for(int i=0;i<32;i++){
		temp="R";
		stringstream ss;
		ss << i;
		temp+=ss.str();
		registers[i].name=temp;
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

	cout<<"\nTotal number of access requests for instruction cache: ";
	cout<<i_cache_acess<<"\n";
	cout<<"Number of instruction cache hits: ";
	cout<<i_cache_acess - i_cache_misses<<"\n\n";

	cout<<"Total number of access requests for data cache: ";
	cout<<d_cache_acess<<"\n";
	cout<<"Number of data cache hits: ";
	cout<<d_cache_acess - d_cache_misses<<"\n\n";
}

void Interpreter::run(){
	bus_use=0;
	int pc=0;
	int cycle=0;
	int pop_cycles=-1;
	pop_d_cycles=-1;
	int count=0;
	while((pc==0||isDone())&&count<1000){
		count++;
		cycle++;
		if(!got_instruction(pc)&&pop_cycles<0&&bus_use==0){
			pop_cycles=23;
			bus_use=1;
		}
		clear_control(cycle);
		wb_control(cycle);
		mem_control(cycle);
		ex3_control(cycle);
		ex2_control(cycle);
		pc=ex1_control(cycle, pc);
		id_control(cycle);
		pc=if_control(pc, cycle);
		if(pop_cycles==0){
			pop_instruction_cache(pc, cycle);
			pop_cycles=-1;
			bus_use=0;
		}
		else if(pop_cycles>0)
			pop_cycles--;

		if(pop_d_cycles==0){
			pop_data_cache();
			pop_d_cycles=-1;
			bus_use=0;
		}
		else if(pop_d_cycles>0)
			pop_d_cycles--;

		/*cout<<"cycle "<<cycle<<" pc "<<pc<<":  ";
		for(int i=0;i<7;i++){
			if(reg_buff[i].line_numb!=0)
				cout<<reg_buff[i].instruction<<"\t";
			else
				cout<<"\t";
		}
		cout<<endl;*/
		//cout<<"pop_cycles: "<<pop_cycles<<" pop_d_cycles: "<<pop_d_cycles<<"\n\n";
	}
	reg_buff[0].line_numb=0;
	reg_buff[1].line_numb=0;
	reg_buff[2].line_numb=0;
	while(reg_empty()){
		cycle++;
		clear_control(cycle);
		wb_control(cycle);
		mem_control(cycle);
		ex3_control(cycle);
		ex2_control(cycle);
		ex1_control(cycle, pc);
	}

}
int Interpreter::isDone(){
	int result =0;
	if(reg_buff[1].instruction.compare("HLT"))
		result=1;
	return result;
}

int Interpreter::got_instruction(int pc){
	int block = (pc/8)%2;
	int word = pc%8;
	if(i_memory[block][word].line_numb==cmd_lines[pc].line_numb)
		return 1;
	else
		return 0;
}

int Interpreter::got_data(cmd_line cmd){
	if(cmd.instruction.compare("LW")==0){
		int temp = cmd.solution-256;
		temp = temp/4;
		int block=(temp/4)%4;
		int word=temp%4;
		if(d_memory[block][word].address==temp)
			return 1;
		else if(pop_d_cycles<0&&bus_use==0){
			pop_d_cycles=12;
			return 0;
		}
		else
			return 0;
	}
	else
		return 1;
}

int Interpreter::pop_instruction_cache(int pc, int cycle){
	int block = (pc/8)%2;
	int word= (pc/8);
	word = word*8;
	i_cache_misses++;
	//cout<<"Populating instructions "<<cycle<<endl;
	for(int i=0;i<8;i++)
		i_memory[block][i]=cmd_lines[word+i];

	return cycle;
}

int Interpreter::pop_data_cache(){
	int temp = reg_buff[5].solution-256;
	temp = temp/4;
	int block=(temp/4)%4;
	int word=temp/4;
	word=word*4;
	d_cache_misses++;
	//cout<<"Populating data cache for "<<reg_buff[4].instruction<<endl;
	for(int i=0;i<4;i++){
		d_memory[block][i].value=memory_data[word+i];
		d_memory[block][i].address=(word+i);
	}
}

int Interpreter::buff_move(int pos){
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
	}else if(line->instruction=="ADDI"||line->instruction=="SUBI"||line->instruction=="ANDI"||line->instruction=="ORI"||line->instruction=="MULTI"){
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
	else if(line->instruction=="BEQ"||line->instruction=="BNE"){
		line->ops[0]=line->operation.substr(0,2);
		line->ops[1]=line->operation.substr(4,2);
		line->ops[4]=line->operation.substr(8);
	}
}

int Interpreter::if_control(int pc, int cycle){
	int block= (pc/8)%2;
	int word= pc%8;
	if(reg_buff[1].line_numb<0){
		reg_buff[1].line_numb=00;
	}
	else if(reg_buff[0].line_numb==00&&i_memory[block][word].line_numb==(pc+1)){
		int block = (pc/8)%2;
		int word = pc%8;
		reg_buff[0]=i_memory[block][word];
		reg_buff[0].proc_done=0;
		pc++;
		i_cache_acess++;
	}
	return pc;
}

void Interpreter::id_control(int cycle){
	if(buff_move(1)){
		decode(&reg_buff[1]);
		cycle--;
		stringstream ss;
		ss << cycle;
		clock_cycles[reg_buff[1].line_numb-1]+=ss.str();
		clock_cycles[reg_buff[1].line_numb-1]+="\t";
	}
}

int Interpreter::ex1_control(int cycle, int pc){
	
	if((reg_buff[1].instruction.compare("BNE")==0||reg_buff[1].instruction.compare("BEQ")==0)&&reg_buff[1].line_numb!=00){
		if(branch_forward()){
			//cycle--;
			stringstream ss;
			ss << cycle;
			clock_cycles[reg_buff[1].line_numb-1]+=ss.str();
			clock_cycles[reg_buff[1].line_numb-1]+="\t";	
			clock_cycles[reg_buff[0].line_numb-1]+=ss.str();
			clock_cycles[reg_buff[0].line_numb-1]+="\t";	

			int result=compare_registers();
			if(result==0&&reg_buff[1].instruction.compare("BEQ"))
				pc = branch(reg_buff[1].ops[4]);
			if(result!=0&&reg_buff[1].instruction.compare("BNE"))
				pc = branch(reg_buff[1].ops[4]);

			reg_buff[1].line_numb=00;
		}
	}
	else if(reg_buff[1].instruction.compare("J")==0&&reg_buff[1].line_numb!=00){
		stringstream ss;
		ss << cycle;
		clock_cycles[reg_buff[1].line_numb-1]+=ss.str();
		clock_cycles[reg_buff[1].line_numb-1]+="\t";	
		clock_cycles[reg_buff[0].line_numb-1]+=ss.str();
		clock_cycles[reg_buff[0].line_numb-1]+="\t";	
		pc = branch(reg_buff[1].ops[4]);

		reg_buff[1].line_numb=00;
	}
	else if(arth_forward()&&buff_move(2)){
		if(reg_buff[2].instruction.compare("LI")==0){
			reg_buff[2].proc_done=1;
			reg_buff[2].solution=atoi(reg_buff[2].ops[3].c_str());
			//int temp=atoi(reg_buff[2].ops[3].c_str());
			//reg_store(reg_buff[2].ops[0], temp);
		}
		if(reg_buff[2].instruction.compare("SW")==0)
			reg_buff[2].proc_done=1;
		cycle--;
		stringstream ss;
		ss << cycle;
		clock_cycles[reg_buff[2].line_numb-1]+=ss.str();
		clock_cycles[reg_buff[2].line_numb-1]+="\t";	
	}
	return pc;
}

void Interpreter::ex2_control(int cycle){
	if(buff_move(3)){
		if(reg_buff[3].instruction.compare("AND")==0||reg_buff[3].instruction.compare("OR")==0){
			//cout<<reg_buff[3].instruction<<" ex2 "<<cycle<<endl;
			reg_buff[3].proc_done=1;
			//reg_buff[3].solution=arth(reg_buff[3].ops[1], reg_buff[3].ops[2], reg_buff[3].instruction);
		}
		if(reg_buff[3].instruction.compare("ANDI")==0||reg_buff[3].instruction.compare("ORI")==0){
			//cout<<reg_buff[3].instruction<<" completed "<<cycle<<endl;
			reg_buff[3].proc_done=1;
			//reg_buff[3].solution=arth(reg_buff[3].ops[1], reg_buff[3].ops[3], reg_buff[3].instruction);
		}
		if(reg_buff[3].instruction.compare("LW")==0||reg_buff[3].instruction.compare("SW")==0){
			//cout<<reg_buff[3].instruction<<" ex2 "<<cycle<<endl;
			reg_buff[3].solution=arth(reg_buff[3].ops[1], reg_buff[3].ops[2], reg_buff[3].instruction);	
		}
	}
}

void Interpreter::ex3_control(int cycle){
	if(buff_move(4)){
		if(reg_buff[4].instruction.compare("ADDI")==0||reg_buff[4].instruction.compare("SUBI")==0){
			//cout<<reg_buff[4].instruction<<" ex3 "<<cycle<<endl;
			reg_buff[4].proc_done=1;
			reg_buff[4].solution=arth(reg_buff[4].ops[1], reg_buff[4].ops[3], reg_buff[4].instruction);
		}
		if(reg_buff[4].instruction.compare("ADD")==0||reg_buff[4].instruction.compare("SUB")==0){
			//cout<<reg_buff[4].instruction<<" ex3 "<<cycle<<endl;
			reg_buff[4].proc_done=1;
			reg_buff[4].solution=arth(reg_buff[4].ops[1], reg_buff[4].ops[2], reg_buff[4].instruction);
		}
	}
}

void Interpreter::mem_control(int cycle){
	if(buff_move(5)){
		if(reg_buff[5].instruction.compare("MULT")==0){
			//cout<<reg_buff[5].instruction<<" "<<cycle<<endl;
			reg_buff[5].proc_done=1;
			reg_buff[5].solution=arth(reg_buff[5].ops[1], reg_buff[5].ops[2], reg_buff[5].instruction);
		}
		if(reg_buff[5].instruction.compare("MULTI")==0){
			//cout<<reg_buff[5].instruction<<" "<<cycle<<endl;
			reg_buff[5].proc_done=1;
			reg_buff[5].solution=arth(reg_buff[5].ops[1], reg_buff[5].ops[3], reg_buff[5].instruction);
		}
		cycle--;
		stringstream ss;
		ss << cycle;
		clock_cycles[reg_buff[5].line_numb-1]+=ss.str();
		clock_cycles[reg_buff[5].line_numb-1]+="\t";
	}
}

void Interpreter::wb_control(int cycle){
	if(got_data(reg_buff[5])&&buff_move(6)){
		if(reg_buff[6].instruction.compare("LW")==0){
			//cout<<reg_buff[6].instruction<<" wb:completed "<<cycle+1<<endl;
			reg_buff[6].proc_done=1;
			d_cache_acess++;
		}
		cycle--;
		stringstream ss;
		ss << cycle;
		clock_cycles[reg_buff[6].line_numb-1]+=ss.str();
		clock_cycles[reg_buff[6].line_numb-1]+="\t";		
	}
	
}

void Interpreter::clear_control(int cycle){
	if(reg_buff[6].line_numb!=00){		
		if(reg_buff[6].instruction.compare("SW")==0)
			cout<<"";
		else
			reg_store(reg_buff[6].ops[0], reg_buff[6].solution);

		cycle--;
		stringstream ss;
		ss << cycle;
		clock_cycles[reg_buff[6].line_numb-1]+=ss.str();
		clock_cycles[reg_buff[6].line_numb-1]+="\t";
		reg_buff[6].line_numb=00;
	}
}

int Interpreter::reg_empty(){
	int result = 0;
	for(int i=1;i<7;i++)
		if(reg_buff[i].line_numb!=0)
			result++;
	return result;
}

int Interpreter::arth_forward(){
	
	if(reg_buff[1].instruction.compare("ADDI")==0||reg_buff[1].instruction.compare("SUBI")==0||
		reg_buff[1].instruction.compare("MULTI")==0||reg_buff[1].instruction.compare("ANDI")==0||
		reg_buff[1].instruction.compare("ORI")==0||reg_buff[1].instruction.compare("LW")==0||
		reg_buff[1].instruction.compare("SW")==0){
		for(int i=2;i<7;i++){
			if(reg_buff[1].ops[1]==reg_buff[i].ops[0]&&reg_buff[i].proc_done==0&&reg_buff[i].line_numb>0)
				return 0;
		}
	}
	else if(reg_buff[1].instruction.compare("ADD")==0||reg_buff[1].instruction.compare("SUB")==0||
		reg_buff[1].instruction.compare("MULT")==0||reg_buff[1].instruction.compare("AND")==0||
		reg_buff[1].instruction.compare("OR")==0){
		for(int i=2;i<7;i++){
			if((reg_buff[1].ops[1]==reg_buff[i].ops[0]||reg_buff[1].ops[2]==reg_buff[i].ops[0])&&reg_buff[i].proc_done==0&&reg_buff[i].line_numb>0){
				//cout<<reg_buff[1].instruction<<" is not ready because of "<<reg_buff[i].instruction<<endl;
				return 0;
			}
		}
	}
	return 1;
}

int Interpreter::branch_forward(){
	for(int i=2;i<7;i++){
		if((reg_buff[1].ops[0]==reg_buff[i].ops[0]||reg_buff[1].ops[1]==reg_buff[i].ops[0])&&
			reg_buff[i].proc_done==0&&reg_buff[i].line_numb>0){
			return 0;
		}
	}
	return 1;
}

void Interpreter::reg_store(string reg_name, int value){
	for(int i=0;i<32;i++){
		if(registers[i].name.compare(reg_name)==0){
			registers[i].value=value;
			//cout<<registers[i].name<<" has the value "<<registers[i].value<<endl;
		}
	}
}

int Interpreter::compare_registers(){
	int temp1, temp2;
	for(int i=0;i<32;i++){
		if(reg_buff[1].ops[0]==registers[i].name)
			temp1=registers[i].value;
		if(reg_buff[1].ops[1]==registers[i].name)
			temp2=registers[i].value;
	}
	return temp1-temp2;
}

int Interpreter::branch(string header){
	header+=":";
	//cout<<"\t\tSearching for "<<header<<endl;
	int result=0;
	for(int i=0;i<num_line;i++){
		//cout<<"\t\t"<<cmd_lines[i].line_numb-1<<" "<<cmd_lines[i].header<<endl;
		if(cmd_lines[i].header.compare(header)==0)
			result=cmd_lines[i].line_numb-1;
	}
	reg_buff[0].line_numb=-99;
	//cout<<"\t\tbranch to "<<result<<endl;
	return result;
}

int Interpreter::arth(string reg1, string reg2, string instruction){
	int value1, value2=0;
	for(int i=0;i<32;i++){
		if(registers[i].name.compare(reg1)==0){
			registers[i].value=value1;
			//cout<<registers[i].name<<" has the value "<<registers[i].value<<endl;
		}
	}
	if(reg2[0]=='R')
		for(int i=0;i<32;i++){
			if(registers[i].name.compare(reg2)==0){
				registers[i].value=value2;
			//cout<<registers[i].name<<" has the value "<<registers[i].value<<endl;
			}
		}
	else{
		value2=atoi(reg2.c_str());
	}

	if(instruction=="ADD"||instruction=="ADDI"||instruction=="LW"||instruction=="SW")
		return value1+value2;
	else if(instruction=="MULT"||instruction=="MULTI")
		return value1*value2;
	else if(instruction=="SUB"||instruction=="SUBI")
		return value1-value2;

}