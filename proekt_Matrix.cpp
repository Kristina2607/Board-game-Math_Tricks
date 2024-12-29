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
	struct requiredFields
	{
		char sign;
		unsigned int number;
	};

	requiredFields requiredFieldsValues[] = {
		{'x', 0}, {'x', 2}, {'/', 2}
	};
	int requiredFieldsCount = 3;

	for (int i = 0; i < requiredFieldsCount; i++)
	{
		int randomRow = rand() % rows;
		int randomCol = rand() % cols;
		matrix[randomRow][randomCol].number = requiredFieldsValues[i].number;
		matrix[randomRow][randomCol].sign = requiredFieldsValues[i].sign;
	}	
}

void playersResult(TableElements** matrix) 
{
	int result = 0;
}

void printMatrix(TableElements** matrix, int rows, int cols)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			std::cout << matrix[i][j].sign << matrix[i][j].number << ' ';
		}
		std::cout << std::endl;
	}
}

bool isValidMove(int x, int y, int rows, int cols)
{
	return (x >= 0 && y >= 0 && x <= rows && y <= cols);
}

struct Position
{
	unsigned int x;
	unsigned int y;
};

void movePlayer(TableElements** matrix, int rows, int cols, Position& player)
{
	int newX, newY;
	std::cout << "Enter new coordinates: ";
	std::cin >> newX >> newY;
	if (isValidMove(newX, newY, rows, cols))
	{
		player.x = newX;
		player.y = newY;
	}
	else
	{
		std::cout << "Invalid coordinates! Try again.";
	}
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
	std::cin >> grid_length >> grid_width;
	if (grid_length < 4 || grid_width < 4)
	{
		std::cout << "The matrix has to be at least 4x4!";
		return -1;
	}

	TableElements** matrix = createMatrix(grid_length, grid_width);
	Position player1 = {0,0};
	Position player2 = { grid_length - 1, grid_width - 1 };
	
	fillWithRandomValues(matrix, grid_length, grid_width);
	configureSpecialCells(matrix, grid_length, grid_width);
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 6);
	std::cout << 6 << player1.x << player2.x << std::endl;
	std::cin.get();

	printMatrix(matrix, grid_length, grid_width);
	movePlayer (matrix, grid_length, grid_width, player1);
	movePlayer(matrix, grid_length, grid_width, player2);


	deleteMatrix(matrix, grid_length);

	return 0;
}
