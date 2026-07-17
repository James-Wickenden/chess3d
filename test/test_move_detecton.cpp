#include <gtest/gtest.h>
#include "logic.hpp"
#include "file_handler.hpp"

using namespace LogicEngine;
using namespace ConsoleEngine;
using namespace std;
using namespace FileHandler;
namespace fs = std::filesystem;

TEST(DetectMoves, DetectQueenMoves) {
	Chessboard queen_board("positions/test_blank.txt");
	vector<Square> queen_moves;
	int queen_col = 3, queen_row = 5;

	// Test with a board with just a white queen in the middle
	queen_board.board[queen_col][queen_row] = Square(Piece::QUEEN, Colour::WHITE, queen_col, queen_row, false, {});
	queen_moves = get_valid_square_moves(queen_board.board[queen_col][queen_row], queen_board, Colour::BLACK);
	ASSERT_EQ(queen_moves.size(), 25);

	// Now add a black rook to block the queen's path orthogonally and test again
	queen_board.board[3][4] = Square(Piece::ROOK, Colour::BLACK, 3, 4, false, {});
	queen_moves = get_valid_square_moves(queen_board.board[queen_col][queen_row], queen_board, Colour::BLACK);
	ASSERT_EQ(queen_moves.size(), 21);

	// Now add a white rook to block the queen's path diagonally and test again
	queen_board.board[4][6] = Square(Piece::ROOK, Colour::WHITE, 4, 6, false, {});
	queen_moves = get_valid_square_moves(queen_board.board[queen_col][queen_row], queen_board, Colour::BLACK);
	ASSERT_EQ(queen_moves.size(), 19);
}

TEST(DetectMoves, DetectKnightMovesInCentre) {
	Chessboard knight_board("positions/test_blank.txt");
	vector<Square> knight_moves;
	int knight_col = 3, knight_row = 5;

	// Test with a board with just a black knight in the middle
	knight_board.board[knight_col][knight_row] = Square(Piece::KNIGHT, Colour::BLACK, knight_col, knight_row, false, {});
	knight_moves = get_valid_square_moves(knight_board.board[knight_col][knight_row], knight_board, Colour::WHITE);
	ASSERT_EQ(knight_moves.size(), 8);

	// Now surround the knight in white rooks which should not block the knight's path and test again
	for (int i = -1; i < 1; i++)
	{
		for (int j = -1; j < 1; j++)
		{
			if (i == 0 && j == 0) continue; // Skip the knight's position
			knight_board.board[knight_col + i][knight_row + j] = Square(Piece::ROOK, Colour::WHITE, knight_col + i, knight_row + j, false, {});
		}
	}
	knight_moves = get_valid_square_moves(knight_board.board[knight_col][knight_row], knight_board, Colour::WHITE);
	ASSERT_EQ(knight_moves.size(), 8);

	// Now add a black pawn to block a knight's target and test again
	knight_board.board[1][4] = Square(Piece::PAWN, Colour::BLACK, 1, 4, false, {});
	knight_moves = get_valid_square_moves(knight_board.board[knight_col][knight_row], knight_board, Colour::WHITE);
	ASSERT_EQ(knight_moves.size(), 7);
}

TEST(DetectMoves, DetectKnightMovesInCorner) {
	Chessboard knight_board("positions/test_blank.txt");
	vector<Square> knight_moves;
	int knight_col = 7, knight_row = 7;

	// Test with a board with just a black knight in the corner
	knight_board.board[knight_col][knight_row] = Square(Piece::KNIGHT, Colour::BLACK, knight_col, knight_row, false, {});
	knight_moves = get_valid_square_moves(knight_board.board[knight_col][knight_row], knight_board, Colour::WHITE);
	ASSERT_EQ(knight_moves.size(), 2);
}

TEST(DetectMoves, DetectKingMoves) {
	Chessboard king_board("positions/test_blank.txt");
	vector<Square> king_moves;
	int king_col = 0, king_row = 0;

	// Test with a board with just a white king in the corner
	king_moves = get_valid_square_moves(king_board.board[king_col][king_row], king_board, Colour::BLACK);
	ASSERT_EQ(king_moves.size(), 3);
}

