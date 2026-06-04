// file_handler.cpp

#include "file_handler.hpp"

using namespace std;
using namespace LogicEngine;
using namespace ConsoleEngine;
using namespace FileHandler;
namespace fs = std::filesystem;

vector<string> split(const string& s, const string& delimiter);
vector<Square> loop_potential_pieces_with_context(int piece_val, vector<Square> potential_movers, string search_type);
vector<Square> use_extra_notation_to_find_mover(char pgn_indicator, vector<Square> potential_movers);


// Take in a string for a text file, read the board, remove newline characters, and return it
string FileHandler::read_board_setup_file(string filename)
{
	// todo: replace this with local pathing
	string path_to_file = "C:\\Users\\snowi\\source\\repos\\chess3d\\";
	ifstream file(path_to_file + filename);
	string line;
	string file_contents;
	while (getline(file, line))
	{
		file_contents += line;
	}

	return file_contents;
}


// Attempt to save the game. Returns true if successful, and false if not.
// The game is saved as a .PGN file containing notation and metadata.
bool FileHandler::save_game(Chessboard cb)
{
	string game_string_data = "";

	// ensure that the game directory exists
	fs::path game_path = fs::current_path().append("games");
	if (!fs::is_directory(game_path))
		fs::create_directory(game_path);

	// Metadata starts with seven tag pairs, formatted as [tag "value"]
	// These are event, site, date, round, white, black, result
	string tag_roster = "";
	tag_roster += "[Event \"James-Wickenden/chess3d Match\"]\n";
	tag_roster += "[Site \"https://github.com/James-Wickenden/chess3d\"]\n";
	tag_roster += "[Date \"" + cb.date + "\"]\n";
	tag_roster += "[Round \"1\"]\n";
	tag_roster += "[White \"" + cb.white_name + "\"]\n";
	tag_roster += "[Black \"" + cb.black_name + "\"]\n";
	tag_roster += "[Result \"" + cb.result + "\"]\n\n";

	game_string_data += tag_roster;

	// Then, we dump the PGN as stored in the chessboard object, ending with the game result.
	game_string_data += cb.notation;
	game_string_data += ' ' + cb.result + '\n';

	ofstream gamefile;
	fs::path filename = fs::current_path().append("games").append(
		cb.date + "_"
		+ cb.white_name + "_"
		+ cb.black_name + ".pgn");
	gamefile.open(filename);
	gamefile << game_string_data;
	gamefile.close();

	return true;
}


