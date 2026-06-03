// logic.cpp

#include "console.hpp"

using namespace std;
using namespace LogicEngine;
using namespace ConsoleEngine;
using namespace FileHandler;
namespace fs = std::filesystem;

static string random_string(size_t length);
static string get_formatted_date();

// Controls the level of debug output.
// Levels are DEBUG, INFO, ERROR, NONE.
Level current_log_level = Level::INFO;


// Compare the log level of the message to be printed to the current log level, and if so then print it to the console.
void ConsoleEngine::debug_print(Level log_level, vector<string> output)
{
	if (current_log_level > log_level) return;

	for (int i = 0; i < output.size(); i++)
	{
		cout << output[i];
	}
}


// Print a coloured board to the console.
void ConsoleEngine::print_board(Chessboard chessboard, vector<Square> valid_moves, Gamestate gamestate)
{
	map<Piece, char> piece_map = {
		{ Piece::EMPTY,  ' ' },
		{ Piece::PAWN,   'P' },
		{ Piece::ROOK,   'R' },
		{ Piece::KNIGHT, 'N' },
		{ Piece::BISHOP, 'B' },
		{ Piece::QUEEN,  'Q' },
		{ Piece::KING,   'K' }
	};
	debug_print(Level::INFO, { "PGN : ", chessboard.notation, "\n"});
	debug_print(Level::INFO, { "Current board : \n\n" });

	for (int i = 0; i < DIM_SIZE; i++)
	{
		string rank(1, '0' + (DIM_SIZE - i));
		debug_print(Level::INFO, { rank, " " });
		for (int j = 0; j < DIM_SIZE; j++)
		{
			/*
			there are different cases for colouring the board: black tile, white tile, black piece, white piece, valid move, invalid move
			first, we colour the square, then draw the piece, then reset the colour

						foreground background
				black        30         40
				red          31         41
				green        32         42
				yellow       33         43
				blue         34         44
				magenta      35         45
				cyan         36         46
				white        37         47
			*/

			string colourcode = "\033[";

			// attacked squares
			if (count(valid_moves.begin(), valid_moves.end(), chessboard.board[DIM_SIZE - (1 + i)][j]) > 0)
			{
				colourcode += "43;"; // yellow bg
			}
			else
			{
				// colour tile black for black square, white for white square
				switch ((i ^ j) & 1)
				{
				case 0:
					colourcode += "40;"; // black bg
					break;
				case 1:
					colourcode += "47;"; // white bg
					break;
				}
			}

			switch (chessboard.board[DIM_SIZE - (1 + i)][j].colour)
			{
			case Colour::WHITE:
				colourcode += "1;31m"; // red, bold fg
				break;
			case Colour::BLACK:
				colourcode += "1;35m"; // purple, bold fg
				break;
			case Colour::EMPTY:
				colourcode += "1m";
				break;
			}

			string print_string = colourcode + piece_map[chessboard.board[DIM_SIZE - (1 + i)][j].piece] + " \033[0m";
			debug_print(Level::INFO, { print_string });
		}
		debug_print(Level::INFO, { "\n" });
	}
	debug_print(Level::INFO, { "\n  a b c d e f g h\n\n" });
	if (gamestate == Gamestate::NORMAL || gamestate == Gamestate::CHECK)
		debug_print(Level::INFO, { "\033[1;32m", (chessboard.active_player == Colour::WHITE ? "WHITE" : "BLACK"), " TO MOVE\033[0m\n\n" });
	return;
}


// Defines the text based entry point, including loading games.
void ConsoleEngine::menu_handler()
{
	bool valid_menu_choice = false;
	while (!valid_menu_choice)
	{
		debug_print(Level::INFO, { "\x1B[2J\x1B[H" });
		debug_print(Level::INFO, { "\033[1;33mchess3d by Mammoth [https://github.com/James-Wickenden/chess3d]\033[0m\n" });
		debug_print(Level::INFO, { "Options [1/2/3]:\n" });
		debug_print(Level::INFO, { "    1. New game\n    2. Test position\n    3. Load game\n\n" });
		debug_print(Level::INFO, { "\033[1;32mEnter choice: \033[0m" });

		string menu_choice;
		getline(cin, menu_choice);
		if (menu_choice.size() == 0) continue;

		Chessboard cb;
		string white_name, black_name, tmp_name, submenu_choice;

		map<char, string> test_board_map = {
					{ '1', "test_positions/test_position.txt" },
					{ '2', "test_positions/test_ep.txt" },
					{ '3', "test_positions/test_notation.txt" },
					{ '4', "test_positions/test_castling.txt" },
					{ '5', "test_positions/test_promotion.txt" },
					{ '6', "test_positions/test_stalemate.txt" }
		};

		switch (menu_choice[0])
		{
		case '1':
			white_name = random_string(8);
			black_name = random_string(8);
			debug_print(Level::INFO, { "White player name [" + white_name + "]: " });
			getline(cin, tmp_name);
			if (tmp_name != "") white_name = tmp_name;
			debug_print(Level::INFO, { "Black player name [" + black_name + "]: " });
			getline(cin, tmp_name);
			if (tmp_name != "") black_name = tmp_name;
			debug_print(Level::INFO, { "\x1B[2J\x1B[H" });

			cb = Chessboard();
			cb.white_name = white_name;
			cb.black_name = black_name;
			cb.date = get_formatted_date();

			loop_board(cb, Gamestate::NEWGAME);
			break;
		case '2':
			debug_print(Level::INFO, { "Select test board [1/2/3/4/5/6]:\n" });
			debug_print(Level::INFO, { "  1. position\n  2. en passant\n  3. notation\n  4. castling\n  5. promotion\n  6. stalemate\n" });
			getline(cin, submenu_choice);
			debug_print(Level::INFO, { "\x1B[2J\x1B[H" });

			cb = Chessboard(test_board_map[submenu_choice[0]]);
			cb.white_name = "test_white";
			cb.black_name = "test_black";
			cb.date = get_formatted_date();

			loop_board(cb, Gamestate::NORMAL);
			break;
		case '3':
			fs::path p = fs::current_path().append("games");
			map<string, string> id_game_map;
			int cur_id = 1;
			for (const auto& entry : fs::directory_iterator(p))
			{
				string gamepath = entry.path().string();
				string base_filename = gamepath.substr(gamepath.find_last_of("/\\") + 1);
				debug_print(Level::INFO, { to_string(cur_id), ".  ", base_filename + "\n" });
				id_game_map[to_string(cur_id)] = base_filename;
				cur_id++;
			}

			debug_print(Level::INFO, { "\nSelect game with id: " });
			getline(cin, submenu_choice);
			string chosen_file = id_game_map[submenu_choice];
			if (exists(p.append(chosen_file)))
				load_game(p);
			break;
		}
	}
}


// Return an alphanumeric string of random characters length n.
static string random_string(size_t length)
{
	auto randchar = []() -> char
		{
			const char charset[] =
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";
			const size_t max_index = (sizeof(charset) - 1);
			return charset[rand() % max_index];
		};
	string str(length, 0);
	generate_n(str.begin(), length, randchar);
	return str;
}


// Return today's date in the form YYYY.MM.DD
static string get_formatted_date()
{
	time_t timestamp;
	time(&timestamp);
	struct tm* timeinfo;
	char formatted_date[12];
	timeinfo = localtime(&timestamp);
	strftime(formatted_date, 12, "%Y.%m.%d", timeinfo);

	return formatted_date;
}