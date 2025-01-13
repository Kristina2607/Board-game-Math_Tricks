#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h> 
#include <fstream>
#include "save_in_file.cpp"

const int MIN_Rows = 4;
const int MIN_Cols = 4;
const int MAX_Value = 15;
char signs[] = { '+', '-', 'x', '/' };

struct TableElements
{
	char sign;
	unsigned int number;
};

TableElements** createMatrix (int rows, int cols)
{
	TableElements** matrix = new TableElements*[rows];
	for (int i = 0; i < rows; i++)
	{
		matrix[i] = new TableElements[cols];
	}
	return matrix;
}

unsigned** createVisitedMatrix(int rows, int cols)
{
	unsigned** visitedMatrix = new unsigned* [rows];
	for (int i = 0; i < rows; i++)
	{
		visitedMatrix[i] = new unsigned[cols];
	}
	return visitedMatrix;
}

void fillWithRandomValues(TableElements** matrix, int rows, int cols)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if ((i == 0 && j == 0) || (i == rows - 1 && j == cols - 1))
			{
				matrix[i][j].number = 0;
				matrix[i][j].sign = ' ';
			}
			else
			{
				matrix[i][j].number = rand() % MAX_Value;
				matrix[i][j].sign = signs[rand() % 4];
			}
		}
	}
}
void configureSpecialCells(TableElements** matrix, int rows, int cols)
{
	TableElements requiredFieldsValues[] = {{'x', 0}, {'x', 2}, {'/', 2}};
	int requiredFieldsCount = 3;

	for (int i = 0; i < requiredFieldsCount; i++)
	{
		int randomRow = rand() % rows;
		int randomCol = rand() % cols;
		matrix[randomRow][randomCol].number = requiredFieldsValues[i].number;
		matrix[randomRow][randomCol].sign = requiredFieldsValues[i].sign;
	}	
}

void printMatrix(TableElements** matrix, unsigned** visitedMatrix, int rows, int cols, HANDLE hConsole)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (visitedMatrix[i][j] == 1)
			{
				SetConsoleTextAttribute(hConsole, 9); // Син за играч 1
			}
			else if (visitedMatrix[i][j] == 2)
			{
				SetConsoleTextAttribute(hConsole, 12); // Червен за играч 2
			}
			else 
			{
				SetConsoleTextAttribute(hConsole, 7); // Бял за непосетени
			}
			std::cout << matrix[i][j].sign << matrix[i][j].number << ' ';
		}
		std::cout << std::endl;
	}
}

struct Position
{
	int x;
	int y;
	Position() : x(0), y(0) {} //default constructor
	Position(int x, int y) : x(x), y(y) {} //constructor with parameters
};

bool isValidMove(Position currP, Position newP, int rows, int cols)
{
	for (int i = currP.x - 1; i <= currP.x + 1; i++)
	{
		if (i == newP.x && i >= 0 && i < rows)
		{
			for (int j = currP.y - 1; j <= currP.y + 1; j++)
			{
				if (j == newP.y && j >= 0 && j < cols)
				{
					if (i == currP.x && j == currP.y)
					{
						return false;
					}
					else 
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}


void addToSumOfPlayer(TableElements** matrix, Position player)
{
	int resultOfPlayer1 = 0;
	int resultOfPlayer2 = 0;

	if (matrix[player.x][player.y].sign == '+') {
		resultOfPlayer1 += matrix[player.x][player.y].number;
	}
	else if (matrix[player.x][player.y].sign == '-') {
		resultOfPlayer1 -= matrix[player.x][player.y].number;
	}
	else if (matrix[player.x][player.y].sign == 'x') {
		resultOfPlayer1 *= matrix[player.x][player.y].number;
	}
	else if (matrix[player.x][player.y].sign == '/') {
		resultOfPlayer1 /= matrix[player.x][player.y].number;
	}
}

bool isAccessible(unsigned** visitedMatrix, int rows, int cols, Position& player)
{
	return visitedMatrix[player.x][player.y] == 0;
}

void movePlayer(TableElements** matrix, int rows, int cols, Position& player)
{
	int newX, newY;
	std::cout << "Enter new coordinates: ";
	std::cin >> newX >> newY;
	if (isValidMove(player, Position(newX,newY), rows, cols))
	{
		player.x = newX;
		player.y = newY;
		addToSumOfPlayer(matrix, player);
	}
	else
	{
		std::cout << "Invalid coordinates! Try again.";
	}
}

bool isWinner()
{

}

void deleteMatrix(TableElements** matrix, int rows)
{
	for (int i = 0; i < rows; i++)
	{
		delete[] matrix[i];
	}
	delete[] matrix;
}

int main()
{
	srand(time(0));
	int grid_length, grid_width;
	std::cout << "Input grid size to start the game:";
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
	for (int i = 0; i < grid_length; i++)
	{
		for (int j = 0; j < grid_width; j++)
		{
			visitedMatrix[i][j] = 0;  // Всички полета започват като непосетени 
		}
	}       

	Position player1 = {0,0};
	Position player2 = { grid_length - 1, grid_width - 1 };

	visitedMatrix[player1.x][player1.y] = 1; // Играч 1
	visitedMatrix[player2.x][player2.y] = 2; // Играч 2

	
	fillWithRandomValues(matrix, grid_length, grid_width);
	configureSpecialCells(matrix, grid_length, grid_width);

	const char* inputFile = "matrix.txt";
	int** matrix = readMatrix(inputFile, grid_length, grid_width);
   
	//Основен цикъл за протичане на играта
	while (true)
	{
		printMatrix(matrix, visitedMatrix, grid_length, grid_width, hConsole);
		std::cout << "Player 1, enter your move (row,column): ";
		if (isAccessible(visitedMatrix, grid_length, grid_width, player1))
		{
			visitedMatrix[player1.x][player1.y] = 1;
			movePlayer(matrix, grid_length, grid_width, player1);
		}
		if (isAccessible(visitedMatrix, grid_length, grid_width, player2))
		{
			visitedMatrix[player2.x][player2.y] = 1;
			movePlayer(matrix, grid_length, grid_width, player2);
		}

	}
	printMatrix(matrix,visitedMatrix, grid_length, grid_width, hConsole);

	deleteMatrix(matrix, grid_length);
	//Position p;
	return 0;
}
