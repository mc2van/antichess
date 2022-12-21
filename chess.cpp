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
Side oppside;
int boardG[8][8];
vector<pair<int, int> > moveset[7];
vector<string> movesG[2];

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
      boardG[i][j] = 0;
    }
  }

  // pawn
  for (int i = 0; i < 8; i++) {
    boardG[1][i] = 2;
    boardG[6][i] = 2;
  }

  // rook
  boardG[0][0] = 4;
  boardG[0][7] = 4;
  boardG[7][0] = 4;
  boardG[7][7] = 4;

  // knight
  boardG[0][1] = 6;
  boardG[0][6] = 6;
  boardG[7][1] = 6;
  boardG[7][6] = 6;

  // bishop
  boardG[0][2] = 8;
  boardG[0][5] = 8;
  boardG[7][2] = 8;
  boardG[7][5] = 8;

  // queen
  boardG[0][3] = 10;
  boardG[7][3] = 10;

  // king
  boardG[0][4] = 12;
  boardG[7][4] = 12;

  // black/white bit
  for(int i = 0; i < 2; i++) {
    for(int j = 0; j < 8; j++) {
      boardG[i][j]++;
    }
  }
}

void printBoard() {
  for (int i = 0 ; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      cout << boardG[i][j] << " ";
    }
    cout << endl;
  }
}

void movesetInit(Side moveSide) {
  // pawn
  moveset[1].push_back(make_pair(moveSide, 0));
  moveset[1].push_back(make_pair(moveSide, 1));
  moveset[1].push_back(make_pair(moveSide, -1));

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

bool anyBlockers(int (&board)[8][8], int i0, int j0, int i1, int j1) {
  int piece = board[i0][j0];
  Side pieceSide = (piece & 1) == 1 ? BLACK : WHITE;
  int pieceType = piece >> 1;
  int goTo = board[i1][j1];
  if (pieceType == 3) {
    return false;
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

bool isKingInCheck(int (&board)[8][8], int x, int y, Side kingSide) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      Side pieceSide = (board[i][j] & 1) == 1 ? BLACK : WHITE;
      int pieceType = board[i][j] >> 1;
      if (pieceSide != kingSide) {
        for (int k = 0; k < moveset[pieceType].size(); k++) {
          pair<int, int> mv = moveset[pieceType][k];
          if (i + mv.first == x && j + mv.second == y) {
            if (!anyBlockers(board, i, j, x, y)) return true;
          }
        }
      }
    }
  }
  return false;
}

