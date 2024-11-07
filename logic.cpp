// logic.cpp

#include "logic.hpp"

using namespace std;
using namespace LogicEngine;

const int DIM_SIZE = 8; // size of the chessboard

// Define constructors for squares based off different input sets.
Square::Square()
{
	colour = Colour::EMPTY;
	piece = Piece::EMPTY;
	row = -1;
	col = -1;
	has_moved = false;
}

Square::Square(int r, int c)
{
	colour = Colour::EMPTY;
	piece = Piece::EMPTY;
	row = r;
	col = c;
	has_moved = false;
}

Square::Square(Piece piece_type, Colour c)
{
	colour = c;
	piece = piece_type;
	row = -1;
	col = -1;
	has_moved = false;
}

// Define equating two squares based on their attributes.
bool Square::operator==(const Square rhs) const
{
	return (colour == rhs.colour)
		&& (piece == rhs.piece)
		&& (row == rhs.row)
		&& (col == rhs.col)
		&& (has_moved == rhs.has_moved);
}

bool Square::operator!=(const Square rhs) const
{
	return !operator==(rhs);
}


// Given two coordinates, return a string of the board coordinates for that square.
string convert_int_to_chessboard_square(int row, int col)
{
	string res = "";
	res += ('a' + row);
	res += ('1' + col);
	return res;
}


// Give a string of the board coordinates, return the two coordinates for that square.
vector<int> convert_chessboard_square_to_int(string position)
{
	vector<int> res;
	res.push_back(position[1] - '1');
	res.push_back(position[0] - 'a');
	return res;
}


// Take a vector<tuple<Square, vector<Square>>> object of the attackable squares and reduce it to a vector of the attacked squares
vector<Square> parse_attackable_squares(vector<tuple<Square, vector<Square>>> attackable_squares)
{
	vector<Square> result;
	for (int i = 0; i < attackable_squares.size(); i++)
	{
		vector<Square> selected_piece_attacked_squares = get<1>(attackable_squares[i]);
		result.insert(result.end(), selected_piece_attacked_squares.begin(), selected_piece_attacked_squares.end());
	}

	return result;
}


// Find the two diagonal squares the pawn can capture on
vector<Square> get_pawn_attacking_squares(Square target, vector<vector<Square>> board, Colour opp_colour, int dir)
{
	vector<Square> pawn_attacking_squares;

	if (target.col > 0) {
		if (board[target.row + dir][target.col - 1].colour == opp_colour) {
			pawn_attacking_squares.push_back(board[target.row + dir][target.col - 1]);
		}
	}
	if (target.col < 7) {
		if (board[target.row + dir][target.col + 1].colour == opp_colour) {
			pawn_attacking_squares.push_back(board[target.row + dir][target.col + 1]);
		}
	}

	return pawn_attacking_squares;
}


// For pawns, look a square ahead in the direction a pawn can move in, or two if the paawn hasn't moved yet.
// Also check for diagonal captures and en passant.
vector<Square> get_prospective_pawn_moves(Square target, vector<vector<Square>> board, Colour opp_colour)
{
	vector<Square> prospective_moves;

	// use +1 for white and -1 for black to find valid squares in the direction (dir) the pawn moves in
	int dir = (target.colour == Colour::WHITE) ? 1 : -1;

	if ((target.colour == Colour::WHITE && target.row < 7) || (target.colour == Colour::BLACK && target.row > 0))
	{
		// moving one square ahead
		if (board[target.row + dir][target.col].colour == Colour::EMPTY)
		{
			prospective_moves.push_back(board[target.row + dir][target.col]);

			// case for pawns on the starting file
			if (!target.has_moved)
			{
				if (board[target.row + (dir * 2)][target.col].colour == Colour::EMPTY)
				{
					prospective_moves.push_back(board[target.row + (dir * 2)][target.col]);
				}
			}
		}

		// in the case where there is an opposite-coloured piece diagonally in front of the pawn: that piece is capturable
		vector<Square> pawn_attacking_Squares = get_pawn_attacking_squares(target, board, opp_colour, dir);
		prospective_moves.insert(prospective_moves.end(), pawn_attacking_Squares.begin(), pawn_attacking_Squares.end());
	}

	//todo: en passant prospective move
	//
	//

	return prospective_moves;
}


