#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

enum Side {
  WHITE = -1,
  BLACK = 1
};

Side side;
int board[8][8];
vector<pair<int, int> > moveset[7];

bool weHaveMovedKing = false;
bool theyHaveMovedKing = false;
bool hasPawnMoved[2][8];


void boardInit() {
  // board[number][letter] (normal 2D array expression)
  // board[x][y] & 1 = white (0) or black (1)
  // board[x][y] >> 1 for piece type
  // 0 = empty
  // 2 = pawn
  // 4 = rook
  // 6 = knight
  // 8 = bishop
  // 10 = queen
  // 12 = king

  // empty
  for (int i = 2; i < 6; i++) {
    for (int j = 0; j < 8; j++) {
      board[i][j] = 0;
    }
  }

  // pawn
  for (int i = 0; i < 8; i++) {
    board[1][i] = 2;
    board[6][i] = 2;
  }

  // rook
  board[0][0] = 4;
  board[0][7] = 4;
  board[7][0] = 4;
  board[7][7] = 4;

  // knight
  board[0][1] = 6;
  board[0][6] = 6;
  board[7][1] = 6;
  board[7][6] = 6;

  // bishop
  board[0][2] = 8;
  board[0][5] = 8;
  board[7][2] = 8;
  board[7][5] = 8;

  // queen
  board[0][3] = 10;
  board[7][3] = 10;

  // king
  board[0][4] = 12;
  board[7][4] = 12;

  // black/white bit
  for(int i = 0; i < 2; i++) {
    for(int j = 0; j < 8; j++) {
      board[i][j]++;
    }
  }
}

void movesetInit(Side moveSide) {
  // pawn
  moveset[1].push_back(make_pair(moveSide, 0));

  // rook
  for(int i = 0; i < 8; i++) {
    moveset[2].push_back(make_pair(i, 0));
    moveset[2].push_back(make_pair(-1 * i, 0));
    moveset[2].push_back(make_pair(0, i));
    moveset[2].push_back(make_pair(0, -1 * i));
  }
  
  // knight
  moveset[3].push_back(make_pair(1, 2));
  moveset[3].push_back(make_pair(2, 1));
  moveset[3].push_back(make_pair(1, -2));
  moveset[3].push_back(make_pair(2, -1));
  moveset[3].push_back(make_pair(-1, 2));
  moveset[3].push_back(make_pair(-2, 1));
  moveset[3].push_back(make_pair(-1, -2));
  moveset[3].push_back(make_pair(-2, -1));

  // bishop
  for(int i = 0; i < 8; i++) {
    moveset[4].push_back(make_pair(i, i));
    moveset[4].push_back(make_pair(i, -1 * i));
    moveset[4].push_back(make_pair(-1 * i, i));
    moveset[4].push_back(make_pair(-1 * i, -1 * i));
  }

  // queen
  for(int i = 0; i < 8; i++) {
    moveset[5].push_back(make_pair(i, 0));
    moveset[5].push_back(make_pair(-1 * i, 0));
    moveset[5].push_back(make_pair(0, i));
    moveset[5].push_back(make_pair(0, -1 * i));
    moveset[5].push_back(make_pair(i, i));
    moveset[5].push_back(make_pair(i, -1 * i));
    moveset[5].push_back(make_pair(-1 * i, i));
    moveset[5].push_back(make_pair(-1 * i, -1 * i));
  }

  // king
  moveset[6].push_back(make_pair(1, 0));
  moveset[6].push_back(make_pair(0, 1));
  moveset[6].push_back(make_pair(-1, 0));
  moveset[6].push_back(make_pair(0, -1));
  moveset[6].push_back(make_pair(1, 1));
  moveset[6].push_back(make_pair(1, -1));
  moveset[6].push_back(make_pair(-1, 1));
  moveset[6].push_back(make_pair(-1, -1));
}

bool validIndex(int i, int j) {
  return (i >= 0) && (i < 8) && (j >= 0) && (j < 8);
}

int checkLegalMove(string move, Side moveSide) {
  // -1 is illegal, 1 is normal, 2 is en passant, 3 is castle, 4 is promotion
  if (move.length() < 4 || move.length() > 5) return -1;
  int i0 = move.at(1) - '0';
  int j0 = move.at(0) - 'a';
  int i1 = move.at(3) - '0';
  int j1 = move.at(2) - 'a';
  if (i0 > 7 || i0 < 0 || j0 > 7 || j0 < 0 || i1 > 7 || i1 < 0 || j1 > 7 || j0 < 0) return -1;
  int piece = board[i0][j0];
  Side pieceSide = (piece & 1) == 1 ? BLACK : WHITE;
  int pieceType = piece >> 1;
  if (moveSide != pieceSide) return -1;
  
  
}

void makeMove(string move) {
  int moveType = checkLegalMove(move, side);
  // -1 is illegal, 1 is normal, 2 is en passant, 3 is castle, 4 is promotion
  if (moveType == -1) {
    // SnitchBot3000
    cout << "WARNING: ENEMY HAS PLAYED INVALID MOVE! INVALID MOVE! INVALID MOVE! WE WIN" << endl;
    cout << "WHAT THE FREAK (GUANG) THEIR BOT IS BROKEN WE WIN!!!" << endl;
    exit(1);
  }
  
  if (moveType == 4) {
    
  } else {
    int i0 = move.at(1) - '0';
    int j0 = move.at(0) - 'a';
    int i1 = move.at(3) - '0';
    int j1 = move.at(2) - 'a';
    board[i1][j1] = board[i0][j0];
    board[i0][j0] = 0;
    if (moveType == 2) {
      board[i0][j1] = 0;
    }
    if (moveType == 3) {
      board[i0][j0] = 4;
    }
  }
}

string parseMove(string s) {
  return "freegga";
}

int main(int argc, char *argv[]) {
  boardInit();
  if (argc < 2) {
    cout << "What the freak (guang)!";
    return 0;
  }

  if (string(argv[1]) == "white") {
    side = WHITE;
    cout << "e2e4";
  } else {
    side = BLACK;
  }

  movesetInit(side);

  string s;
  while (true)
{
    getline(cin, s);
    string move = parseMove(s);
    cout << move;
  }
  return 0;
}