// logic.cpp

#include "console.hpp"

using namespace ConsoleEngine;
using namespace std;

bool show_debug = true; // set to true to show debug output, false to hide it

void ConsoleEngine::debug_print(vector<string> output)
{
	if (!show_debug) return;

	for (int i = 0; i < output.size(); i++)
	{
		cout << output[i];
	}
}