// For rooks and bishops, test to see if the prospective square is valid (i.e. not occupied by a piece of the same colour.
// If not valid, set the progress_search flag to false to prevent looking any further in the piece's sightline.
void progress_rook_bishop_search(Square target, Square test_square, Colour opp_colour, vector<Square>* prospective_moves, bool* progress_search)
{
	if (test_square.colour == Colour::EMPTY)
	{
		(*prospective_moves).push_back(test_square);
	}
	else if (test_square.colour == opp_colour)
	{
		(*prospective_moves).push_back(test_square);
		*progress_search = false;
	}
	else if (test_square.colour == target.colour)
	{
		*progress_search = false;
	}
}


// For each cardinal direction outwards from the piece location, we:
// progress forwards in the direction, adding all empty squares to the list of prospective moves
// if we reach a non-empty square:
// if the opposite colour: add it to the list and prevent moving further.
// if the same colour: just prevent moving further
vector<Square> get_prospective_rook_moves(Square target, vector<vector<Square>> board, Colour opp_colour)
{
	vector<Square> prospective_moves;

	// forwards
	bool progress_search = true;
	for (int i = 1; i < DIM_SIZE; i++)
	{
		if ((target.row + i) >= DIM_SIZE) progress_search = false;
		if (!progress_search) continue;
		Square test_square = board[target.row + i][target.col];
		progress_rook_bishop_search(target, test_square, opp_colour, &prospective_moves, &progress_search);
	}

	// backwards
	progress_search = true;
	for (int i = 1; i < DIM_SIZE; i++)
	{
		if ((target.row - i) < 0) progress_search = false;
		if (!progress_search) continue;
		Square test_square = board[target.row - i][target.col];
		progress_rook_bishop_search(target, test_square, opp_colour, &prospective_moves, &progress_search);
	}

	// left
	progress_search = true;
	for (int i = 1; i < DIM_SIZE; i++)
	{
		if ((target.col + i) >= DIM_SIZE) progress_search = false;
		if (!progress_search) continue;
		Square test_square = board[target.row][target.col + i];
		progress_rook_bishop_search(target, test_square, opp_colour, &prospective_moves, &progress_search);
	}

	// right
	progress_search = true;
	for (int i = 1; i < DIM_SIZE; i++)
	{
		if ((target.col - i) < 0) progress_search = false;
		if (!progress_search) continue;
		Square test_square = board[target.row][target.col - i];
		progress_rook_bishop_search(target, test_square, opp_colour, &prospective_moves, &progress_search);
	}

	return prospective_moves;
}


// For each diagonal direction outwards from the piece location, we:
// progress in the direction, adding all empty squares to the list of prospective moves
// if we reach a non-empty square:
// if the opposite colour: add it to the list and prevent moving further.
// if the same colour: just prevent moving further
vector<Square> get_prospective_bishop_moves(Square target, vector<vector<Square>> board, Colour opp_colour)
{
	vector<Square> prospective_moves;

	// up-right
	bool progress_search = true;
	for (int i = 1; i < DIM_SIZE; i++)
	{
		if ((target.row + i) >= DIM_SIZE || (target.col + i) >= DIM_SIZE) progress_search = false;
		if (!progress_search) continue;
		Square test_square = board[target.row + i][target.col + i];
		progress_rook_bishop_search(target, test_square, opp_colour, &prospective_moves, &progress_search);
	}

	// down-right
	progress_search = true;
	for (int i = 1; i < DIM_SIZE; i++)
	{
		if ((target.row - i) < 0 || (target.col + i) >= DIM_SIZE) progress_search = false;
		if (!progress_search) continue;
		Square test_square = board[target.row - i][target.col + i];
		progress_rook_bishop_search(target, test_square, opp_colour, &prospective_moves, &progress_search);
	}

	// up-left
	progress_search = true;
	for (int i = 1; i < DIM_SIZE; i++)
	{
		if ((target.row + i) >= DIM_SIZE || (target.col - i) < 0) progress_search = false;
		if (!progress_search) continue;
		Square test_square = board[target.row + i][target.col - i];
		progress_rook_bishop_search(target, test_square, opp_colour, &prospective_moves, &progress_search);
	}

	// down-left
	progress_search = true;
	for (int i = 1; i < DIM_SIZE; i++)
	{
		if ((target.row - i) < 0 || (target.col - i) < 0) progress_search = false;
		if (!progress_search) continue;
		Square test_square = board[target.row - i][target.col - i];
		progress_rook_bishop_search(target, test_square, opp_colour, &prospective_moves, &progress_search);
	}

	return prospective_moves;
}


