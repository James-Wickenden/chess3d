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

	return prospective_moves;
}


vector<Square> get_prospective_rook_moves(Square target, vector<vector<Square>> board, Colour opp_colour)
{
	vector<Square> prospective_moves;

	// forwards
	bool progress_search = true;
	for (int i = 0; i < DIM_SIZE; i++)
	{
		if ((target.row + i) >= DIM_SIZE) progress_search = false;
		if (!progress_search) continue;
		if (board[target.row + i][target.col].colour == Colour::EMPTY)
		{
			prospective_moves.push_back(board[target.row + i][target.col]);
		}
		else if (board[target.row + i][target.col].colour == opp_colour)
		{
			prospective_moves.push_back(board[target.row + i][target.col]);
			progress_search = false;
		}
		else if (board[target.row + i][target.col].colour == target.colour)
		{
			progress_search = false;
		}
	}

	// backwards
	progress_search = true;
	for (int i = 0; i < DIM_SIZE; i++)
	{
		if ((target.row - i) >= DIM_SIZE) progress_search = false;
		if (!progress_search) continue;
		if (board[target.row - i][target.col].colour == Colour::EMPTY)
		{
			prospective_moves.push_back(board[target.row - i][target.col]);
		}
		else if (board[target.row - i][target.col].colour == opp_colour)
		{
			prospective_moves.push_back(board[target.row - i][target.col]);
			progress_search = false;
		}
		else if (board[target.row - i][target.col].colour == target.colour)
		{
			progress_search = false;
		}
	}

	// left
	progress_search = true;
	for (int i = 0; i < DIM_SIZE; i++)
	{
		if ((target.col + i) >= DIM_SIZE) progress_search = false;
		if (!progress_search) continue;
		if (board[target.row][target.col + i].colour == Colour::EMPTY)
		{
			prospective_moves.push_back(board[target.row][target.col + i]);
		}
		else if (board[target.row][target.col + i].colour == opp_colour)
		{
			prospective_moves.push_back(board[target.row][target.col + i]);
			progress_search = false;
		}
		else if (board[target.row][target.col + i].colour == target.colour)
		{
			progress_search = false;
		}
	}

	// right
	progress_search = true;
	for (int i = 0; i < DIM_SIZE; i++)
	{
		if ((target.col - i) >= DIM_SIZE) progress_search = false;
		if (!progress_search) continue;
		if (board[target.row][target.col - i].colour == Colour::EMPTY)
		{
			prospective_moves.push_back(board[target.row][target.col - i]);
		}
		else if (board[target.row][target.col - i].colour == opp_colour)
		{
			prospective_moves.push_back(board[target.row][target.col - i]);
			progress_search = false;
		}
		else if (board[target.row][target.col - i].colour == target.colour)
		{
			progress_search = false;
		}
	}

	return prospective_moves;
}


