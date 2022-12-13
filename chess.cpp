#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

string parseMove(string s)
{
  return "freegga";
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    cout << "What the freak (guang)!";
    return 0;
  }
  if (string(argv[1]) == "white")
  {
    cout << "e2e4";
  }

  string s;
  while (true)
  {
    getline(cin, s);
    string move = parseMove(s);
    cout << move;
  }
  return 0;
}