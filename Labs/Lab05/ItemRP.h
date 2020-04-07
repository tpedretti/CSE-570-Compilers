#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

class AugmentedProduction
{
public:
    char lhs;
    string rhs;

    AugmentedProduction() {}
    AugmentedProduction(char _lhs, string _rhs) : lhs(_lhs), rhs(_rhs) {}
};

class ItemRP
{

    private:
        vector <AugmentedProduction* > productions;

    public:
        map<int, char> gotos;

        ItemRP() {}
        ~ItemRP() {}

        void Push(AugmentedProduction *p)    
		{
			productions.push_back(p);
		}

        // return the number of productions
        int Size()  
		{
			return int(productions.size());
		}

        // return item contains the production
        bool Contains(string production)
		{
			for (auto it = productions.begin(); it != productions.end(); it++)
			{
				string existing = string(&(*it)->lhs, 1) + "->" + (*it)->rhs;
				if (strcmp(production.c_str(), existing.c_str()) == 0)
					return true;
			}
			return false;
		}

        AugmentedProduction* operator[](const int index)
		{
			return productions[index];
		}
};