vector<Square> get_prospective_bishop_moves(Square target, vector<vector<Square>> board, Colour opp_colour)
{
	vector<Square> prospective_moves;

	// up-right
	bool progress_search = true;
	for (int i = 0; i < DIM_SIZE; i++)
	{
		if ((target.row + i) >= DIM_SIZE && (target.col + i) >= DIM_SIZE) progress_search = false;
		if (!progress_search) continue;
		if (board[target.row + i][target.col + i].colour == Colour::EMPTY)
		{
			prospective_moves.push_back(board[target.row + i][target.col + i]);
		}
		else if (board[target.row + i][target.col + i].colour == opp_colour)
		{
			prospective_moves.push_back(board[target.row + i][target.col + i]);
			progress_search = false;
		}
		else if (board[target.row + i][target.col + i].colour == target.colour)
		{
			progress_search = false;
		}
	}

	// down-right
	progress_search = true;
	for (int i = 0; i < DIM_SIZE; i++)
	{
		if ((target.row - i) >= DIM_SIZE && (target.col + i) >= DIM_SIZE) progress_search = false;
		if (!progress_search) continue;
		if (board[target.row - i][target.col + i].colour == Colour::EMPTY)
		{
			prospective_moves.push_back(board[target.row - i][target.col + i]);
		}
		else if (board[target.row - i][target.col + i].colour == opp_colour)
		{
			prospective_moves.push_back(board[target.row - i][target.col + i]);
			progress_search = false;
		}
		else if (board[target.row - i][target.col + i].colour == target.colour)
		{
			progress_search = false;
		}
	}

	// up-left
	progress_search = true;
	for (int i = 0; i < DIM_SIZE; i++)
	{
		if ((target.row + i) >= DIM_SIZE && (target.col - i) >= DIM_SIZE) progress_search = false;
		if (!progress_search) continue;
		if (board[target.row + i][target.col - i].colour == Colour::EMPTY)
		{
			prospective_moves.push_back(board[target.row + i][target.col - i]);
		}
		else if (board[target.row + i][target.col - i].colour == opp_colour)
		{
			prospective_moves.push_back(board[target.row + i][target.col - i]);
			progress_search = false;
		}
		else if (board[target.row + i][target.col - i].colour == target.colour)
		{
			progress_search = false;
		}
	}

	// down-left
	progress_search = true;
	for (int i = 0; i < DIM_SIZE; i++)
	{
		if ((target.row - i) >= DIM_SIZE && (target.col - i) >= DIM_SIZE) progress_search = false;
		if (!progress_search) continue;
		if (board[target.row - i][target.col - i].colour == Colour::EMPTY)
		{
			prospective_moves.push_back(board[target.row - i][target.col - i]);
		}
		else if (board[target.row - i][target.col - i].colour == opp_colour)
		{
			prospective_moves.push_back(board[target.row - i][target.col - i]);
			progress_search = false;
		}
		else if (board[target.row - i][target.col - i].colour == target.colour)
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


vector<Square> get_prospective_knight_moves(Square target, vector<vector<Square>> board, Colour opp_colour)
{
	vector<Square> prospective_moves;


	return prospective_moves;
}


vector<Square> get_prospective_king_moves(Square target, vector<vector<Square>> board, Colour opp_colour)
{
	vector<Square> prospective_moves;


	return prospective_moves;
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

	this->valid_moves = prospective_moves;
	return prospective_moves;
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

	cout << "Current board : \n\n";
	for (int i = 0; i < DIM_SIZE; i++)
	{
		cout << (char)('0' + (DIM_SIZE - i)) << ' ';
		for (int j = 0; j < DIM_SIZE; j++)
		{
			cout << piece_map[chessboard.board[DIM_SIZE-(1+i)][j].piece];
		}
		cout << '\n';
	}
	cout << "\n  abcdefgh\n\n";
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
		cout << "Input target square: ";
		getline(cin, target_square);

		vector<int> target_position = convert_chessboard_square_to_int(target_square);

		cout << "Moves: ";
		vector<Square> vms = cb.find_valid_moves(cb.board[target_position[0]][target_position[1]]);
		for (int i = 0; i < vms.size(); i++)
		{
			cout << convert_int_to_chessboard_square(vms[i].col, vms[i].row) << ' ';
		}

		// then get the square to move to, and if on the list, make the move
		cout << "\nChoose destination square: ";
		getline(cin, destination_square);

		vector<int> destination_position = convert_chessboard_square_to_int(destination_square);

		for (int i = 0; i < vms.size(); i++)
		{
			if (vms[i].row == destination_position[0] && vms[i].col == destination_position[1])
			{
				// move the piece
				cb.board[destination_position[0]][destination_position[1]] = cb.board[target_position[0]][target_position[1]];
				cb.board[target_position[0]][target_position[1]] = Square(target_position[0], target_position[1]);
			}
		}
	}

	return;
}


int main()
{
	Chessboard cb = Chessboard();
	loop_board(cb);
	
}