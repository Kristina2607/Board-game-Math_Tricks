/* Solution to course project #2 >
* Introduction to programming course
* Faculty of Mathematics and Informatics.of Sofia University
* Winter semester 2024 / 2025
* @author Kristina Georgieva
* @idnumber 3MI0600457
* @compiler  VC
* Math Tricks */


#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h> 
#include <fstream>

const int MIN_Rows = 4;
const int MIN_Cols = 4;
const int MAX_Value_for_addAndSubtraction = 10;
const int MAX_Value_for_multiAndDivision = 4;
const char signs[] = { '+', '-', 'x', '/' };

const int BLUE_color_Player1 = 9;
const int RED_color_Player2 = 12;
const int WHITE_color = 7;
const int YELLOW_color = 14;

struct TableElements
{
	char sign;
	unsigned int number;
};

TableElements** createMatrix(int rows, int cols)
{
	TableElements** matrix = new TableElements * [rows];
	for (int i = 0; i < rows; i++)
	{
		matrix[i] = new TableElements[cols];
	}
	return matrix;
}

struct Position
{
	int x;
	int y;
	Position() : x(0), y(0) {} //default constructor
	Position(int x, int y) : x(x), y(y) {} //constructor with parameters
};

unsigned** createVisitedMatrix(int rows, int cols)
{
	unsigned** visitedMatrix = new unsigned* [rows];
	for (int i = 0; i < rows; i++)
	{
		visitedMatrix[i] = new unsigned[cols];
	}
	return visitedMatrix;
}

bool** createBoolMatrix(int rows, int cols)
{
	bool** matrix = new bool* [rows];
	for (int i = 0; i < rows; i++)
	{
		matrix[i] = new bool[cols] {0};
	}
	return matrix;
}

void deleteBoolMatrix(bool** matrix, int rows)
{
	for (int i = 0; i < rows; i++)
	{
		delete[] matrix[i];
	}
	delete[] matrix;
}

void VisitedMatrixInTheStart(unsigned** visitedMatrix, int rows, int cols)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			visitedMatrix[i][j] = 0;  // Всички полета започват като непосетени 
		}
	}
}

void updateVisitedMatrix(unsigned** visitedMatrix, Position& player, int playerNumber)
{
	visitedMatrix[player.x][player.y] = playerNumber;
}

void fillWithRandomValues(TableElements** matrix, int rows, int cols)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			char sign = signs[rand() % 4];
			int maxValue = 0;
			switch (sign)
			{
			case '-':
			case '+': maxValue = MAX_Value_for_addAndSubtraction; break;
			case 'x':
			case '/': maxValue = MAX_Value_for_multiAndDivision; break;
			}
			int currNumber = rand() % maxValue;
			if (sign == '/' && currNumber == 0)
			{
				j--;
				continue;
			}
			matrix[i][j].number = currNumber;
			matrix[i][j].sign = sign;
		}
	}
}

double differenceFromMiddle(Position& player, int rows, int cols)
{
	int currentRow = player.x;
	int currentCol = player.y;
	int centerRow = rows / 2;
	int centerCol = cols / 2;
	return sqrt((currentRow - centerRow) * (currentRow - centerRow) + (currentCol - centerCol) * (currentCol - centerCol)); //Евклидово разстояние
}

void findWhichRequiredFieldsAreInTheMatrix(TableElements** matrix, int rows, int cols, TableElements* requiredFieldsValues, bool* alreadyInTheMatrix, bool** used, int size)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			for (int k = 0; k < size; k++)
			{
				if (matrix[i][j].number == requiredFieldsValues[k].number &&
					matrix[i][j].sign == requiredFieldsValues[k].sign)
				{
					used[i][j] = true;
					alreadyInTheMatrix[k] = true;
				}
			}
		}
	}
}

