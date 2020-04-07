// Name         : Taylor Pedretti
// Lab          : Lab 5
/* Description  : You will need the programs from Lab4 and Lab6.

Write a program that constructs the ACTION and GOTO parse
tables for an SLR parser. Method for doing this is
algorithm 4.46 (Purple Dragon Book) pg 253, Algorithm
4.8 (Red Dragon Book), pg 227. Algorithm to do this is
briefly described in Feb 16 notes, Shift-Reduce parser
that uses these tables is described in Feb 9 notes.

Once you have the ACTION and GOTO tables, it is easy
to construct your own SLR parser.*/

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
void actionGoto(vector<ItemRP>&, AugmentedGrammar&, int&, gotoMap&);

string file = "test.txt";
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
                if (isTerminal(rhs[0]) == false)
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

    actionGoto(ItemRPs, AugGrammar, itemid, globalGoto);

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

    followMap[nonTerminal[0]].insert('$');

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
                    if (firstChar == 'e')
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

void actionGoto(vector<ItemRP>& ItemRPs, AugmentedGrammar& grammar, int& itemid, gotoMap& globalGoto)
{
    //maps for both grammar, action and goto table
    map <string, int> augGrammar;
    map < string, map <string, string > > action;
    map < string, map <string, string > > gotoTable;

    char lookahead;
    int state = 0;

    for (int i = 0; i < ItemRPs[0].Size(); i++)
    {
        string lhs = "";
        lhs.push_back(ItemRPs[0][i]->lhs);
        string temp = lhs + "->" + ItemRPs[0][i]->rhs;
        temp.erase(remove(temp.begin(), temp.end(), '.'), temp.end());
        augGrammar[temp] = i;
    }

    for (int i = 0; i < terminal.size(); i++)
    {
        string temp = "";
        temp.push_back(terminal[i]);
        action["0State"][temp] = temp;

    }

    for (int i = 0; i < nonTerminal.size(); i++)
    {

        string temp = "";
        temp.push_back(nonTerminal[i]);
        gotoTable["0State"][temp] = temp;
    }

    while (state < itemid)
    {
        for (int i = 0; i < terminal.size(); i++)
        {
            string firstmap;
            string temp = "";
            if (state >= 10)
                firstmap = "I-" + to_string(state);
            else
                firstmap = "I- " + to_string(state);
            temp.push_back(terminal[i]);
            action[firstmap][temp] = " ";

        }

        for (int i = 0; i < nonTerminal.size(); i++)
        {
            string firstmap;
            if (state >= 10)
                firstmap = "I-" + to_string(state);
            else
                firstmap = "I- " + to_string(state);
            string temp = "";
            temp.push_back(nonTerminal[i]);
            gotoTable[firstmap][temp] = " ";
        }

        for (int i = 0; i < ItemRPs[state].Size(); i++)
        {
            char lhs = ItemRPs[state][i]->lhs;
            string rhs = ItemRPs[state][i]->rhs;
            string production = "";
            production.push_back(lhs);
            char lookahead = rhs[rhs.find('.') + 1];

            if (lookahead == '\0')
            {
                lookahead = rhs[rhs.find('.') - 1];

                if (isTerminal(lookahead))
                {
                    string firstmap;
                    for (auto it = followMap[lhs].begin(); it != followMap[lhs].end(); it++)
                    {
                        string temp = "";
                        temp.push_back(lhs);
                        temp += "->" + rhs;
                        temp.erase(remove(temp.begin(), temp.end(), '.'), temp.end());
                        int reduceto = augGrammar[temp];
                        if (reduceto == 0)
                            continue;
                        if (state >= 10)
                            firstmap = "I-" + to_string(state);
                        else
                            firstmap = "I- " + to_string(state);
                        string second = "";
                        second.push_back(*it);
                        if(reduceto == 0)                        
                            action[firstmap][second] += "acc";
                        else
                            action[firstmap][second] += "r" + to_string(reduceto);
                    }

                }
                else
                {

                    string firstmap;
                    for (auto it = followMap[lhs].begin(); it != followMap[lhs].end(); it++)
                    {
                        string temp = "";
                        temp.push_back(lhs);
                        temp += "->" + rhs;
                        temp.erase(remove(temp.begin(), temp.end(), '.'), temp.end());
                        int reduceto = augGrammar[temp];
                        if (reduceto == 0)
                            continue;
                        if (state >= 10)
                            firstmap = "I-" + to_string(state);
                        else
                            firstmap = "I- " + to_string(state);
                        string second = "";
                        second.push_back(*it);
                        if (reduceto == 1)
                            action[firstmap][second] += "acc";
                        else
                            action[firstmap][second] += "r" + to_string(reduceto);
                    }
                }
            }

            if (isTerminal(lookahead))
            {
                string firstmap;
                int stateto = globalGoto[production + "->" + rhs];
                if (stateto == 0)
                    continue;
                if (state >= 10)
                    firstmap = "I-" + to_string(state);
                else
                    firstmap = "I- " + to_string(state);
                string second = "";
                second.push_back(lookahead);
                action[firstmap][second] += "s" + to_string(stateto);
            }
            else
            {
                string firstmap;
                int stateto = globalGoto[production + "->" + rhs];
                if (stateto == 0)
                    continue;
                if (state >= 10)
                    firstmap = "I-" + to_string(state);
                else
                    firstmap = "I- " + to_string(state);
                string second = "";
                second.push_back(lookahead);
                gotoTable[firstmap][second] += to_string(stateto);
            }


        }
        state++;

    }

    cout << "\t" << "ACTION" << "\t" << "GOTO" << endl;
    auto it3 = gotoTable.begin();

    for (auto it = action.begin(); it != action.end(); it++)
    {
        cout << "\n" << it->first << "\t";
        {
            for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                cout << " | " << it2->second << "\t";
            }
            for (auto it4 = it3->second.begin(); it4 != it3->second.end(); it4++)
            {
                cout << " || " << it4->second << "\t";
            }
        }
        if(it3 != gotoTable.end())
            it3++;
    }
    cout << endl;
}
