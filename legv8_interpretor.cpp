#include <iostream>
#include <algorithm>
#include <string>
#include <deque>
#include <map>
#include <cctype>
#include <fstream>

using namespace std;

enum sepcial_registers {sp = 28, fp, lr, xzr};
map<string, long long int> gotos;
class Registers
{

	public:
		long long int registers[32];
		Registers()
		{
			memset(registers, 0, sizeof(long long int) * 32);

		}

		void setRegister(int register_name, long long int new_value)
		{
			// if not xzr then modify register value
			if(register_name != 31)
			{
				registers[register_name] = new_value;
			}
		}

};
Registers register_block;
bool zero_flag = 0;  // Z
bool negative_flag = 0; // N
bool overflow_flag = 0; // V
bool carry_flag = 0; /// C
// need something for the STK memory
// is it just this?
//long long int stack[255];
deque<string> stack; // each string will have 8 characters in it, 1 byte
//deque<long long int> stack;
int program_counter = 0;

void printContents()
{
	// print out flags
	cout << "\nflags\n";
	cout << "zero flag\n" << zero_flag << endl;
	cout << "negative flag\n" << negative_flag << endl;
	cout << "overflow flag\n" << overflow_flag << endl;
	cout << "carry flag\n" << carry_flag << endl;

	/*
  regs.insert(pair<string, long long int>("X28", 28));
  regs.insert(pair<string, long long int>("SP",  28));
  regs.insert(pair<string, long long int>("X29", 29));
  regs.insert(pair<string, long long int>("FP",  29));
  regs.insert(pair<string, long long int>("X30", 30));
  regs.insert(pair<string, long long int>("LR",  30));
  regs.insert(pair<string, long long int>("XZR", 31));
  regs.insert(pair<string, long long int>("X31", 31));
	*/
	cout << endl;
	cout << "registers\n";
	for(int i = 0; i < 32; i++)
	{
		cout << i << " ";
		if(i == 28)
		{
			cout << "SP";
		}
		else if(i == 29)
		{
			cout << "FP";
		}
		else if(i == 30)
		{
			cout << "LR";
		}
		else if(i == 31)
		{
			cout << "XZR";
		}
		cout << " " << register_block.registers[i] << endl;
	}
	cout << endl;
	cout << "stack\n";
	for(int i = 0; i < stack.size(); i++)
	{
		
		cout << stack[i] << " " << (8 * i) << " " << i << endl;
	}
	cout << endl;
	cout << "program counter\n";
	cout << program_counter << endl;

	cout << endl;
	cout << "goto table\n";
	for(map<string, long long int>::iterator key = gotos.begin(); key != gotos.end(); key++)
	{
		cout << key->first << " -> " << key->second << endl;
	}
	//map<string, long long int> gotos;


}

void writeContents()
{
	ofstream output_file;
  	output_file.open ("ravi.txt");
	// print out flags
	output_file << "\nflags\n";
	output_file << "zero flag\n" << zero_flag << "\n";
	output_file << "negative flag\n" << negative_flag << "\n";
	output_file << "overflow flag\n" << overflow_flag << "\n";
	output_file << "carry flag\n" << carry_flag << "\n";

	/*
  regs.insert(pair<string, long long int>("X28", 28));
  regs.insert(pair<string, long long int>("SP",  28));
  regs.insert(pair<string, long long int>("X29", 29));
  regs.insert(pair<string, long long int>("FP",  29));
  regs.insert(pair<string, long long int>("X30", 30));
  regs.insert(pair<string, long long int>("LR",  30));
  regs.insert(pair<string, long long int>("XZR", 31));
  regs.insert(pair<string, long long int>("X31", 31));
	*/
	output_file << "\n";
	output_file << "registers\n";
	for(int i = 0; i < 32; i++)
	{
		output_file << i << " ";
		if(i == 28)
		{
			output_file << "SP";
		}
		else if(i == 29)
		{
			output_file << "FP";
		}
		else if(i == 30)
		{
			output_file << "LR";
		}
		else if(i == 31)
		{
			output_file << "XZR";
		}
		output_file << " " << register_block.registers[i] << "\n";
	}
	output_file << "\n";
	output_file << "stack\n";
	for(int i = 0; i < stack.size(); i++)
	{
		
		output_file << stack[i] << " " << (8 * i) << " " << i << "\n";
	}
	output_file << "\n";
	output_file << "program counter\n";
	output_file << program_counter << "\n";

	output_file << "\n";
	output_file << "goto table\n";
	for(map<string, long long int>::iterator key = gotos.begin(); key != gotos.end(); key++)
	{
		output_file << key->first << " -> " << key->second << "\n";
	}
	//map<string, long long int> gotos;


}

