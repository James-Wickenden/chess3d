// logic.cpp

#include "logic.hpp"

using namespace std;
using namespace LogicEngine;
namespace fs = std::filesystem;

const int DIM_SIZE = 8; // size of the chessboard

// Define constructors for squares based off different input sets.
Square::Square()
{
	colour = Colour::EMPTY;
	piece = Piece::EMPTY;
	row = -1;
	col = -1;
	has_moved = true;
	when_moved = {};
}

Square::Square(int r, int c)
{
	colour = Colour::EMPTY;
	piece = Piece::EMPTY;
	row = r;
	col = c;
	has_moved = true;
	when_moved = {};
}


Square::Square(Piece p, Colour c, int i, int j, bool h_m, vector<int> w_m)
{
	piece = p;
	colour = c;
	row = i;
	col = j;
	has_moved = h_m;
	when_moved = w_m;
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

	// handle case where pawn is at the end of the board and cannot attack the squares in front
	if ((target.colour == Colour::WHITE && target.row == 7) || (target.colour == Colour::BLACK && target.row == 0))
		return pawn_attacking_squares;

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


// Go through the logic of en passant, determining whether the move is legal for a specified direction (-1 for left, 1 for right)
bool can_en_passant(Square pawn, vector<vector<Square>> board, Colour opp_colour, int dir, int move_no)
{
	int test_col = pawn.col + dir;

	if (test_col < 0 || test_col >= DIM_SIZE)
		return false;

	if (board[pawn.row][test_col].piece != Piece::PAWN)
		return false;

	// If the last move was the pawn, we can en passant it
	if ((board[pawn.row][test_col].when_moved.size() == 1) && (board[pawn.row][test_col].when_moved.back() == move_no - 1))
		return true;
	else
		return false;
}


// For pawns, look a square ahead in the direction a pawn can move in, or two if the paawn hasn't moved yet.
// Also check for diagonal captures and en passant.
vector<Square> get_prospective_pawn_moves(Square target, vector<vector<Square>> board, Colour opp_colour, int move_no)
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
			if (!target.has_moved && target.row == ((target.colour == Colour::WHITE) ? 1 : 6))
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

	// Check for en passant. For this, the piece must have advanced exactly three ranks: white pawns must be on rank 5 and black on rank 4
	// Then, an opponent pawn must have moved two ranks forward in the previous move to be adjacent to the pawn.
	// If so, we can move the pawn diagonally and capture the opponent pawn.
	if (target.row == ((target.colour == Colour::WHITE) ? 4 : 3))
	{
		// to the left
		if (can_en_passant(target, board, opp_colour, -1, move_no))
			prospective_moves.push_back(board[target.row + dir][target.col - 1]);
		// to the right
		if (can_en_passant(target, board, opp_colour, 1, move_no))
			prospective_moves.push_back(board[target.row + dir][target.col + 1]);
	}
	
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


// detect validity of castling by looking for empty squares and unattacked squares between king and rook
// the two checks are performed on different lists of squares, so must be handled separately
bool can_castle(int target_row, Chessboard *chessboard, Colour opp_colour, vector<int> empty_cols, vector<int> unattacked_cols)
{
	vector<vector<Square>> board = chessboard->board;

	// iterate rows { 1, 2, 3 } for queenside, and { 5, 6 } for kingside
	// check that squares inbetween the king and rook must be empty
	for (int i = 0; i < empty_cols.size(); i++)
	{
		int column_under_test = empty_cols[i];
		if (!(board[target_row][column_under_test].piece == Piece::EMPTY))
			return false;
	}

	// iterate rows { 2, 3, 4 } for queenside, and { 4, 5, 6 } for kingside
	// check that squares the king moves through cannot be attacked by the opponent, and the king cannot be in check
	vector<Square> attacked_squares = parse_attackable_squares(chessboard->attacking_moves[opp_colour]);
	for (int i = 0; i < unattacked_cols.size(); i++)
	{
		int column_under_test = unattacked_cols[i];
		Square square_under_test = board[target_row][column_under_test];
		if (count(attacked_squares.begin(), attacked_squares.end(), square_under_test) > 0)
			return false;
	}

	return true;
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

	
	// handling castling
	if (!target.has_moved && target.col == 4)
	{
		// queenside O-O-O
		if (!board[target.row][0].has_moved && board[target.row][0].piece == Piece::ROOK)
		{
			bool can_queenside_castle = can_castle(target.row, &chessboard, opp_colour, vector<int>({1, 2, 3}), vector<int>({ 2, 3, 4 }));
			if (can_queenside_castle)
				prospective_moves.push_back(board[target.row][2]);
		}

		// kingside O-O
		if (!board[target.row][7].has_moved && board[target.row][0].piece == Piece::ROOK)
		{
			bool can_kingside_castle = can_castle(target.row, &chessboard, opp_colour, vector<int>({5, 6}), vector<int>({4, 5, 6}));
			if (can_kingside_castle)
				prospective_moves.push_back(board[target.row][6]);
		}
	}
	

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
		test_board[prosp_move.row][prosp_move.col].when_moved = test_board[target.row][target.col].when_moved;

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
					{
						vector<Square> king_knight_moves = get_prospective_knight_moves(king, test_board, opp_colour);
						bool king_attacked_by_knight = false;
						for (int i = 0; i < king_knight_moves.size(); i++)
						{
							if (king_knight_moves[i].piece == Piece::KNIGHT) king_attacked_by_knight = true;
						}
						if (!king_attacked_by_knight) confirmed_moves.push_back(prosp_move);
					}
						
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
		prospective_moves = get_prospective_pawn_moves(target, board, opp_colour, chessboard.move_no);
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


// handles the logic of moving one square into another, and replacing the moved square with an empty square
void switch_pieces(Chessboard *cb, vector<int> target_position, vector<int> destination_position)
{
	cb->board[destination_position[0]][destination_position[1]].piece = cb->board[target_position[0]][target_position[1]].piece;
	cb->board[destination_position[0]][destination_position[1]].colour = cb->board[target_position[0]][target_position[1]].colour;
	cb->board[destination_position[0]][destination_position[1]].has_moved = true;

	cb->board[destination_position[0]][destination_position[1]].when_moved = cb->board[target_position[0]][target_position[1]].when_moved;
	cb->board[destination_position[0]][destination_position[1]].when_moved.push_back(cb->move_no);

	cb->board[target_position[0]][target_position[1]] = Square(target_position[0], target_position[1]);
}


bool is_dest_square_attackable_by_piece(tuple<Square, vector<Square>> potential_mover, vector<int> dest_position)
{
	vector<Square> potential_mover_destinations = get<1>(potential_mover);
	for (int j = 0; j < potential_mover_destinations.size(); j++)
	{
		Square test_piece_dest = potential_mover_destinations[j];
		if (test_piece_dest.col == dest_position[1] && test_piece_dest.row == dest_position[0])
			return true;
	}

	return false;
}


// Reference the piece notation map
string get_piece_notation_map(Piece p)
{
	map<Piece, string> piece_notation_map = {
		{ Piece::EMPTY,  " " },
		{ Piece::PAWN,   "" },
		{ Piece::ROOK,   "R"},
		{ Piece::KNIGHT, "N"},
		{ Piece::BISHOP, "B"},
		{ Piece::QUEEN,  "Q"},
		{ Piece::KING,   "K"}
	};

	return piece_notation_map[p];
}


// Build the notation for the ply, e.g. "Nbxd2"
string get_ply_notation(Chessboard* cb, vector<int> target_position, vector<int> destination_position, bool is_capture)
{
	string result_notation = "";

	// First get the section of the string for the moved piece
	Square moved_piece = cb->board[destination_position[0]][destination_position[1]];
	result_notation += get_piece_notation_map(moved_piece.piece);

	// If multiple pieces could move to that location, we need to show more details about which one moved there.
	// In the case of pawns, we want the attacking moves if a piece was captured, but the valid moves if not.
	// This is because a pawn moving forward should only have one option- directly in front. En passant does not affect this as it still involves capturing.
	vector<tuple<Square, vector<Square>>> potential_movers = ( is_capture ? cb->attacking_moves[cb->active_player] : cb->valid_moves[cb->active_player] );
	bool candidate_on_row = false;
	bool candidate_on_col = false;

	for (int i = 0; i < potential_movers.size(); i++)
	{
		Square potential_mover = get<0>(potential_movers[i]);

		// Don't inspect the same piece that we're moving
		if ((potential_mover.piece == moved_piece.piece)
			&& (potential_mover.row == target_position[0])
			&& (potential_mover.col == target_position[1]))
			continue;
		// We only care about pieces of the same type that could land in that square
		if (potential_mover.piece != moved_piece.piece) continue;
		
		// Look to see if the piece is on the same rank or file as the piece that moved.
		if (potential_mover.row == target_position[0])
		{
			if (is_dest_square_attackable_by_piece(potential_movers[i], destination_position))
				candidate_on_row = true;
		}
		if (potential_mover.col == target_position[1])
		{
			if (is_dest_square_attackable_by_piece(potential_movers[i], destination_position))
				candidate_on_col = true;
		}

		// For knights, this isn't necessarily true and we must perform more tests to dig a level deeper.
		// So, for the potential mover, go straight to the is_dest_square_attackable_by_piece() function.
		// NOTE there is a bug here where a player with 3 knights can get incorrect notation. but who cares
		if (potential_mover.piece == Piece::KNIGHT && !candidate_on_col)
		{
			if (is_dest_square_attackable_by_piece(potential_movers[i], destination_position))
				candidate_on_row = true;
		}

		// If the piece is a pawn and theres a capture we must always include the file.
		if (is_capture && moved_piece.piece == Piece::PAWN && !candidate_on_col)
			candidate_on_row = true;
	}

	if (candidate_on_row) result_notation += convert_int_to_chessboard_square(target_position[1], target_position[0])[0];
	if (candidate_on_col) result_notation += convert_int_to_chessboard_square(target_position[1], target_position[0])[1];
	
	
	// If a piece was captured, we need to represent a capture
	if (is_capture) result_notation += "x";

	// Then append the coordinates of the landing square
	result_notation += convert_int_to_chessboard_square(destination_position[1], destination_position[0]);

	return result_notation;
}


// Promote a pawn within the terminal by calling for input, and halting until a valid input is made. Then return the selection.
Piece get_pawn_promotion_terminal()
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
		cout << "Choose piece to promote to [Queen/Rook/Bishop/Knight]: ";
		getline(cin, promotion_choice);
		std::transform(promotion_choice.begin(), promotion_choice.end(), promotion_choice.begin(), ::toupper);
		if (piece_string_map.count(promotion_choice) > 0) return piece_string_map.at(promotion_choice);
	}

	return Piece::EMPTY;
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
	Square destination_piece = cb->board[destination_position[0]][destination_position[1]];
	string ply_notation = "";

	// 1. Make the move
	for (int i = 0; i < valid_piece_moves.size(); i++)
	{
		if (valid_piece_moves[i].row == destination_position[0] && valid_piece_moves[i].col == destination_position[1])
		{
			// move the piece
			bool is_capture = (cb->board[destination_position[0]][destination_position[1]].piece == Piece::EMPTY) ? false : true;

			switch_pieces(cb, target_position, destination_position);
			ply_notation = get_ply_notation(cb, target_position, destination_position, is_capture);
			break;
		}
	}

	Square moved_piece = cb->board[destination_position[0]][destination_position[1]];
	switch (moved_piece.piece)
	{
		case Piece::KING:
			// check to see if we castled; if so, then the rook needs to be moved too.
			// Queenside
			if (target_position[1] == 4 && destination_position[1] == 2)
			{
				switch_pieces(cb, vector<int>({ target_position[0], 0 }), vector<int>({ target_position[0], 3 }));
				ply_notation = "O-O-O";
			}
			// Kingside
			else if (target_position[1] == 4 && destination_position[1] == 6)
			{
				switch_pieces(cb, vector<int>({ target_position[0], 7 }), vector<int>({ target_position[0], 5 }));
				ply_notation = "O-O";
			}
			break;
		case Piece::PAWN:
			// check to see if we captured via en passant; if so, remove the captured pawn
			if (destination_piece.piece == Piece::EMPTY && destination_piece.col != target_position[1])
			{
				cb->board[target_position[0]][destination_position[1]] = Square(target_position[0], destination_position[1]);
				ply_notation += "ep";
			}
			break;
	}
	
	// If the moved piece is a pawn moving to the opposite back rank, we must intercept for promotion.
	if (moved_piece.piece == Piece::PAWN && moved_piece.row == (moved_piece.colour == Colour::WHITE ? 7 : 0))
	{
		Piece promotion_choice = get_pawn_promotion_terminal();
		cb->board[destination_position[0]][destination_position[1]].piece = promotion_choice;
		ply_notation += get_piece_notation_map(promotion_choice);
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

	// 2. Look for check, checkmate and stalemate
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
			ply_notation += (is_checkmate ? "#" : "+");
		}
	}
	if (!is_check)
	{
		bool is_stalemate = test_for_checkmate_stalemate(cb, opp_colour, colour);
		if (is_stalemate) return Gamestate::STALEMATE;
	}

	// 3. Add the ply notation to the game's notation
	string full_ply_notation = "";
	if (cb->active_player == Colour::WHITE)
	{
		full_ply_notation += to_string((cb->move_no + 1) / 2);
		full_ply_notation += ".";
		full_ply_notation += ply_notation;
	}
	else
	{
		full_ply_notation += " ";
		full_ply_notation += ply_notation;
		full_ply_notation += " ";
	}
	cb->notation += full_ply_notation;

	// 4. At the end of the move, the active player colour is switched.
	cb->active_player = opp_colour;

	// 5. Increment the move counter.
	cb->move_no++;

	

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
	cout << chessboard.notation << '\n';

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

	vector<Square> row(DIM_SIZE);
	vector<vector<Square>> b(DIM_SIZE, row);
	active_player = Colour::WHITE;
	move_no = 1;

	board = b;

	for (int i = 0; i < DIM_SIZE; i++)
	{
		for (int j = 0; j < DIM_SIZE; j++)
		{
			Piece p = get<0>(piece_map[setup_position[((DIM_SIZE - (i + 1)) * DIM_SIZE) + j]]);
			Colour c = get<1>(piece_map[setup_position[((DIM_SIZE - (i + 1)) * DIM_SIZE) + j]]);
			board[i][j] = Square(p, c, i, j, false, vector<int>());
		}
	}
}