void configureSpecialCells(TableElements** matrix, int rows, int cols)
{
	TableElements requiredFieldsValues[] = { {'x', 0}, {'x', 2}, {'/', 2}, {'+', 2}, {'-', 2} };
	int requiredFieldsCount = 5;
	bool alreadyInTheMatrix[5]{ 0 };
	bool** used = createBoolMatrix(rows, cols);
	findWhichRequiredFieldsAreInTheMatrix(matrix, rows, cols, requiredFieldsValues, alreadyInTheMatrix, used, requiredFieldsCount);

	for (int i = 0; i < requiredFieldsCount; i++)
	{
		if (alreadyInTheMatrix[i]) {
			continue;
		}
		int randomRow, randomCol;
		do
		{
			randomRow = rand() % rows;
			randomCol = rand() % cols;
		} while (used[randomRow][randomCol]);

		used[randomRow][randomCol] = true;

		matrix[randomRow][randomCol].number = requiredFieldsValues[i].number;
		matrix[randomRow][randomCol].sign = requiredFieldsValues[i].sign;
	}

	matrix[0][0].number = matrix[rows - 1][cols - 1].number = 0;
	matrix[0][0].sign = matrix[rows - 1][cols - 1].sign = ' ';
	used[0][0] = true;
	used[rows - 1][cols - 1] = true;
	deleteBoolMatrix(used, rows);
}

void printMatrix(TableElements** matrix, unsigned** visitedMatrix, int rows, int cols, Position& player, HANDLE hConsole)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (i == player.x && j == player.y)
			{
				SetConsoleTextAttribute(hConsole, YELLOW_color);
			}
			else if (visitedMatrix[i][j] == 1)
			{
				SetConsoleTextAttribute(hConsole, BLUE_color_Player1 * 16 + WHITE_color);
			}
			else if (visitedMatrix[i][j] == 2)
			{
				SetConsoleTextAttribute(hConsole, RED_color_Player2 * 16 + WHITE_color);
			}
			else
			{
				SetConsoleTextAttribute(hConsole, WHITE_color);
			}
			std::cout << "[ " << matrix[i][j].sign << matrix[i][j].number << " ] ";
		}
		std::cout << std::endl;
	}
	SetConsoleTextAttribute(hConsole, WHITE_color);
}

void saveMatrix(const char* file, TableElements** matrix, int rows, int cols, int& currentSumPlayer1, int& currentSumPlayer2)
{
	std::ofstream ofs(file);
	if (!ofs.is_open())
	{
		std::cout << "Error: Could not open file for saving.\n";
		return;
	}

	ofs << rows << ' ' << cols << '\n';
	ofs << currentSumPlayer1 << ' ' << currentSumPlayer2 << '\n';

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			ofs << matrix[i][j].sign;
			ofs << matrix[i][j].number;
			if (j < cols - 1)
			{
				ofs << " ";
			}
		}
		ofs << std::endl;
	}
	ofs.close();
	std::cout << "Matrix saved successfully to file: " << file << std::endl;
	std::cout << "You can continue playing or exit." << std::endl;
	system("pause");
}

int myAbs(int num)
{
	return (num < 0) ? -num : num;
}

bool isValidMove(Position currP, Position newP, int rows, int cols)
{
	return myAbs(currP.x - newP.x) <= 1 &&
		myAbs(currP.y - newP.y) <= 1 &&
		newP.x >= 0 && newP.x <= rows &&
		newP.y >= 0 && newP.y <= cols &&
		!(currP.x == newP.x && currP.y == newP.y);
}

int addToSumOfPlayer(TableElements** matrix, Position& player, int& currentSum)
{
	if (matrix[player.x][player.y].sign == '+')
	{
		currentSum += matrix[player.x][player.y].number;
	}
	else if (matrix[player.x][player.y].sign == '-')
	{
		currentSum -= matrix[player.x][player.y].number;
	}
	else if (matrix[player.x][player.y].sign == 'x')
	{
		currentSum *= matrix[player.x][player.y].number;
	}
	else if (matrix[player.x][player.y].sign == '/')
	{
		currentSum /= matrix[player.x][player.y].number;
	}
	return currentSum;
}