void setOverFlowFlag(long long int X, long long int Y, long long int Result)
{
	// IfXandY have opposite signs then V = 0. If they have the same sign, and if the result has sign different from the operands, then V = 1 else V = 0.
	if(((X > 0 && Y < 0) ||
	    (X < 0 && Y > 0))
	   )
	{
		overflow_flag = 0;
	}
	else if(X > 0 && Y > 0)
	{
		if(Result < 0)
		{
			overflow_flag = 1;
		}
	}
	else if (X < 0 && Y < 0)
	{
		if(Result > 0)
		{
			overflow_flag = 1;
		}
	}
	else
	{
		overflow_flag = 0;
	}
}
void setCarryFlag(long long int X, long long int Y, long long int Z)
{
	// To determine if there is a carry, create unsigned long long int X1, Y1 and Z1. Copy X to X1, Y to Y1, and set X1 = Y1 + Z1. If X1 < Y1 or X1 < Z1, then C = 1, else C = 0.
	long long int X1 = X;
	long long int Y1 = Y;

	// setting Z1 to Z was not mentioned in the comment above, but nothing about how Z1 was set was present.
	long long int Z1 = Z;
	X1 = Y1 + Z1;
	if(X1 < Y1 || X1 < Z1)
	{
		carry_flag = 1;
	}
	else
	{
		carry_flag = 0;
	}

}



string toLower(string s)
{
	string new_string;
	for (int i = 0; i < s.size(); ++i)
	{
	      new_string += tolower(s[i]);
   	}
   	return new_string;
}

/*
int getRegisterIdOrIntermediateValue(string register_name)

	
	this will return an integer between 0 and 32

	if register_name == "sp"
		return 31
	if the register name says like "x12" then you will return 12
	if the register name says like "x22" then you will return 22


	if the register name says like "xzr" then you will return 32
	if the register name says like "lr" then you will return 30   (link register)


	"x12" is encoded in ascii so you will have to convert the ascii value of "1", and "2" to 1 and 2
	"x12" => {120, 49, 50}

	have it also get intermediate values like "#234562" -> 234562
	
#include "iostream"
#include <typeinfo>

std::string temp = "12345";

int main() {
  
std::cout << typeid(temp).name() << std::endl;
  
int n = std::stoi(temp);

std::cout << temp << " --->" << n << std::endl;


  return 0;
}

*/

/*

assumes you already converted from ascii
the array contains only integers
take an array [2, 3, 4, 5, 6, 2] and return 234562
iterations of algorithm

start with 2
20 + 3 = 23
230 + 4 = 234


you can do this directly from the string by taking each ascii char and subtracting 48 from it(I think the value is 48)
no need to use an integer array




registers[getRegisterIdOrIntermediateValue("sp")]

*/


/*

subi sp, sp, #number will need to record the distance from the sp to the new sp
so b lr can go back to sp's original location in stack
*/



//Sou's Function
int getRegisterValue(string register_name)
{
  
  if (register_name.length() == 0)
  {
  		// so there is no out of bounds accessing
      	return 0;
  }
  
  map<string, int> regs;
  
  //registerName = "#3859352";
  //cout << "name_to_convert -> " << register_name << endl;
  if (register_name[0] == '#')
  {
  	  //cout << "convert to integer\n";

      std::string temp = register_name.substr(1, register_name.length() - 1 );
      //cout << std::stoi(temp) << endl;
      return std::stoi(temp);   
  }
  // so values > 32 don't become register indexes
  if(register_name[0] != 'X' &&
  	register_name[0] != 'x' &&
  	register_name[0] != 'S' &&
  	register_name[0] != 's'
  	)
  {
  	//cout << "convert to integer\n";
  	std::string::size_type sz;
  	//cout << register_name << endl;
  	return std::stoi(register_name, &sz);
  }
  // convert "x" to "X"
  //cout << register_name << endl;
  if(register_name == "sp")
  {
	register_name = "SP";
  }
  else if(register_name == "fp")
  {
  	register_name = "FP";
  }
  else if(register_name == "lr")
  {
  	register_name = "LR";
  }
  else if(register_name == "xzr")
  {
  	register_name = "XZR";
  }
  else
  {
  	register_name[0] = toupper(register_name[0]);

  }
  //cout << register_name << endl;

  regs.insert(pair<string, long long int>("X0",   0));
  regs.insert(pair<string, long long int>("X1",   1));
  regs.insert(pair<string, long long int>("X2",   2));
  regs.insert(pair<string, long long int>("X3",   3));
  regs.insert(pair<string, long long int>("X4",   4));
  regs.insert(pair<string, long long int>("X5",   5));
  regs.insert(pair<string, long long int>("X6",   6));
  regs.insert(pair<string, long long int>("X7",   7));
  regs.insert(pair<string, long long int>("X8",   8));
  regs.insert(pair<string, long long int>("X9",   9));
  regs.insert(pair<string, long long int>("X10", 10));
  regs.insert(pair<string, long long int>("X11", 11));
  regs.insert(pair<string, long long int>("X12", 12));
  regs.insert(pair<string, long long int>("X13", 13));
  regs.insert(pair<string, long long int>("X14", 14));
  regs.insert(pair<string, long long int>("X15", 15));
  regs.insert(pair<string, long long int>("X16", 16));
  regs.insert(pair<string, long long int>("X17", 17));
  regs.insert(pair<string, long long int>("X18", 18));
  regs.insert(pair<string, long long int>("X19", 19));
  regs.insert(pair<string, long long int>("X20", 20));
  regs.insert(pair<string, long long int>("X21", 21));
  regs.insert(pair<string, long long int>("X22", 22));
  regs.insert(pair<string, long long int>("X23", 23));
  regs.insert(pair<string, long long int>("X24", 24));
  regs.insert(pair<string, long long int>("X25", 25));
  regs.insert(pair<string, long long int>("X26", 26));
  regs.insert(pair<string, long long int>("X27", 27));
  regs.insert(pair<string, long long int>("X28", 28));
  regs.insert(pair<string, long long int>("SP",  28));
  regs.insert(pair<string, long long int>("X29", 29));
  regs.insert(pair<string, long long int>("FP",  29));
  regs.insert(pair<string, long long int>("X30", 30));
  regs.insert(pair<string, long long int>("LR",  30));
  regs.insert(pair<string, long long int>("XZR", 31));
  regs.insert(pair<string, long long int>("X31", 31));
  
  return regs.at(register_name);

}


