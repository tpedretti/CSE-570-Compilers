// Name         : Taylor Pedretti
// Lab          : Lab 4
/* Description  : Write grammars for the following languages:


	1. Strings of 0's and 1's with unequal numbers of 0's and 1's.
	2. Strings of 0's and 1's that are palindromes (the second
	   half of the string is the reverse of the first half.
	3. Strings of 0's and 1's of the form xx

Generate LR(0) tables for these grammars (use your LAB6 program)

For each of the three grammars above, answer the question: 
	Is the grammar SLR? Yes or No. If No, indicate at least one
	conflict. See algorithm 4.8, page 227, example of conflict
	described at the bottom of page 229. */

#include <iostream>
#include <cstdlib>
#include <string>
#include <string.h>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <set>
#include "ItemRP.h"

using namespace std;

bool firstProduction = true;
void printAll();
bool isTerminal(char);
void first();
void follow();
void printGrammar();
void nextClosure(string);
void add_closure(char, ItemRP& iem, AugmentedGrammar&);
void getItemRPs(vector<ItemRP>&, AugmentedGrammar&, int&, gotoMap&);

string file = "grammar1";
vector<char> terminal;
vector<char> nonTerminal;
map<char, string> production;
map<char, set<char> > firstMap;
map<char, set<char> > followMap;
typedef map<char, vector<string> > AugmentedGrammar;
typedef map<string, int> gotoMap;

int main()
{
    int itemid = -1;
    AugmentedGrammar AugGrammar;
    vector<ItemRP> ItemRPs = { ItemRP() };
    gotoMap globalGoto;

    string line;
    string lhs, rhs;
    string delim = "->";

    fstream input;
    input.open(file.c_str(), ios::in);
    if (!input)
        cerr << "error opening grammar.txt" << endl;

    printf("Augmented Grammar\n");

    getline(input, line);
    while (!input.eof())
    {
        char val = line[0];
        if (islower(val) || !isalpha(val))
            terminal.push_back(val);

        if (isupper(val))
        {
            if (nonTerminal.empty())
            {
                AugGrammar['\''].push_back(val + "");
                string temp = ".";
                temp.push_back(val);
                ItemRPs[0].Push(new AugmentedProduction('\'', temp));
                printf("'->%c\n", val);
                nonTerminal.push_back(val);
                continue;
            }
            else if (!(find(nonTerminal.begin(), nonTerminal.end(), val) != nonTerminal.end()))
                nonTerminal.push_back(val);

            auto pos = line.find(delim);
            rhs = line.substr(pos + delim.length(), std::string::npos);
            if (production[val] == "")
            {
                production[val] = line.substr(3, line.length());
                AugGrammar[val].push_back(rhs);
                printf("%c->%s\n", val, rhs.c_str());
                if (isTerminal(rhs[0]) == false)
                    ItemRPs[0].Push(new AugmentedProduction(val, "." + rhs));
            }
            else
            {
                production[val] += "|" + line.substr(3, line.length());
                AugGrammar[val].push_back(rhs);
                printf("%c->%s\n", val, rhs.c_str());
                if(isTerminal(rhs[0]) == false)
                    ItemRPs[0].Push(new AugmentedProduction(val, "." + rhs));
            }
        }

        getline(input, line);
    }

    input.close();

    first();
    follow();
    printAll();

    printf("\nClosures and Gotos\n");
    while (++itemid < int(ItemRPs.size())) {
        getItemRPs(ItemRPs, AugGrammar, itemid, globalGoto);
    }

    return 0;
}

//Prints the grammar.
void printGrammar()
{
    printf("Condesned Grammar\n");
    cout << "Grammar" << endl;
    for (int i = 0; i < terminal.size(); i++)
        cout << terminal[i] << endl;

    cout << "$" << endl;

    for (int i = 0; i < nonTerminal.size(); i++)
        cout << nonTerminal[i] << "->" << production[nonTerminal[i]] << endl;

    cout << "$" << endl;


    cout << endl;
}