// Find queen moves by imagining the queen as a rook and bishop and concatenating the prospective moves of the two
vector<Square> get_prospective_queen_moves(Square target, vector<vector<Square>> board, Colour opp_colour)
{
	vector<Square> prospective_moves_rook = get_prospective_rook_moves(target, board, opp_colour);
	vector<Square> prospective_moves_bishop = get_prospective_bishop_moves(target, board, opp_colour);

	// concatenate prospective moves for rook and bishop
	vector<Square> prospective_moves = prospective_moves_rook;
	prospective_moves.insert(prospective_moves.end(), prospective_moves_bishop.begin(), prospective_moves_bishop.end());
	return prospective_moves;
}


// Given a square the knight could move to, determine if that square is on the board.
// If so, find out if its a prospective move (i.e. not occupied by a piece of the same colour).
bool is_knight_square_prospective(Square target, vector<vector<Square>> board, Colour opp_colour, int row_t, int col_t)
{
	if ((target.row + row_t >= DIM_SIZE) || (target.row + row_t < 0) || (target.col + col_t >= DIM_SIZE) || (target.col + col_t < 0))
		return false;
	Square test_square = board[target.row + row_t][target.col + col_t];

	if (test_square.colour == Colour::EMPTY || test_square.colour == opp_colour)
		return true;

	return false;
}


// Find knight moves by defining a list of all the knight's move patterns, and testing for each.
vector<Square> get_prospective_knight_moves(Square target, vector<vector<Square>> board, Colour opp_colour)
{
	vector<Square> prospective_moves;
	vector<tuple<int, int>> knight_moves = { {-2, -1}, {-2, 1}, {2, -1}, {2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2} };

	for (int i = 0; i < knight_moves.size(); i++)
	{
		if (is_knight_square_prospective(target, board, opp_colour, get<0>(knight_moves[i]), get<1>(knight_moves[i])))
		{
			Square test_square = board[target.row + get<0>(knight_moves[i])][target.col + get<1>(knight_moves[i])];
			prospective_moves.push_back(test_square);
		}
	}

	return prospective_moves;
}


// Look in the immediate 3x3 grid around the king and check for any empty or opponent-occupied squares.
// Then check for castling opportunies
vector<Square> get_prospective_king_moves(Square target, Chessboard chessboard, Colour opp_colour)
{
	vector<Square> prospective_moves;
	//vector<Square> all_enemy_attacking_squares = parse_attackable_squares(find_all_attackable_squares(board, opp_colour, true));
	vector<Square> all_enemy_attacking_squares = parse_attackable_squares(chessboard.attacking_moves[opp_colour]);
	vector<vector<Square>> board = chessboard.board;

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if ((target.row + i >= DIM_SIZE) || (target.row + i < 0) || (target.col + j >= DIM_SIZE) || (target.col + j < 0))
				continue;

			Square test_square = board[target.row + i][target.col + j];

			// skip the king in question
			if (test_square.piece == Piece::KING) continue;

			// to prevent the king moving next to another king, look in the 3x3 radius around the prospective square for the opponent king
			bool neighbours_opponent_king = false;
			for (int k = -1; k <= 1; k++)
			{
				for (int l = -1; l <= 1; l++)
				{
					int test_row = target.row + i + k;
					int test_col = target.col + j + l;
					if ((test_row >= DIM_SIZE) || (test_row < 0) || (test_col >= DIM_SIZE) || (test_col < 0))
						continue;

					if (board[test_row][test_col].piece == Piece::KING && board[test_row][test_col].colour == opp_colour)
						neighbours_opponent_king = true;
				}
			}
			if (neighbours_opponent_king) continue;

			// the king cannot move into an attacked square
			if (count(all_enemy_attacking_squares.begin(), all_enemy_attacking_squares.end(), test_square) > 0) continue;

			if (test_square.colour == Colour::EMPTY || test_square.colour == opp_colour) prospective_moves.push_back(test_square);
		}
	}

	/*
	// handling castling
	if (!target.has_moved)
	{
		// queenside O-O-O
		if (!board[target.row][0].has_moved)
		{
			if (board[target.row][1].piece == Piece::EMPTY && board[target.row][2].piece == Piece::EMPTY && board[target.row][3].piece == Piece::EMPTY)
				prospective_moves.push_back(board[target.row][2]);
		}
		// kingside O-O
		if (!board[target.row][7].has_moved)
		{
			if (board[target.row][6].piece == Piece::EMPTY && board[target.row][5].piece == Piece::EMPTY)
				prospective_moves.push_back(board[target.row][6]);
		}
	}
	*/

	return prospective_moves;
}


