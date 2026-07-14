#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "logic.hpp"
#include "console.hpp"

namespace FileHandler
{   
    std::string read_board_setup_file(std::string filename);
    bool save_game(LogicEngine::Chessboard cb);
    std::tuple<LogicEngine::Chessboard, LogicEngine::Gamestate> parse_pgn(LogicEngine::Chessboard cb, std::vector<std::string> pgn_moves);
    void load_game(std::filesystem::path gamepath);
}
