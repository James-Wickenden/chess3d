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

Square::Square(Piece piece_type, Colour c)
{
	colour = c;
	piece = piece_type;
	row = -1;
	col = -1;
	has_moved = false;
}

void Chessboard::find_valid_moves(Square target)
{
	vector<Square> valid_moves;
	vector<vector<Square>> board = this->board;

	switch (target.piece)
	{
	case Piece::EMPTY:
		break;
	case Piece::PAWN:
		// use +1 for white and -1 for black to find valid squares in the direction (dir) the pawn moves in
		int dir = (target.colour == Colour::WHITE) ? 1 : -1;

		if ((target.colour == Colour::WHITE && target.row < 7) || (target.colour == Colour::BLACK && target.row > 0))
		{
			if (board[target.row + dir][target.col].colour == Colour::EMPTY)
			{
				valid_moves.push_back(board[target.row + dir][target.col]);

				// case for pawns on the starting file.
				// note this will cause errors on custom boards with pawns being able to move two spaces from different files. ignore for now.
				if (!target.has_moved) 
				{
					if (board[target.row + (dir * 2)][target.col].colour == Colour::EMPTY)
					{
						valid_moves.push_back(board[target.row + (dir * 2)][target.col]);
					}
				}
			}
		}
		break;
	}

	this->valid_moves = valid_moves;
	return;
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

	for (int i = 0; i < DIM_SIZE; i++)
	{
		for (int j = 0; j < DIM_SIZE; j++)
		{
			cout << piece_map[chessboard.board[i][j].piece];
		}
		cout << '\n';
	}
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


int main()
{
	Chessboard cb = Chessboard();
	print_board(cb);
	cb.find_valid_moves(cb.board[1][0]);
}