//Prints the first and follow sets for the given grammar.
void printAll()
{
    cout << endl;

    printf("First Sets\n");
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

    printf("Follow Sets\n");
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
    bool changes = true;
    int i = 0;

    while (!complete)
    {
        lhs = nonTerminal[i++%nonTerminal.size()];
        i = i % nonTerminal.size();
        if (i == 0)
            changes = false;

        if (lhs == '\'')
            continue;

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

        if (i == nonTerminal.size() - 1 && changes == false)
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

        if (lhs == '\'')
            continue;

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
                    if(firstChar == 'e')
                        continue;
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
                while (firstChar == '|' || followChar == '|' || isTerminal(firstChar) == true)
                {
                    firstChar = expr[j];
                    if (firstChar == '\0')
                        continue;
                    followChar = expr[j + 1];
                    j++;
                }

                j++;
            }

            if (firstChar == '\0' && followChar == '\0')
                continue;

            if (isTerminal(firstChar) == false && followChar == '\0')
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

            if (isTerminal(firstChar) == false && isTerminal(followChar) == true && followChar != '\0')
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

            if (isTerminal(firstChar) == false && isTerminal(followChar) == false && followChar != '\0')
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

void add_closure(char lookahead, ItemRP& item, AugmentedGrammar& grammar)
{
    if (!isupper(lookahead))
        return;

    string lhs = string(&lookahead, 1);
	
    for (int i = 0; i < grammar[lookahead].size(); i++)
    {
        string rhs = "." + grammar[lookahead][i];
        if (!item.Contains(lhs + "->" + rhs))
            item.Push(new AugmentedProduction(lookahead, rhs));
    }
}

void getItemRPs(vector<ItemRP>& ItemRPs, AugmentedGrammar& grammar, int& itemid, gotoMap& globalGoto)
{
    printf("I%d:\n", itemid);

    for (int i = 0; i < ItemRPs[itemid].Size(); i++)
    {
        string rhs = ItemRPs[itemid][i]->rhs;
        char lookahead = rhs[rhs.find('.') + 1];
        add_closure(lookahead, ItemRPs[itemid], grammar);
    }

    int nextPos;
    char lookahead;
    char lhs;
    string rhs;
    AugmentedProduction *prod;

    for (int i = 0; i < ItemRPs[itemid].Size(); i++)
    {
        lhs = ItemRPs[itemid][i]->lhs;
        rhs = ItemRPs[itemid][i]->rhs;
        string production = string(&lhs, 1) + "->" + rhs;

        lookahead = rhs[rhs.find('.') + 1];
        if (lookahead == '\0')
        {
            printf("\t%-20s\n", &production[0]);
            continue;
        }

        if (ItemRPs[itemid].gotos.find(lookahead) == ItemRPs[itemid].gotos.end())
        {
            if (globalGoto.find(production) == globalGoto.end())
            {
                ItemRPs.push_back(ItemRP());
                string newRhs = rhs;
                int atpos = newRhs.find('.');
                swap(newRhs[atpos], newRhs[atpos + 1]);
                ItemRPs.back().Push(new AugmentedProduction(lhs, newRhs));
                ItemRPs[itemid].gotos[lookahead] = ItemRPs.size() - 1;
                globalGoto[production] = ItemRPs.size() - 1;
            }
            else
            {
                ItemRPs[itemid].gotos[lookahead] = globalGoto[production];
            }
            printf("\t%-20s goto(I%d,%c)\n", &production[0], globalGoto[production], lookahead);
        }
        else
        {
            int at = rhs.find('.');
            swap(rhs[at], rhs[at + 1]);
            int nextItem = ItemRPs[itemid].gotos[lookahead];
            if (!ItemRPs[nextItem].Contains(string(&lhs, 1) + "->" + rhs))
                ItemRPs[nextItem].Push(new AugmentedProduction(lhs, rhs));
            swap(rhs[at], rhs[at + 1]);
            printf("\t%-20s\n", &production[0]);
        }
    }
}