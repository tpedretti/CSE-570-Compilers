#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

class ExtendedProduction
{
public:
    char lhs;
    string rhs;

    ExtendedProduction() {}
    ExtendedProduction(char _lhs, string _rhs) : lhs(_lhs), rhs(_rhs) {}
};

class ItemRP
{

    private:
        // list of productions
        vector <ExtendedProduction* > productions;

    public:
        // list of out-edges
        map<int, char> gotos;

        ItemRP() {}
        ~ItemRP() {}

        // add production
        void Push(ExtendedProduction *p)
		{
			productions.push_back(p);
		}

        // return the number of productions
        int Size()
		{
			return int(productions.size());
		}

        // return item if it contains it.
        bool Contains(string production)
		{
			for (auto it = productions.begin(); it != productions.end(); it++)
			{
				string existing = string(&(*it)->lhs, 1) + "->" + (*it)->rhs;
				//cout << " Comparing: " << thisStr << " , " << prodStr << endl;
				if (strcmp(production.c_str(), existing.c_str()) == 0)
					return true;
			}
			return false;
		}

        // overloaded index operator; access pointer to production.
        ExtendedProduction* operator[](const int index)
		{
			return productions[index];
		}
};
