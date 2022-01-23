#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
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
}

string remove_trailing_space(string s){
    int l = 0, r = s.size()-1;
    while(r>=0 && s[r]==' '){
        r--;
    }
    string ans = "";
    for(int i=l; i<=r; ++i){
        ans += (char)s[i];
    }
    return ans;
}

string dec_to_hex(int n){
    string ans = "";
    while(n){
        int digit = n%16;
        if(digit>9){
            ans =(char)('A'+digit-10) + ans;
        }else{
            ans = (char)('0'+digit) + ans;
        }
        n/=16;
    }
    return ans;
}

void fit_to_field_length(string &s){
    while(s.size()!=10){
        s+=" ";
    }
}

string print_pass1(string label, string opcode, string operand, int locctr, bool end){
    string LOCCTR = (end? "" :dec_to_hex(locctr));
    fit_to_field_length(label);
    fit_to_field_length(opcode);
    fit_to_field_length(LOCCTR);
    string record = LOCCTR+label+opcode+operand;
    return record;
}

int first_pass(){
    build_OPTAB();
    fstream fread, fwrite;
    fread.open("input.txt");
    fwrite.open("intermediate.txt");
    string line; 
    int LOCCTR=0, START = 0, LENGTH=0;
    bool duplicate_error = false;
    bool invalid_opcode_error = false;
    while(!fread.eof()){
        getline(fread,line);
        string label,opcode,operand;
        label = remove_trailing_space(line.substr(0,10));
        if(label == "."){
            continue;
        }
        opcode = remove_trailing_space(line.substr(10,10));
        operand = remove_trailing_space(line.substr(20));

        if(opcode == "START"){
            LOCCTR = stoi(operand, nullptr, 16);
            START = LOCCTR;
            string record =  print_pass1(label, opcode, operand, LOCCTR,false);
            fwrite<<record<<endl;
        }else if(opcode == "END"){
            LENGTH = LOCCTR - START;
            string record =  print_pass1(label, opcode, operand, LOCCTR, true);
            fwrite<<record<<endl;
        }else{
            if(label != ""){
                if(SYMTAB.find(label)!=SYMTAB.end()){
                    duplicate_error = true;
                }else{
                    SYMTAB[label] = LOCCTR;
                }
            }
            string record =  print_pass1(label, opcode, operand, LOCCTR, false);
            fwrite<<record<<endl;
            if(OPTAB.find(opcode)!=OPTAB.end()){
                LOCCTR+=3;
            }else if(opcode == "WORD"){
                LOCCTR+=3;
            }else if(opcode == "RESW"){
                int len = stoi(operand);
                LOCCTR += 3*len;
            }else if(opcode == "RESB"){
                LOCCTR+= stoi(operand);
            }else if(opcode == "BYTE"){
                char identifier = operand[0];
                int len = operand.size();
                if(identifier == 'C'){
                    LOCCTR += (len-3);
                }else if(identifier == 'X'){
                    LOCCTR += (len-2)/2;
                }else{
                    invalid_opcode_error = true;
                }
            }else{
                invalid_opcode_error = true;
            }
        }
    }
    fread.close();
    fwrite.close();
    return LENGTH;
}

void second_pass(){
    
}

int main(){
    first_pass();
}


