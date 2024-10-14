// logic.cpp

#include "logic.hpp"

using namespace std;
using namespace LogicEngine;

const int DIM_SIZE = 8;

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


// Find the two diagonal squares the pawn can capture on
vector<Square> get_pawn_attacking_squares(Square target, vector<vector<Square>> board, Colour opp_colour, int dir, bool assume_king)
{
	vector<Square> pawn_attacking_squares;

	if (target.col > 0) {
		if (board[target.row + dir][target.col - 1].colour == opp_colour || assume_king) {
			pawn_attacking_squares.push_back(board[target.row + dir][target.col - 1]);
		}
	}
	if (target.col < 7) {
		if (board[target.row + dir][target.col + 1].colour == opp_colour || assume_king) {
			pawn_attacking_squares.push_back(board[target.row + dir][target.col + 1]);
		}
	}

	return pawn_attacking_squares;
}


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
		vector<Square> pawn_attacking_Squares = get_pawn_attacking_squares(target, board, opp_colour, dir, false);
		prospective_moves.insert(prospective_moves.end(), pawn_attacking_Squares.begin(), pawn_attacking_Squares.end());
	}

	//todo: en passant prospective move
	//
	//

	return prospective_moves;
}


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


vector<Square> get_prospective_queen_moves(Square target, vector<vector<Square>> board, Colour opp_colour)
{
	vector<Square> prospective_moves_rook = get_prospective_rook_moves(target, board, opp_colour);
	vector<Square> prospective_moves_bishop = get_prospective_bishop_moves(target, board, opp_colour);

	// concatenate prospective moves for rook and bishop
	vector<Square> prospective_moves = prospective_moves_rook;
	prospective_moves.insert(prospective_moves.end(), prospective_moves_bishop.begin(), prospective_moves_bishop.end());
	return prospective_moves;
}


bool is_knight_square_prospective(Square target, vector<vector<Square>> board, Colour opp_colour, int row_t, int col_t)
{
	if ((target.row + row_t >= DIM_SIZE) || (target.row + row_t < 0) || (target.col + col_t >= DIM_SIZE) || (target.col + col_t < 0))
		return false;
	Square test_square = board[target.row + row_t][target.col + col_t];

	if (test_square.colour == Colour::EMPTY || test_square.colour == opp_colour)
		return true;

	return false;
}


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
vector<Square> get_prospective_king_moves(Square target, vector<vector<Square>> board, Colour opp_colour)
{
	vector<Square> prospective_moves;
	vector<Square> all_enemy_attacking_squares = find_all_attackable_squares(board, opp_colour);

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if ((target.row + i >= DIM_SIZE) || (target.row + i < 0) || (target.col + j >= DIM_SIZE) || (target.col + j < 0))
				continue;

			Square test_square = board[target.row + i][target.col + j];

			// skip the king and prevent the king moving next to another king
			if (test_square.piece == Piece::KING) continue;
			// the king cannot move into an attacked square
			if (count(all_enemy_attacking_squares.begin(), all_enemy_attacking_squares.end(), test_square) > 0) continue;

			if (test_square.colour == Colour::EMPTY || test_square.colour == opp_colour) prospective_moves.push_back(test_square);
		}
	}

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
				if (board[row][col].piece == Piece::KING && board[row][col].colour == target.colour)
				{
					Square king = board[row][col];
					if (!is_king_attacked(king, test_board, opp_colour))
						confirmed_moves.push_back(prosp_move);
				}
			}
		}
	}

	return confirmed_moves;
}


// For a given square, find all the moves that piece can move to
vector<Square> get_valid_square_moves(Square target, vector<vector<Square>> board, Colour opp_colour)
{
	vector<Square> prospective_moves;

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
		prospective_moves = get_prospective_king_moves(target, board, opp_colour);
		break;
	}

	// take the list of prospective moves and reduce it to only valid ones
	vector<Square> confirmed_moves = trim_valid_moves(target, board, opp_colour, prospective_moves);
	return confirmed_moves;
}


