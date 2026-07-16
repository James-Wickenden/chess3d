#include <gtest/gtest.h>
#include "logic.hpp"
#include "console.hpp"

using namespace LogicEngine;
using namespace ConsoleEngine;
using namespace std;
using namespace FileHandler;
namespace fs = std::filesystem;

vector<int> simulate_target_square_input(const string& input, Chessboard* test_board, stack<Chessboard>* board_stack, stringstream* cout_buffer);
vector<int> simulate_destination_square_input(const string& input, Chessboard* test_board, vector<int> target_position, stringstream* cout_buffer);

const int NUM_TEST_POSITIONS = 8;

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
	result = simulate_target_square_input("undo\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_EQ(board_stack.size(), 2);
	ASSERT_TRUE(cout_buffer.str().find("Undoing move") != std::string::npos);
	cout_buffer.str("");

	result = simulate_target_square_input("undo\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_EQ(board_stack.size(), 1);
	ASSERT_TRUE(cout_buffer.str().find("Undoing move") != std::string::npos);
	cout_buffer.str("");
	
	result = simulate_target_square_input("undo\nexit\n", &test_board, &board_stack, &cout_buffer);
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
	result = simulate_target_square_input("z2\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Invalid input") != std::string::npos);
	cout_buffer.str("");

	result = simulate_target_square_input("e9\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Invalid input") != std::string::npos);
	cout_buffer.str("");

	result = simulate_target_square_input("TEST!!!!\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Invalid input") != std::string::npos);
	cout_buffer.str("");

	result = simulate_target_square_input(" \nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Invalid input") != std::string::npos);
	cout_buffer.str("");

	result = simulate_target_square_input("e7\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Piece selected belongs to opposite player") != std::string::npos);
	cout_buffer.str("");

	result = simulate_target_square_input("e3\nexit\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Piece selected belongs to opposite player") != std::string::npos);
	cout_buffer.str("");

	result = simulate_target_square_input("e1\nexit\n", &test_board, &board_stack, &cout_buffer);
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
	result = simulate_target_square_input("e2\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Moves:") != std::string::npos);
	ASSERT_EQ(result, vector<int>({ 1, 4 }));
	cout_buffer.str("");

	result = simulate_target_square_input("b1\n", &test_board, &board_stack, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Moves:") != std::string::npos);
	ASSERT_EQ(result, vector<int>({ 0, 1 }));
	cout_buffer.str("");
}

