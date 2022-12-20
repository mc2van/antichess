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
vector<string> moves[2];

// castling booleans
bool weHaveMovedKing = false;
bool weHaveMovedaRook = false; // no rooks moved
bool weHaveMovedhRook = false; // 7 rooks moved (there are only 4?)

// en passant check
string prevMove;


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

bool anyBlockers(int i0, int j0, int i1, int j1) {
  int piece = board[i0][j0];
  Side pieceSide = (piece & 1) == 1 ? BLACK : WHITE;
  int pieceType = piece >> 1;
  int goTo = board[i1][j1];
  if (pieceType == 3) {
    return 1;
  } else {
    if (i0 == i1) {
      // vertical
      for (int k = min(j0, j1) + 1; k < max(j0, j1); k++) {
        if (board[i0][k] != 0) return true;
      }
    } else if (j0 == j1) {
      // horizontal
      for (int k = min(i0, i1) + 1; k < max(i0, i1); k++) {
        if (board[k][j0] != 0) return true;
      }
    } else {
      // diagonal
      if ((i0 < i1 && j0 < j1) || (i1 < i0 && j1 < j0)) {
        // (/) diagonal like that
        for (int k = min(i0, i1) + 1; k < max(i0, i1); k++) {
          for (int l = min(j0, j1) + 1; l < max(j0, j1); l++) {
            if (k - i0 == l - j0 && board[k][l] != 0) return true;
          }
        }
      } else {
        // (\) diagonal like that
        // this might be wrong
        for (int k = i0 - 1; k > i1; k--) {
          for (int l = j0 + 1; l < j1; l++) {
            if (i0 - k == l - j0 && board[k][l] != 0) return true;
          }
        }
      }
    }
  }
  return false;
}

bool isKingInCheck(int x, int y, Side kingSide) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      Side pieceSide = (board[i][j] & 1) == 1 ? BLACK : WHITE;
      int pieceType = board[i][j] >> 1;
      if (pieceSide != kingSide) {
        for (int k = 0; k < moveset[pieceType].size(); k++) {
          pair<int, int> mv = moveset[pieceType][k];
          if (i + mv.first == x && j + mv.second == y) {
            if (!anyBlockers(i, j, x, y)) return true;
          }
        }
      }
    }
  }
  return false;
}

bool isKingInCheck(Side kingSide) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      Side pieceSide = (board[i][j] & 1) == 1 ? BLACK : WHITE;
      int pieceType = board[i][j] >> 1;
      if (pieceType == 6 && pieceSide == kingSide) {
        return isKingInCheck(i, j, kingSide);
      }
    }
  }
}

bool validIndex(int i, int j) {
  return (i >= 0) && (i < 8) && (j >= 0) && (j < 8);
}

string stringifyCoord(int x, int y) {
  char first = y + 97;
  string first2(1, first);
  string second = to_string(8 - x);
  return first2 + second;
}

int destringifyCoord(string move, int ind) {
  switch (ind) {
    case 1:
      return 8 - (move.at(1) - '0');
      break;
    case 2:
      return move.at(0) - 'a';
      break;
    case 3:
      return 8 - (move.at(3) - '0');
      break;
    case 4:
      return move.at(2) - 'a';
      break;
  }
}

