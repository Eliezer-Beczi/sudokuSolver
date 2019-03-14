#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// Data structures
struct square {
  int numbers[10];
};

struct cell {
  int row;
  int col;
  int square;
  int numOfPossibilities;
  vector<int> numbers;
};

// Global variables
const auto compare = [](const cell& a, const cell& b) {
  return a.numOfPossibilities < b.numOfPossibilities;
};

bool rows[9][10];
bool cols[9][10];

square squares[9];
vector<cell> cells;

int board[9][9];
int solution[9][9];

int freeSpaces;
int numOfSolutions;

// Functions
void init() {
  for (int i = 0; i < 9; ++i) {
    for (int j = 1; j < 10; ++j) {
      rows[i][j] = 1;
      cols[i][j] = 1;
      squares[i].numbers[j] = 1;
    }
  }

  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      board[i][j] = 0;
      solution[i][j] = 0;
    }
  }

  cells.clear();
  freeSpaces = 81;
  numOfSolutions = 0;
}

void buildBoard(const string& line) {
  freeSpaces = 0;

  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      char c = line[i * 9 + j];

      if (c == '.') {
        board[i][j] = 0;
        ++freeSpaces;
      } else {
        board[i][j] = c - '0';
      }
    }
  }
}

void print(int board[9][9], ofstream& outputFile) {
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      outputFile << board[i][j];
    }
  }

  outputFile << endl;
}

void sendHelp() {
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      if (board[i][j] == 0) {
        continue;
      }

      int elem = board[i][j];

      if (rows[i][elem] || cols[j][elem]) {
        rows[i][elem] = 0;
        cols[j][elem] = 0;
      }
    }
  }

  int rowStart = 0;
  int colStart = 0;

  int rowEnd = 3;
  int colEnd = 3;

  for (int i = 0; i < 9; ++i) {
    if (i != 0 && i % 3 == 0) {
      rowStart += 3;
      rowEnd += 3;

      colStart = 0;
      colEnd = 3;
    }

    for (int row = rowStart; row < rowEnd; ++row) {
      for (int col = colStart; col < colEnd; ++col) {
        squares[i].numbers[board[row][col]] = 0;
      }
    }

    colStart += 3;
    colEnd += 3;
  }
}

void cachePosOfFreeSpaces() {
  int rowStart = 0;
  int colStart = 0;

  int rowEnd = 3;
  int colEnd = 3;

  for (int i = 0; i < 9; ++i) {
    if (i != 0 && i % 3 == 0) {
      rowStart += 3;
      rowEnd += 3;

      colStart = 0;
      colEnd = 3;
    }

    for (int row = rowStart; row < rowEnd; ++row) {
      for (int col = colStart; col < colEnd; ++col) {
        if (board[row][col] != 0) {
          continue;
        }

        cells.push_back(cell());
        cells.back().row = row;
        cells.back().col = col;
        cells.back().square = i;

        int possibilities = 0;

        for (int num = 1; num < 10; ++num) {
          if (rows[row][num] && cols[col][num] && squares[i].numbers[num]) {
            ++possibilities;
            cells.back().numbers.push_back(num);
          }
        }

        cells.back().numOfPossibilities = possibilities;
      }
    }

    colStart += 3;
    colEnd += 3;
  }
}

void update(const cell& c, const int& num) {
  rows[c.row][num] = 0;
  cols[c.col][num] = 0;
  squares[c.square].numbers[num] = 0;

  board[c.row][c.col] = num;
  --freeSpaces;

  for (int i = 0; i < cells.size(); ++i) {
    if (cells[i].row == c.row || cells[i].col == c.col ||
        cells[i].square == c.square) {
      vector<int>::iterator pos =
          find(cells[i].numbers.begin(), cells[i].numbers.end(), num);

      if (pos == cells[i].numbers.end()) {
        continue;
      }

      --cells[i].numOfPossibilities;
      cells[i].numbers.erase(pos);
    }
  }
}

inline bool accept(const cell& c, const int& num) {
  if (rows[c.row][num] && cols[c.col][num] && squares[c.square].numbers[num]) {
    return true;
  }

  return false;
}

void downgrade(const cell& c, const int& num) {
  rows[c.row][num] = 1;
  cols[c.col][num] = 1;
  squares[c.square].numbers[num] = 1;

  board[c.row][c.col] = 0;
  ++freeSpaces;

  for (auto& elem : cells) {
    if ((elem.row == c.row || elem.col == c.col || elem.square == c.square) &&
        accept(elem, num)) {
      ++elem.numOfPossibilities;
      elem.numbers.push_back(num);
    }
  }
}

void copy(int src[9][9], int dest[9][9]) {
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      dest[i][j] = src[i][j];
    }
  }
}

bool backtrack() {
  if (freeSpaces == 0) {
    copy(board, solution);
    ++numOfSolutions;

    if (numOfSolutions == 1) {
      return true;
    }

    return false;
  }

  auto it = min_element(cells.begin(), cells.end(), compare);

  cell c = (*it);
  cells.erase(it);

  for (const int& num : c.numbers) {
    update(c, num);

    if (backtrack()) {
      return true;
    }

    downgrade(c, num);
  }

  cells.push_back(c);

  return false;
}

int main(int argc, char const* argv[]) {
  string line;
  ifstream inputFile;
  ofstream outputFile;

  inputFile.open("top95.txt");
  outputFile.open("output.txt");

  while (inputFile >> line) {
    init();
    buildBoard(line);
    sendHelp();
    cachePosOfFreeSpaces();
    backtrack();
    print(solution, outputFile);
  }

  outputFile.close();
  inputFile.close();

  return 0;
}