// Create a deep copy clone of an input chessboard
vector<vector<Square>> deep_clone_board(vector<vector<Square>> original)
{
	vector<Square> row(DIM_SIZE);
	vector<vector<Square>> copy(DIM_SIZE, row);

	for (int row = 0; row < DIM_SIZE; row++)
	{
		for (int col = 0; col < DIM_SIZE; col++)
		{
			copy[row][col].col = original[row][col].col;
			copy[row][col].row = original[row][col].row;
			copy[row][col].has_moved = original[row][col].has_moved;
			copy[row][col].piece = original[row][col].piece;
			copy[row][col].colour = original[row][col].colour;
		}
	}

	return copy;
}


// Treat the king like a queen and find all the pieces it can see
// If any of those pieces is a piece that could attack the king, it is under attack
bool is_king_attacked(Square king, vector<vector<Square>> board, Colour opp_colour)
{
	vector<Square> raycast_king_moves_orthogonal = get_prospective_rook_moves(king, board, opp_colour);
	for (int i = 0; i < raycast_king_moves_orthogonal.size(); i++)
	{
		if (raycast_king_moves_orthogonal[i].piece == Piece::ROOK ||
			raycast_king_moves_orthogonal[i].piece == Piece::QUEEN)
			return true;
	}

	vector<Square> raycast_king_moves_diagonal = get_prospective_bishop_moves(king, board, opp_colour);
	for (int i = 0; i < raycast_king_moves_diagonal.size(); i++)
	{
		if (raycast_king_moves_diagonal[i].piece == Piece::BISHOP ||
			raycast_king_moves_diagonal[i].piece == Piece::QUEEN)
			return true;
	}

	return false;
}


// Take the prospective moves and identify which ones can be made.
// Prospective moves cannot be made if making the move would open up the king to being captured by another piece
vector<Square> trim_valid_moves(Square target, vector<vector<Square>> board, Colour opp_colour, vector<Square> prospective_moves)
{
	vector<Square> confirmed_moves;
	// catch for the case where no prospective moves can be made
	if (prospective_moves.size() == 0) return confirmed_moves;

	for (int i = 0; i < prospective_moves.size(); i++)
	{
		vector<vector<Square>> test_board = deep_clone_board(board);
		Square prosp_move = prospective_moves[i];

		// make the prospective move on the test board
		test_board[prosp_move.row][prosp_move.col].piece = test_board[target.row][target.col].piece;
		test_board[prosp_move.row][prosp_move.col].colour = test_board[target.row][target.col].colour;
		test_board[prosp_move.row][prosp_move.col].has_moved = true;

		test_board[target.row][target.col] = Square(target.row, target.col);

		// now, trace rays from the white king in each horizontal, vertical and diagonal direction.
		// if we find a queen, rook or bishop then we are putting the king into check and the move is rejected
		for (int row = 0; row < DIM_SIZE; row++)
		{
			for (int col = 0; col < DIM_SIZE; col++)
			{
				if (test_board[row][col].piece == Piece::KING && test_board[row][col].colour == target.colour)
				{
					Square king = test_board[row][col];
					if (!is_king_attacked(king, test_board, opp_colour))
						confirmed_moves.push_back(prosp_move);
				}
			}
		}
	}

	return confirmed_moves;
}

// For a given square, find all the moves that piece can move to
vector<Square> get_valid_square_moves(Square target, Chessboard chessboard, Colour opp_colour)
{
	vector<Square> prospective_moves;
	vector<vector<Square>> board = chessboard.board;

	switch (target.piece)
	{
	case Piece::EMPTY:
		break;
	case Piece::PAWN:
		prospective_moves = get_prospective_pawn_moves(target, board, opp_colour);
		break;
	case Piece::ROOK:
		prospective_moves = get_prospective_rook_moves(target, board, opp_colour);
		break;
	case Piece::BISHOP:
		prospective_moves = get_prospective_bishop_moves(target, board, opp_colour);
		break;
	case Piece::QUEEN:
		prospective_moves = get_prospective_queen_moves(target, board, opp_colour);
		break;
	case Piece::KNIGHT:
		prospective_moves = get_prospective_knight_moves(target, board, opp_colour);
		break;
	case Piece::KING:
		prospective_moves = get_prospective_king_moves(target, chessboard, opp_colour);
		break;
	}

	// take the list of prospective moves and reduce it to only valid ones
	vector<Square> confirmed_moves = trim_valid_moves(target, chessboard.board, opp_colour, prospective_moves);
	return confirmed_moves;
}


