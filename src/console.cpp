// logic.cpp

#include "console.hpp"

using namespace std;
using namespace LogicEngine;
using namespace ConsoleEngine;
using namespace FileHandler;
namespace fs = std::filesystem;

static string random_string(size_t length);
static string get_formatted_date();
bool is_square_input_valid(string input);
int get_int_input(string request_phrase, int min_allowed, int max_allowed);

// Controls the level of debug output.
// Levels are DEBUG, INFO, ERROR, NONE.
Level current_log_level = Level::DEBUG;


// Compare the log level of the message to be printed to the current log level, and if so then print it to the console.
void ConsoleEngine::debug_print(Level log_level, vector<string> output)
{
	if (current_log_level > log_level) return;

	for (int i = 0; i < output.size(); i++)
	{
		cout << output[i];
	}
}


// Get the target square to move from, and validate the input. 
// Also handles special inputs for undoing moves, saving games and exiting to the menu.
vector<int> ConsoleEngine::get_input_target_square(Chessboard *cb, stack<Chessboard> *board_stack)
{
	string move_choice;

	while (true)
	{
		// first select the piece to move and get a list of the squares the piece can move to
		debug_print(Level::INFO, { "Input target square or choice: " });
		getline(cin, move_choice);

		if (move_choice == "") continue;
		else if (move_choice == "undo")
		{
			// Handle the 'undo' operation.
			if (board_stack->size() <= 1)
			{
				debug_print(Level::ERROR, { "\033[1;31mNo more moves to undo.\033[0m\n" });
				continue;
			}
			debug_print(Level::INFO, { "\033[1;31mUndoing move.\033[0m\n" });

			board_stack->pop();
			*cb = board_stack->top();
			print_board(*cb, vector<Square>(), Gamestate::NORMAL);

			continue;
		}
		else if (move_choice == "save")
		{
			// Handle the 'save' operation.
			debug_print(Level::INFO, { "\033[1;31mSaving game.\033[0m\n" });
			save_game(*cb);

			continue;
		}
		else if (move_choice == "exit")
		{
			debug_print(Level::INFO, { "\x1B[2J\x1B[H" });
			debug_print(Level::INFO, { "\033[1;32mReturning to menu...\033[0m\n" });

			// Indicates exiting the main loop and returning to the menu.
			return vector<int> { -1 };
		}
		else
		{
			// Validate and handle the inputted move
			if (!is_square_input_valid(move_choice))
			{
				debug_print(Level::ERROR, { "\033[1;31mInvalid input. Should be of form [a-h][1-8]\033[0m\n" });
				continue;
			}

			// convert the input into a position on the board, and check that the piece on that square belongs to the active player
			vector<int> target_position = convert_chessboard_square_to_int(move_choice);
			if (cb->board[target_position[0]][target_position[1]].colour != cb->active_player)
			{
				debug_print(Level::INFO, { "\x1B[2J\x1B[H" });
				debug_print(Level::INFO, { "\033[1;31mPiece selected belongs to opposite player\033[0m\n" });
				continue;
			}

			// find and print the list of moves from that square's piece
			debug_print(Level::DEBUG, { "\x1B[2J\x1B[H" });
			debug_print(Level::DEBUG, { "Moves: " });

			vector<Square> vms = cb->find_valid_moves(cb->board[target_position[0]][target_position[1]]);
			// if the chosen piece has no valid moves, restart the loop
			if (vms.size() == 0)
			{
				debug_print(Level::ERROR, { "\x1B[2J\x1B[H" });
				debug_print(Level::ERROR, { "\033[1;31mNo valid moves for that piece\033[0m\n" });
				continue;
			}

			return target_position;
		}
	}
}