void setFlags(long long int result, long long int X, long long int Y)
{

	if(result < 0)
	{
		negative_flag = 1;
	}
	if(result == 0)
	{
		zero_flag = 1;
	}
	setCarryFlag(X, Y, result);
	setOverFlowFlag(X, Y, result);
}
	


// push back the stack and stack pointer
void subi(string destination_register, string source_register, string immediate_value)
{
	long long int source 		= 	getRegisterValue(source_register);
	long long int destination 	= 	getRegisterValue(destination_register);
	long long int offset 		= 	getRegisterValue(immediate_value);
	//cout << "all converted\n";
	//cout << source_register << " " << destination_register << " " << immediate_value << endl;

	//cout << source << " " << destination << " " << offset << endl;
	// subi sp, sp, #alpha
	//cout << destination << endl;

	if(source == 28 && destination == 28)
	{
		for(int i = 0; i < offset; i++)
		{
			stack.push_back("00000000");
		}
//				void setRegister(int register_name, long long int new_value)
		register_block.setRegister(source, register_block.registers[source] + offset);


	}
	else
	{
		register_block.setRegister(destination, register_block.registers[source] - offset);
		//register_block.registers[destination] = register_block.registers[source] - offset;
		setFlags(register_block.registers[destination],
			     register_block.registers[source],
			     offset);
	}
	program_counter++;
}

// resets the stack pointer but leavs the stack untouched
void addi(string destination_register, string source_register, string immediate_value)
{
	long long int source 		= 	getRegisterValue(source_register);
	long long int destination 	= 	getRegisterValue(destination_register);
	long long int offset 		= 	getRegisterValue(immediate_value);

	//cout << destination << endl;

	// subi sp, sp, #alpha
	if(source == 28 && destination == 28)
	{
		register_block.setRegister(source, register_block.registers[source] - offset);
		//register_block.registers[source] -= offset;

	}
	else
	{
		register_block.setRegister(destination, register_block.registers[source] + offset);
		//register_block.registers[destination] = register_block.registers[source] + offset;
		setFlags(register_block.registers[destination],
			register_block.registers[source],
			offset);

	}
	program_counter++;

}
void add(string destination_register, string source_register1, string source_register2)
{
	long long int source1 		= 	getRegisterValue(source_register1);
	long long int destination 	= 	getRegisterValue(destination_register);
	long long int source2 		= 	getRegisterValue(source_register2);

	//cout << destination << endl;

	// subi sp, sp, #alpha
	if(source1 == 28 && destination == 28)
	{
		register_block.setRegister(source1, register_block.registers[source1] - source2);
		//register_block.registers[source1] -= source2;

	}
	else
	{
		register_block.setRegister(destination, register_block.registers[source1] + register_block.registers[source2]);
		//register_block.registers[destination] = register_block.registers[source1] + register_block.registers[source2];
		setFlags(register_block.registers[destination],
				 register_block.registers[source1],
				 source2);

	}
	program_counter++;

}

string convertToBinary(long long int number)
{
	string binary_string;
	long long int number_to_convert = number;
	while(number_to_convert > 0)
	{
		int binary_digit = number_to_convert % 2;
		if(binary_digit)
		{
			binary_string = "1" + binary_string;
		}
		else
		{
			binary_string = "0" + binary_string;
		}
		number_to_convert /= 2;
	}
	return binary_string;
}

