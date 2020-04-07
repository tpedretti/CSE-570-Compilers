//Name         : Taylor Pedretti
//Lab          : Lab 2
/* Description : Write a program that does the following:

1. Reads a grammar.
2. Finds all FIRST(X) sets, following algorithm on pg. 189 of text.
3. Finds all FOLLOW(A) sets following algorithm on pg. 190 of text.

You may write the program in any language that is supported by the
computers in the lab, but C++ is preferred.*/

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <set>

using namespace std;

bool firstProduction = true;
void printAllGrammar();
bool isTerminal(char);
void first();
void follow();
void printGrammar();

string file = "grammar.txt";
vector<char> terminal;
vector<char> nonTerminal;
map<char, string> production;
map<char, set<char> > firstMap;
map<char, set<char> > followMap;

int main()
{
    string line;
    fstream input;
    input.open(file.c_str(), ios::in);
    if (!input)
        cerr << "error opening grammar.txt" << endl;

    getline(input, line);
    while (!input.eof())
    {
        char val = line[0];
        if (islower(val) || val == '+' || val == '*' || val == '(' || val == ')')
            terminal.push_back(val);

        if (isupper(val))
        {
            if (nonTerminal.empty())
                nonTerminal.push_back(val);
            else if (!(find(nonTerminal.begin(), nonTerminal.end(), val) != nonTerminal.end()))
                nonTerminal.push_back(val);
            
            if (production[val] == "")
                production[val] = line.substr(3, line.length());
            else
                production[val] += "|" + line.substr(3, line.length());
        }

        getline(input, line);       
    }
    input.close();

    printGrammar();
    first();
    follow();
    printAllGrammar();
}

//Prints the grammar.
void printGrammar()
{
    cout << "Grammar" << endl;
    for (int i = 0; i < terminal.size(); i++)
        cout << terminal[i] << endl;

    cout << "$" << endl;

    for (int i = 0; i < nonTerminal.size(); i++)
        cout << nonTerminal[i] << "->" << production[nonTerminal[i]] << endl;

    cout << "$" << endl;
}

//Prints the first and follow sets for the given grammar.
void printAllGrammar()
{
    cout << endl;
    
    //print first set
    for (int i = 0; i < nonTerminal.size(); i++)
    {
        cout << "First(" << nonTerminal[i] << ") = {";
        for (auto it = firstMap[nonTerminal[i]].begin(); it != firstMap[nonTerminal[i]].end(); it++)
        {
            cout << *it;
            auto it2 = next(it, 1);
            if (it2 != firstMap[nonTerminal[i]].end())
            {
                cout << ",";
            }
        }
        
        cout << "}" << endl;
    }

    cout << endl;

    //print follow set
    for (int i = 0; i < nonTerminal.size(); i++)
    {
        cout << "Follow(" << nonTerminal[i] << ") = {";
        for (auto it = followMap[nonTerminal[i]].begin(); it != followMap[nonTerminal[i]].end(); it++)
        {
            cout << *it;
            auto it2 = next(it, 1);
            if (it2 != followMap[nonTerminal[i]].end())
            {
                cout << ",";
            }
        }

        cout << "}" << endl;
    }
}

//Checks if the given char is a terminal or not terminal value
bool isTerminal(char input)
{
    if (input == 'e')
        return true;
    if (find(terminal.begin(), terminal.end(), input) != terminal.end())
        return true;
    else
        return false;
}

