#include <gtest/gtest.h>
#include "logic.hpp"
#include "file_handler.hpp"

using namespace LogicEngine;
using namespace ConsoleEngine;
using namespace std;
using namespace FileHandler;
namespace fs = std::filesystem;

TEST(PromotionTest, AssertPromotionIsHandledCorrectly) {
	Chessboard test_board("positions/test_promotion.txt");
	get_valid_and_attacking_moves(&test_board);

	// Redirect cout to stringstream buffer
	streambuf* sbuf = std::cout.rdbuf();
	stringstream cout_buffer;
	cout.rdbuf(cout_buffer.rdbuf());

	// Simulate user input for promoting to a queen
	string input = "Queen\nexit\n";
	istringstream iss(input);
	cin.rdbuf(iss.rdbuf());

	make_move(&test_board, 
			  test_board.find_valid_moves(test_board.board[6][6]),
			  { 6, 6 },
			  { 7, 6 });

	ASSERT_EQ(test_board.board[6][6].piece, Piece::EMPTY);
	ASSERT_EQ(test_board.board[7][6].piece, Piece::QUEEN);

	// When done redirect cout to its old self
	cout_buffer.str("");
	cout.rdbuf(sbuf);
}

TEST(CastlingTest, AssertLongCastlingIsHandledCorrectly) {
	Chessboard test_board("positions/test_castling.txt");
	test_board.active_player = Colour::BLACK;
	get_valid_and_attacking_moves(&test_board);

	// Redirect cout to stringstream buffer
	streambuf* sbuf = std::cout.rdbuf();
	stringstream cout_buffer;
	cout.rdbuf(cout_buffer.rdbuf());

	make_move(&test_board,
		test_board.find_valid_moves(test_board.board[7][4]),
		{ 7, 4 },
		{ 7, 2 });

	ASSERT_EQ(test_board.board[7][4].piece, Piece::EMPTY);
	ASSERT_EQ(test_board.board[7][2].piece, Piece::KING);
	ASSERT_EQ(test_board.board[7][3].piece, Piece::ROOK);
	ASSERT_EQ(test_board.board[7][0].piece, Piece::EMPTY);

	// When done redirect cout to its old self
	cout_buffer.str("");
	cout.rdbuf(sbuf);
}

TEST(CastlingTest, AssertShortCastlingIsHandledCorrectly) {
	Chessboard test_board("positions/test_castling.txt");
	test_board.active_player = Colour::BLACK;
	get_valid_and_attacking_moves(&test_board);

	// Redirect cout to stringstream buffer
	streambuf* sbuf = std::cout.rdbuf();
	stringstream cout_buffer;
	cout.rdbuf(cout_buffer.rdbuf());

	make_move(&test_board,
		test_board.find_valid_moves(test_board.board[7][4]),
		{ 7, 4 },
		{ 7, 6 });

	ASSERT_EQ(test_board.board[7][4].piece, Piece::EMPTY);
	ASSERT_EQ(test_board.board[7][6].piece, Piece::KING);
	ASSERT_EQ(test_board.board[7][5].piece, Piece::ROOK);
	ASSERT_EQ(test_board.board[7][7].piece, Piece::EMPTY);

	// When done redirect cout to its old self
	cout_buffer.str("");
	cout.rdbuf(sbuf);
}

TEST(CastlingTest, AssertCastlingThroughOrIntoCheckIsInvalid) {
	Chessboard test_board("positions/test_castling.txt");
	vector<Square> king_moves;

	// The white king should have three valid moves as it cannot castle
	king_moves = test_board.find_valid_moves(test_board.board[0][4]);
	ASSERT_EQ(king_moves.size(), 3);
}

TEST(EnPassantTest, AssertEnPassantIsHandledCorrectly) {
	Chessboard test_board("positions/test_ep.txt");
	vector<Square> pawn_moves;
	//get_valid_and_attacking_moves(&test_board);

	switch_pieces(&test_board, { 1, 6 }, { 3, 6 }); // Move white g pawn two squares forward
	test_board.board[3][6].has_moved = true;
	test_board.board[3][6].when_moved = vector<int>({ 1 });
	test_board.move_no = 2;

	// Now both black pawns should be able to capture en passant
	pawn_moves = test_board.find_valid_moves(test_board.board[3][5]);
	ASSERT_EQ(pawn_moves.size(), 2);

	pawn_moves = test_board.find_valid_moves(test_board.board[3][7]);
	ASSERT_EQ(pawn_moves.size(), 2);
}

TEST(EnPassantTest, AssertEnPassantIsNotGivenWhenOpportunityPassed) {
	Chessboard test_board("positions/test_ep.txt");
	vector<Square> pawn_moves;
	//get_valid_and_attacking_moves(&test_board);

	switch_pieces(&test_board, { 1, 6 }, { 3, 6 }); // Move white g pawn two squares forward
	test_board.board[3][6].has_moved = true;
	test_board.board[3][6].when_moved = vector<int>({ 1 });
	test_board.move_no = 3;

	// Neither pawn should be able to capture en passant as the opportunity has passed
	pawn_moves = test_board.find_valid_moves(test_board.board[3][5]);
	ASSERT_EQ(pawn_moves.size(), 1);

	pawn_moves = test_board.find_valid_moves(test_board.board[3][7]);
	ASSERT_EQ(pawn_moves.size(), 1);
}

TEST(EnPassantTest, AssertEnPassantIsNotGivenWhenTwoIndividualSquaresMoved) {
	Chessboard test_board("positions/test_ep.txt");
	vector<Square> pawn_moves;
	//get_valid_and_attacking_moves(&test_board);

	switch_pieces(&test_board, { 1, 6 }, { 3, 6 }); // Move white g pawn two squares forward
	test_board.board[3][6].has_moved = true;
	test_board.board[3][6].when_moved = vector<int>({ 1, 3 });
	test_board.move_no = 4;

	// Neither pawn should be able to capture en passant as the opportunity has passed
	pawn_moves = test_board.find_valid_moves(test_board.board[3][5]);
	ASSERT_EQ(pawn_moves.size(), 1);

	pawn_moves = test_board.find_valid_moves(test_board.board[3][7]);
	ASSERT_EQ(pawn_moves.size(), 1);
}