// access the stack using stack pointer
void stur(string source_register, string memory_register, string offset)
{
	long long int source 			= getRegisterValue(source_register);
	long long int memory_register2 	= getRegisterValue(memory_register);
	long long int offset2 			= getRegisterValue(offset);

	//cout << "here\n";
	//cout << offset2 << endl;
	//int stack_location;
	if(memory_register2 == 28)
	{

		string value_in_binary = convertToBinary(register_block.registers[source]);
		//cout << "\n" << value_in_binary << endl;
		while(value_in_binary.size() < 64)
		{
			value_in_binary = "0" + value_in_binary;
		}
		// still having problems with this
		// not adjusting to where the stack
		// now it overwrites the stack data with clipped parts not getting all of the value to store
		//cout << "\n value in binary " << value_in_binary << endl;
		long long int bottom_sp = register_block.registers[28] - offset2 - 1;
		for(int i = 64, j = 0; (i - 8) > 0; i -= 8, j++)
		{
				//cout << (bottom_sp - j) << " "<< value_in_binary.substr(i - 8, 8) << endl;
				stack[bottom_sp - j] = value_in_binary.substr(i - 8, 8);

			
		}
		

	}
	program_counter++;
	

	

}
long long int convertBinaryStringToDecimal(string binary_string)
{
	long long int decimal_value = 0;
	for(int i = 0; i < binary_string.size(); i++)
	{
		decimal_value = (2 * decimal_value) + (binary_string[i] - 48);
	}
	return decimal_value;
}
int convertDecimalStringToDecimal(string decimal_string)
{
	int decimal_value = 0;
	for(int i = 0; i < decimal_string.size(); i++)
	{
		decimal_value = (10 * decimal_value) + (decimal_string[i] - 48);
	}
	return decimal_value;
}

void ldur(string source_register, string memory_register, string offset)
{
	int source 				= getRegisterValue(source_register);
	int memory_register2 	= getRegisterValue(memory_register);
	int offset2 			= getRegisterValue(offset);

	int stack_location;
	if(memory_register2 == 28)
	{
		string binary_value;
		long long int bottom_sp = register_block.registers[28] - offset2 - 1;
		for(int i = bottom_sp; i > bottom_sp - 8; i--)
		{
			binary_value = stack[i] + binary_value;
		}
		//cout << "binary value" << endl;
		//cout << binary_value << endl;
		//cout << binary_value.size() << endl;
		long long int decimal_value = convertBinaryStringToDecimal(binary_value);
		//cout << decimal_value << endl;
		register_block.setRegister(source, decimal_value);
		//register_block.registers[source] = decimal_value;
		// A = slot range [sp - 1, sp - 1 - 8]
		// convert A into decimal = B
		// store B into registers[source]
		//stack_location = register_block.registers[memory_register2] - offset2;
	}
	program_counter++;
}

// if each slot is 1 bit so 1 byte is a concatenation
// I'm going to assume each memory slot is 8 bytes
void bl(string label)
{
	register_block.registers[lr] = program_counter;
	int goto_location = gotos[label];
	program_counter = goto_location;
}

void br(string link_register)
{
	//cout << "new line to move to\n";
	//cout << (register_block.registers[lr] + 1) << endl;
	//register_block.registers[program_counter] = register_block.registers[lr] + 1;
	program_counter = register_block.registers[lr] + 1;
	//program_counter++;
}

void cbz(string register_name, string label)
{
	// if register == 0 jump to label
	// if (RFILE[17]==0) N = j else N++;
		// where j is the line number of the instruction with label LABEL.
	int register_ = getRegisterValue(register_name);
	int jump_label = gotos[label];
	if(register_block.registers[register_] == 0)
	{
		program_counter = jump_label;
	}
	else
	{
		program_counter++;
	}
}

void cbnz(string register_name, string label)
{
	// if register == 0 jump to label
	// if (RFILE[17]==0) N = j else N++;
		// where j is the line number of the instruction with label LABEL.
	int register_ = getRegisterValue(register_name);
	int jump_label = gotos[label];
	if(register_block.registers[register_] != 0)
	{
		program_counter = jump_label;
	}
	else
	{
		program_counter++;
	}
}


void cmpi(string register_name, string immediate_value)
{
	/*
CMP X9,X10// compare X9 to X10 and set condition codes

Patterson, David A.; Hennessy, John L.. Computer Organization and Design ARM Edition (The Morgan Kaufmann Series in Computer Architecture and Design) (Page 129). Elsevier Science. Kindle Edition. 
	*/
	// treat like subtraction but don't store anything but the flags
	// immediate_value - register and set flags
	// register 1 - register/immediate value
	int register_ = getRegisterValue(register_name);
	long long int offset = getRegisterValue(immediate_value);
	long long int value = register_block.registers[register_];
	long long int result = value - offset;
	/*if(result < 0)
	{
		negative_flag = 1;
	}
	*/
	if(result == 0)
	{
		zero_flag = 1;
	}
	//setCarryFlag(offset, value, result);
	//setOverFlowFlag(offset, value, result);
	program_counter++;

}
/*
Signed numbers Unsigned numbers Comparison Instruction CC Test Instruction CC Test = B.EQ Z=1 B.EQ Z=1 ≥ B.GE N=V B.HS C=1 < B.LT N!=V B.LO C=0 > B.GT (Z=0 & N=V) B.HI (Z=0 & C=1) ≤ B.LE ~(Z=0 & N=V) B.LS ~(Z=0 & C=1) ≠ B.NE Z=0 B.NE Z=0

Patterson, David A.; Hennessy, John L.. Computer Organization and Design ARM Edition (The Morgan Kaufmann Series in Computer Architecture and Design) (Page 98). Elsevier Science. Kindle Edition. 
page 98
*/
void beq(string label)
{
	// if flags mean last comparison resulted in an equality jump to label
	// an equality means the zero flag is set(cause result was 0 and only A-A can produce that result)
	int line_number = gotos[label];
	if(zero_flag)
	{
		program_counter = line_number;
	}
	else
	{
		program_counter++;
	}
}