//Find the first set for each nonTerminal.
void first()
{
    char lhs;
    bool complete = false;
    bool changes = false;
    int i = 0;

    while (!complete)
    {
        lhs = nonTerminal[i++%nonTerminal.size()];
        i = i % nonTerminal.size();
        if(i == 0)
            changes = false;

        int j = 0;
        bool firstChar = true;
        string expr = production[lhs];

        while (j < expr.length())
        {
            char first;
            if (firstChar == true)
            {
                firstChar = false;
                first = expr[0];
            }
            else
            {
                char temp = expr[j];
                while (temp != '|' && j <= expr.length())
                    temp = expr[j++];
                if (j >= expr.length())
                    continue;
                first = expr[j];
            }
            if (isTerminal(first))
            {
                set<char> builder = firstMap[lhs];
                bool found = builder.find(first) != builder.end();
                if (!found)
                {
                    firstMap[lhs].insert(first);
                    changes = true;            
                }
            }
            else
            {
                set<char> next = firstMap[first];
                for (auto k = next.begin(); k != next.end(); k++)
                {
                    char temp = *k;
                    bool found = firstMap[lhs].find(temp) != firstMap[lhs].end();
                    if (!found)
                    {
                        set<char> builder = firstMap[lhs];
                        firstMap[lhs].insert(temp);
                        changes = true;
                    }
                }
            }
        }
		
        if (i == nonTerminal.size()-1 && changes == false)
            complete = true;
    }
}

//Find the follow set for each nonTerminal.
void follow()
{
    char lhs;
    bool complete = false;
    bool changes = false;
    int i = 0;

    while (!complete)
    {
        lhs = nonTerminal[i++%nonTerminal.size()];
        i = i % nonTerminal.size();
        if (i == 0)
            changes = false;

        int j = 0;
        bool first = true;
        string expr = production[lhs];

        while (j < expr.length())
        {
            char firstChar;
            char followChar;

            if (first == true)
            {
                firstChar = expr[0];
                followChar = expr[1];
                if (isTerminal(firstChar))
                {
                    j++;
                    firstChar = expr[j];
                    followChar = expr[j + 1];
                }
                first = false;
                j++;
            }
            else
            {
                firstChar = expr[j];
                followChar = expr[j + 1];
                while (firstChar == '|' || followChar == '|' && !isTerminal(firstChar))
                {
                    firstChar = expr[j];
                    followChar = expr[j + 1];
                    j++;
                }

                j++;
            }

            if (isTerminal(firstChar) == false and followChar == '\0')
            {
                set<char> builder = followMap[lhs];
                set<char> builder2 = followMap[firstChar];
                for (auto k = builder.begin(); k != builder.end(); k++)
                {
                    bool found = followMap[firstChar].find(*k) != followMap[firstChar].end();
                    if (!found && *k != 'e')
                    {
                        followMap[firstChar].insert(*k);
                        changes = true;
                    }
                }
            }

            if (isTerminal(firstChar) == false and isTerminal(followChar) == true)
            {
                if (followChar != 'e')
                {
                    bool found = followMap[firstChar].find(followChar) != followMap[firstChar].end();
                    if (!found)
                    {
                        followMap[firstChar].insert(followChar);
                        changes = true;
                    }
                }
            }

            if (isTerminal(firstChar) == false and isTerminal(followChar) == false)
            {
                set<char> builder = followMap[firstChar];
                set<char> builder2 = firstMap[followChar];
                set<char> builder3 = followMap[lhs];
                bool epsilon = false;
                for (auto k = builder2.begin(); k != builder2.end(); k++)
                {
                    if (*k == 'e')
                        epsilon = true;
                    if (*k != 'e')
                    {
                        bool found = followMap[firstChar].find(*k) != followMap[firstChar].end();
                        if (!found)
                        {
                            followMap[firstChar].insert(*k);
                            changes = true;
                        }
                    }
                }

                if (epsilon)
                {
                    for (auto k = builder3.begin(); k != builder3.end(); k++)
                    {
                        if (*k != 'e')
                        {
                            bool found = followMap[firstChar].find(*k) != followMap[firstChar].end();
                            if (!found)
                            {
                                followMap[firstChar].insert(*k);
                                changes = true;
                            }
                        }
                    }
                }
            }

            if (i == nonTerminal.size() - 1 && changes == false)
                complete = true;
        }
    }
    followMap[nonTerminal[0]].insert('$');
}