int checkLegalMove(string move) {
  // -1 is illegal, 1 is normal, 2 is en passant, 3 is castle, 4 is promotion
  // illegal move length
  if (move.length() < 4 || move.length() > 5) {
    return -1;
  }
  int i0 = destringifyCoord(move, 1);
  int j0 = destringifyCoord(move, 2);
  int i1 = destringifyCoord(move, 3);
  int j1 = destringifyCoord(move, 4);
  //0-7 index checking
  if (!validIndex(i0, j0) || !validIndex(i1, j1)) {
    cout << "here1";
    return -1;
  }
  int piece = board[i0][j0];
  Side pieceSide = (piece & 1) == 1 ? BLACK : WHITE;
  int pieceType = piece >> 1;
  int goTo = board[i1][j1];
  // tryna take same side piece
  if (goTo != 0 && ((WHITE == side) ? (goTo & 1) == 0 : (goTo & 1) == 1)) {
    cout << "here2";
    return -1;
  }
  // moving wrong side
  if (side != pieceSide) {
    cout << "here3";
    return -1;
  }
  // checks for promotion
  if (move.length() == 5) {
    if ((move.at(4) != 'q' && move.at(4) != 'r' && move.at(4) != 'b' && move.at(4) != 'n')
    || (i0 != i1) || (side == WHITE && (j0 != 1 || j1 != 0)) 
    || (side == BLACK && (j0 != 6 || j1 != 7))) {
      cout << "here4";
      return -1;
    }
    return 4;
  }
  // pawn moves
  if (pieceType == 1) {
    cout << "here5";
    if (j1 == j0) {
      cout << "here6";
      // straight moving pawn
      if (side * (i1 - i0) > 2 || side * (i1 - i0) <= 0) {
        cout << "here7";
        return -1;
      }
      // if things are in the way
      if (board[i1][j1] != 0) {
        cout << "here8";
        return -1;
      }
      if (board[i1][j0 + side] != 0) {
        cout << "here9";
        return -1;
      }
      return 1;
    } else {
      // diag capture
      if (side * (i1 - i0) != 1) {
        return -1;
      }
      if (abs(j1 - j0) > 1) {
        return -1;
      }
      // en passant
      if (board[i1][j1] == 0) {
        if (board[i0][j1] >> 1 == 1 && i0 == 4 + ((side - 1) / 2)) {
          if (prevMove == stringifyCoord((side == WHITE) ? 1 : 6, j1) 
          + stringifyCoord((side == WHITE) ? 3 : 4, j1)) {
            return 2;
          }
        }
        return -1;
      }
      return 1;
    }
  }
  // normal moves
  for (int i = 0; i < moveset[pieceType].size(); i++) {
    pair<int, int> mv = moveset[pieceType][i];
    if (i0 + mv.first == i1 && j0 + mv.second == j1 && !anyBlockers(i0, j0, i1, j1)) {
      if (pieceType == 6) {
        weHaveMovedKing = true;
      }
      if (pieceType == 2) {
        if (i0 == (7 / (((side + 1) * 100000) + 1))) {
          if (j0 == 0) {
            weHaveMovedaRook = true;
          }
          if (j0 == 7) {
            weHaveMovedhRook = true;
          }
        }
      }
      return 1;
    }
  }
  // castles
  if ((move == "e1g1" || move == "e1c1" || move == "e8g8" || move == "e8c8") && pieceType == 6) {
    if (anyBlockers(i0, j0, i1, j1) || weHaveMovedKing || (j1 == 7 && weHaveMovedhRook) || (j1 == 0 && weHaveMovedaRook)) {
      return -1;
    }
    weHaveMovedKing = true;
    return 3;
  }
  return -1;
}

void getLegalMoves() {
  for(int i = 0; i < 2; i++) {
    moves[i].clear();
  }

  int compareSide = side;

  if (compareSide == -1) {
    compareSide++;
  }

  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      if (board[i][j] && ((board[i][j] & 1) == compareSide)) {
        // our piece
        int curPiece = board[i][j] >> 1;
        for (auto m : moveset[curPiece]) {
          int newX = i + m.first;
          int newY = j + m.second;
          string moveString = stringifyCoord(i, j) + stringifyCoord(newX, newY);    
          if (validIndex(newX, newY) && checkLegalMove(moveString)) {
            if (board[newX][newY]) {
              //take move              
              moves[1].push_back(moveString);
            } else {
              // just move
              moves[0].push_back(moveString);
            }
          }
        }
      }
    }
  }
}

string chooseMove() {
  if (moves[1].size()) {
    // take move
    return moves[1][0];  
  } else {
    // other move
    if (moves[0].size()) {
      return moves[0][0];
    } else {
      return "NO MOVE POSSIBLE";
    }
  }
}

void makeMove(string move) {
  /*
  Castling
  En passant
  Pawn advance by two spaces
  Draw by repetition
  50-move rule
  */


  int moveType = checkLegalMove(move);
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
    makeMove("e2e4");
  } else {
    side = BLACK;
  }

  movesetInit(side);

  string opp;
  string us;
  while(true) {
    cin >> opp;
    prevMove = opp;
    makeMove(opp);
    getLegalMoves();
    us = chooseMove();
    prevMove = us;
    makeMove(us);
    cout << us;
  }

  /*

  string s;
  while (true)
  {
    getline(cin, s);
    prevMove = s;
    string move = parseMove(s);
    cout << move;
  }
  return 0;

  */ 
}