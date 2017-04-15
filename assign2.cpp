#include "header.h"
#include "opcode.h"

std::vector<std::string> symTab;
std::map<std::string, std::map<std::string, std::string>> symbolValues;
std::map<std::string, std::map<std::string, std::string>> literalValues;

int get_textColLen(char tens, char ones) {
  int onesPlace = hex_conversion[ones];
  int tensPlace = hex_conversion[tens] * 16;
  return (onesPlace + tensPlace) * 2;
}

std::string get_opcode_instruction(char tens, char ones) {
  std::string opcode;
  opcode += "0x";
  opcode += tens;
  opcode += ones;
  unsigned int x = std::stoul(opcode, nullptr, 16);
  opcode = opcode_table[x - 1];
  return opcode;
}

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
  
  char headerCol[19];
  char textRecordLen[10];
  file.read(headerCol, 19);
  for (auto& n : headerCol) {
    std::cout << n;
  }
  std::cout << '\n';
  file.read(textRecordLen, 10);
  for (auto& n : textRecordLen) {
    std::cout << n;
  }
  std::cout << '\n';
  int textColLen = get_textColLen(textRecordLen[8], textRecordLen[9]);
  char textObjCode[textColLen];
  file.read(textObjCode, textColLen);
  std::string opcode_insruction = get_opcode_instruction(textObjCode[0], textObjCode[1]);
  std::cout << opcode_insruction << '\n';

}

void readSym(char *fname) {
  /*
    Read the symtab file. Still deciding where to put this
  */
  std::ifstream symFile (fname, std::ifstream::in);
  std::string line;
  /*
    While loop populates a vector with each line of the symtab at each vector index.
  */
  while(symFile.good()) {
    getline(symFile, line);
    symTab.push_back(line);
  }
  int i = 2; //skip over the first two lines
  /*
    For loop will build out the symtab map. The map will store the
    address of the symbol and a map with all of the values pretaining to that symbol
  */
  for (i; i < symTab.size() - 1; i = i + 1) {
    if (symTab[i][0] != (char)NULL) {
      std::string symbolName = symTab[i].substr(0, 6);
      std::string symbolAddress = symTab[i].substr(8, 6);
      std::string symbolFlag = symTab[i].substr(16, 1);
      std::map<std::string, std::string> temp = {
        {"name", symbolName},
        {"address", symbolAddress},
        {"flag", symbolFlag}
      };
      symbolValues.insert(std::make_pair(symbolAddress, temp));
    } else {
      break;
    }
  }
  i = i + 3; //skip three lines of spaces according to the provided sample.sym file...
  /*
    For loop will build the literal map in the same fashion as the symtab map.
  */
  for (i; i < symTab.size() -1; i = i + 1) {
    std::string literal = symTab[i].substr(8, 6);
    std::string literalLength = symTab[i].substr(19, 1);
    std::string literalAddress = symTab[i].substr(24, 6);
    std::map<std::string, std::string> temp {
      {"literal", literal},
      {"length", literalLength},
      {"address", literalAddress}
    };
    literalValues.insert(std::make_pair(literalAddress, temp));
  }
  std::cout << "Done building symtab/literal maps..." << std::endl;
}

int main(int argc, char *argv[])
{
  readSym(argv[2]);
  return 0;
}