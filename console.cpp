// logic.cpp

#include "console.hpp"
#include "logic.hpp"

using namespace ConsoleEngine;
using namespace LogicEngine;
using namespace std;
namespace fs = std::filesystem;

static string random_string(size_t length);
static string get_formatted_date();


bool show_debug = true; // set to true to show debug output, false to hide it


void ConsoleEngine::debug_print(vector<string> output)
{
	if (!show_debug) return;

	for (int i = 0; i < output.size(); i++)
	{
		cout << output[i];
	}
}


// Defines the text based entry point, including loading games.
void ConsoleEngine::menu_handler()
{
	bool valid_menu_choice = false;
	while (!valid_menu_choice)
	{
		debug_print({ "\x1B[2J\x1B[H" });
		debug_print({ "\033[1;33mchess3d by Mammoth [https://github.com/James-Wickenden/chess3d]\033[0m\n" });
		debug_print({ "Options [1/2/3]:\n" });
		debug_print({ "    1. New game\n    2. Test position\n    3. Load game\n\n" });
		debug_print({ "\033[1;32mEnter choice: \033[0m" });

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
			debug_print({ "White player name [" + white_name + "]: " });
			getline(cin, tmp_name);
			if (tmp_name != "") white_name = tmp_name;
			debug_print({ "Black player name [" + black_name + "]: " });
			getline(cin, tmp_name);
			if (tmp_name != "") black_name = tmp_name;
			debug_print({ "\x1B[2J\x1B[H" });

			cb = Chessboard();
			cb.white_name = white_name;
			cb.black_name = black_name;
			cb.date = get_formatted_date();

			loop_board(cb, Gamestate::NEWGAME);
			break;
		case '2':
			debug_print({ "Select test board [1/2/3/4/5/6]:\n" });
			debug_print({ "  1. position\n  2. en passant\n  3. notation\n  4. castling\n  5. promotion\n  6. stalemate\n" });
			getline(cin, submenu_choice);
			debug_print({ "\x1B[2J\x1B[H" });

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
				debug_print({ to_string(cur_id), ".  ", base_filename + "\n" });
				id_game_map[to_string(cur_id)] = base_filename;
				cur_id++;
			}

			debug_print({ "\nSelect game with id: " });
			getline(cin, submenu_choice);
			string chosen_file = id_game_map[submenu_choice];
			if (fs::exists(p.append(chosen_file)))
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