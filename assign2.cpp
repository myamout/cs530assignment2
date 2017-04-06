#include "header.h"
#include "opcode.h"

void readObj(char *fname) {
  std::ifstream file(fname, std::ios::binary);
  /*
    Read the object code in this method...
    1. Read the header column
    2. Start reading through all of the Text columns
    3. Use the End record as a stop condition somehow
    4. Each Object read needs to be checked for extended format...
    5. Need to figure out the parse here. 

    The main structures are already set-up here so we just need to figure out how to parse and store each string
    in a List Node
  */
  char headerCol[200];
  long charCount;
  file.read(headerCol, 200);
  for (auto& n : headerCol) {
    std::cout << n;
  }

}

void readSym(char *fname) {
  /*
    Read the Symbol table into a global map object in this method...
  */
}

int main(int argc, char *argv[])
{
  readObj(argv[1]);

  return 0;
}