tuple<Chessboard, Gamestate> FileHandler::parse_pgn(Chessboard cb, vector<string> pgn_moves)
{
	// Each element in the pgn_moves vector is a ply. Every other element will contain the move number, i.e. 4.e4
	// To parse, we take off this number if necessary, then use the context of the current board to get the target and destination position.
	Gamestate gs;
	if (pgn_moves[0] == "") return make_tuple(cb, Gamestate::NEWGAME);

	for (int i = 0; i < pgn_moves.size(); i++)
	{
		string cur_pgn = pgn_moves[i];
		if (cur_pgn == "") continue;
		cur_pgn = split(cur_pgn, ".").back();
		ConsoleEngine::debug_print(Level::DEBUG, { "parsing " + cur_pgn });

		// setup the basic move data we want to extract
		Colour active_colour = ((i % 2) == 0) ? Colour::WHITE : Colour::BLACK;

		// determine if the move represents anything notable
		map<string, bool> move_config =
		{
			{ "is_capture", false },
			{ "is_check", false },
			{ "is_checkmate", false },
			{ "is_castling", false },
			{ "is_promotion", false }
		};
		Piece promotion_choice = Piece::EMPTY;

		map<char, Piece> piece_map = {
			{ 'R', Piece::ROOK   },
			{ 'N', Piece::KNIGHT },
			{ 'B', Piece::BISHOP },
			{ 'Q', Piece::QUEEN  },
			{ 'K', Piece::KING   }
		};

		if (cur_pgn.find("x") != string::npos) move_config["is_capture"] = true;
		if (cur_pgn.find("+") != string::npos) move_config["is_check"] = true;
		if (cur_pgn.find("#") != string::npos) move_config["is_checkmate"] = true;
		if (cur_pgn.find("O") != string::npos) move_config["is_castling"] = true;
		for (auto const& piece_map_items : piece_map)
		{
			if (cur_pgn.back() == piece_map_items.first)
			{
				move_config["is_promotion"] = true;
				promotion_choice = piece_map_items.second;
			}
		}

		if (!move_config["is_castling"])
		{
			// remove special characters from the end of the move string
			cur_pgn = split(split(split(cur_pgn, "+")[0], "#")[0], "ep")[0];
			if (move_config["is_promotion"])
				cur_pgn = cur_pgn.erase(cur_pgn.length() - 1);

			// the first character should represent the moving piece, so we can extract that
			Piece moving_piece = Piece::PAWN;
			if (piece_map.find(cur_pgn[0]) != piece_map.end()) moving_piece = piece_map[cur_pgn[0]];

			// the last two characters should now represent the destination square, so we can extract that too
			string dest_square_str = cur_pgn.substr(cur_pgn.length() - 2);
			vector<int> dest_square = convert_chessboard_square_to_int(dest_square_str);

			// Find the valid move lists for each player
			cb.valid_moves[Colour::WHITE] = find_all_attackable_squares(cb, Colour::WHITE, Piece_Finding_Mode::VALID);
			cb.valid_moves[Colour::BLACK] = find_all_attackable_squares(cb, Colour::BLACK, Piece_Finding_Mode::VALID);

			cb.attacking_moves[Colour::WHITE] = find_all_attackable_squares(cb, Colour::WHITE, Piece_Finding_Mode::ATTACKABLE);
			cb.attacking_moves[Colour::BLACK] = find_all_attackable_squares(cb, Colour::BLACK, Piece_Finding_Mode::ATTACKABLE);

			vector<Square> potential_movers;
			// Finding the piece that moved
			cb.active_player = active_colour;
			vector<tuple<Square, vector<Square>>> all_movers = (move_config["is_capture"] ? cb.attacking_moves[cb.active_player] : cb.valid_moves[cb.active_player]);
			for (int i = 0; i < all_movers.size(); i++)
			{
				Square potential_mover = get<0>(all_movers[i]);
				if (is_dest_square_attackable_by_piece(all_movers[i], dest_square))
				{
					if (potential_mover.piece == moving_piece)
					{
						ConsoleEngine::debug_print(Level::DEBUG, { '\n' + to_string(potential_mover.row) + " " + to_string(potential_mover.col) + "\n" });
						potential_movers.push_back(potential_mover);
					}
				}
			}

			Square mvr;
			// If only one piece can move to the destination square: we can just take that move.
			if (potential_movers.size() == 1) mvr = potential_movers[0];
			else
			{
				// If multiple pieces can move to that square: we must analyse the move PGN more to find the moving piece.
				// So, first append a P to the start of the string in the case of a moving pawn for PGN length consistency.
				// Then, remove the dest square from the end and the x if a move is a capture, and remove the first character.
				// Whats left is the extra notation.
				if (moving_piece == Piece::PAWN) cur_pgn = "P" + cur_pgn;
				string trimmed_pgn = cur_pgn.substr(1, cur_pgn.size() - 3);
				trimmed_pgn.erase(remove(trimmed_pgn.begin(), trimmed_pgn.end(), 'x'), trimmed_pgn.end());

				// Now, take the trimmed_pgn and match it to the potential_movers
				if (trimmed_pgn.size() == 1)
				{
					char pgn_indicator = trimmed_pgn[0];
					mvr = use_extra_notation_to_find_mover(pgn_indicator, potential_movers)[0];
				}
				else
				{
					// two lists detailing all the potential dest squares for row and col,
					// so find the square on both lists and take that piece.
					vector<vector<Square>> mvrs = {
						use_extra_notation_to_find_mover(trimmed_pgn[0], potential_movers),
						use_extra_notation_to_find_mover(trimmed_pgn[1], potential_movers) };

					for (vector<Square>::iterator i = mvrs[0].begin(); i != mvrs[0].end(); ++i)
					{
						if (find(mvrs[1].begin(), mvrs[1].end(), *i) != mvrs[1].end())
						{
							mvr = *i;
						}
					}
				}
			}

			switch_pieces(&cb, { mvr.row, mvr.col }, dest_square);
			cb.move_no++;

			if (move_config["is_promotion"])
			{
				cb.board[dest_square[0]][dest_square[1]].piece = promotion_choice;
			}
		}
		else
		{
			// this is the case when there is castling

			bool queenside = true;
			if (cur_pgn == "O-O") queenside = false;

			Square player_king = active_colour == Colour::WHITE ? cb.board[0][4] : cb.board[7][4];
			if (queenside)
			{
				switch_pieces(&cb, { player_king.row, player_king.col }, vector<int>({ player_king.row, 2 }));
				switch_pieces(&cb, { player_king.row, 0 }, vector<int>({ player_king.row, 3 }));
			}
			else
			{
				switch_pieces(&cb, { player_king.row, player_king.col }, vector<int>({ player_king.row, 6 }));
				switch_pieces(&cb, { player_king.row, 7 }, vector<int>({ player_king.row, 5 }));
			}
		}

		gs = Gamestate::NORMAL;
		if (move_config["is_check"]) gs = Gamestate::CHECK;
		if (move_config["is_checkmate"]) gs = Gamestate::CHECKMATE;
	}

	Colour opp_colour = (cb.active_player == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;
	cb.active_player = opp_colour;

	return tuple<Chessboard, Gamestate>(cb, gs);
}


// Parse a game file and load it, then read through all the PGN moves to arrive at the current gamestate.
void FileHandler::load_game(fs::path gamepath)
{
	// read the PGN file
	ifstream t(gamepath);
	string gamedata((istreambuf_iterator<char>(t)),
		istreambuf_iterator<char>());

	// split the PGN file into its metadata and movedata
	vector<string> gamedata_elements = split(gamedata, "\n");

	// assign metadata to the game object
	Chessboard cb = Chessboard();
	cb.white_name = split(gamedata_elements[4], "\"")[1];
	cb.black_name = split(gamedata_elements[5], "\"")[1];
	cb.date = split(gamedata_elements[2], "\"")[1];
	cb.result = split(gamedata_elements[6], "\"")[1];
	// todo - note we need to split based on the result here to handle viewing finished games.

	// parse the moves
	string movedata = gamedata_elements[8];
	cb.notation = movedata;

	// look for characters in the PGN that indicate comments.
	// these are not handled and the PGN will be rejected.
	for (char c : {'{', '}', ';'})
	{
		if (movedata.find(c) != string::npos)
		{
			string comment_char(1, c);
			debug_print(Level::ERROR, { "Found comment character: " });
			debug_print(Level::ERROR, { comment_char });
			debug_print(Level::ERROR, { " at pos: " + to_string(gamedata.find(c)) + "\n" });
			debug_print(Level::ERROR, { "Remove comments from PGN before loading.\n    Press ENTER:" });
			getline(cin, movedata);
			return;
		}
	}

	vector<string> pgn_moves = split(movedata, " ");

	tuple<Chessboard, Gamestate> game_state;
	game_state = parse_pgn(cb, pgn_moves);

	debug_print(Level::DEBUG, { "parsed\n" });
	loop_board(get<0>(game_state), get<1>(game_state));
}


vector<string> split(const string& s, const string& delimiter)
{
	vector<string> tokens;
	size_t last = 0;
	size_t next = 0;
	while ((next = s.find(delimiter, last)) != string::npos)
	{
		string token = s.substr(last, next - last);
		tokens.push_back(token);
		last = next + 1;
	}

	tokens.push_back(s.substr(last));

	return tokens;
}


vector<Square> loop_potential_pieces_with_context(int piece_val, vector<Square> potential_movers, string search_type)
{
	vector<Square> result;
	for (int i = 0; i < potential_movers.size(); i++)
	{
		int potential_index = (search_type == "row" ? potential_movers[i].row : potential_movers[i].col);
		if (piece_val == potential_index) result.push_back(potential_movers[i]);
	}

	return result;
}


vector<Square> use_extra_notation_to_find_mover(char pgn_indicator, vector<Square> potential_movers)
{
	if (pgn_indicator >= 'a' && pgn_indicator <= 'h')
	{
		int piece_col = pgn_indicator - 'a';
		return loop_potential_pieces_with_context(piece_col, potential_movers, "col");
	}
	else
	{
		int piece_row = pgn_indicator - '1';
		return loop_potential_pieces_with_context(piece_row, potential_movers, "row");
	}
}
