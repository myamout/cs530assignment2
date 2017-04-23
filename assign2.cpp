/* 
  Matthew Yamout cssc1002
  James Hammon   cssc0895
  Veer Talta     cssc0847
  assign2.cpp
*/

#include "header.h"
#include "opcode.h"

/* 
  List of global variables to help make
  everything easier on us.
*/
std::vector<std::string> symTab;
std::map<std::string, std::map<std::string, std::string>> symbolValues;
std::map<std::string, std::map<std::string, std::string>> literalValues;
std::vector<std::string> sourceCode;
std::vector<std::string> loopVector;
std::vector<std::string> addresses;
std::vector<std::array<std::string, 3>> definitions;
std::string startAddress;
std::string progLength;
std::string baseRegister;
int progStartingAddress = 0;

/* 
  Method will obtain the length of the current text column
*/
int get_textColLen(char tens, char ones) {
  int onesPlace = hex_conversion[ones];
  int tensPlace = hex_conversion[tens] * 16;
  return (onesPlace + tensPlace) * 2;
}

/* 
  This method takes the opcode of the given instruction, queries the opcode
  map from the opcode.h file and returns an array with the opcode and a flag
  to indicate if the instruction calls for immediate addressing.
*/
std::array<std::string, 2> get_opcode_instruction(char tens, char ones) {
  std::array<std::string, 2> opcode;
  opcode[0] += "0x";
  opcode[0] += tens;
  opcode[0] += ones;
  unsigned int x = std::stoul(opcode[0], nullptr, 16);
  if (opcode_table.count(x - 1) > 0) {
    opcode[0] = opcode_table[x - 1];
    opcode[1] = "1";
  } else if (opcode_table.count(x - 3) > 0) {
    opcode[0] = opcode_table[x - 3];
    opcode[1] = "0";
  }
  return opcode;
}

/* 
  This function will generate an array for the XBPE flags
  of the given instruction.
*/
std::array<int, 4> parseFlags(std::string instruction) {
  int nixpbe = hex_conversion[instruction[2]];
  std::array<int, 4> returnArray {{0, 0, 0, 0}};
  for (int i = 3; i >= 0; i = i - 1) {
    returnArray[i] = nixpbe & (1 << i) ? 1 : 0;
  }
  std::reverse(std::begin(returnArray), std::end(returnArray));
  return returnArray;
}

/*
  This method helps generate the address as a hex string by adding on the
  any leading zeros that are needed. 
*/
std::string getZeros(int length) {
  std::string returnString = "";
  for (int i = 0; i < 6 - length; i = i + 1) {
    returnString = returnString + "0";
  }
  return returnString;
}

/* 
  Function converts the current address from an integer to a hex string.
  This helps with quick look ups as the keys in the map are strings.
*/
std::string convertAddressToHexString(int startingAddress) {
  std::stringstream hexStream;
  std::locale loc;
  hexStream << std::hex << startingAddress;
  std::string temp = hexStream.str();
  for (int i = 0; i < temp.length(); i = i + 1) {
    temp[i] = std::toupper(temp[i], loc);
  }
  return temp;

}