bool isKingInCheck(int (&board)[8][8], Side kingSide) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      Side pieceSide = (board[i][j] & 1) == 1 ? BLACK : WHITE;
      int pieceType = board[i][j] >> 1;
      if (pieceType == 6 && pieceSide == kingSide) {
        return isKingInCheck(board, i, j, kingSide);
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

void guangDebug(int i) {
  //JUMP
  //cout << "here" << i;
}

int checkLegalMove(string move, Side curSide, int (&board)[8][8]) {
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
    guangDebug(1);
    return -1;
  }
  int piece = board[i0][j0];
  Side pieceSide = (piece & 1) == 1 ? BLACK : WHITE;
  int pieceType = piece >> 1;
  int goTo = board[i1][j1];
  // tryna take same side piece
  if (goTo != 0 && ((WHITE == curSide) ? (goTo & 1) == 0 : (goTo & 1) == 1)) {
    guangDebug(2);
    return -1;
  }
  // moving wrong side
  if (curSide != pieceSide) {
    guangDebug(3);
    return -1;
  }
  // checks for promotion
  if (move.length() == 5) {
    if ((move.at(4) != 'q' && move.at(4) != 'r' && move.at(4) != 'b' && move.at(4) != 'n')
    || (i0 != i1) || (curSide == WHITE && (j0 != 1 || j1 != 0)) 
    || (curSide == BLACK && (j0 != 6 || j1 != 7))) {
      guangDebug(4);
      return -1;
    }
    return 4;
  }
  // pawn moves
  if (pieceType == 1) {
    guangDebug(5);
    if (j1 == j0) {
      guangDebug(6);
      // straight moving pawn
      if (side * (i1 - i0) > 2 || curSide * (i1 - i0) <= 0) {
        guangDebug(7);
        return -1;
      }
      // if things are in the way
      if (board[i1][j1] != 0) {
        guangDebug(8);
        return -1;
      }
      if (board[i0 + curSide][j0] != 0) {
        guangDebug(9);
        return -1;
      }
      return 1;
    } else {
      // diag capture
      if (curSide * (i1 - i0) != 1) {
        guangDebug(10);
        return -1;
      }
      if (abs(j1 - j0) > 1) {
        guangDebug(11);
        return -1;
      }
      // en passant
      if (board[i1][j1] == 0) {
        if (board[i0][j1] >> 1 == 1 && i0 == 4 + ((curSide - 1) / 2)) {
          if (prevMove == stringifyCoord((curSide == WHITE) ? 1 : 6, j1) 
          + stringifyCoord((curSide == WHITE) ? 3 : 4, j1)) {
            return 2;
          }
        }
        guangDebug(12);
        return -1;
      }
      return 1;
    }
  }
  // normal moves
  for (int i = 0; i < moveset[pieceType].size(); i++) {
    pair<int, int> mv = moveset[pieceType][i];
    if (i0 + mv.first == i1 && j0 + mv.second == j1 && !anyBlockers(board, i0, j0, i1, j1)) {
      guangDebug(15);
      if (pieceType == 2) {
        if (i0 == (7 / (((curSide + 1) * 100000) + 1))) {
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
    if (curSide != side) return 3;
    int rLocation = (j1 == 6) ? 7 : 0;
    if (anyBlockers(board, i0, j0, i1, rLocation) || weHaveMovedKing || (rLocation == 7 && weHaveMovedhRook) || (rLocation == 0 && weHaveMovedaRook)) {
      guangDebug(13);
      cout << weHaveMovedKing << endl;
      return -1;
    }
    return 3;
  }
  guangDebug(14);
  return -1;
}

void getLegalMoves(vector<string> (&moves)[2], int (&board)[8][8]) {
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
          int moveType = checkLegalMove(moveString, side, board);    
          if (validIndex(newX, newY) && moveType != -1) {
            if (board[newX][newY] != 0 || moveType == 2) {
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

void makeMove(string move, Side side, int (&board)[8][8]);

string chooseMove(vector<string> (&moves)[2], int (&board)[8][8]) {
  if (isKingInCheck(board, side)) {
    for (int i = 0; i < moves[0].size(); i++) {
      int newBoard[8][8];
      for (int j = 0; j < 8; j++) {
        for (int k = 0; k < 8; k++) {
          newBoard[j][k] = board[j][k];
        }
      }
      // remember to i-- when removing
      makeMove(moves[0][i], side, newBoard);
      if (isKingInCheck(newBoard, side)) {
        moves[0].erase(moves[0].begin() + i--);
      }
    }
    for (int i = 0; i < moves[1].size(); i++) {
      int newBoard[8][8];
      for (int j = 0; j < 8; j++) {
        for (int k = 0; k < 8; k++) {
          newBoard[j][k] = board[j][k];
        }
      }
      // remember to i-- when removing
      makeMove(moves[1][i], side, newBoard);
      if (isKingInCheck(newBoard, side)) {
        moves[1].erase(moves[1].begin() + i--);
      }
    }
  }
  if (moves[1].size()) {
    // take move
    return moves[1][0];
    return moves[1][rand() % moves[1].size()];  
  } else {
    // other move
    if (moves[0].size()) {
      string moveMade = moves[0][rand() % moves[0].size()];
      if (board[destringifyCoord(moveMade, 1)][destringifyCoord(moveMade, 2)] >> 1 == 6) {
        weHaveMovedKing = true;
      }
      return moveMade;
    } else {
      return "NO MOVE POSSIBLE";
    }
  }
}

void makeMove(string move, Side side, int (&board)[8][8]) {
  /*
  Castling
  En passant
  Pawn advance by two spaces
  Draw by repetition
  50-move rule
  */


  int moveType = checkLegalMove(move, side, board);
  // -1 is illegal, 1 is normal, 2 is en passant, 3 is castle, 4 is promotion
  if (moveType == -1) {
    // SnitchBot3000
    cout << "WARNING: ENEMY HAS PLAYED INVALID MOVE! INVALID MOVE! INVALID MOVE! WE WIN" << endl;
    cout << "WHAT THE FREAK (GUANG) THEIR BOT IS BROKEN WE WIN!!!" << endl;
    exit(1);
  }
  
  if (moveType == 4) {
    int i0 = destringifyCoord(move, 1);
    int j0 = destringifyCoord(move, 2);
    int i1 = destringifyCoord(move, 3);
    int j1 = destringifyCoord(move, 4);
    board[i0][j0] = 0;
    switch (move.at(4)) {
      case 'q':
        board[i1][j1] = 10;
        break;
      case 'r':
        board[i1][j1] = 4;
        break;
      case 'b':
        board[i1][j1] = 8;
        break;
      case 'n':
        board[i1][j1] = 6;
        break;
    }
    if (side == BLACK) {
      board[i1][j1]++;
    }
  } else {
    int i0 = destringifyCoord(move, 1);
    int j0 = destringifyCoord(move, 2);
    int i1 = destringifyCoord(move, 3);
    int j1 = destringifyCoord(move, 4);
    board[i1][j1] = board[i0][j0];
    board[i0][j0] = 0;
    if (moveType == 2) {
      board[i0][j1] = 0;
    }
    if (moveType == 3) {
      board[i0][j0] = 4;
    }
  }

  return;
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
    oppside = BLACK;
    cout << "e2e4" << endl;
    makeMove("e2e4", side, boardG);
  } else {
    side = BLACK;
    oppside = WHITE;
  }

  movesetInit(side);

  string opp;
  string us;
  while(true) {
    cin >> opp;
    prevMove = opp;
    makeMove(opp, oppside, boardG);
    getLegalMoves(movesG, boardG);
    us = chooseMove(movesG, boardG);
    cout << "Our Bot Plays: " << us << endl;
    makeMove(us, side, boardG);
    printBoard();
    // cout << us;
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