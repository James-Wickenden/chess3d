// chess3d.cpp

#include "chess3d.hpp"

using namespace std;
using namespace LogicEngine;
using namespace ConsoleEngine;
using namespace FileHandler;
namespace fs = std::filesystem;


int main()
{
	srand(time(NULL));
	menu_handler();

	return 0;
}