/* 
  This functions takes the disp bytes from the given instruction and then returns the correct
  symbol value name, hex number, or whatever is in the base register. 
*/
std::string getDisp(std::string instruction, int format, std::string currentAddress, std::array<int, 4> flags) {
  std::stringstream stream;
  std::string disp;
  /* 
    Format 4 Disp Handle
  */
  if (format == 1) {
    stream << instruction[instruction.length() - 4];
    stream << instruction[instruction.length() - 3];
    stream << instruction[instruction.length() - 2];
    stream << instruction[instruction.length() - 1];
    disp = stream.str();
    std::string temp = getZeros(disp.length());
    disp = temp + disp;
    if (symbolValues.count(disp) > 0) {
      std::array<std::string, 2> opcode = get_opcode_instruction(instruction[0], instruction[1]);
      if (opcode[0].compare("LDB") == 0) {
        baseRegister = symbolValues[disp]["name"];
      }
      std::string name = symbolValues[disp]["name"];
      std::string addr = symbolValues[disp]["address"];
      std::string flag = symbolValues[disp]["flag"];
      definitions.push_back({{name, addr, flag}});
      return symbolValues[disp]["name"];
    }
  }
  /* 
    Else handle for Format 3
  */ 
  else {
    stream << instruction[instruction.length() - 3];
    stream << instruction[instruction.length() - 2];
    stream << instruction[instruction.length() - 1];
    disp = stream.str();
    std::string placeHolder = disp;
    disp = "0x" + disp;
    currentAddress = "0x" + currentAddress;
    unsigned int x = std::stoul(disp, nullptr, 16);
    unsigned int y = std::stoul(currentAddress, nullptr, 16);
    int swag = x + y + 3;
    std::array<std::string, 2> opcode = get_opcode_instruction(instruction[0], instruction[1]);
    if (opcode[0].compare("JLT") == 0 || opcode[0].compare("JEQ") == 0 || opcode[0].compare("JGT") == 0 || opcode[0].compare("JSUB") == 0) {
      std::vector<std::string> reverseAddresses;
      for (int i = addresses.size() - 1; i >= 0; i = i - 1) {
        reverseAddresses.push_back(addresses[i]);
      }
      for (std::string e : reverseAddresses) {
        for (std::string k : loopVector) {
          if (e.compare(k) == 0) {
            return symbolValues[k]["name"];
          }
        }
      }
    }
    std::string test = convertAddressToHexString(swag);
    test = getZeros(test.length()) + test;
    if (symbolValues.count(test) > 0) {
      std::array<std::string, 2> opcode = get_opcode_instruction(instruction[0], instruction[1]);
      if (opcode[0].compare("LDB") == 0) {
        baseRegister = symbolValues[test]["name"];
      }
      std::string name = symbolValues[test]["name"];
      std::string addr = symbolValues[test]["address"];
      std::string flag = symbolValues[test]["flag"];
      definitions.push_back({{name, addr, flag}});
      return symbolValues[test]["name"];
    }  else if (flags[1] == 1) {
      return baseRegister;
    } else if (placeHolder.compare("000") == 0) {
      return "0";
    } else {
      return placeHolder.erase(0, disp.find_first_not_of('0'));
    }
  }
  return "";
}

/* 
  Method handles if the given instruction calls for a literal.
*/
void literalHandler(std::string instruction, std::map<std::string, std::string> literal, std::string currentAddress) {
  std::stringstream stream;
  std::array<std::string, 2> opcode = get_opcode_instruction(instruction[0], instruction[1]);
  if (currentAddress.compare(startAddress) == 0) {
    stream << std::setw(9) << std::left << symbolValues[startAddress]["name"] << "+" << opcode[0] << "     " << literal["literal"] << std::endl;
  } else if (symbolValues.count(currentAddress) > 0) {
    stream << std::setw(9) << std::left << symbolValues[currentAddress]["name"] << "+" << opcode[0] << "     " << literal["literal"] << std::endl;
    loopVector.push_back(currentAddress);
  } else {
    stream << std::setw(9) << "   " << opcode[0] << "     " << literal["literal"] << std::endl;
  }
  sourceCode.push_back(stream.str());
  stream.str("");
  stream << std::setw(9) << "     " << "LTORG" << std::endl;
  sourceCode.push_back(stream.str());
  addresses.push_back(currentAddress);
}

/* 
  Handles Format 4 instructions
*/
int formatFourHandler(std::string instruction, std::array<int, 4> flags, std::string currentAddress) {
  std::stringstream stream;
  std::array<std::string, 2> opcode = get_opcode_instruction(instruction[0], instruction[1]);
  if (currentAddress.compare(startAddress) == 0) {
    stream << std::setw(8) << std::left << symbolValues[startAddress]["name"] << "+" << opcode[0] << "     ";
  } else if (symbolValues.count(currentAddress) > 0) {
    stream << std::setw(8) << std::left << symbolValues[currentAddress]["name"] << "+" << opcode[0] << "     ";
    loopVector.push_back(currentAddress);
  } else {
    stream << std::setw(8) << "   " << "+" << opcode[0] << "     ";
  }
  if (opcode[1].compare("1") == 0) {
    stream << "#";
  }
  std::string disp = getDisp(instruction, 1, currentAddress, flags);
  if (opcode[0].compare("RSUB") != 0) {
    stream << disp;
    if (flags[0] == 1) {
      stream << ",X";
    }
  }
  stream << std::endl;
  sourceCode.push_back(stream.str());
  // If the opcode instruction loads the base then we need to incrememnt the address by one?
  if (opcode[0].compare("LDB") == 0) {
    stream.str("");
    stream << std::setw(9) << "     " << "BASE" << "     " << disp << std::endl;
    sourceCode.push_back(stream.str());
    return 1;
  }
  addresses.push_back(currentAddress);
  return 0;
}

