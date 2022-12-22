#include <fstream>
#include <iostream>
#include <memory>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <cstdlib>

using namespace std;

enum Side {
  WHITE = -1,
  BLACK = 1
};

Side sideG;
Side oppside;
int boardG[8][8];
vector<pair<int, int> > moveset[7];
vector<pair<int, int> > oppMoveset[7];
int numOfMoves;

// castling booleans
bool weHaveMovedKing = false;
bool weHaveMovedaRook = false; // no rooks moved
bool weHaveMovedhRook = false; // 7 rooks moved (there are only 4?)

bool theyHaveMovedKing = false;
bool theyHaveMovedaRook = false;
bool theyHaveMovedhRook = false;

// en passant check
string prevMove;

void makeMove(string move, Side side, int board[8][8]);

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
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      boardG[i][j] = 0;
    }
  }

  // pawn
  for (int i = 0; i < 8; i++) {
    boardG[1][i] = 2;
    boardG[6][i] = 2;
  }

  //rook
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
      if (boardG[i][j]) {
        boardG[i][j]++;
      }
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

void movesetInit(Side moveSide, vector<pair<int, int> > (&moveset)[7]) {
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
  // castling
  moveset[6].push_back(make_pair(0, 2));
  moveset[6].push_back(make_pair(0, -2));
}

bool validIndex(int i, int j);

bool anyBlockers(int board[8][8], int i0, int j0, int i1, int j1) {
  int piece = board[i0][j0];
  Side pieceSide = (piece & 1) == 1 ? BLACK : WHITE;
  int pieceType = piece >> 1;
  int goTo = board[i1][j1];
  if (pieceType == 3) {
    return false;
  } else {
    if ((pieceType == 2 || pieceType == 5 || pieceType == 1) && (j0 != j1)) {
      for (int k = min(j0, j1) + 1; k < max(j0, j1); k++) {
        if (board[i0][k] != 0) return true;
      }
    }
    if ((pieceType == 2 || pieceType == 5) && (i0 != i1)) {
      for (int k = min(i0, i1) + 1; k < max(i0, i1); k++) {
        if (board[k][j0] != 0) return true;
      }
    } 
    if ((pieceType == 4 || pieceType == 1 || pieceType == 6) && (j0 != j1) && (i0 != i1)) {
      int dir[] = {abs(i0 - i1) / (i0 - i1), abs(j0 - j1) / (j0 - j1)};
      for (int i = 1; i < abs(i0 - i1); i++) {
        if (board[i0 - (dir[0] * i)][j0 - (dir[1] * i)] != 0) return true;
      }
      // for (int i = 0; i < 4; i++) {
      //   int xOffset = dir[i];
      //   int yOffset = dir[i + 1];
      //   bool hitAThing = false;
      //   for (int i = 1; i < 9; i++) {
      //     if (!validIndex(i0 + (i * xOffset), j0 + (i * yOffset))) {
      //       break;
      //     }
      //     if (i0 + (i * xOffset) == i1 && j0 + (i * yOffset) == j1) {
      //       return hitAThing;
      //     }
      //     if (board[i0 + (i * xOffset)][j0 + (i * yOffset)] != 0) {
      //       hitAThing = true;
      //     }
      //   }
      // }
    }
  }
  return false;
}

bool isKingInCheck(int board[8][8], int x, int y, Side kingSide) {
  pair<int, int> kingLoc;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      Side pieceSide = (board[i][j] & 1) == 1 ? BLACK : WHITE;
      int pieceType = board[i][j] >> 1;
      if (pieceType == 6 && pieceSide == kingSide) {
        kingLoc.first = i;
        kingLoc.second = j;
      }
    }
  }
  board[kingLoc.first][kingLoc.second] = 0;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      Side pieceSide = (board[i][j] & 1) == 1 ? BLACK : WHITE;
      int pieceType = board[i][j] >> 1;
      if (pieceSide != kingSide) {
        for (int k = 0; k < moveset[pieceType].size(); k++) {
          pair<int, int> mv = moveset[pieceType][k];
          if (i + mv.first == x && j + mv.second == y) {
            if (!anyBlockers(board, i, j, x, y)) {
              board[kingLoc.first][kingLoc.second] = 12 + ((kingSide == BLACK) ? 1 : 0);
              return true;
            }
          }
        }
      }
    }
  }
  board[kingLoc.first][kingLoc.second] = 12 + ((kingSide == BLACK) ? 1 : 0);
  return false;
}

bool isKingInCheck(int board[8][8], Side kingSide) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      Side pieceSide = (board[i][j] & 1) == 1 ? BLACK : WHITE;
      int pieceType = board[i][j] >> 1;
      if (pieceType == 6 && pieceSide == kingSide) {
        return isKingInCheck(board, i, j, kingSide);
      }
    }
  }
  return false;
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
  return -1;
}

void guangDebug(int i) {
  //JUMP
  //cout << "here" << i;
}

