#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <filesystem>

#include "logic.hpp"
#include "file_handler.hpp"

namespace ConsoleEngine
{
    enum class Level
    {
        DEBUG,
        INFO,
        ERROR,
        NONE
    };

	void debug_print(Level log_level, std::vector<std::string> output);
	void menu_handler();
	void print_board(LogicEngine::Chessboard chessboard, std::vector<LogicEngine::Square> valid_moves, LogicEngine::Gamestate gamestate);
}
