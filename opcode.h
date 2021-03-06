/* 
  Matthew Yamout cssc1002
  James Hammon   cssc0895
  Veer Talta     cssc0847
  opcode.h
*/

#include <map>
#include <string>

std::map<const short int, const std::string> opcode_table = {
  {0x18, "ADD"},
  {0x58, "ADDF"},
  {0x90, "ADDR"},
  {0x40, "AND"},
  {0xB4, "CLEAR"},
  {0x28, "COMP"},
  {0x88, "COMPF"},
  {0xA0, "COMPR"},
  {0x24, "DIV"},
  {0x64, "DIVF"},
  {0x9C, "DIVR"},
  {0xC4, "FIX"},
  {0xC0, "FLOAT"},
  {0xF4, "HIO"},
  {0x3C, "J"},
  {0x30, "JEQ"},
  {0x34, "JGT"},
  {0x38, "JLT"},
  {0x48, "JSUB"},
  {0x00, "LDA"},
  {0x68, "LDB"},
  {0x50, "LDCH"},
  {0x70, "LDF"},
  {0x08, "LDL"},
  {0x6C, "LDS"},
  {0x74, "LDT"},
  {0x04, "LDX"},
  {0xD0, "LPS"},
  {0x60, "MULF"},
  {0x98, "MULR"},
  {0xC8, "NORM"},
  {0x44, "OR"},
  {0xD8, "RD"},
  {0xAC, "RMO"},
  {0x4C, "RSUB"},
  {0xA4, "SHIFTL"},
  {0xA8, "SHIFTR"},
  {0xF0, "SIO"},
  {0xEC, "SSK"},
  {0x0C, "STA"},
  {0x78, "STB"},
  {0x54, "STCH"},
  {0x80, "STF"},
  {0xD4, "STI"},
  {0x14, "STL"},
  {0x7C, "STS"},
  {0xE8, "STSW"},
  {0x84, "STT"},
  {0x10, "STX"},
  {0x1C, "SUB"},
  {0x5C, "SUBF"},
  {0x94, "SUBR"},
  {0xB0, "SVC"},
  {0xE0, "TD"},
  {0x2C, "TIX"},
  {0xB8, "TIXR"},
  {0xDC, "WD"}
};

std::map<const char, const int> hex_conversion = {
  {'1', 1},
  {'2', 2},
  {'3', 3},
  {'4', 4},
  {'5', 5},
  {'6', 6},
  {'7', 7},
  {'8', 8},
  {'9', 9},
  {'A', 10},
  {'B', 11},
  {'C', 12},
  {'D', 13},
  {'E', 14},
  {'F', 15}
};