bool checkValidCastle(string move, Side curSide, int board[8][8]) {
  if (move == "e1g1") {
    return !(isKingInCheck(board, 7, 4, curSide) || isKingInCheck(board, 7, 5, curSide) || isKingInCheck(board, 7, 6, curSide));
  } else if (move == "e1c1") {
    return !(isKingInCheck(board, 7, 4, curSide) || isKingInCheck(board, 7, 3, curSide) || isKingInCheck(board, 7, 2, curSide));
  } else if (move == "e8g8") {
    return !(isKingInCheck(board, 0, 4, curSide) || isKingInCheck(board, 0, 5, curSide) || isKingInCheck(board, 0, 6, curSide));
  } else { // e8c8
    return !(isKingInCheck(board, 0, 4, curSide) || isKingInCheck(board, 0, 3, curSide) || isKingInCheck(board, 0, 2, curSide));
  }
}

int checkLegalMove(string move, Side curSide, int board[8][8]) {
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
    || (j0 != j1) || (curSide == WHITE && (i0 != 1 || i1 != 0)) 
    || (curSide == BLACK && (i0 != 6 || i1 != 7))) {
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
      if (curSide * (i1 - i0) > 2 || curSide * (i1 - i0) <= 0) {
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
  // castles
  if ((move == "e1g1" || move == "e1c1" || move == "e8g8" || move == "e8c8") && pieceType == 6) {
    // DO KING CHECK CHECKS HERE
    if (checkValidCastle(move, curSide, board)) {
      int rLocation = (j1 == 6) ? 7 : 0;
      if (board[i1][rLocation] >> 1 != 2) {
        return -1;
      }
      if (curSide == sideG) {
        if (anyBlockers(board, i0, j0, i1, rLocation) || weHaveMovedKing || (rLocation == 7 && weHaveMovedhRook) || (rLocation == 0 && weHaveMovedaRook)) {
          guangDebug(13);
          return -1;
        }
        return 3;
      } else {
        if (anyBlockers(board, i0, j0, i1, rLocation) || theyHaveMovedKing || (rLocation == 7 && theyHaveMovedhRook) || (rLocation == 0 && theyHaveMovedaRook)) {
          guangDebug(113);
          return -1;
        }
        return 3;
      }
    } else {
      return -1;
    }
  }
  // normal moves
  for (int i = 0; i < moveset[pieceType].size(); i++) {
    pair<int, int> mv = moveset[pieceType][i];
    if (i0 + mv.first == i1 && j0 + mv.second == j1 && !anyBlockers(board, i0, j0, i1, j1)) {
      guangDebug(15);
      return 1;
    }
  }
  guangDebug(14);
  return -1;
}

vector<string> getLegalMoves(int board[8][8], Side side) {
  vector<string> moves[2];
  int compareSide = side;

  if (compareSide == -1) {
    compareSide = 0;
  }

  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      if (board[i][j] && ((board[i][j] & 1) == compareSide)) {
        // our piece
        int curPiece = board[i][j] >> 1;
        for (auto m : moveset[curPiece]) {
          int newX = i + m.first;
          int newY = j + m.second;
          if (validIndex(newX, newY)) {
            string moveString = stringifyCoord(i, j) + stringifyCoord(newX, newY);
            int moveType = checkLegalMove(moveString, side, board);    
            if (moveType != -1) {
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
  return (moves[1].size() > 0) ? moves[1] : moves[0];
}


int gigaEval(int board[8][8]) {
  map<int, int> pieceValues;
  pieceValues.insert(make_pair(1, 1));
  pieceValues.insert(make_pair(2, 4));
  pieceValues.insert(make_pair(3, 4));
  pieceValues.insert(make_pair(4, 3));
  pieceValues.insert(make_pair(5, 6));
  pieceValues.insert(make_pair(6, 10000));
  int adjSide = (sideG == WHITE) ? 0 : 1;
  int kingSafety = isKingInCheck(board, sideG) ? -11 : 5;
  int freedom = getLegalMoves(board, sideG).size() * 7;
  if (freedom == 0) {
    freedom = -10000;
  }
  int piecesDiff = 0;
  int numPieces = 0;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      numPieces++;
      if (board[i][j] != 0) {
        if ((board[i][j] & 1) == adjSide) {
          piecesDiff += pieceValues[board[i][j] >> 1];
        } else {
          piecesDiff -= pieceValues[board[i][j] >> 1];
        }
      }
    }
  }
  piecesDiff += 16 * ((32 - numPieces) / 32);
  return kingSafety + freedom + piecesDiff;
}

int sillybilly(int board[8][8], bool usPlaying, int a, int b, int depth) {
  if (depth == 0) {
    return gigaEval(board);
  }
  Side curSide = usPlaying ? sideG : oppside;
  vector<string> possibleMoves = getLegalMoves(board, curSide);
  if (usPlaying) {
    int highest = INT_MIN;
    for (string move : possibleMoves) {
      int newBoard[8][8];
      for (int j = 0; j < 8; j++) {
        for (int k = 0; k < 8; k++) {
          newBoard[j][k] = board[j][k];
        }
      }
      makeMove(move, curSide, newBoard);
      int ret = sillybilly(board, false, a, b, depth - 1);
      highest = max(highest, ret);
      a = max(a, ret);
      if (b <= a) {
        break;
      }
    }
    return highest;
  } else {
    int lowest = INT_MAX;
    for (string move : possibleMoves) {
      int newBoard[8][8];
      for (int j = 0; j < 8; j++) {
        for (int k = 0; k < 8; k++) {
          newBoard[j][k] = board[j][k];
        }
      }
      makeMove(move, curSide, newBoard);
      int ret = sillybilly(board, true, a, b, depth - 1);
      lowest = min(lowest, ret);
      b = min(b, ret);
      if (b <= a) {
        break;
      }
    }
    return lowest;
  }
}

string gigaGuang(int board[8][8]) {
  int maxVal = INT_MIN;
  string res;
  vector<string> possibleMoves = getLegalMoves(board, sideG);
  for (string move : possibleMoves) {
    int newBoard[8][8];
    for (int j = 0; j < 8; j++) {
      for (int k = 0; k < 8; k++) {
        newBoard[j][k] = board[j][k];
      }
    }
    makeMove(move, sideG, newBoard);
    int ret = sillybilly(board, false, INT_MIN, INT_MAX, 5);
    if (maxVal <= ret) {
      res = move;
      maxVal = ret;
    }
  }
  return res;
}

string chooseMove(vector<string> moves, int board[8][8]) {
  string moveMade;
  if (moves.size()) {
    string moveMade = gigaGuang(board);
    return moveMade;
  } else {
    return "gg";
  }
}

void makeMove(string move, Side side, int board[8][8]) {
  /*
  Castling
  En passant
  Pawn advance by two spaces
  */

  int moveType = checkLegalMove(move, side, board);
  if (side == WHITE && board == boardG) {
  } else if (board == boardG) {
  }
  // -1 is illegal, 1 is normal, 2 is en passant, 3 is castle, 4 is promotion
  if (moveType == -1) {
    // SnitchBot3000
    exit(1);
  }
  
  if (moveType == 4) {
    int i0 = destringifyCoord(move, 1);
    int j0 = destringifyCoord(move, 2);
    int i1 = destringifyCoord(move, 3);
    int j1 = destringifyCoord(move, 4);

    if (i0 == (side == WHITE ? 7 : 0) && board == boardG) {
      if (j0 == 0) {
        weHaveMovedaRook = true;
      }
      if (j0 == 7) {
        weHaveMovedhRook = true;
      }
    }

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

    if (i0 == (side == WHITE ? 7 : 0) && board == boardG) {
      if (j0 == 0) {
        if (side == sideG) {
          weHaveMovedaRook = true;
        } else {
          theyHaveMovedaRook = true;
        }
      }
      if (j0 == 7) {
        if (side == sideG) {
          weHaveMovedhRook = true;
        } else {
          theyHaveMovedhRook = true;
        }
      }
    }

    if (board[destringifyCoord(move, 1)][destringifyCoord(move, 2)] >> 1 == 6
    && board == boardG) {
      if (side == sideG) {
        weHaveMovedKing = true;
      } else {
        theyHaveMovedKing = true;
      }
    }

    board[i1][j1] = board[i0][j0];
    board[i0][j0] = 0;
    if (moveType == 2) {
      board[i0][j1] = 0;
    }
    if (moveType == 3) {
      board[i0][j1 - ((j1 - j0) / abs(j1 - j0))] = 4;
      if (side == BLACK) {
        board[i0][j1 - ((j1 - j0) / abs(j1 - j0))]++;
      }
      if (j1 == 2) {
        board[i0][0] = 0;
      } else {
        board[i0][7] = 0;
      }
    }
  }

  return;
}

int main(int argc, char *argv[]) {
  srand(time(0));
  boardInit();
  if (argc < 2) {
    return 0;
  }

  if (string(argv[1]) == "white") {
    sideG = WHITE;
    oppside = BLACK;
    movesetInit(sideG, moveset);
    movesetInit(oppside, oppMoveset);
    string initMove = "c2c3";
    cout << initMove << endl;
    makeMove(initMove, sideG, boardG);
  } else {
    sideG = BLACK;
    oppside = WHITE;
    movesetInit(sideG, moveset);
    movesetInit(oppside, oppMoveset);
  }

  string opp;
  string us;
  while(true) {
    cin >> opp;
    prevMove = opp;
    makeMove(opp, oppside, boardG);
    vector<string> possibleMoves = getLegalMoves(boardG, sideG);
    us = chooseMove(possibleMoves, boardG);
    cout << us << endl;
    makeMove(us, sideG, boardG);
    numOfMoves += 2;
  }
}