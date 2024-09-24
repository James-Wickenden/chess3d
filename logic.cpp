// logic.cpp

#include "logic.hpp"

using namespace std;
using namespace LogicEngine;

const int DIM_SIZE = 8;


Square::Square()
{
	colour = Colour::EMPTY;
	piece = Piece::EMPTY;
}

Square::Square(Piece piece_type, Colour c)
{
	colour = c;
	piece = piece_type;
}

vector<Square> Chessboard::get_valid_moves()
{
	return vector<Square>();
}


void print_board(Chessboard cb)
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
			cout << piece_map[cb.board[i][j].piece];
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
}


int main()
{
	Chessboard cb = Chessboard();
	print_board(cb);

}