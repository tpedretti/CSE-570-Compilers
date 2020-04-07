//Name: Taylor Pedretti
//Class: CSE-570
//Lab: Lab 1
/*Description: DFAs are specified by a set of states (among them one starting and one 
or more accepting), and transitions between pairs of states which are 
labeled by a symbol of some alphabet. DFAs are typically given in one 
of two formats: 
(1) a transition diagram (that is, a directed graph), and
(2) a transition table. 
Both representations are equivalant to each other.  

As an example of a DFA, take a look in Purple dragon book on p. 151, Fig. 
3.28, a transition diagram. The DFA algorithm is descrived in pages 148-151. */

#include <iostream>
#include <string>
#include <vector>
using namespace std;

void stateOutput(string DFA, int loc, string lang);
void startupPrompt(); 
void ValidLanguage(string& inputString);

//enum values used to make it easier to read for the states
typedef enum
{
	STATE0 = 0,
	STATE1,
	STATE2,
	FINAL_STATE
} DFASTATES;

//Transition table for DFA1
const DFASTATES DFA_1[4][2] =
{
	//a			b
	{STATE1,	STATE0},
	{STATE1,	STATE2},
	{STATE1,	FINAL_STATE},
	{STATE1,	STATE0}
};

//Transition table for DFA2
const DFASTATES DFA_2[4][2] =
{
	//a				b
	{STATE0,		STATE1},
	{STATE2,		STATE1},
	{FINAL_STATE,	STATE1},
	{STATE0,		STATE1}
};

//Main Program
int main()
{
	//Vector of test cases.
	vector<string> DFATest({ "ababababaazzzzxxxxoisdiabb", "abbabayabbabaababaab", "abbbaa" });

	//Prompt to tell user what is going on
	startupPrompt();
	for (int i = 0; i < DFATest.size(); i++) //Loop to check all cases
	{
		cout << "DFA Test for string #" << (i + 1) << ": " << DFATest[i] << endl;
		ValidLanguage(DFATest[i]);
		cout << endl;
	}

	cout << "End of tests!" << endl;
	return 0;
}

//Prompt to tell user what is going on
void startupPrompt()
{
	cout << "DFA based off two different languages:\n";
	cout << "L1 = (a|b)*abb\n";
	cout << "L2 = (a|b)*baa\n\n";
}

//Function to check all cases coming it's way to make sure they can be ran in one of our two DFA languages
void ValidLanguage(string& inputString)
{
	DFASTATES DFA1State = STATE0;
	DFASTATES DFA2State = STATE0;
	bool inValidChar = false;
	int inValidCharLoc = 0;

	//Loop to go through the DFA given to us.
	for (int i = 0; i < inputString.length(); i++)
	{
		switch (inputString[i])
		{
		case 'a':
			DFA1State = DFA_1[DFA1State][0];
			DFA2State = DFA_2[DFA2State][0];

			if (DFA1State == FINAL_STATE)//If our state is at the Final state output that string
				stateOutput(inputString.substr(inValidCharLoc, (i+1)), (i + 1), "L1");

			if(DFA2State == FINAL_STATE)//If our state is at the Final state output that string
				stateOutput(inputString.substr(inValidCharLoc, (i + 1)), (i + 1), "L2");

			continue;

		case 'b':
			DFA1State = DFA_1[DFA1State][1];
			DFA2State = DFA_2[DFA2State][1];

			if (DFA1State == FINAL_STATE) //If our state is at the Final state output that string
				stateOutput(inputString.substr(inValidCharLoc, (i+1)), (i+1), "L1");

			if (DFA2State == FINAL_STATE)//If our state is at the Final state output that string
				stateOutput(inputString.substr(inValidCharLoc, (i + 1)), (i + 1), "L2");

			continue;

		default:
			inValidChar = true;
			DFA1State = STATE0;
			DFA2State = STATE0;
			inValidCharLoc = (i+1);
		}
	}

	if (DFA1State == FINAL_STATE && inValidChar == false)
		cout << inputString << " is in L1 \n\n";

	if (DFA2State == FINAL_STATE && inValidChar == false)
		cout << inputString << " is in L2 \n\n";

	if (DFA1State != FINAL_STATE && DFA2State != FINAL_STATE || inValidChar == true)
		cout << inputString << " is not in any language \n\n";
}

//Outputs valid strings of the DFA and which lang they go with.
void stateOutput(string DFA, int loc, string lang)
{
	int maxLength = loc;
	int startPos = (loc - DFA.length()) + 1;
	for(int i = 0; (DFA.length()-2) > 0; i++)
	{
		cout << startPos << "-" << maxLength << " ";
		for (int j = 0; j < DFA.length(); j++)
		{
			cout << DFA[j];
		}

		cout << " is valid in our " << lang << "." << endl;
		startPos++;
		DFA.erase(0, 1);
	}

	cout << endl;
}