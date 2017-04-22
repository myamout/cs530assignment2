#include "header.h"
#include "opcode.h"

std::vector<std::string> symTab;
std::map<std::string, std::map<std::string, std::string>> symbolValues;
std::map<std::string, std::map<std::string, std::string>> literalValues;
std::vector<std::string> sourceCode;
int progStartingAddress = 0;

int get_textColLen(char tens, char ones) {
  int onesPlace = hex_conversion[ones];
  int tensPlace = hex_conversion[tens] * 16;
  return (onesPlace + tensPlace) * 2;
}

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



std::array<int, 4> parseFlags(std::string instruction) {
  int nixpbe = hex_conversion[instruction[2]];
  std::array<int, 4> returnArray {{0, 0, 0, 0}};
  for (int i = 3; i >= 0; i = i - 1) {
    returnArray[i] = nixpbe & (1 << i) ? 1 : 0;
  }
  std::reverse(std::begin(returnArray), std::end(returnArray));
  return returnArray;
}

std::string getZeros(int length) {
  std::string returnString = "";
  for (int i = 0; i < 6 - length; i = i + 1) {
    returnString = returnString + "0";
  }
  return returnString;
}

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

std::string getDisp(std::string instruction, int format, std::string currentAddress) {
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
    disp = "0x" + disp;
    currentAddress = "0x" + currentAddress;
    unsigned int x = std::stoul(disp, nullptr, 16);
    unsigned int y = std::stoul(currentAddress, nullptr, 16);
    int swag = x + y + 3;
    std::string test = convertAddressToHexString(swag);
    test = getZeros(test.length()) + test;
    if (symbolValues.count(test) > 0) {
      return symbolValues[test]["name"];
    } 
  }
  return "";
}

void literalHandler(std::string instruction, std::map<std::string, std::string> literal, std::string currentAddress) {
  std::array<std::string, 2> opcode = get_opcode_instruction(instruction[0], instruction[1]);
  std::cout << "         " << opcode[0] << "     " << literal["literal"] << std::endl;
}

int formatFourHandler(std::string instruction, std::array<int, 4> flags, std::string currentAddress) {
  std::array<std::string, 2> opcode = get_opcode_instruction(instruction[0], instruction[1]);
  std::cout << "         " << "+" << opcode[0] << "     ";
  if (opcode[1].compare("1") == 0) {
    std::cout << "#";
  }
  std::string disp = getDisp(instruction, 1, currentAddress);
  std::cout << disp;
  std::cout << std::endl;
  // If the opcode instruction loads the base then we need to incrememnt the address by one?
  if (opcode[0].compare("LDB") == 0) {
    std::cout << "        BASE" << "    " << disp <<std::endl;
    return 1;
  }
  return 0;
}

int formatThreeHandler(std::string instruction, std::array<int, 4> flags, std::string currentAddress) {
  std::array<std::string, 2> opcode = get_opcode_instruction(instruction[0], instruction[1]);
  if (currentAddress.compare("000000") == 0) {
    std::cout << "FIRST" << "     " << opcode[0] << "     ";
  } else if (symbolValues.count(currentAddress) > 0) {
    std::cout << symbolValues[currentAddress]["name"] << "     " << opcode[0] << "     ";
  } else {
    std::cout << "         " << opcode[0] << "     "; 
  }
  
  if (opcode[1].compare("1") == 0) {
    std::cout << "#";
  }
  std::string disp = getDisp(instruction, 0, currentAddress);
  std::cout << disp << std::endl;
  if (opcode[0].compare("LDB") == 0) {
    std::cout << "        BASE" << std::endl;
    return 1;
  }
  return 0;
}

void headerRecord(std::string line) {
  /*
    This function will build the header line instruction 
  */
  std::string progName = line.substr(1, 6);
  std::string startingAddr = line.substr(7, 6);
  progStartingAddress = std::stoi(startingAddr);
  std::string progLength = line.substr(13, 6);
  std::stringstream stream;
  stream << std::setw(9) << std::left << progName << "START   " << startingAddr << std::endl;
  std::string headerLine = stream.str();
  sourceCode.push_back(headerLine);
}

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
  int startingAddress = progStartingAddress;

  std::string textLen = line.substr(7, 2);
  char tensPlace = textLen[0];
  char onesPlace = textLen[1];
  int textColLength = get_textColLen(tensPlace, onesPlace);
  std::string instructions = line.substr(9, textColLength);

  while (currentInstruction <= line.length() - 6) {
    
    /* 
    ` 1. check literal map to see if address is there and then grab extra stuff depending on the length of the literal
         if literal has been found, skip the flag parse. just get opcode and go straight to printing
      2. parse the flags
      3. if E flag is 1, grab two extra bytes and increment the location counter
      4. go method for format 3 or method for format 4
      ** We need to pass in the current address string we build to check the symbol tab for loops n shit **
      ** If the opcode is for a jump we put the loop on the left of the instruction **
      5. get the return string and push_back to source code vector
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
void modRecord(std::string line) {}
void endRecord(std::string line) {}

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
      } else if (line[0] == 'M') {
        modRecord(line);
      } else if (line[0] == 'E') {
        endRecord(line);
      }
    } 
  }
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
  readObj(argv[1]);
  return 0;
}