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

void try1(vector<vector<string> > &test) {
  test[0][0] = "3";
}

void try2(vector<vector<string> > test) {
  test[0][0] = "4";
}

int main() {
    movesetInit();
    vector<vector<string> > test;
    test.push_back(vector<string>());
    test.push_back(vector<string>());
    test[0].push_back("0");
    test[1].push_back("1");
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < test[i].size(); j++) {
        cout << test[i][j] << endl;
      }
    }
    try1(test);
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < test[i].size(); j++) {
        cout << test[i][j] << endl;
      }
    }
    try2(test);
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < test[i].size(); j++) {
        cout << test[i][j] << endl;
      }
    }
}