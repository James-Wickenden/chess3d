// chess3d.cpp

#include "chess3d.hpp"

using namespace std;
using namespace LogicEngine;
using namespace ConsoleEngine;
using namespace FileHandler;
using namespace Renderer;
namespace fs = std::filesystem;


int main()
{
	srand(time(NULL));
	init_window();
	menu_handler();
	
	return 0;
}
