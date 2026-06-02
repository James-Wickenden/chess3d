#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <filesystem>

#include "logic.hpp"

namespace ConsoleEngine
{
	void debug_print(std::vector<std::string> output);
	void menu_handler();
	void print_board(LogicEngine::Chessboard chessboard, std::vector<LogicEngine::Square> valid_moves, LogicEngine::Gamestate gamestate);
}