void b(string jump_label)
{
	int line_number = gotos[jump_label];
	program_counter = line_number;

}






















string getRidOfCharacter(string input, char character_to_remove)
{
	string new_string;
	int i = 0;
	while(i < input.size())
	{
		if(input[i] != character_to_remove)
		{
			new_string += input[i];
		}
		i++;
	}
	return new_string;
}


string getLabel(string line_starting_with_a_label)
{
	string label;
	int end = line_starting_with_a_label.find(":");
	if(end != std::string::npos)
	{
		label = line_starting_with_a_label.substr(0, end);
	}
	return label;
}
string getRidOfLeftParens(string token)
{
	if(token[0] == '[')
	{
		//cout << token<< " [\n";

		token = token.substr(1, token.size() - 2);
		//cout << token << endl;

	}
	return token;
}

string getRidOfCommaOnRightSide(string token)
{

	if(token[token.size() - 1] == ',')
	{
		//cout << token << " ,\n";

		token = token.substr(0, token.size() - 1);
		//cout << token << endl;
	}
	return token;
}
string getRidOfRightParenseOnRightSide(string token)
{

	if(token[token.size() - 1] == ']')
	{
		//cout << token << " ]\n";

		token = token.substr(0, token.size() - 1);
		//cout << token << endl;
	}
	return token;
}

deque<string>* split(string input, char delimiter)
{
	// assume all keywords+ a little punctuation are separated out by whitespace
	deque<string>* keyword_puncs = new deque<string>;
	string keyword_punc;
	int i = 0;
	while(i < input.size())
	{
		if(input[i] != delimiter)
		{
			keyword_punc += tolower(input[i]);
		}
		else
		{
			keyword_puncs->push_back(keyword_punc);
			keyword_punc = "";
		}
		i++;
	}
	if(keyword_punc.size() > 0)
	{
		keyword_puncs->push_back(keyword_punc);

	}
	return keyword_puncs;
}