TEST(GetInputDestinationSquareTest, InvalidInput) {
	// Initialize a chessboard and stack for testing
	Chessboard test_board("positions/starting_position.txt");
	get_valid_and_attacking_moves(&test_board);

	stringstream cout_buffer;
	vector<int> target_square, result;

	// Simulate invalid inputs and check for the expected output
	target_square = { 1, 4 }; // e2

	result = simulate_destination_square_input("e5\nback\n", &test_board, target_square, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Invalid move for that piece") != std::string::npos);
	ASSERT_EQ(result, vector<int>({ -1 }));
	cout_buffer.str("");

	result = simulate_destination_square_input("f3\nback\n", &test_board, target_square, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Invalid move for that piece") != std::string::npos);
	ASSERT_EQ(result, vector<int>({ -1 }));
	cout_buffer.str("");

	result = simulate_destination_square_input("TEST\nback\n", &test_board, target_square, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Invalid input") != std::string::npos);
	ASSERT_EQ(result, vector<int>({ -1 }));
	cout_buffer.str("");

	result = simulate_destination_square_input(" \nback\n", &test_board, target_square, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Invalid input") != std::string::npos);
	ASSERT_EQ(result, vector<int>({ -1 }));
	cout_buffer.str("");

	result = simulate_destination_square_input("e9\nback\n", &test_board, target_square, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Invalid input") != std::string::npos);
	ASSERT_EQ(result, vector<int>({ -1 }));
	cout_buffer.str("");

	result = simulate_destination_square_input("i3\nback\n", &test_board, target_square, &cout_buffer);
	ASSERT_TRUE(cout_buffer.str().find("Invalid input") != std::string::npos);
	ASSERT_TRUE(cout_buffer.str().find("Returning to piece selection") != std::string::npos);
	ASSERT_EQ(result, vector<int>({ -1 }));
	cout_buffer.str("");
}

TEST(GetInputDestinationSquareTest, ValidInput) {
	// Initialize a chessboard and stack for testing
	Chessboard test_board("positions/starting_position.txt");
	get_valid_and_attacking_moves(&test_board);

	stringstream cout_buffer;
	vector<int> target_square, result;

	// Simulate valid input and check for the expected output
	target_square = { 1, 4 }; // e2 pawn
	result = simulate_destination_square_input("e3\n", &test_board, target_square, &cout_buffer);
	ASSERT_EQ(result, vector<int>({ 2, 4 }));
	cout_buffer.str("");

	target_square = { 0, 1 }; // b1 knight
	result = simulate_destination_square_input("c3\n", &test_board, target_square, &cout_buffer);
	ASSERT_EQ(result, vector<int>({ 2, 2 }));
	cout_buffer.str("");
}

TEST(PrintBoardTest, WellformedOutput) {
	// Initialize a chessboard and stack for testing
	Chessboard test_board("positions/starting_position.txt");
	get_valid_and_attacking_moves(&test_board);

	string board_string =
		"PGN : \nCurrent board : "
		"\n\n8 \x1B[40;1;35mR \x1B[0m\x1B[47;1;35mN \x1B[0m\x1B[40;1;35mB "
		"\x1B[0m\x1B[47;1;35mQ \x1B[0m\x1B[40;1;35mK \x1B[0m\x1B[47;1;35mB "
		"\x1B[0m\x1B[40;1;35mN \x1B[0m\x1B[47;1;35mR \x1B[0m\n7 \x1B[47;1;35mP "
		"\x1B[0m\x1B[40;1;35mP \x1B[0m\x1B[47;1;35mP \x1B[0m\x1B[40;1;35mP "
		"\x1B[0m\x1B[47;1;35mP \x1B[0m\x1B[40;1;35mP \x1B[0m\x1B[47;1;35mP "
		"\x1B[0m\x1B[40;1;35mP \x1B[0m\n6 \x1B[40;1m  \x1B[0m\x1B[47;1m  "
		"\x1B[0m\x1B[40;1m  \x1B[0m\x1B[47;1m  \x1B[0m\x1B[40;1m  "
		"\x1B[0m\x1B[47;1m  \x1B[0m\x1B[40;1m  \x1B[0m\x1B[47;1m  "
		"\x1B[0m\n5 \x1B[47;1m  \x1B[0m\x1B[40;1m  \x1B[0m\x1B[47;1m  "
		"\x1B[0m\x1B[40;1m  \x1B[0m\x1B[47;1m  \x1B[0m\x1B[40;1m  "
		"\x1B[0m\x1B[47;1m  \x1B[0m\x1B[40;1m  \x1B[0m\n4 \x1B[40;1m  "
		"\x1B[0m\x1B[47;1m  \x1B[0m\x1B[40;1m  \x1B[0m\x1B[47;1m  "
		"\x1B[0m\x1B[40;1m  \x1B[0m\x1B[47;1m  \x1B[0m\x1B[40;1m  "
		"\x1B[0m\x1B[47;1m  \x1B[0m\n3 \x1B[47;1m  \x1B[0m\x1B[40;1m  "
		"\x1B[0m\x1B[47;1m  \x1B[0m\x1B[40;1m  \x1B[0m\x1B[47;1m  "
		"\x1B[0m\x1B[40;1m  \x1B[0m\x1B[47;1m  \x1B[0m\x1B[40;1m  "
		"\x1B[0m\n2 \x1B[40;1;31mP \x1B[0m\x1B[47;1;31mP "
		"\x1B[0m\x1B[40;1;31mP \x1B[0m\x1B[47;1;31mP "
		"\x1B[0m\x1B[40;1;31mP \x1B[0m\x1B[47;1;31mP "
		"\x1B[0m\x1B[40;1;31mP \x1B[0m\x1B[47;1;31mP "
		"\x1B[0m\n1 \x1B[47;1;31mR \x1B[0m\x1B[40;1;31mN "
		"\x1B[0m\x1B[47;1;31mB \x1B[0m\x1B[40;1;31mQ "
		"\x1B[0m\x1B[47;1;31mK \x1B[0m\x1B[40;1;31mB "
		"\x1B[0m\x1B[47;1;31mN \x1B[0m\x1B[40;1;31mR \x1B[0m\n\n  "
		"a b c d e f g h\n\n\x1B[1;32mWHITE TO MOVE\x1B[0m\n\n";

	// Redirect cout to stringstream buffer
	streambuf* sbuf = std::cout.rdbuf();
	stringstream cout_buffer;
	cout.rdbuf(cout_buffer.rdbuf());

	print_board(test_board, vector<Square>(), Gamestate::NORMAL);
	ASSERT_EQ(cout_buffer.str(), board_string);

	// When done redirect cout to its old self
	cout.rdbuf(sbuf);
}

TEST(GetFileMapTest, CorrectFileMapReturned) {
	int cur_id = 1;
	fs::path test_position_path = fs::current_path().append("positions");

	// Redirect cout to stringstream buffer
	streambuf* sbuf = std::cout.rdbuf();
	stringstream cout_buffer;
	cout.rdbuf(cout_buffer.rdbuf());

	map<int, string> actual_id_game_map = get_file_map(test_position_path, &cur_id);
	
	ASSERT_EQ(actual_id_game_map.size(), NUM_TEST_POSITIONS);

	// When done redirect cout to its old self
	cout_buffer.str("");
	cout.rdbuf(sbuf);
}

TEST(GetFileMapTest, InvalidFileMapHandled) {
	int cur_id = 1;
	fs::path test_position_path = fs::current_path().append("invalid");

	// Simulate user input to accept the "No files found" message and return to menu
	string input = "\n";
	istringstream iss(input);
	cin.rdbuf(iss.rdbuf());

	// Redirect cout to stringstream buffer
	streambuf* sbuf = std::cout.rdbuf();
	stringstream cout_buffer;
	cout.rdbuf(cout_buffer.rdbuf());

	map<int, string> actual_id_game_map = get_file_map(test_position_path, &cur_id);
	map<int, string> failed_id_game_map = { { -1, "No files found" } };
	ASSERT_EQ(actual_id_game_map, failed_id_game_map);

	// When done redirect cout to its old self
	cout_buffer.str("");
	cout.rdbuf(sbuf);
}

vector<int> simulate_target_square_input(const string& input, Chessboard* test_board, stack<Chessboard>* board_stack, stringstream* cout_buffer) {
	istringstream iss(input);
	cin.rdbuf(iss.rdbuf());

	// Redirect cout to stringstream buffer
	streambuf* sbuf = std::cout.rdbuf();
	cout.rdbuf(cout_buffer->rdbuf());
	vector<int> result = ConsoleEngine::get_input_target_square(test_board, board_stack);

	// When done redirect cout to its old self
	cout.rdbuf(sbuf);

	return result;
}

vector<int> simulate_destination_square_input(const string& input, Chessboard* test_board, vector<int> target_position, stringstream* cout_buffer) {
	istringstream iss(input);
	cin.rdbuf(iss.rdbuf());

	// Redirect cout to stringstream buffer
	streambuf* sbuf = std::cout.rdbuf();
	cout.rdbuf(cout_buffer->rdbuf());

	vector<Square> vms = test_board->find_valid_moves(test_board->board[target_position[0]][target_position[1]]);
	vector<int> result = ConsoleEngine::get_input_destination_square(vms);

	// When done redirect cout to its old self
	cout.rdbuf(sbuf);

	return result;
}