#include <iostream>
#include <algorithm>
#include <string>
#include <deque>
#include <map>
using namespace std;




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

	have it also get intermediate values like "#234562" -> 234562

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


deque<string>* tokenize(string input, string commands[])
{
	int j = 0;
	deque<string>* tokens = new deque<string>;
	/*
	cout << input << endl;
	for(int i = 0; i < 15; i++)
	{
		cout << commands[i] << endl;
	}
	*/
	while(j < input.size())
	{
		//printf("j = %i\n", j);
		// tries all possible tokens
		// if a match
		// collect and bump up j
		bool found = false;

		for(int i = 0; i < 15; i++)
		{
			//printf("%i\n", i);
			//cout << tests[0].substr(j, commands[i].size()) << "|"<< endl;
			//cout << commands[i] << "|" << endl << endl;

			if(input.substr(j, commands[i].size()) == commands[i])
			{
				//printf("got here\n");
				tokens->push_back(commands[i]);
				//printf("got here 2\n");

				j += commands[i].size();
				found = true;
				//for_each(tokens.begin(), tokens.end(), [](string item){cout << item << "|";});
				//cout << endl;
				break;
			}
			

		}
		// otherwise check for a prefix of the longer tokens and longer set of short tokens

		// prefixes: "#", "0x", "x", "X"
		// save prefix and the digits comming after it
			// save it and increment
		//cout << found << endl;
		//printf("j = %i\n", j);
		// the input started with a label
		if(!found)
		{
			// it is a label so grab the rest
			//cout << tests[0].size() << " " << j << endl;
			tokens->push_back(input.substr(j, input.size() - j));
			j = input.size();
		}

	}
	return tokens;
}
string getRidOfLabel(string line_starting_with_a_label)
{
	// find location of label
	// cut it off
	// 
	string label;

	int end = line_starting_with_a_label.find(":");
	if(end != std::string::npos)
	{
		// +1 for the ":"
		label = line_starting_with_a_label.substr(end + 1, line_starting_with_a_label.size() - end);

	}
	return label;

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
int main()
{

	string tests[] = {	"Start:", "b.eq ,label",
						"another_label:",
						"another_label2:",
						"label:  ldurx0[sp#8]",
						"label_7:add x0, x0, x1",
						"addi x0, x4, #3",
						"b.eq another_label",
						"b Start"
					};
	// make sure longer variations of the word start first
	string tokens[] = {"ldur", "addi", "add", "b.eq", "b", "x0", "[", "sp", "]", "x4", "x3", "x1", "#3", ",", "#8"};
	deque<deque<string>* > tests_token_lines;
	map<string, int> gotos;
	long long int mem[255];
	long long int registers[32];
	// need something for the STK memory
	// is it just this?
	long long int stack[255];
	int program_counter = 0;
	// will need functions to make the following sets of tokens "Xi", "#number", "number"
	// have the token function recognize hex values using "0x"
	// "#"
	// digit
	// x, X

	for(int i = 0; i < 9; i++)
	{
		//cout << getRidOfWhiteSpace(tests[i]) << endl;
		string removed_spaces = getRidOfCharacter(tests[i], ' ');
		string removed_commas = getRidOfCharacter(removed_spaces, ',');
		string removed_left_square_bracket = getRidOfCharacter(removed_commas, '[');
		string removed_right_square_bracket = getRidOfCharacter(removed_left_square_bracket, ']');
		tests[i] = removed_right_square_bracket;
		// if there is only one item left and it ends in : then it's a label
		if(tests[i].find(":") != std::string::npos)
		{

			string label;
			//cout << "tokens with label\n";
			//for_each(split_test_i->begin(), split_test_i->end(), [](string item){cout << item << " ";});
			cout << endl;
			cout << "->at label " << removed_right_square_bracket << endl;
			label = getLabel(removed_right_square_bracket);
			cout << "label name =>" << getLabel(removed_right_square_bracket) << endl;
			tests[i] = getRidOfLabel(removed_right_square_bracket);
			cout << "got rid of label ==>" <<"|" << tests[i] << "|"<< endl;
			//if()
			// want the jump to be for the next instruction = last instructions + 1 = size of instruction deque
			gotos.insert(pair<string, int>(label, tests_token_lines.size()));
			cout << "pair saved " << label << " " << tests_token_lines.size() << endl;

			// delete label
			// save label and location
			//gotos.insert(pair<string, int>())
			// the label was the only thing on the line, so we want the program counter to start on the line immediately after this one
			if(tests[i].size() == 0)
			{
				//gotos[label] += 1;
				continue;
			}
		}
		deque<string>* split_test_i = tokenize(tests[i], tokens);
		tests_token_lines.push_back(split_test_i);
		// there may be an instruction on the same line as the label
		// save it into the goto list
		// assuming there is no label that comes before its definition(this can be changed)
		cout << "printing tokens\n";
		for_each(split_test_i->begin(), split_test_i->end(), [](string item){cout << item << " ";});
		cout << endl;
		

	}
	for(int j = 0; j < tests_token_lines.size(); j++)
	{
		cout << j << endl;
		for(int i = 0; i < tests_token_lines[j]->size(); i++)
		{
			cout << tests_token_lines.at(j)->at(i) << " ";
		}
		cout << endl;

	};
	// i should be the program counter
	// each deque inside tests_token_lines is a deque of the tokenzed strings from the original string
	for(int i = 0; i < tests_token_lines.size(); i++)
	{
		cout << i << endl;
		// skip over empty lines
		/*if(tests_token_lines.at(i)->size() == 0)
		{
			continue;
		}*/
		for_each(tests_token_lines.at(i)->begin(), tests_token_lines.at(i)->end(), [](string item){cout << item << " ";});

		if(tests_token_lines.at(i)->at(0) == "b.eq")
		{
			cout << "->b.eq instruction" << endl;
			
			/*cout << "goto list\n";
			for(map<string, int>::iterator j = gotos.begin(); j != gotos.end(); j++)
			{
				cout << j->first << " => " << gotos.at(tests_token_lines.at(i)->at(0)) << endl;
			}*/
			
			cout << tests_token_lines.at(i)->at(1) << " -> i " << gotos.at(tests_token_lines.at(i)->at(1)) << endl;
			int location = gotos.at(tests_token_lines.at(i)->at(1));
			cout << "->jump to " << tests_token_lines.at(location)->at(0) << " "<< tests_token_lines.at(location)->at(1)  << " when flags say equal" << endl;
			// run a function called b.eq to affect the memory and registers however the instruction says
			// bEQ(split_test_i, mem, registers)
		}
		if(tests_token_lines.at(i)->at(0) == "b")
		{
			cout << "->b instruction" << endl;
			
			/*cout << "goto list\n";
			for(map<string, int>::iterator j = gotos.begin(); j != gotos.end(); j++)
			{
				cout << j->first << " => " << gotos.at(tests_token_lines.at(i)->at(0)) << endl;
			}*/
			
			cout << tests_token_lines.at(i)->at(1) << " -> i " << gotos.at(tests_token_lines.at(i)->at(1)) << endl;

			int location = gotos.at(tests_token_lines.at(i)->at(1));
			cout << "->jump to " << tests_token_lines.at(location)->at(0) << " "<< tests_token_lines.at(location)->at(1) << " when flags say equal" << endl;
			// run a function called b.eq to affect the memory and registers however the instruction says
			// b(split_test_i, mem, registers)
		}
		else if(tests_token_lines.at(i)->at(0) == "add")
		{
			cout << "->add instruction" << endl;
			cout << "->add p1 and p2 and store the result in p0" << endl;
			// add(split_test_i, mem, registers)

		}
		else if(tests_token_lines.at(i)->at(0) == "addi")
		{
			cout << "->addi instruction" << endl;
			cout << "->add p1 and number and store the result in p0" << endl;

		}
		cout << endl;
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
	*/
	return 0;
}