#include <gtest/gtest.h>
#include "logic.hpp"
#include "console.hpp"

using namespace LogicEngine;
using namespace ConsoleEngine;
using namespace std;

vector<int> simulate_input(const string& input, Chessboard* test_board, stack<Chessboard>* board_stack, stringstream* cout_buffer);

TEST(InputTargetSquareTest, UndoAndExitInput)
{
	// Initialize a chessboard and stack for testing
	Chessboard test_board("positions/starting_position.txt");
	stack<Chessboard> board_stack;
	board_stack.push(test_board);

	// Move white pawn from e2 to e4
	switch_pieces(&test_board, { 1, 4 }, { 3, 4 });
	board_stack.push(test_board);

	// Move black pawn from e7 to e5
	switch_pieces(&test_board, { 6, 4 }, { 4, 4 });
	board_stack.push(test_board);
	
	stringstream cout_buffer;
	vector<int> result;

	// Simulate user input for "undo"
	result = simulate_input("undo\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_EQ(board_stack.size(), 2);
	ASSERT_TRUE(cout_buffer.str().find("Undoing move") != std::string::npos);
	cout_buffer.str("");

	result = simulate_input("undo\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_EQ(board_stack.size(), 1);
	ASSERT_TRUE(cout_buffer.str().find("Undoing move") != std::string::npos);
	cout_buffer.str("");
	
	result = simulate_input("undo\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_EQ(board_stack.size(), 1);
	ASSERT_TRUE(cout_buffer.str().find("No more moves to undo") != std::string::npos);
	ASSERT_TRUE(cout_buffer.str().find("Returning to menu") != std::string::npos);
}

TEST(InputTargetSquareTest, InvalidInput)
{
	// Initialize a chessboard and stack for testing
	Chessboard test_board("positions/starting_position.txt");
	stack<Chessboard> board_stack = stack<Chessboard>({ test_board });
	get_valid_and_attacking_moves(&test_board);

	stringstream cout_buffer;
	vector<int> result;

	// Simulate invalid inputs and check for the expected output
	result = simulate_input("z2\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Invalid input") != std::string::npos);
	cout_buffer.str("");

	result = simulate_input("e9\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Invalid input") != std::string::npos);
	cout_buffer.str("");

	result = simulate_input("TEST!!!!\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Invalid input") != std::string::npos);
	cout_buffer.str("");

	result = simulate_input(" \nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Invalid input") != std::string::npos);
	cout_buffer.str("");

	result = simulate_input("e7\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Piece selected belongs to opposite player") != std::string::npos);
	cout_buffer.str("");

	result = simulate_input("e3\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Piece selected belongs to opposite player") != std::string::npos);
	cout_buffer.str("");

	result = simulate_input("e1\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("No valid moves for that piece") != std::string::npos);
	cout_buffer.str("");
}

TEST(InputTargetSquareTest, ValidInput)
{
	// Initialize a chessboard and stack for testing
	Chessboard test_board("positions/starting_position.txt");
	stack<Chessboard> board_stack = stack<Chessboard>({ test_board });
	get_valid_and_attacking_moves(&test_board);

	stringstream cout_buffer;
	vector<int> result;

	// Simulate valid inputs and check for the expected output
	result = simulate_input("e2\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Moves:") != std::string::npos);
	ASSERT_EQ(result, vector<int>({ 1, 4 }));
	cout_buffer.str("");

	// Simulate valid inputs and check for the expected output
	result = simulate_input("b1\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Moves:") != std::string::npos);
	ASSERT_EQ(result, vector<int>({ 0, 1 }));
	cout_buffer.str("");
}

TEST(GetInputDestinationSquareTest, InvalidInput) {

}

TEST(GetInputDestinationSquareTest, ValidInput) {

}

TEST(PrintBoardTest, WellformedOutput) {

}

TEST(GetFileMapTest, CorrectFileMapReturned) {

}

vector<int> simulate_input(const string& input, Chessboard* test_board, stack<Chessboard>* board_stack, stringstream* cout_buffer) {
	std::istringstream iss(input);
	std::cin.rdbuf(iss.rdbuf());

	// Redirect cout to stringstream buffer
	std::streambuf* sbuf = std::cout.rdbuf();
	std::cout.rdbuf(cout_buffer->rdbuf());
	vector<int> result = ConsoleEngine::get_input_target_square(test_board, board_stack);

	// When done redirect cout to its old self
	std::cout.rdbuf(sbuf);

	return result;
}