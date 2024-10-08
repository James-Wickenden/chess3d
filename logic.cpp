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
		if (target.col > 0) {
			if (board[target.row + dir][target.col - 1].colour == opp_colour) {
				prospective_moves.push_back(board[target.row + dir][target.col - 1]);
			}
		}
		if (target.col < 7) {
			if (board[target.row + dir][target.col + 1].colour == opp_colour) {
				prospective_moves.push_back(board[target.row + dir][target.col + 1]);
			}
		}
	}

	//todo: en passant prospective move
	//
	//

	return prospective_moves;
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
		if (test_square.colour == Colour::EMPTY)
		{
			prospective_moves.push_back(test_square);
		}
		else if (test_square.colour == opp_colour)
		{
			prospective_moves.push_back(test_square);
			progress_search = false;
		}
		else if (test_square.colour == target.colour)
		{
			progress_search = false;
		}
	}

	// backwards
	progress_search = true;
	for (int i = 1; i < DIM_SIZE; i++)
	{
		if ((target.row - i) < 0) progress_search = false;
		if (!progress_search) continue;
		Square test_square = board[target.row - i][target.col];
		if (test_square.colour == Colour::EMPTY)
		{
			prospective_moves.push_back(test_square);
		}
		else if (test_square.colour == opp_colour)
		{
			prospective_moves.push_back(test_square);
			progress_search = false;
		}
		else if (test_square.colour == target.colour)
		{
			progress_search = false;
		}
	}

	// left
	progress_search = true;
	for (int i = 1; i < DIM_SIZE; i++)
	{
		if ((target.col + i) >= DIM_SIZE) progress_search = false;
		if (!progress_search) continue;
		Square test_square = board[target.row][target.col + i];
		if (test_square.colour == Colour::EMPTY)
		{
			prospective_moves.push_back(test_square);
		}
		else if (test_square.colour == opp_colour)
		{
			prospective_moves.push_back(test_square);
			progress_search = false;
		}
		else if (test_square.colour == target.colour)
		{
			progress_search = false;
		}
	}

	// right
	progress_search = true;
	for (int i = 1; i < DIM_SIZE; i++)
	{
		if ((target.col - i) < 0) progress_search = false;
		if (!progress_search) continue;
		Square test_square = board[target.row][target.col - i];
		if (test_square.colour == Colour::EMPTY)
		{
			prospective_moves.push_back(test_square);
		}
		else if (test_square.colour == opp_colour)
		{
			prospective_moves.push_back(test_square);
			progress_search = false;
		}
		else if (test_square.colour == target.colour)
		{
			progress_search = false;
		}
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
		if (test_square.colour == Colour::EMPTY)
		{
			prospective_moves.push_back(test_square);
		}
		else if (test_square.colour == opp_colour)
		{
			prospective_moves.push_back(test_square);
			progress_search = false;
		}
		else if (test_square.colour == target.colour)
		{
			progress_search = false;
		}
	}

	// down-right
	progress_search = true;
	for (int i = 1; i < DIM_SIZE; i++)
	{
		if ((target.row - i) < 0 || (target.col + i) >= DIM_SIZE) progress_search = false;
		if (!progress_search) continue;
		Square test_square = board[target.row - i][target.col + i];
		if (test_square.colour == Colour::EMPTY)
		{
			prospective_moves.push_back(test_square);
		}
		else if (test_square.colour == opp_colour)
		{
			prospective_moves.push_back(test_square);
			progress_search = false;
		}
		else if (test_square.colour == target.colour)
		{
			progress_search = false;
		}
	}

	// up-left
	progress_search = true;
	for (int i = 1; i < DIM_SIZE; i++)
	{
		if ((target.row + i) >= DIM_SIZE || (target.col - i) < 0) progress_search = false;
		if (!progress_search) continue;
		Square test_square = board[target.row + i][target.col - i];
		if (test_square.colour == Colour::EMPTY)
		{
			prospective_moves.push_back(test_square);
		}
		else if (test_square.colour == opp_colour)
		{
			prospective_moves.push_back(test_square);
			progress_search = false;
		}
		else if (test_square.colour == target.colour)
		{
			progress_search = false;
		}
	}

	// down-left
	progress_search = true;
	for (int i = 1; i < DIM_SIZE; i++)
	{
		if ((target.row - i) < 0 || (target.col - i) < 0) progress_search = false;
		if (!progress_search) continue;
		Square test_square = board[target.row - i][target.col - i];
		if (test_square.colour == Colour::EMPTY)
		{
			prospective_moves.push_back(test_square);
		}
		else if (test_square.colour == opp_colour)
		{
			prospective_moves.push_back(test_square);
			progress_search = false;
		}
		else if (test_square.colour == target.colour)
		{
			progress_search = false;
		}
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

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if ((target.row + i >= DIM_SIZE) || (target.row + i < 0) || (target.col + j >= DIM_SIZE) || (target.col + j < 0))
				continue;

			Square test_square = board[target.row + i][target.col + j];
			if (test_square.piece == Piece::KING) continue;
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


vector<Square> Chessboard::find_valid_moves(Square target)
{
	vector<Square> prospective_moves;
	vector<vector<Square>> board = this->board;
	Colour opp_colour = (target.colour == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;

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
	this->valid_moves = confirmed_moves;
	return confirmed_moves;
}


void print_board(Chessboard chessboard)
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

	std::cout << "Current board : \n\n";
	for (int i = 0; i < DIM_SIZE; i++)
	{
		std::cout << (char)('0' + (DIM_SIZE - i)) << ' ';
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
			if (count(chessboard.valid_moves.begin(), chessboard.valid_moves.end(), chessboard.board[DIM_SIZE - (1 + i)][j]) > 0)
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

			std::cout << colourcode << piece_map[chessboard.board[DIM_SIZE - (1 + i)][j].piece] << "\033[0m";
		}
		std::cout << '\n';
	}
	std::cout << "\n  abcdefgh\n\n";
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


string convert_int_to_chessboard_square(int row, int col)
{
	string res = "";
	res += ('a' + row);
	res += ('1' + col);
	return res;
}


vector<int> convert_chessboard_square_to_int(string position)
{
	vector<int> res;
	res.push_back(position[1] - '1');
	res.push_back(position[0] - 'a');
	return res;
}


void loop_board(Chessboard cb)
{
	while(true)
	{
		print_board(cb);

		// first select the piece to move and get a list of the squares the piece can move to
		string target_square, destination_square;
		std::cout << "Input target square: ";
		getline(cin, target_square);

		vector<int> target_position = convert_chessboard_square_to_int(target_square);

		std::cout << "Moves: ";
		vector<Square> vms = cb.find_valid_moves(cb.board[target_position[0]][target_position[1]]);
		for (int i = 0; i < vms.size(); i++)
		{
			std::cout << convert_int_to_chessboard_square(vms[i].col, vms[i].row) << ' ';
		}
		std::cout << '\n';

		print_board(cb);

		// then get the square to move to, and if on the list, make the move
		std::cout << "\nChoose destination square: ";
		getline(cin, destination_square);
		if (destination_square == "") continue;

		vector<int> destination_position = convert_chessboard_square_to_int(destination_square);

		for (int i = 0; i < vms.size(); i++)
		{
			if (vms[i].row == destination_position[0] && vms[i].col == destination_position[1])
			{
				// move the piece
				cb.board[destination_position[0]][destination_position[1]].piece = cb.board[target_position[0]][target_position[1]].piece;
				cb.board[destination_position[0]][destination_position[1]].colour = cb.board[target_position[0]][target_position[1]].colour;
				cb.board[destination_position[0]][destination_position[1]].has_moved = true;

				cb.board[target_position[0]][target_position[1]] = Square(target_position[0], target_position[1]);
			}
		}

		cb.valid_moves = vector<Square>();
	}

	return;
}


int main()
{
	Chessboard cb = Chessboard();
	loop_board(cb);
	
}