#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

using namespace std;

unordered_map <string,string> OPTAB;
unordered_map <string,string> SYMTAB;

void build_OPTAB(){
    OPTAB["LDA"] = "00";
    OPTAB["LDX"] = "04";
    OPTAB["LDL"] = "08";
    OPTAB["STA"] = "0C";
    OPTAB["STX"] = "10";
    OPTAB["STL"] = "14";
    OPTAB["LDCH"] = "50";
    OPTAB["STCH"] = "54";
    OPTAB["ADD"] = "18";
    OPTAB["SUB"] = "1C";
    OPTAB["MUL"] = "20";
    OPTAB["DIV"] = "24";
    OPTAB["COMP"] = "28";
    OPTAB["J"] = "3C";
    OPTAB["JLT"] = "38";
    OPTAB["JEQ"] = "30";
    OPTAB["JGT"] = "34";
    OPTAB["JSUB"] = "48";
    OPTAB["RSUB"] = "4C";
    OPTAB["TIX"] = "2C";
    OPTAB["TD"] = "E0";
    OPTAB["RD"] = "D8";
    OPTAB["WD"] = "DC";
    OPTAB["RESW"] = "";
    OPTAB["RESB"] = "";
    OPTAB["WORD"] = "";
    OPTAB["BYTE"] = "";
}


