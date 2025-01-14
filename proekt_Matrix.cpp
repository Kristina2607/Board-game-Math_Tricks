#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h> 
#include <fstream>

const int MIN_Rows = 4;
const int MIN_Cols = 4;
const int MAX_Value = 10;
//const int MAX_Value_for_multiAndDivision = 4;
const char signs[] = { '+', '-', 'x', '/' };

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

void VisitedMatrixInTheStart(unsigned** visitedMatrix,int rows, int cols)
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

bool isValidMove(Position currP, Position newP, int rows, int cols)
{
		return abs(currP.x - newP.x) <= 1 && abs(currP.y - newP.y) <= 1 &&
			newP.x >= 0 && newP.x < rows && newP.y >= 0 && newP.y < cols &&
			!(currP.x == newP.x && currP.y == newP.y);
}

int addToSumOfPlayer(TableElements** matrix, Position& player)
{
	int resultOfPlayer = 0;
	if (matrix[player.x][player.y].sign == '+') 
	{
		resultOfPlayer += matrix[player.x][player.y].number;
	}
	else if (matrix[player.x][player.y].sign == '-')
	{
		resultOfPlayer -= matrix[player.x][player.y].number;
	}
	else if (matrix[player.x][player.y].sign == 'x') 
	{
		resultOfPlayer *= matrix[player.x][player.y].number;
	}
	else if (matrix[player.x][player.y].sign == '/')
	{
		resultOfPlayer /= matrix[player.x][player.y].number;
	}
	return resultOfPlayer;
}

bool isAccessible(unsigned** visitedMatrix, int rows, int cols, Position& player)
{
	return visitedMatrix[player.x][player.y] == 0;
}

void movePlayer(TableElements** matrix, unsigned** visitedMatrix, int rows, int cols, Position& player, int playerNumber)
{
	int newX, newY;
	std::cout << "Player " << playerNumber <<"Enter your move (row,column):" ;
	std::cin >> newX >> newY;
	Position newP = {newX, newY};

	if (isValidMove(player, newP, rows, cols) && isAccessible(visitedMatrix, rows, cols, newP))
	{
		visitedMatrix[player.x][player.y] = playerNumber;
		player = newP;
		updateVisitedMatrix(visitedMatrix, player, playerNumber);
		addToSumOfPlayer(matrix, player);
	}
	else
	{
		std::cout << "Invalid coordinates! Try again.";
	}
}

void determineWinner(int player1Sum, int player2Sum)
{
	std::cout << "Game over!"<< std::endl;
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

bool hasValidMoves(Position& player, unsigned** visitedMatrix, int rows, int cols) {
	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dy = -1; dy <= 1; dy++) 
		{
			if (dx == 0 && dy == 0)
			{
				continue;
			}
			Position newP(player.x + dx, player.y + dy);
			if (newP.x >= 0 && newP.x < rows && newP.y >= 0 && newP.y < cols && visitedMatrix[newP.x][newP.y] == 0) 
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
	char current;
	int result = 0;
	while (true) {
		current = ifs.get();

		if (ifs.eof()) {
			break;
		}

		if (current == target) {
			result++;
		}
	}
	return result + 1;
}

int countRows(std::ifstream& ifs)
{
	return getCharOccurance(ifs, '\n');
}

void calculateDimensions(const char* inputFile, int& rows, int& cols) {
	std::ifstream ifs(inputFile);

	if (!ifs.is_open()) {
		return;
	}
	int currentPos = ifs.tellg();
	cols = countCols(ifs);
	ifs.seekg(0, currentPos);
	rows = countRows(ifs);
	ifs.close();
}

TableElements** readMatrix(const char* file, TableElements** matrix, int rows, int cols) {
	
	std::ifstream ifs(file);
	if (!ifs.is_open()) {
		return nullptr;
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			//ifs >> matrix[i][j];
		}
	}
	return matrix;
}

double** createCopy(const int* const* matrix, int startingRow, int startingCol, int size)
{
	double** temp = new double* [size];
	for (int i = 0; i < size; i++)
	{
		temp[i] = new double[size];
	}
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			temp[i][j] = matrix[startingRow + i][startingCol + j];
		}
	}
	return temp;
}

void saveMatrix(const char* file, TableElements** matrix, int rows, int cols)
{
	std::ofstream ofs(file);
	if (!ofs.is_open())
	{
		std::cout << "Error: Could not open file for saving.\n";
		return;
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			//ofs << matrix[i][j];
			if (j < cols - 1)
			{
				ofs << " ";
			}
		}
		ofs << std::endl;
	}
	 ofs.close();
	 std::cout << "Matrix saved successfully to file: " << file << std::endl;
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
	VisitedMatrixInTheStart(visitedMatrix, grid_length, grid_width);

	Position player1 = {0,0};
	Position player2 = { grid_length - 1, grid_width - 1 };

	visitedMatrix[player1.x][player1.y] = 1; // Играч 1
	visitedMatrix[player2.x][player2.y] = 2; // Играч 2
	
	fillWithRandomValues(matrix, grid_length, grid_width);
	configureSpecialCells(matrix, grid_length, grid_width);

	const char* inputFile = "matrix.txt";
	readMatrix(inputFile, matrix, grid_length, grid_width);
   
	int player1Sum = 0;
	int player2Sum = 0;

	//Основен цикъл за протичане на играта
	while (hasValidMoves(player1, visitedMatrix, grid_length, grid_width)||
		hasValidMoves(player2, visitedMatrix, grid_length, grid_width))
	{
		clearConsole();
		printMatrix(matrix, visitedMatrix, grid_length, grid_width, hConsole); 
		std::cout << "Player 1's turn. Current score: " << player1Sum << std::endl;
		movePlayer(matrix, visitedMatrix, grid_length, grid_width, player1, 1);

		clearConsole();
		printMatrix(matrix, visitedMatrix, grid_length, grid_width, hConsole);
		std::cout << "Player 2's turn. Current score: " << player2Sum << std::endl;
		movePlayer(matrix,visitedMatrix, grid_length, grid_width, player2, 2);
	}
	determineWinner(player1Sum, player2Sum);
	deleteMatrix(matrix, grid_length);
	deleteVisitedMatrix(visitedMatrix, grid_length);
	return 0;
}