/* 
  Method handles Format 3 instructions
*/
int formatThreeHandler(std::string instruction, std::array<int, 4> flags, std::string currentAddress) {
  std::stringstream stream;
  std::array<std::string, 2> opcode = get_opcode_instruction(instruction[0], instruction[1]);
  if (currentAddress.compare(startAddress) == 0) {
    stream << std::setw(9) << std::left << symbolValues[startAddress]["name"] << opcode[0] << "     ";
  } else if (symbolValues.count(currentAddress) > 0) {
    stream << std::setw(9) << std::left << symbolValues[currentAddress]["name"] << opcode[0] << "     ";
    loopVector.push_back(currentAddress);
  } else {
    stream << std::setw(9) << "     " << opcode[0] << "     "; 
  }
  
  if (opcode[1].compare("1") == 0) {
    stream << "#";
  }
  std::string disp = getDisp(instruction, 0, currentAddress, flags);
  if (opcode[0].compare("RSUB") != 0) {
    stream << disp;
    if (flags[0] == 1) {
      stream << ",X";
    }  
  }
  stream << std::endl;
  sourceCode.push_back(stream.str());
  if (opcode[0].compare("LDB") == 0) {
    stream.str("");
    stream << std::setw(9) << "BASE" << "     " << disp;
    return 1;
  }
  addresses.push_back(currentAddress);
  return 0;
}

/* 
  Method will handle the header record
*/
void headerRecord(std::string line) {
  /*
    This function will build the header line instruction 
  */
  std::string progName = line.substr(1, 6);
  std::string startingAddr = line.substr(7, 6);
  startAddress = startingAddr;
  progStartingAddress = std::stoi(startingAddr);
  progLength = line.substr(13, 6);
  std::stringstream stream;
  stream << std::setw(9) << std::left << progName << "START   " << startingAddr << std::endl;
  std::string headerLine = stream.str();
  sourceCode.push_back(headerLine);
}

/* 
  Text record method will parse through all the instructions on each line.
*/
void textRecord(std::string line) {
  /* 
    Opcode instructions start at index 9
  */
  //Keeps track of where we are in the string
  int currentInstruction = 9;
  //Default constant for how many bytes to get... We grab two more depending on Format 4
  int bytesToPull = 6;
  //Array to test for the flags (NIXPBE)
  std::array<int, 4> flags {{0, 0, 0, 0}};
  std::string currentAddress = "";
  std::string addressCounter = "";
  std::string textStartingAddr = line.substr(1, 6);
  int startingAddress = std::stoi(textStartingAddr);

  std::string textLen = line.substr(7, 2);
  char tensPlace = textLen[0];
  char onesPlace = textLen[1];
  int textColLength = get_textColLen(tensPlace, onesPlace);
  std::string instructions = line.substr(9, textColLength);

  while (currentInstruction <= line.length() - 6) {
    
    /* 
    ` - Check for literal at current address
      - Check for format 4
      - Check for format 3
    */
    addressCounter = convertAddressToHexString(startingAddress);
    int addrLength = addressCounter.length();
    currentAddress = getZeros(addrLength);
    currentAddress = currentAddress + addressCounter;
    /* 
      Check Literal Table
      If we find a literal at this address we will pass off to another method after getting our extra bytes
      The else statement will handle the normal format 3 and 4 stuff
      The count method returns how many keys have the given parameter given
    */
    if (literalValues.count(currentAddress) > 0) {
      std::stringstream temp;
      temp << std::hex << literalValues[currentAddress]["length"];
      std::string hexLength = temp.str();
      int extraBytes = std::stoi(hexLength);
      std::string instruction = line.substr(currentInstruction, bytesToPull + extraBytes);
      
      literalHandler(instruction, literalValues[currentAddress], currentAddress);
      
      currentInstruction = currentInstruction + bytesToPull + extraBytes;
      startingAddress = startingAddress + 3;
    } else {
      std::string instruction = line.substr(currentInstruction, bytesToPull);
      flags = parseFlags(instruction);
      /* 
        Flow control here will determine if the instruction is format 3 or 4
        If Format 4 we will grab the extra bytes
      */
      /* 
        This means Format 4. We are pulling two extra bytes so
        we don't need to worry about converting anything to hex
        like when we are trying to get the literal length!
      */
      if (flags[3] == 1) {
        instruction = line.substr(currentInstruction, bytesToPull + 2);

        int baseFlag = formatFourHandler(instruction, flags, currentAddress);
        
        startingAddress = startingAddress + 4;
        if (baseFlag == 1)
          startingAddress = startingAddress + 1;
        currentInstruction = currentInstruction + 8;
      }
      /* 
        Nothing special to do here, we've already parsed out our six bytes!
      */ 
      else {
        int baseFlag = formatThreeHandler(instruction, flags, currentAddress);

        currentInstruction = currentInstruction + 6;
        startingAddress = startingAddress + 3;
        if (baseFlag == 1)
          startingAddress = startingAddress + 1;
      }
    } // end of large else
    
  } // end of while loop

} // end of method

