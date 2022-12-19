#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

vector<pair<int, int> > moveset[7];

void movesetInit() {
  // pawn
  moveset[1].push_back(make_pair(1, 0));

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

int main() {
    movesetInit();
    for(auto i : moveset[6]) {
        cout << i.first << i.second;
    }
}