string readFile(string file_name)
{
	FILE* file = fopen(file_name.c_str(), "r");
	string string_;
	size_t n = 0;
	int c;

	if(file == NULL)
	{
	   return NULL;
	}

   fseek(file, 0, SEEK_END);
   long f_size = ftell(file);

   fseek(file, 0, SEEK_SET);
   //string_ = (char*) malloc(f_size);
   string_.resize(f_size);
	c = fgetc(file);
	while(c != EOF)
	{

		string_[n] = c;
	    n++;
	    c = fgetc(file);
   }
   //string[n] = '\0';

   return string_;
}
// I don't have time to setup how the user can use this.
void setupInitMem(string init_mem)
{
	// assume there will be an even number of numbers
	deque<string>* numbers = split(init_mem, ' ');
	int max_stack_index = 0;
	// find max size of stack
	for(int i = 0; i < numbers->size(); i++)
	{
		int index = convertDecimalStringToDecimal(numbers->at(i));
		if( (i % 2 == 0) && (index > max_stack_index) )
		{
			max_stack_index = index;
		}
	}
	// set up the stack
	for(int i = 0; i < max_stack_index; i++)
	{
		stack.push_back("00000000");

	}
	// set the init values
	for(int i = 0; i < numbers->size(); i ++)
	{
		// i == index in memory
		if(i % 2 == 0)
		{
			string value = numbers->at(i + 1);
			int decimal_value = convertBinaryStringToDecimal(value);
			// assume decimal_value is 8 bits
			string value_in_binary = convertToBinary(decimal_value);
			stack[i] = value_in_binary;
		}
	}


}
int main(int argc, char *argv[])
{
	/*
	Written by David Tauraso, Nicholas Ivanoff, Soumana S., Robert Rojes
	*/
	/// Do UI
	// ask user for input file
	//cout << "please specify the input file\n";
	//string file_name = "test_case_1.txt";
	//cin >> file_name;
	// ask for memory inits
	cout << "this program simulates Legv8 assmebly code.  It assumes you wrote valid and good code.  if you want to init things, it has to go into the assmebly code\n";
	
	string input = readFile(argv[1]);

	deque<string>* lines = split(input, '\n');

	// read in file
	// set initial locations on the stack and registers
	// set 1 step or go through all
	// if have time then do some infinite loop checking
	
	/*for(int i = 0; i < lines->size(); i++)
	{
		deque<string>* tokens2 = split(lines->at(i), ' ');
		//for_each(tokens2->begin(), tokens2->end(), [](string item){cout << item << "|";});
		//cout << endl << endl;

	}*/
	//return 0;

	/*
ADDI X0, X31, #12 FUN: CBZ X0, DONE
CMPI X0, #1
B.EQ DONE
SUBI SP, SP, #32
STUR X0, [SP, #0]
STUR X30, [SP, #8]
STUR X19, [SP, #16]
SUBI X0, X0, #1
BL FUN
ADDI X19, X0, #0
LDUR X0, [SP, #0]
SUBI X0, X0, #2
BL FUN
ADD X0, X0, X19
LDUR X30, [SP, #8]
LDUR X19, [SP, #16]
ADDI SP, SP, #32 // (*)
DONE: BR X30
	*/
/*
FUNC: ADDI X1, XZR, #220
ADD X2, X1, X1 
STUR X2, [X0, #16]
LDUR X3, [X0, #16]
LDURB X7, [X0, #16]
SUB X3, XZR, X7
SUB X4, X3, X1
STURB X4, [X1, #8]
ADDS X4, X4, X2
ADDI X9, XZR, #131073
STUR X9, [X2, #32]
LDURH X10, [X1, #10]
STURW X9, [X2, #16]
LDURSW X9, [X2, #16]
CBNZ XZR, Notfun
B Done

Notfun: B FUNC

Done: 
*/
	// make sure longer variations of the word start first

	string tokens[] = {

		"b.eq",
		"b.lt",

		"b.gt",
		"b.lo",
		
		"b.hi",
		"b.ne",
		
		"b.le",
		"b.ge",
		
		"b.ls",
		"b.hs",
		
		"bl",
		"br",
		
		"b",
		"sturb",
		
		"sturw",
		"subis",
		
		"sturh",
		"stxr",
		
		"stur",
		"subs",
		
		"subi",
		"sub",
		
		"ldursw",
		"ldurb",
		
		"ldurh",
		"ldxr",
		
		"ldur",
		"lsr",
		
		"lsl",
		"orri",
		
		"orr",
		"eori",
		
		"eor",
		"andis",
		
		"addi",
		"andi",
		
		"ands",
		"adds",
		
		"and",
		"add",
		
		"cbnz",
		"cbz",
		
		"[",
		"sp",
		
		"]",
		","

		// add stop
	};

	deque<deque<string>* > tests_token_lines;

	// will need functions to make the following sets of tokens "Xi", "#number", "number"
	// have the token function recognize hex values using "0x"
	// "#"
	// digit
	// x, X
	for(int i = 0; i < lines->size(); i++)
	{
		deque<string>* tokens = split(lines->at(i), ' ');
		deque<string>* trimmed_tokens = new deque<string>(tokens->size());

		transform(tokens->begin(), tokens->end(), trimmed_tokens->begin(), [](string item){
			return getRidOfRightParenseOnRightSide(getRidOfCommaOnRightSide(getRidOfLeftParens(item)));

		});
		//for_each(trimmed_tokens->begin(), trimmed_tokens->end(), [](string item){cout << item << "|";});

		//cout << (tests[i].find(":") != std::string::npos) << endl;
		// if there is only one item left and it ends in : then it's a label
		if(lines->at(i).find(":") != std::string::npos)
		{
			// fun:|cbz|x0,|
			// done:|br|
			// done:
			string label;
			// get first element of tokens
			label = trimmed_tokens->at(0).substr(0, trimmed_tokens->at(0).size() - 1);
				//cout << label<<"\n";

			trimmed_tokens->pop_front();
			if(trimmed_tokens->size() > 0)
			{
				gotos.insert(pair<string, int>(label, tests_token_lines.size()));
				tests_token_lines.push_back(trimmed_tokens);
				//cout << label << " " << gotos[label] << endl;
			}
			else
			{
				gotos.insert(pair<string, int>(label, tests_token_lines.size()));
				//cout << label << " " << gotos[label] << endl;

			}
			
		}
		else
		{
			tests_token_lines.push_back(trimmed_tokens);

		}
		//deque<string>* split_test_i = tokenize(tests[i], tokens);
		// there may be an instruction on the same line as the label
		// save it into the goto list
		// assuming there is no label that comes before its definition(this can be changed)
		//for_each(split_test_i->begin(), split_test_i->end(), [](string item){cout << item << " ";});
		//cout << endl;
		

	}

	int run_setting;
	cout << "choose step: 1 or run: 0\n";
	cin >> run_setting;
	
	//cout << endl;
	//for_each(gotos.begin(), gotos.end(), [](pair<string, int> goto_){cout << goto_.first << " " << goto_.second;});
	//cout << endl;
	//return 0;

	// i should be the program counter
	// each deque inside tests_token_lines is a deque of the tokenzed strings from the original string
	for(int i = 0; program_counter < tests_token_lines.size(); i++)
	{
		//cout << "command " << program_counter << endl;
		// skip over empty lines
		/*if(tests_token_lines.at(i)->size() == 0)
		{
			continue;
		}*/

		//for_each(tests_token_lines.at(program_counter)->begin(), tests_token_lines.at(program_counter)->end(), [](string item){cout << item << " ";});
		string command = tests_token_lines.at(program_counter)->at(0);
		deque<string>* instruction = tests_token_lines.at(program_counter);
		/*if(command == "b.eq")
		{
			cout << "->b.eq instruction" << endl;
			
			/*cout << "goto list\n";
			for(map<string, int>::iterator j = gotos.begin(); j != gotos.end(); j++)
			{
				cout << j->first << " => " << gotos.at(tests_token_lines.at(i)->at(0)) << endl;
			}*/
			
			//cout << tests_token_lines.at(i)->at(1) << " -> i " << gotos.at(tests_token_lines.at(i)->at(1)) << endl;
			//int location = gotos.at(tests_token_lines.at(i)->at(1));
			//cout << "->jump to " << tests_token_lines.at(location)->at(0) << " "<< tests_token_lines.at(location)->at(1)  << " when flags say equal" << endl;
			// run a function called b.eq to affect the memory and registers however the instruction says
			// bEQ(split_test_i, mem, registers)
		//}
				/*
		if(command == "b")
		{
			cout << "->b instruction" << endl;
			
			/*cout << "goto list\n";
			for(map<string, int>::iterator j = gotos.begin(); j != gotos.end(); j++)
			{
				cout << j->first << " => " << gotos.at(tests_token_lines.at(i)->at(0)) << endl;
			}*/
			
			//cout << tests_token_lines.at(i)->at(1) << " -> i " << gotos.at(tests_token_lines.at(i)->at(1)) << endl;

			//int location = gotos.at(tests_token_lines.at(i)->at(1));
			//cout << "->jump to " << tests_token_lines.at(location)->at(0) << " "<< tests_token_lines.at(location)->at(1) << " when flags say equal" << endl;
			// run a function called b.eq to affect the memory and registers however the instruction says
			// b(split_test_i, mem, registers)
		//}
		/*else if(command == "add")
		{
			cout << "->add instruction" << endl;
			cout << "->add p1 and p2 and store the result in p0" << endl;
			// add(split_test_i, mem, registers)

		}*/
		/*else if(command == "addi")
		{
			// ge rid of the commas on the end of the register names
			cout << "->addi instruction" << endl;
			cout << "->add p1 and number and store the result in p0" << endl;

		}*/

		// http://www.eecs.umich.edu/courses/eecs370/eecs370.w19/resources/materials/ARM-v8-Quick-Reference-Guide.pdf
		// https://www.ibm.com/support/knowledgecenter/en/ssw_aix_72/com.ibm.aix.alangref/idalangref_comp_immed.htm
		if(command == "b.eq")
		{

			beq(instruction->at(1)/*jump_label*/);
		}
		else if(command == "b.lt")
		{
			// blt(jump_label)
			program_counter++;
		}
		else if(command == "b.gt")
		{
			// bgt(jump_label)
			program_counter++;
		}
		else if(command == "b.lo")
		{
			// blo(jump_label)
			program_counter++;
		}
		else if(command == "b.hi")
		{
			// bhi(jump_label)
			program_counter++;
		}
		else if(command == "b.ne")
		{
			// bne(jump_label)
			program_counter++;
		}
		else if(command == "b.le")
		{
			// ble(jump_label)
			program_counter++;
		}
		else if(command == "b.ge")
		{
			// bge(jump_label)
			program_counter++;
		}
		else if(command == "b.ls")
		{
			// bls(jump_label)
			program_counter++;
		}
		else if(command == "b.hs")
		{
			// bhs(jump_label)
			program_counter++;
		}
		else if(command == "bl")
		{
			// set pc to jump_label line number and store line number into link register
			bl(instruction->at(1) /*jump_label*/);
		}
		else if(command == "br")
		{
			// set pc to register
			br(instruction->at(1)/*register*/);
		}
		else if(command == "b")
		{
			// set pc to jump_label line number or register value
			b(instruction->at(1));
			//program_counter++;
		}
		else if(command == "sturb")
		{
			// sturb(register1, register2, offset_from_stack_pointer)
			program_counter++;
		}
		else if(command == "sturw")
		{
			// sturw(register1, register2, offset_from_stack_pointer)
			program_counter++;

		}
		else if(command == "sturh")
		{
			// sturw(register1, register2, offset_from_stack_pointer)
			program_counter++;
		}
		else if(command == "stxr")
		{
			// sturw(register1, register2, offset_from_stack_pointer)
			program_counter++;

		}
		else if(command == "stur")
		{
			stur(instruction->at(1), instruction->at(2), instruction->at(3)/*offset_from_stack_pointer*/);

		}
		else if(command == "subis")
		{
			// subis(register1, register2, immediate_value)
			program_counter++;

		}

		else if(command == "subs")
		{
			// subs(register1, register2, register3)
			program_counter++;
		}
		else if(command == "subi")
		{
			subi(instruction->at(1), instruction->at(2), instruction->at(3) /*imediate value*/);
		}
		else if(command == "sub")
		{
			// sub(register1, register2, register3)
			program_counter++;

		}
		else if(command == "ldursw")
		{
			// ldursw(register1, register2, offset_from_stack_pointer)
			program_counter++;

		}
		else if(command == "ldurb")
		{
			// ldurb(register1, register2, offset_from_stack_pointer)
			program_counter++;
		}
		else if(command == "ldurh")
		{
			// ldurh(register1, register2, offset_from_stack_pointer)
			program_counter++;
		}
		else if(command == "ldxr")
		{
			// ldxr(register1, register2, offset_from_stack_pointer)
			program_counter++;
		}
		else if(command == "ldur")
		{
			ldur(instruction->at(1), instruction->at(2), instruction->at(3)/*offset_from_stack_pointer*/);
		}
		else if(command == "lsr")
		{
			// lsr(register1, register2, shift_amount)
			program_counter++;
		}
		else if(command == "lsl")
		{
			// lsl(register1, register2, shift_amount)
			program_counter++;
		}
		else if(command == "orri")
		{
			// orri(register1, register2, immediate_value)
			program_counter++;
		}
		else if(command == "orr")
		{
			// orr(register1, register2, register3)
			program_counter++;
		}
		else if(command == "eori")
		{
			// eori(register1, register2, immediate_value)
			program_counter++;

		}
		else if(command == "eor")
		{
			// eori(register1, register2, register3)
			program_counter++;

		}
		else if(command == "andis")
		{
			// andis(register1, register2, immediate_value)
			program_counter++;
		}
		else if(command == "addi")
		{
			addi(instruction->at(1),  instruction->at(2), instruction->at(3)/*imediate value*/);

		}
		else if(command == "andi")
		{
			// andi(register1, register2, immediate_value)
			program_counter++;

		}
		else if(command == "ands")
		{
			// ands(register1, register2, register3)
			program_counter++;

		}
		else if(command == "adds")
		{
			// adds(register1, register2, register3)
			program_counter++;

		}
		else if(command == "and")
		{
			// and(register1, register2, register3)
			program_counter++;
		}
		else if(command == "add")
		{
			add(instruction->at(1), instruction->at(2), instruction->at(3));
		}
		else if(command == "cbnz")
		{
			cbnz(instruction->at(1), instruction->at(2)/*label*/);
			program_counter++;
		}
		else if(command == "cbz")
		{
			cbz(instruction->at(1), instruction->at(2)/*label*/);
			program_counter++;
		}
		else if(command == "cmp")
		{
			// cmp(register1, register2, register3)
			program_counter++;
		}
		else if(command == "cmpi")
		{
			cmpi(instruction->at(1), instruction->at(2)/*immediate value*/);
		}
		/*
		0"ADDI X0, X31, #12",
		1 "FUN: CBZ X0, DONE",
		2 "CMPI X0, #1",
		3 "B.EQ DONE",
		4 "SUBI SP, SP, #32",
		5 "STUR X0, [SP, #0]",
		6 "STUR X30, [SP, #8]",
		7 "STUR X19, [SP, #16]",
		8 "SUBI X0, X0, #1",
		9 "BL FUN",
		10 "ADDI X19, X0, #0",
		11 "LDUR X0, [SP, #0]",
		12 "SUBI X0, X0, #2",
		13 "BL FUN",
		14 "ADD X0, X0, X19",
		15 "LDUR X30, [SP, #8]",
		16 "LDUR X19, [SP, #16]",
		17 "ADDI SP, SP, #32",
		18 "DONE: BR X30"
		*/

		
		/*
		if(options->size() == 2)
		{
			
		}*/
		// step through the program
		// works
		if(run_setting)
		{
			string continuation_options;
			string mem_index;
			cout << "specify contents of memory\n";
			cin >> continuation_options;
			cin >> mem_index;
			// example of continuation_options: RFILE 12
			//deque<string>* options = split(continuation_options, ' ');
			//cout << options->at(0) << endl;
			//cout << options->at(1) << endl;

			long long int index = convertBinaryStringToDecimal(mem_index);
			if(continuation_options == "RFILE")
			{
				// print register[index]
				if(index < 32)
				{
					cout << register_block.registers[index] << endl;

				}


			}
			else if(continuation_options == "STK" || continuation_options == "MEM")
			{
				// print stack[index]
				if(index < stack.size())
				{
					cout << stack[index] << endl;

				}

			}
			// finish testing 
			string continuation;
			cout << "press any letter/word to continue. Type in STOP to end simulation\n";
			cin >> continuation;

			if(continuation == "STOP")
			{
				// output data to file
				writeContents();
				return 0;
			}

		}

		// x0 should be 144 at the end
		// x0 actually is 56
		/*if(i == 350)//  105 takes us to "LDUR X0, [SP, #0]",
			// started skipping 20 rounds at a time starting at round 117 
		{
			//printContents();
			return 0;
		}*/
		printContents();

		//cout << endl;
	}

	// delete the inner deques
	/*
	for(map<string, int>::iterator i = gotos.begin(); i != gotos.end(); i++)
	{
		cout << i->first << " => " << i->second << " " << tests[i->second] << endl;
	}*/
	cout << endl;

	/*
	idea for nested functions
	stur to stack pointer pushes things to stack
	b label adjusts the stack pointer to make a new stack
	bl link_register to reset the stack register



	LEGv8 has a branch-and-link (BL) instruction:
 It saves the address of the following instruction (the return address) in register LR, and then branches to specified address
BL ProcedureAddress
 The BL command saves PC + 4 into register LR
push

To return from the procedure, the branch
register instruction is used: BR LR
 The BR instruction copies address stored in register LR to PC register
pop


if it says b or b.eq just goto it and don't assume its a function call


http://www.cas.mcmaster.ca/~leduc/slides2ga3/2GA3slides2.pdf
	*/
	return 0;
}
