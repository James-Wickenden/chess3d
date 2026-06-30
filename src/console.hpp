#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <stack>

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
    std::vector<int> get_input_target_square(LogicEngine::Chessboard *cb, std::stack<LogicEngine::Chessboard> *board_stack);
    std::vector<int> get_input_destination_square(std::vector<LogicEngine::Square> vms);
	void menu_handler();
	void print_board(LogicEngine::Chessboard chessboard, std::vector<LogicEngine::Square> valid_moves, LogicEngine::Gamestate gamestate);
    void print_game_load_header(std::string active_player_str, LogicEngine::Gamestate gs, std::string white_name, std::string black_name);
    LogicEngine::Piece get_pawn_promotion_terminal();
}