/* 
  This function will obtain the sizes of all the symbols in the symtab
*/
std::vector<std::array<std::string, 3> > getSymbolSizes(std::set<std::array<std::string, 3>> sortedSym) {
  std::vector<std::array<std::string, 3> > returnVector;
  for (auto const &e : sortedSym) {
    returnVector.push_back(e);
  }
  for (int i = 0; i < returnVector.size(); i = i + 1) {
    if (i == returnVector.size() - 1) {
      unsigned int x = std::stoul(returnVector[i][1], nullptr, 16);
      unsigned int y = std::stoul(progLength, nullptr, 16);
      returnVector[i][1] = std::to_string((y - x) / 3);
      if ((y - x) % 3 == 0) {
        returnVector[i][2] = "RESW";
      } else {
        returnVector[i][2] = "RESB";
      }
    } else {
      unsigned int x = std::stoul(returnVector[i][1], nullptr, 16);
      unsigned int y = std::stoul(returnVector[i + 1][1], nullptr, 16);
      returnVector[i][1] = std::to_string((y - x) / 3);
      if ((y - x) % 3 == 0) {
        returnVector[i][2] = "RESW";
      } else {
        returnVector[i][2] = "RESB";
      }
    }
  }
  return returnVector;
}

/* 
  This method is what writes definitions to the output file.
*/
void printDefinitions(std::vector<std::array<std::string, 3> > sortedDefinitions) {
  for (auto const &e : sortedDefinitions) {
    std::stringstream stream;
    stream << std::setw(9) << std::left << e[0] << e[2] << "     " << e[1] << std::endl;
    sourceCode.push_back(stream.str());
  }
}

/* 
  This functions sorts the symbols based on the address given in the symtab.
  We take advantage of a Set here which sorts the addresses lowest to highest and makes sure
  there are only unqiue symbols here.
*/
void sortDefinitions() {
  std::vector<int> test;
  std::vector<std::string> meh;
  std::vector<std::array<std::string, 3> > sortedDefinitions;
  for (auto const &e : definitions) {
    std::string addr = e[1];
    addr = "0x" + addr;
    unsigned int x = std::stoul(addr, nullptr, 16);
    test.push_back(x);
  }
  std::set <int> test2(std::begin(test), std::end(test));
  for (auto const &e : test2) {
    std::string tempString = convertAddressToHexString(e);
    tempString = getZeros(tempString.length()) + tempString;
    meh.push_back(tempString);
  }
  for (auto const &e : meh) {
    for (auto const &k : definitions) {
      if (e.compare(k[1]) == 0) {
        sortedDefinitions.push_back(k);
      }
    }
  }
  std::set<std::array<std::string, 3>> sortedSym(std::begin(sortedDefinitions), std::end(sortedDefinitions));
  sortedDefinitions = getSymbolSizes(sortedSym);
  printDefinitions(sortedDefinitions);
}

/* 
  End record method handles all the end record needs
*/
void endRecord(std::string line) {
  std::stringstream stream;
  sortDefinitions();
  stream << std::setw(9) << "     " << "END" << "     " << symbolValues[startAddress]["name"] << std::endl;
  sourceCode.push_back(stream.str());
}

/* 
  Writes the contents of the source code vector to the output file.
  Each element of the source code vector is a line of source code.
  This makes writing the contents relatively easily.
*/
void writeOutput() {
  std::ofstream file("output.sic", std::ios::out | std::ios::trunc);
  for (auto const &e : sourceCode) {
    file << e;
  }
}

/* 
  Method opens up the object code file and reads line by line
  until end of file. We determine which method to call by simply
  checking the first character of each line.
*/
void readObj(char *fname) {
  std::ifstream objFile (fname, std::ifstream::in);
  std::string line;
  while(objFile.good()) {
    getline(objFile, line);
    if (line[0] != (char)NULL) {
      if (line[0] == 'H') {
        headerRecord(line);
      } else if (line[0] == 'T') {
        textRecord(line);
      } else if (line[0] == 'E') {
        endRecord(line);
      }
    } 
  }
  writeOutput();
}

/* 
  Reads the contents of the symbol file into a symbol value map 
  and literal value map.
*/
void readSym(std::string fname) {
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
        {"flag", symbolFlag},
        {"define", "0"}
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
}

bool findFile(std::string objFile) {
  std::ifstream file(objFile);
  return file.good();
}

int main(int argc, char *argv[])
{
  std::string objFile = argv[1];
  bool fileExists = findFile(objFile);
  if (fileExists) {
    std::string symbolFile;
    for (auto const &e : objFile) {
      if (e == '.') {
        break;
      } else {
        symbolFile += e;
      }
    }
    symbolFile = symbolFile + ".sym";
    readSym(symbolFile);
    readObj(argv[1]);
  }
  return 0;
}