bool isAccessible(unsigned** visitedMatrix, int rows, int cols, Position& player)
{
	return visitedMatrix[player.x][player.y] == 0;
}

void movePlayer(TableElements** matrix, unsigned** visitedMatrix, int rows, int cols, Position& player, int playerNumber, int& currentSumPlayer1, int& currentSumPlayer2)
{
	int newX, newY;
	std::cout << "Player " << playerNumber << ", enter your move (row,column):";
	std::cin >> newX >> newY;
	Position newP = {newX, newY};

	if (isValidMove(player, newP, rows, cols) && isAccessible(visitedMatrix, rows, cols, newP))
	{
		visitedMatrix[player.x][player.y] = playerNumber;
		player = newP;
		updateVisitedMatrix(visitedMatrix, player, playerNumber);
		if (playerNumber == 1) 
		{
			addToSumOfPlayer(matrix, player, currentSumPlayer1);
		}
		else
		{
			addToSumOfPlayer(matrix, player, currentSumPlayer2);
		}
	}
	else if (newX == -1 && newY == -1)
	{
		saveMatrix("matrix.txt", matrix, rows, cols, currentSumPlayer1, currentSumPlayer2);
	}
	else
	{
		std::cout << "Invalid coordinates! Try again." << std::endl;
		movePlayer(matrix, visitedMatrix, rows, cols, player, playerNumber, currentSumPlayer1, currentSumPlayer2);
	}
}

bool hasValidMoves(Position& player, unsigned** visitedMatrix, int rows, int cols)
{
	for (int distanceX = -1; distanceX <= 1; distanceX++)
	{
		for (int distanceY = -1; distanceY <= 1; distanceY++)
		{
			if (distanceX == 0 && distanceY == 0)
			{
				continue;
			}
			Position newP(player.x + distanceX, player.y + distanceY);
			if (isValidMove(player, newP, rows, cols) && isAccessible(visitedMatrix, rows, cols, newP))
			{
				return true;
			}
		}
	}
	return false;
}

int countCols(std::ifstream& ifs)
{
	char ch;
	int count = 0;
	while (true)
	{
		ch = ifs.get();

		if (ch == ' ')
		{
			count++;
		}

		if (ch == '\n')
		{
			count++;
			break;
		}
	}
	return count;
}

int getCharOccurance(std::ifstream& ifs, char target)
{
	int startPos = ifs.tellg();
	char current;
	int result = 0;
	while (true)
	{
		current = ifs.get();
		if (ifs.eof())
		{
			break;
		}
		if (current == target)
		{
			result++;
		}
	}
	ifs.clear();
	ifs.seekg(0, startPos);
	return result + 1;
}

int countRows(std::ifstream& ifs)
{
	return getCharOccurance(ifs, '\n');
}

bool readMatrix(const char* file, TableElements** matrix, int rows, int cols, int& currentSumPlayer1, int& currentSumPlayer2)
{
	std::ifstream ifs(file);
	if (!ifs.is_open())
	{
		return false;
	}

	int rowCount = countRows(ifs);
	if (rowCount == 1)
	{
		return false;
	}

	int fileRows = 0, fileCols = 0;
	ifs >> fileRows;
	ifs >> fileCols;
	ifs >> currentSumPlayer1 >> currentSumPlayer2;
	ifs.ignore();

	if (fileRows != rows || fileCols != cols)
	{
		return false;
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			matrix[i][j].sign = ifs.get();
			ifs >> matrix[i][j].number;
			ifs.ignore();
		}
	}
	ifs.close();
	return true;
}

void determineWinner(int player1Sum, int player2Sum)
{
	std::cout << "Game over!" << std::endl;
	std::cout << "Player 1's final score: " << player1Sum << std::endl;
	std::cout << "Player 2's final score: " << player2Sum << std::endl;
	if (player1Sum > player2Sum)
	{
		std::cout << "Player 1 wins!" << std::endl;
	}
	else if (player2Sum > player1Sum)
	{
		std::cout << "Player 2 wins!" << std::endl;
	}
	else
	{
		std::cout << "It's a draw!" << std::endl;
	}
}

