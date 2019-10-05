#include <iostream>
#include <vector>

using namespace std;

//transition table
static int transitonTable[4][2] = { {1, 0}, {1, 2}, {1, 3}, {1, 0} };

int main()
{
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 2; c++)
		{
			cout << transitonTable[r][c] << " ";
		}

		cout << endl;
	}


	return 0;
}