// Go through the board and compile a list of all the squares a player is currently targeting
vector<tuple<Square, vector<Square>>> LogicEngine::find_all_attackable_squares(Chessboard chessboard, Colour colour, int mode)
{
	vector<tuple<Square, vector<Square>>> all_attackable_squares;
	Colour opp_colour = (colour == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;
	vector<vector<Square>> board = chessboard.board;

	for (int row = 0; row < DIM_SIZE; row++)
	{
		for (int col = 0; col < DIM_SIZE; col++)
		{
			if (board[row][col].colour == colour)
			{
				tuple<Square, vector<Square>> confirmed_piece_moves;
				switch (mode)
				{
					// find all the valid moves
					case 0:
						confirmed_piece_moves = { board[row][col], get_valid_square_moves(board[row][col], chessboard, opp_colour) };
						all_attackable_squares.push_back(confirmed_piece_moves);
						break;

					// find all the attacking moves. this only excludes pawns moving forward, which are not squares attacked by the pawn
					case 1:
						int dir = (board[row][col].colour == Colour::WHITE) ? 1 : -1;
						switch (board[row][col].piece)
						{
							// Pawns are handled separately since they cannot attack the same squares they can move to.
							case Piece::PAWN:
								confirmed_piece_moves = { board[row][col], get_pawn_attacking_squares(board[row][col], board, opp_colour, dir) };
								all_attackable_squares.push_back(confirmed_piece_moves);
								break;
							// For other pieces: we find their moves and add them to the list.
							default:
								confirmed_piece_moves = { board[row][col], get_valid_square_moves(board[row][col], chessboard, opp_colour) };
								all_attackable_squares.push_back(confirmed_piece_moves);
								break;
						}
						break;
				}
			}
		}
	}

	return all_attackable_squares;
}


// Test for checkmate and stalemate.
// We look at every possible move a checked player can make.
// If the list is empty, the player is checkmated.
// If the player is not in check, the game ends in stalemate.
bool test_for_checkmate_stalemate(Chessboard* cb, Colour player, Colour opp_colour)
{
	vector<Square> potential_moves = parse_attackable_squares((*cb).valid_moves[player]);
	return potential_moves.size() == 0;
}


// Go through the move making procedure. Assume from prior checks that the move is valid.
// 1. Make the move, storing the details of the moved piece and destination square
// 2. Look for check, checkmate and stalemate.
// 3. Build the PGN string for the move.
// 4. Update the active player's turn
// 5. Return a gamestate depending on the state of the game
Gamestate make_move(Chessboard* cb, vector<Square> valid_piece_moves, vector<int> target_position, vector<int> destination_position)
{
	Colour colour = cb->board[target_position[0]][target_position[1]].colour;
	Colour opp_colour = (colour == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;

	// 1. Make the move
	for (int i = 0; i < valid_piece_moves.size(); i++)
	{
		if (valid_piece_moves[i].row == destination_position[0] && valid_piece_moves[i].col == destination_position[1])
		{
			// move the piece
			cb->board[destination_position[0]][destination_position[1]].piece = cb->board[target_position[0]][target_position[1]].piece;
			cb->board[destination_position[0]][destination_position[1]].colour = cb->board[target_position[0]][target_position[1]].colour;
			cb->board[destination_position[0]][destination_position[1]].has_moved = true;

			cb->board[target_position[0]][target_position[1]] = Square(target_position[0], target_position[1]);
			break;
		}
	}

	// Find the valid and attacking move lists for each player
	// Attacking moves do not include the squares that pawns can move to, as they are not attacking these squares.
	// We store these two lists separately so that we can use the valid moves list to find squares to move to,
	//	and the attacking moves list to search for stalemate and checkmate conditions.
	// Both 'lists' are actually stored as a map with colour keys and a vector of tuples for values,
	//	where the tuples contain the coloured piece and a vector of the squares that piece can move to or attack.
	// i.e.: std::map<Colour, std::vector<std::tuple<Square, std::vector<Square>>>>
	(*cb).valid_moves[Colour::WHITE] = find_all_attackable_squares(*cb, Colour::WHITE, 0);
	(*cb).valid_moves[Colour::BLACK] = find_all_attackable_squares(*cb, Colour::BLACK, 0);

	(*cb).attacking_moves[Colour::WHITE] = find_all_attackable_squares(*cb, Colour::WHITE, 1);
	(*cb).attacking_moves[Colour::BLACK] = find_all_attackable_squares(*cb, Colour::BLACK, 1);

	//2. Look for check, checkmate and stalemate
	vector<Square> attackable_squares = parse_attackable_squares((*cb).attacking_moves[colour]);
	bool is_check = false;
	for (int i = 0; i < attackable_squares.size(); i++)
	{
		if (attackable_squares[i].piece == Piece::KING)
		{
			// The opponent king is in check.
			// We already restrict the player's moves once in check to those that escape check.
			// We must however look for checkmate.
			is_check = true;
			bool is_checkmate = test_for_checkmate_stalemate(cb, opp_colour, colour);
			if (is_checkmate) return Gamestate::CHECKMATE;
		}
	}
	if (!is_check)
	{
		bool is_stalemate = test_for_checkmate_stalemate(cb, opp_colour, colour);
		if (is_stalemate) return Gamestate::STALEMATE;
	}

	// At the end of the move, the active player colour is switched.
	cb->active_player = opp_colour;
	if (is_check) return Gamestate::CHECK;
	return Gamestate::NORMAL;
}


// For a given piece, find all the squares that piece can move to.
// Doesn't include illegal moves, moves that would put the king in check, etc.
// Includes special moves i.e. castling, en passant.
vector<Square> Chessboard::find_valid_moves(Square target)
{
	vector<Square> prospective_moves;
	vector<vector<Square>> board = this->board;
	Colour opp_colour = (target.colour == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;

	vector<Square> confirmed_moves = get_valid_square_moves(target, *this, opp_colour);
	return confirmed_moves;
}


// Print a coloured board to the console.
void print_board(Chessboard chessboard, vector<Square> valid_moves, Gamestate gamestate)
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

	cout << "Current board : \n\n";
	for (int i = 0; i < DIM_SIZE; i++)
	{
		cout << (char)('0' + (DIM_SIZE - i)) << ' ';
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

			cout << colourcode << piece_map[chessboard.board[DIM_SIZE - (1 + i)][j].piece] << "\033[0m";
		}
		cout << '\n';
	}
	cout << "\n  abcdefgh\n\n";
	if (gamestate == Gamestate::NORMAL || gamestate == Gamestate::CHECK)
		cout << "\033[1;32m" << (chessboard.active_player == Colour::WHITE ? "WHITE" : "BLACK") << " TO MOVE\033[0m\n\n";
	return;
}


// Take in a string for a text file, read the board, remove newline characters, and return it
string read_board_setup_file(string filename)
{
	// todo: replace this with local pathing
	string path_to_file = "C:/Users/James/source/repos/chess3d/";
	std::ifstream file(path_to_file + filename);
	std::string line;
	std::string file_contents;
	while (getline(file, line))
	{
		file_contents += line;
	}

	return file_contents;
}


// Initialise a chessboard from a text file.
// The piece mmap defines how the text file should be structured, with uppercase for white pieces and lowercase for black.
Chessboard::Chessboard(string filename)
{
	string setup_position = read_board_setup_file(filename);

	map<char, tuple<Piece, Colour>> piece_map = {
		{ '_', {Piece::EMPTY,  Colour::EMPTY }},
		{ 'P', {Piece::PAWN,   Colour::WHITE }},
		{ 'R', {Piece::ROOK,   Colour::WHITE }},
		{ 'N', {Piece::KNIGHT, Colour::WHITE }},
		{ 'B', {Piece::BISHOP, Colour::WHITE }},
		{ 'Q', {Piece::QUEEN,  Colour::WHITE }},
		{ 'K', {Piece::KING,   Colour::WHITE }},
		{ 'p', {Piece::PAWN,   Colour::BLACK }},
		{ 'r', {Piece::ROOK,   Colour::BLACK }},
		{ 'n', {Piece::KNIGHT, Colour::BLACK }},
		{ 'b', {Piece::BISHOP, Colour::BLACK }},
		{ 'q', {Piece::QUEEN,  Colour::BLACK }},
		{ 'k', {Piece::KING,   Colour::BLACK }}
	};

	vector<Square> row(DIM_SIZE);
	vector<vector<Square>> b(DIM_SIZE, row);
	active_player = Colour::WHITE;

	board = b;

	for (int i = 0; i < DIM_SIZE; i++)
	{
		for (int j = 0; j < DIM_SIZE; j++)
		{
			board[i][j].piece =  get<0>(piece_map[setup_position[((DIM_SIZE - (i + 1)) * DIM_SIZE) + j]]);
			board[i][j].colour = get<1>(piece_map[setup_position[((DIM_SIZE - (i + 1)) * DIM_SIZE) + j]]);
			board[i][j].row = i;
			board[i][j].col = j;
		}
	}
}


// Main game loop
void loop_board(Chessboard cb)
{
	// Find the valid move lists for each player
	cb.valid_moves[Colour::WHITE] = find_all_attackable_squares(cb, Colour::WHITE, 0);
	cb.valid_moves[Colour::BLACK] = find_all_attackable_squares(cb, Colour::BLACK, 0);

	cb.attacking_moves[Colour::WHITE] = find_all_attackable_squares(cb, Colour::WHITE, 1);
	cb.attacking_moves[Colour::BLACK] = find_all_attackable_squares(cb, Colour::BLACK, 1);


	while(true)
	{
		vector<Square> potential_moves = parse_attackable_squares(cb.valid_moves[cb.active_player]);
		cout << "Number of valid moves: " << potential_moves.size() << "\nValid moves are: ";
		for (int i = 0; i < potential_moves.size(); i++)
		{
			cout << convert_int_to_chessboard_square(potential_moves[i].col, potential_moves[i].row) << ' ';
		}
		cout << '\n';

		print_board(cb, vector<Square>(), Gamestate::NORMAL);

		// first select the piece to move and get a list of the squares the piece can move to
		string target_square, destination_square;
		cout << "Input target square: ";
		getline(cin, target_square);

		vector<int> target_position = convert_chessboard_square_to_int(target_square);
		if (cb.board[target_position[0]][target_position[1]].colour != cb.active_player)
		{
			cout << "\x1B[2J\x1B[H";
			cout << "\033[1;31mInvalid piece selected\033[0m\n";
			continue;
		}

		// find and print the list of moves from that square's piece
		cout << "\x1B[2J\x1B[H";
		cout << "Moves: ";

		vector<Square> vms = cb.find_valid_moves(cb.board[target_position[0]][target_position[1]]);
		//vector<Square> vms = parse_attackable_squares(cb.valid_moves[cb.active_player]);
		for (int i = 0; i < vms.size(); i++)
		{
			cout << convert_int_to_chessboard_square(vms[i].col, vms[i].row) << ' ';
		}
		cout << '\n';

		print_board(cb, vms, Gamestate::NORMAL);

		// then get the square to move to, and if on the list, we can make the move
		cout << "\nChoose destination square: ";
		getline(cin, destination_square);
		if (destination_square == "") continue;

		vector<int> destination_position = convert_chessboard_square_to_int(destination_square);

		// finally: make the move
		Gamestate gs = make_move(&cb, vms, target_position, destination_position);

		// Handle the result of making the move
		string winner;
		switch (gs)
		{
			case Gamestate::CHECK:
				cout << "\033[1;33mThe king is in check\033[0m\n";
				break;
			case Gamestate::CHECKMATE:
				winner = (cb.active_player == Colour::WHITE) ? "White" : "Black";
				cout << "\033[1;33mThe king is checkmated! Game over. " << winner << " wins!\033[0m\n";
				print_board(cb, vector<Square>(), gs);
				return;
			case Gamestate::STALEMATE:
				cout << "\033[1;33mIts a stalemate! Game ends in a tie.\033[0m\n";
				print_board(cb, vector<Square>(), gs);
				return;
		}
	}

	return;
}


int main()
{
	Chessboard cb = Chessboard();
	//Chessboard cb = Chessboard("test_position.txt");
	loop_board(cb);
}