void clearConsole()
{
	system("cls");
}

void deleteMatrix(TableElements** matrix, int rows)
{
	for (int i = 0; i < rows; i++)
	{
		delete[] matrix[i];
	}
	delete[] matrix;
}

void deleteVisitedMatrix(unsigned** visitedMatrix, int rows)
{
	for (int i = 0; i < rows; i++)
	{
		delete[] visitedMatrix[i];
	}
	delete[] visitedMatrix;
}

int main()
{
	srand(time(0));
	int grid_length, grid_width;
	std::cout << "Let's start the game! " << std::endl;
	std::cout << "You can save the game anytime and continue playing later if you write the coordinates (-1, -1)." << std::endl;
	std::cout << "Input grid size:";
	bool validGridSize = false;
	while (!validGridSize)
	{
		std::cin >> grid_length >> grid_width;
		if (grid_length >= 4 || grid_width >= 4)
		{
			validGridSize = true;
		}
		else
		{
			std::cout << "The matrix has to be at least 4x4! Try again!";
		}
	}

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	TableElements** matrix = createMatrix(grid_length, grid_width);
	unsigned** visitedMatrix = createVisitedMatrix(grid_length, grid_width);
	VisitedMatrixInTheStart(visitedMatrix, grid_length, grid_width);

	Position player1 = { 0,0 };
	Position player2 = { grid_length - 1, grid_width - 1 };

	int player1Sum = 0;
	int player2Sum = 0;

	visitedMatrix[player1.x][player1.y] = 1; // Играч 1
	visitedMatrix[player2.x][player2.y] = 2; // Играч 2

	const char* inputFile = "matrix.txt";
	bool isRead = readMatrix(inputFile, matrix, grid_length, grid_width, player1Sum, player2Sum);

	if (!isRead)
	{
		player1Sum = 0;
		player2Sum = 0;
		std::cout << "No saved game found or invalid file. Starting a new game..." << std::endl;
		fillWithRandomValues(matrix, grid_length, grid_width);
		configureSpecialCells(matrix, grid_length, grid_width);
	}
	else
	{
		char choice;
		std::cout << "A saved game was found for this grid size. Do you want to load it? (y/n}: ";
		std::cin >> choice;
		if (choice == 'n')
		{
				player1Sum = 0;
				player2Sum = 0;
				std::cout << "Starting a new game..." << std::endl;
				fillWithRandomValues(matrix, grid_length, grid_width);
				configureSpecialCells(matrix, grid_length, grid_width);
		}
		else if (choice == 'y')
		{
			std::cout << "Saved game loaded successfully!" << std::endl;
		}
		else
		{
			std::cout << "Invalid input. Please enter 'y' or 'n'." << std::endl;
		}
	}

	//Основен цикъл за протичане на играта
	while (hasValidMoves(player1, visitedMatrix, grid_length, grid_width) &&
		hasValidMoves(player2, visitedMatrix, grid_length, grid_width))
	{
		clearConsole();
		printMatrix(matrix, visitedMatrix, grid_length, grid_width, player1, hConsole);
		std::cout << "Player 1's turn. Current score: " << player1Sum << std::endl;
		movePlayer(matrix, visitedMatrix, grid_length, grid_width, player1, 1, player1Sum, player2Sum);

		clearConsole();
		printMatrix(matrix, visitedMatrix, grid_length, grid_width, player2, hConsole);
		std::cout << "Player 2's turn. Current score: " << player2Sum << std::endl;
		movePlayer(matrix, visitedMatrix, grid_length, grid_width, player2, 2, player1Sum, player2Sum);
	}
	determineWinner(player1Sum, player2Sum);
	deleteMatrix(matrix, grid_length);
	deleteVisitedMatrix(visitedMatrix, grid_length);
	return 0;
}