// Go through the board and compile a list of all the squares a player is currently targeting
vector<Square> LogicEngine::find_all_attackable_squares(vector<vector<Square>> board, Colour colour)
{
	vector<Square> all_attackable_squares;
	Colour opp_colour = (colour == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;

	for (int row = 0; row < DIM_SIZE; row++)
	{
		for (int col = 0; col < DIM_SIZE; col++)
		{
			if (board[row][col].colour == colour)
			{
				vector<Square> confirmed_piece_moves;
				int dir = (board[row][col].colour == Colour::WHITE) ? 1 : -1;
				switch (board[row][col].piece)
				{
					// Skip over the king. We already catch it in the king prospective move function and it would recurse infinitely.
					case Piece::KING:
						continue;
						break;
					// Pawns are handled separately since they cannot attack the same squares they can move to.
					case Piece::PAWN:
						confirmed_piece_moves = get_pawn_attacking_squares(board[row][col], board, opp_colour, dir, true);
						all_attackable_squares.insert(all_attackable_squares.end(), confirmed_piece_moves.begin(), confirmed_piece_moves.end());
						break;
					// For other pieces: we find their moves and add them to the list.
					default:
						confirmed_piece_moves = get_valid_square_moves(board[row][col], board, opp_colour);
						all_attackable_squares.insert(all_attackable_squares.end(), confirmed_piece_moves.begin(), confirmed_piece_moves.end());
						break;
				}
			}
		}
	}

	return all_attackable_squares;
}


// Go through the move making procedure. Assume from prior checks that the move is valid.
// 1. Make the move, storing the details of the moved piece and destination square
// 2. Look for check, checkmate and stalemate.
// 3. Build the PGN string for the move.
// 4. Update the active player's turn
void make_move(Chessboard* cb, vector<Square> valid_piece_moves, vector<int> target_position, vector<int> destination_position)
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

	// Find the valid move lists for each player
	(*cb).valid_moves[Colour::WHITE] = find_all_attackable_squares(cb->board, Colour::WHITE);
	(*cb).valid_moves[Colour::BLACK] = find_all_attackable_squares(cb->board, Colour::BLACK);

	//2. Look for check, checkmate and stalemate
	for (int i = 0; i < (*cb).valid_moves[colour].size(); i++)
	{
		if ((*cb).valid_moves[colour][i].piece == Piece::KING)
		{
			// The opponent king is in check.
			cout << "check\n";
		}
	}

	cb->active_player = opp_colour;
}


// For a given piece, find all the squares that piece can move to.
// Doesn't include illegal moves, moves that would put the king in check, etc.
// Includes special moves i.e. castling, en passant.
vector<Square> Chessboard::find_valid_moves(Square target)
{
	vector<Square> prospective_moves;
	vector<vector<Square>> board = this->board;
	Colour opp_colour = (target.colour == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;

	vector<Square> confirmed_moves = get_valid_square_moves(target, board, opp_colour);
	return confirmed_moves;
}


// Print a coloured board to the console.
void print_board(Chessboard chessboard, vector<Square> valid_moves)
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
	cout << "\033[1;32m" << (chessboard.active_player == Colour::WHITE ? "WHITE" : "BLACK") << " TO MOVE\033[0m\n\n";
	return;
}


Chessboard::Chessboard()
{
	/* Generate a new board that should look as follows :
	
	RNBQKBNR
	PPPPPPPP
	________
	________
	________
	________
	PPPPPPPP
	RNBQKBNR

	*/

	vector<Square> row(DIM_SIZE);
	vector<vector<Square>> b(DIM_SIZE, row);
	active_player = Colour::WHITE;

	board = b;
	for (int i=0; i < DIM_SIZE; i++)
	{
		board[1][i].piece = Piece::PAWN;
		board[1][i].colour = Colour::WHITE;

		board[6][i].piece = Piece::PAWN;
		board[6][i].colour = Colour::BLACK;

		board[0][i].colour = Colour::WHITE;
		board[7][i].colour = Colour::BLACK;
	}

	for (int i = 0; i < DIM_SIZE; i += 7)
	{
		board[i][0].piece = Piece::ROOK;
		board[i][1].piece = Piece::KNIGHT;
		board[i][2].piece = Piece::BISHOP;
		board[i][3].piece = Piece::QUEEN;
		board[i][4].piece = Piece::KING;
		board[i][5].piece = Piece::BISHOP;
		board[i][6].piece = Piece::KNIGHT;
		board[i][7].piece = Piece::ROOK;
	}

	for (int i = 0; i < DIM_SIZE; i++)
	{
		for (int j = 0; j < DIM_SIZE; j++)
		{
			board[i][j].row = i;
			board[i][j].col = j;
		}
	}
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


// Main game loop
void loop_board(Chessboard cb)
{
	while(true)
	{
		print_board(cb, vector<Square>());

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
		for (int i = 0; i < vms.size(); i++)
		{
			cout << convert_int_to_chessboard_square(vms[i].col, vms[i].row) << ' ';
		}
		cout << '\n';

		print_board(cb, vms);

		// then get the square to move to, and if on the list, we can make the move
		cout << "\nChoose destination square: ";
		getline(cin, destination_square);
		if (destination_square == "") continue;

		vector<int> destination_position = convert_chessboard_square_to_int(destination_square);

		// finally: make the move
		make_move(&cb, vms, target_position, destination_position);
	}

	return;
}


int main()
{
	Chessboard cb = Chessboard();
	loop_board(cb);
}