vector<int> ConsoleEngine::get_input_destination_square(vector<Square> vms)
{
	while (true)
	{
		string destination_square;

		debug_print(Level::INFO, { "\nChoose destination square, or type 'back' to rechoose a piece to move: " });
		getline(cin, destination_square);

		if (destination_square == "") continue;
		// Handle the 'back' operation.
		else if (destination_square == "back")
		{
			debug_print(Level::INFO, { "\033[1;31mReturning to piece selection...\033[0m\n" });
			return vector<int> { -1 };
		}

		if (!is_square_input_valid(destination_square))
		{
			debug_print(Level::ERROR, { "\033[1;31mInvalid input. Should be of form [a-h][1-8]\033[0m\n" });
			continue;
		}

		vector<int> destination_position = convert_chessboard_square_to_int(destination_square);

		// make sure we can move to that square with the piece we selected. if not, restart the loop.
		bool found_valid_move_match = false;
		for (int i = 0; i < vms.size(); i++)
		{
			if (vms[i].row == destination_position[0] && vms[i].col == destination_position[1])
			{
				found_valid_move_match = true;
			}
		}
		if (!found_valid_move_match)
		{
			debug_print(Level::ERROR, { "\033[1;31mInvalid move for that piece\033[0m\n" });
			continue;
		}

		return destination_position;
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

		int menu_choice = get_int_input("\033[1;32mEnter choice: \033[0m", 1, 3);

		Chessboard cb;
		string white_name, black_name, tmp_name;

		map<int, string> test_board_map = {
					{ 1, "positions/test_position.txt" },
					{ 2, "positions/test_ep.txt" },
					{ 3, "positions/test_notation.txt" },
					{ 4, "positions/test_castling.txt" },
					{ 5, "positions/test_promotion.txt" },
					{ 6, "positions/test_stalemate.txt" }
		};

		switch (menu_choice)
		{
		case 1:
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
		case 2:
			debug_print(Level::INFO, { "Select test board [1/2/3/4/5/6]:\n" });
			menu_choice = get_int_input("  1. position\n  2. en passant\n  3. notation\n  4. castling\n  5. promotion\n  6. stalemate\n" , 1, 6);
			debug_print(Level::INFO, { "\x1B[2J\x1B[H" });

			cb = Chessboard(test_board_map[menu_choice]);
			cb.white_name = "test_white";
			cb.black_name = "test_black";
			cb.date = get_formatted_date();

			loop_board(cb, Gamestate::NORMAL);
			break;
		case 3:
			fs::path p = fs::current_path().append("games");
			map<int, string> id_game_map;
			int cur_id = 1;
			for (const auto& entry : fs::directory_iterator(p))
			{
				string gamepath = entry.path().string();
				string base_filename = gamepath.substr(gamepath.find_last_of("/\\") + 1);
				debug_print(Level::INFO, { to_string(cur_id), ".  ", base_filename + "\n" });
				id_game_map[cur_id] = base_filename;
				cur_id++;
			}

			menu_choice = get_int_input("\nSelect game with id: ", 1, cur_id - 1);
			string chosen_file = id_game_map[menu_choice];
			if (exists(p.append(chosen_file)))
				load_game(p);
			break;
		}
	}
}


// Print the header for a new or loaded game, including the player names, active player and instructions for inputting moves.
void ConsoleEngine::print_game_load_header(string active_player_str, Gamestate gs, string white_name, string black_name)
{
	if (gs == Gamestate::NEWGAME) debug_print(Level::INFO, { "\033[1;32mNEW GAME\033[0m\n" });
	else debug_print(Level::INFO, { "\033[1;32mLOADED GAME\033[0m\n" });
	debug_print(Level::INFO, { "\033[1;33m" + white_name + " vs " + black_name + "\n" });
	debug_print(Level::INFO, { "\033[1;33m" + active_player_str + " to move.\n" });
	debug_print(Level::INFO, { "When inputting target square:\n  - Type 'undo' to undo move.\n  - Type 'save' to save PGN file.\n  - Type 'exit' to return to menu.\033[0m\n\n" });
}


// Promote a pawn within the terminal by calling for input, and halting until a valid input is made. Then return the selection.
Piece ConsoleEngine::get_pawn_promotion_terminal()
{
	string promotion_choice;
	map<string, Piece> piece_string_map =
	{
		{ "QUEEN",  Piece::QUEEN },
		{ "ROOK",   Piece::ROOK },
		{ "BISHOP", Piece::BISHOP },
		{ "KNIGHT", Piece::KNIGHT }
	};

	while (true)
	{
		debug_print(Level::INFO, { "Choose piece to promote to [Queen/Rook/Bishop/Knight]: " });
		getline(cin, promotion_choice);
		std::transform(promotion_choice.begin(), promotion_choice.end(), promotion_choice.begin(), ::toupper);
		if (piece_string_map.count(promotion_choice) > 0) return piece_string_map.at(promotion_choice);
	}

	return Piece::EMPTY;
}


// Get an integer input from the user, and validate that it is an integer within the specified bounds. 
// If not, keep requesting input until a valid input is given, and then return that input.
int get_int_input(string request_phrase, int min_allowed, int max_allowed)
{
	debug_print(Level::INFO, { request_phrase });
	string input;
	while (true)
	{
		getline(cin, input);
		try
		{
			int int_input = stoi(input);
			if (int_input >= min_allowed && int_input <= max_allowed) 
				return int_input;
			else
			{
				debug_print(Level::ERROR, { "\033[1;31mInvalid input. Should be an integer between " + to_string(min_allowed) + " and " + to_string(max_allowed) + ".\033[0m\n" });
				debug_print(Level::INFO, { request_phrase });
				continue;
			}
		}
		catch (exception& e)
		{
			debug_print(Level::ERROR, { "\033[1;31mInvalid input. Should be an integer between " + to_string(min_allowed) + " and " + to_string(max_allowed) + ".\033[0m\n" });
			debug_print(Level::INFO, { request_phrase });
			continue;
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


// For a given square, determine if the input is in the form of a valid chessboard square, e.g. "a1", "h8", etc.
bool is_square_input_valid(string input)
{
	if (input.length() != 2) return false;
	char file = input[0];
	char rank = input[1];
	if (file < 'a' || file > 'h') return false;
	if (rank < '1' || rank > '8') return false;
	return true;
}