TEST(DetectMoves, DetectKingMovesIntoDanger) {
	Chessboard king_board("positions/test_blank.txt");
	vector<Square> king_moves;
	int king_col = 4, king_row = 4;

	// Move the king to the centre of the board
	switch_pieces(&king_board, { 0, 0 }, { king_col, king_row });

	// Add some black pieces that could capture the white king in certain squares
	king_board.board[4][6] = Square(Piece::KING, Colour::BLACK, 4, 6, false, {});
	king_board.board[3][0] = Square(Piece::ROOK, Colour::BLACK, 3, 0, false, {});
	king_board.board[4][3] = Square(Piece::PAWN, Colour::BLACK, 4, 3, false, {});

	king_moves = get_valid_square_moves(king_board.board[king_col][king_row], king_board, Colour::BLACK);
	ASSERT_EQ(king_moves.size(), 3);
}

TEST(DetectMoves, DetectPawnMovesFoward) {
	Chessboard pawn_board("positions/test_blank.txt");
	vector<Square> pawn_moves;

	// A white pawn on the second rank should be able to move one or two squares forward
	int pawn_row = 1, pawn_col = 4;
	pawn_board.board[pawn_row][pawn_col] = Square(Piece::PAWN, Colour::WHITE, pawn_row, pawn_col, false, {});
	pawn_moves = get_valid_square_moves(pawn_board.board[pawn_row][pawn_col], pawn_board, Colour::BLACK);
	ASSERT_EQ(pawn_moves.size(), 2);

	// Move the pawn forward to the seventh rank
	switch_pieces(&pawn_board, { pawn_row, pawn_col }, { 6, pawn_col });
	pawn_board.board[6][pawn_col].has_moved = true;
	pawn_moves = get_valid_square_moves(pawn_board.board[6][pawn_col], pawn_board, Colour::BLACK);
	ASSERT_EQ(pawn_moves.size(), 1);

	// Move the pawn forward to the eighth rank
	switch_pieces(&pawn_board, { pawn_row, pawn_col }, { 7, pawn_col });
	pawn_moves = get_valid_square_moves(pawn_board.board[7][pawn_col], pawn_board, Colour::BLACK);
	ASSERT_EQ(pawn_moves.size(), 0);

	// A black pawn on the seventh rank should be able to move one or two squares forward
	pawn_row = 7, pawn_col = 5;
	pawn_board.board[pawn_row][pawn_col] = Square(Piece::PAWN, Colour::BLACK, pawn_row, pawn_col, false, {});
	pawn_moves = get_valid_square_moves(pawn_board.board[pawn_row][pawn_col], pawn_board, Colour::WHITE);
	ASSERT_EQ(pawn_moves.size(), 2);
}

TEST(DetectMoves, DetectPawnMovesAttacking) {
	Chessboard pawn_board("positions/test_blank.txt");
	vector<Square> pawn_moves;

	// A white pawn on the second rank should be able to move one or two squares forward
	// If there are black pieces diagonally in front of it, it should be able to capture them
	int pawn_row = 1, pawn_col = 4;
	pawn_board.board[pawn_row][pawn_col] = Square(Piece::PAWN, Colour::WHITE, pawn_row, pawn_col, false, {});
	
	pawn_board.board[pawn_row + 1][pawn_col - 1] = Square(Piece::ROOK, Colour::BLACK, pawn_row + 1, pawn_col - 1, false, {});
	pawn_board.board[pawn_row + 1][pawn_col + 1] = Square(Piece::ROOK, Colour::BLACK, pawn_row + 1, pawn_col + 1, false, {});
	
	pawn_moves = get_valid_square_moves(pawn_board.board[pawn_row][pawn_col], pawn_board, Colour::BLACK);
	ASSERT_EQ(pawn_moves.size(), 4);

	// If the rooks are white they should not be capturable
	pawn_board.board[pawn_row + 1][pawn_col - 1].colour = Colour::WHITE;
	pawn_board.board[pawn_row + 1][pawn_col + 1].colour = Colour::WHITE;

	pawn_moves = get_valid_square_moves(pawn_board.board[pawn_row][pawn_col], pawn_board, Colour::BLACK);
	ASSERT_EQ(pawn_moves.size(), 2);
}