// Return today's date in the form YYYY.MM.DD
string get_formatted_date()
{
	time_t timestamp;
	time(&timestamp);
	struct tm* timeinfo;
	char formatted_date[12];
	timeinfo = localtime(&timestamp);
	strftime(formatted_date, 12, "%Y.%m.%d", timeinfo);

	return formatted_date;
}


// Attempt to save the game. Returns true if successful, and false if not.
// The game is saved as a .PGN file containing notation and metadata.
bool save_game(Chessboard cb)
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
	game_string_data += cb.result + '\n';

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


// Return an alphanumeric string of random characters length n.
string random_string(size_t length)
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


void handle_gamestate(Chessboard cb, Gamestate gs, string winner)
{
	switch (gs)
	{
		case Gamestate::CHECK:
			cout << "\033[1;33mThe king is in check\033[0m\n";
			return;
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

// Main game loop
void loop_board(Chessboard cb, Gamestate gs)
{
	stack<Chessboard> board_stack;
	board_stack.push(cb);

	// Find the valid move lists for each player
	cb.valid_moves[Colour::WHITE] = find_all_attackable_squares(cb, Colour::WHITE, 0);
	cb.valid_moves[Colour::BLACK] = find_all_attackable_squares(cb, Colour::BLACK, 0);

	cb.attacking_moves[Colour::WHITE] = find_all_attackable_squares(cb, Colour::WHITE, 1);
	cb.attacking_moves[Colour::BLACK] = find_all_attackable_squares(cb, Colour::BLACK, 1);

	string active_player_str = (cb.active_player == Colour::WHITE) ? "White" : "Black";
	if (gs == Gamestate::NEWGAME) cout << "\033[1;32mNEW GAME\033[0m\n";
	else cout << "\033[1;32mLOADED GAME\033[0m\n";
	cout << "\033[1;33m" + cb.white_name + " vs " + cb.black_name + "\n";
	cout << "\033[1;33m" + active_player_str + " to move.\n";
	cout << "When inputting target square:\n  - Type 'undo' to undo move.\n  - Type 'save' to save PGN file.\n  - Type 'exit' to return to menu.\033[0m\n\n";

	handle_gamestate(cb, gs, "???");
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

		bool move_selected = false;
		string target_square, destination_square, move_choice;
		while (!move_selected)
		{
			// first select the piece to move and get a list of the squares the piece can move to
			cout << "Input target square or choice: ";
			getline(cin, move_choice);

			if (move_choice == "") continue;
			else if (move_choice == "undo")
			{
				// Handle the 'undo' operation.
				if (board_stack.size() <= 1)
				{
					cout << "\033[1;31mNo more moves to undo.\033[0m\n";
					continue;
				}
					
				cout << "\033[1;31mUndoing move.\033[0m\n";
				board_stack.pop();
				cb = board_stack.top();
				print_board(cb, vector<Square>(), Gamestate::NORMAL);

				continue;
			}
			else if (move_choice == "save")
			{
				cout << "\033[1;31mSaving game.\033[0m\n";
				save_game(cb);
			}
			else if (move_choice == "exit")
			{
				return;
			}
			else
			{
				target_square = move_choice;
				move_selected = true;
			}
		}
		
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

		// Push the new board to the stack of boards. This includes the notation stack, state of the board and pieces, and game metadata e.g. move no.
		board_stack.push(cb);

		// Handle the result of making the move
		string winner;
		handle_gamestate(cb, gs, winner);
		if (gs == Gamestate::CHECKMATE || gs == Gamestate::STALEMATE) return;
	}

	return;
}


vector<string> split(const string& s, const string& delimiter) {
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

tuple<Chessboard, Gamestate> parse_pgn(Chessboard cb, vector<string> pgn_moves)
{
	// Each element in the pgn_moves vector is a ply. Every other element will contain the move number, i.e. 4.e4
	// To parse, we take off this number if necessary, then use the context of the current board to get the target and destination position.
	Gamestate gs;
	for (int i = 0; i < pgn_moves.size(); i++)
	{
		string cur_pgn = pgn_moves[i];
		if (cur_pgn == "") continue;
		cur_pgn = split(cur_pgn, ".").back();
		cout << "parsing " + cur_pgn;

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
			cb.valid_moves[Colour::WHITE] = find_all_attackable_squares(cb, Colour::WHITE, 0);
			cb.valid_moves[Colour::BLACK] = find_all_attackable_squares(cb, Colour::BLACK, 0);

			cb.attacking_moves[Colour::WHITE] = find_all_attackable_squares(cb, Colour::WHITE, 1);
			cb.attacking_moves[Colour::BLACK] = find_all_attackable_squares(cb, Colour::BLACK, 1);

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
						cout << '\n' + to_string(potential_mover.row) + " " + to_string(potential_mover.col) + "\n";
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
void load_game(fs::path gamepath)
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
			cout << "Found comment character: ";
			cout << c;
			cout << " at pos: " + to_string(gamedata.find(c)) + "\n";
			cout << "Remove comments from PGN before loading.\n    Press ENTER:";
			getline(cin, movedata);
			return;
		}
	}

	vector<string> pgn_moves = split(movedata, " ");

	tuple<Chessboard, Gamestate> game_state;
	game_state = parse_pgn(cb, pgn_moves);

	cout << "parsed\n";
	loop_board(get<0>(game_state), get<1>(game_state));
}


// Defines the text based entry point, including loading games.
void menu_handler()
{
	bool valid_menu_choice = false;
	while (!valid_menu_choice)
	{
		cout << "\x1B[2J\x1B[H";
		cout << "\033[1;33mchess3d by Mammoth [https://github.com/James-Wickenden/chess3d]\033[0m\n";
		cout << "Options [1/2/3]:\n";
		cout << "    1. New game\n    2. Test position\n    3. Load game\n\n";
		cout << "\033[1;32mEnter choice: \033[0m";

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
				cout << "White player name [" + white_name + "]: ";
				getline(cin, tmp_name);
				if (tmp_name != "") white_name = tmp_name;
				cout << "Black player name [" + black_name + "]: ";
				getline(cin, tmp_name);
				if (tmp_name != "") black_name = tmp_name;
				cout << "\x1B[2J\x1B[H";

				cb = Chessboard();
				cb.white_name = white_name;
				cb.black_name = black_name;
				cb.date = get_formatted_date();

				loop_board(cb, Gamestate::NEWGAME);
				break;
			case '2':
				cout << "Select test board [1/2/3/4/5/6]:\n";
				cout << "  1. position\n  2. en passant\n  3. notation\n  4. castling\n  5. promotion\n  6. stalemate\n";
				getline(cin, submenu_choice);
				cout << "\x1B[2J\x1B[H";

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
					cout << cur_id << ".  " << base_filename + '\n';
					id_game_map[to_string(cur_id)] = base_filename;
					cur_id++;
				}

				cout << "\nSelect game with id: ";
				getline(cin, submenu_choice);
				string chosen_file = id_game_map[submenu_choice];
				if (fs::exists(p.append(chosen_file)))
					load_game(p);
				break;
		}
	}
}


int main()
{
	srand(time(NULL));
	menu_handler();
}
