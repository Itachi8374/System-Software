#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>

using namespace std;

#define FIELD_LENGTH 10

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

//remove spaces at the end of string
string remove_trailing_space(string s){
    int l = 0, r = s.size()-1;
    while(r>=0 && s[r]==' ' || s[r] == '\t'){
        r--;
    }
    string ans = "";
    for(int i=l; i<=r; ++i){
        ans += (char)s[i];
    }
    return ans;
}

//converts a decimal number to a hexadecimal string
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

//Adds empty spaces at the end of a string till its length is not equal to specified length
void fit_to_field_length(string &s, int len){
    while(s.size()<len){
        s+=" ";
    }
}

//adds 0 at the front of string till its length is not equal to specified length
void fit_to_field_length_add(string &s, int len){
    while(s.size()<len){
        s = "0" +s;
    }
}

//Comments are defined using . Therefore we search and if first non-empty character is . then its a comment
bool is_comment(string s){
    bool first_char = false;
    for(int i=0; i<s.size(); ++i){
        if(s[i]==' ') continue;
        if(s[i] == '.' && !first_char) return true;
        first_char = true;
    }
    return false;
}

//prepares the various components to be printed in first pass to intermediate file
string print_pass1(string label, string opcode, string operand, int locctr, bool end){
    string LOCCTR = dec_to_hex(locctr);
    fit_to_field_length(label, FIELD_LENGTH);
    fit_to_field_length(opcode, FIELD_LENGTH);
    fit_to_field_length_add(LOCCTR,4);
    LOCCTR = (end?"":LOCCTR);
    fit_to_field_length(LOCCTR, FIELD_LENGTH);
    string record = LOCCTR+label+opcode+operand;
    return record;
}

//handles first pass and returns the length of program
int first_pass(){
    //first we build OPTAB which stores opcodes of instruction set
    build_OPTAB();
    fstream fread, fwrite;
    fread.open("input.txt"); //input source code to be given in this file
    fwrite.open("intermediate.txt", ios::out); //intermediate.txt will be produced after pass1

    string line; 
    int LOCCTR=0, START = 0, LENGTH=0;
    bool duplicate_error = false;
    bool invalid_opcode_error = false;

    while(!fread.eof()){
        getline(fread,line);

        string label,opcode,operand;
        label = remove_trailing_space(line.substr(0,FIELD_LENGTH));
        if(is_comment(label)){
            continue;
        }
        opcode = remove_trailing_space(line.substr(10,FIELD_LENGTH));
        operand = remove_trailing_space(line.substr(20));

        if(opcode == "START"){
            LOCCTR = stoi(operand, nullptr, 16); //initialize location counter to start address of program
            START = LOCCTR;
            string record =  print_pass1(label, opcode, operand, LOCCTR,false);
            fwrite<<record<<endl;
        }else if(opcode == "END"){
            LENGTH = LOCCTR - START; //saves the length of program
            string record =  print_pass1(label, opcode, operand, LOCCTR, true);
            fwrite<<record;
        }else{
            if(label != ""){
                //terminate the program when a symbol is defined twice
                //Otherwise add it to Symbol table
                if(SYMTAB.find(label)!=SYMTAB.end()){
                    duplicate_error = true;
                    cerr<<"duplicate definition found for symbol "<<label<<endl<<"Previous definition "<<label<<": "<<SYMTAB[label];
                    exit(1);
                }else{
                    string symb = dec_to_hex(LOCCTR);
                    fit_to_field_length_add(symb,4);
                    SYMTAB[label] = symb;
                }
            }

            string record =  print_pass1(label, opcode, operand, LOCCTR, false);
            fwrite<<record<<endl;
            //Increment location counter suitably based on opcode and operand
            //Terminate the program if opcode doesn't exist in optable
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
                    cerr<<"Unidentified type "<<identifier<<" found for BYTE"<<endl;
                    exit(1);
                }
            }else{
                cerr<<"OPCODE "<<opcode<<" not found in instruction set"<<endl;
                exit(1);
            }
        }
    }
    fread.close();
    fwrite.close();
    return LENGTH;
}

//checks if the indexed addressing is used 
bool indexed_addressing(string s){
    for(int i=0; i<s.size(); ++i){
        if(s[i] == ','){
            return true;
        }
    }
    return false;
}

//if indexed addressing is used then it converts the record based on it
void convert_indexed(string &s){
    int address = stoi(s,nullptr,16);
    address += (1<<15);
    s = dec_to_hex(address);
}

//calculates object code in both indirect and indexed addressing modes
string calc_object_code(string object_code, string symbol, bool indexed){
    if(indexed){
        convert_indexed(symbol);
    }
    return object_code+symbol;
}

//if BYTE's operand is C'' ie Character
string get_cBYTE(string s){
    string ans = "";
    for(int i=2; i<s.size()-1; ++i){
        int c = s[i];
        ans+=dec_to_hex(c);
    }
    return ans;
}

//if BYTE's operand is X'' ie Hexadecimal
string get_xBYTE(string s){
    return s.substr(2,s.size()-3);
}

//prepares to print record in listing.txt file
string print_pass2(string locctr, string label, string opcode, string operand){
    fit_to_field_length(locctr,FIELD_LENGTH);
    fit_to_field_length(label, FIELD_LENGTH);
    fit_to_field_length(opcode, FIELD_LENGTH);
    fit_to_field_length(operand, 3*FIELD_LENGTH);
    return locctr+label+opcode+operand;
}

//returns the Header field of object_code
string get_header(string name, string start, int LENGTH){
    string ans = "H\u2038";
    string prog_name = name.substr(0,6); fit_to_field_length(prog_name, 6);
    string start_addr = start.substr(0,6); fit_to_field_length_add(start_addr,6);
    string length = dec_to_hex(LENGTH); fit_to_field_length_add(length,6);
    ans+=prog_name+"\u2038"+start_addr+"\u2038"+length;
    return ans;
}

//returns the Text record field of object code
string get_text_record(string T_record, string T_start_addr, int caret_count){
    fit_to_field_length_add(T_start_addr,6);
    string ans = "T\u2038"+T_start_addr+"\u2038";
    string len = dec_to_hex((T_record.size()+1-caret_count)/2);
    fit_to_field_length_add(len,2);
    string temp_T = T_record.substr(3);
    fit_to_field_length_add(temp_T, stoi(len)*2+caret_count-3);
    T_record = "\u2038" +temp_T;
    ans+=len+T_record;
    return ans;
}

//returns the End field of object code
string get_end(string s){
    fit_to_field_length_add(s,6);
    return "E\u2038"+s;
}

//takes length of program as input from first pass and produces the objectcode
void second_pass(int LENGTH){
    fstream fr_intermediate, fw_listing, fw_object;
    fr_intermediate.open("intermediate.txt");
    fw_listing.open("listing.txt", ios::out);
    fw_object.open("object_code.o", ios::out);

    string line, first_executable_instruction; 
    string T_record = "", T_start_addr = "";
    bool undefined_symbol = false;
    bool first = false;
    int caret_count = 0;

    while(!fr_intermediate.eof()){
        getline(fr_intermediate,line);
        string locctr,label,opcode,operand;
        locctr = remove_trailing_space(line.substr(0,FIELD_LENGTH));
        label = remove_trailing_space(line.substr(10,FIELD_LENGTH));
        opcode = remove_trailing_space(line.substr(20,FIELD_LENGTH));
        operand = remove_trailing_space(line.substr(30));

        if(opcode == "START"){
            string header = get_header(label, locctr, LENGTH);
            fw_object<<header<<endl;
            fw_listing<<line<<endl;
            T_start_addr = locctr;
        }else if(opcode == "END"){
            //If the previous text record is not yet printed than print it first
            if(T_record!=""){
                string text_record = get_text_record(T_record,T_start_addr,caret_count);
                fw_object<<text_record<<endl;
                T_record = "";
            }
            string end = get_end(first_executable_instruction);
            fw_listing<<line;
            fw_object<<end;
        }else{
            string object_code = "";
            //Checks if the opcode exists in OPTAB
            if(OPTAB.find(opcode)!=OPTAB.end()){
                //If its the first executable instruction than save its address
                if(!first){
                    first = true;
                    first_executable_instruction = locctr;
                }
                object_code+=OPTAB[opcode];
                //Check for object code of operand
                if(operand != ""){
                    bool i_address = indexed_addressing(operand);
                    if(i_address){
                        operand = operand.substr(0,operand.size()-2);
                    }
                    //If operand not found in symbol table then generate error message and use "0000" as its address
                    if(SYMTAB.find(operand)!=SYMTAB.end()){
                        object_code = calc_object_code(object_code,SYMTAB[operand],i_address);
                    }else{
                        object_code += "0000";
                        undefined_symbol = true;
                        cout<<"Unidentified symbol "<<operand<<endl<<"Using 0000 as the address";
                    }
                }else{
                    object_code+= "0000";
                }
            }else if(opcode == "BYTE"){
                if(operand[0] == 'C'){
                    object_code+=get_cBYTE(operand);
                }else if(operand[0] == 'X'){
                    object_code+=get_xBYTE(operand);
                }
            }else if(opcode == "WORD"){
                object_code+=dec_to_hex(stoi(operand));
                fit_to_field_length_add(object_code,6);
            }
            string record = print_pass2(locctr,label,opcode,operand) + object_code;
            fw_listing<<record<<endl;

            if(T_start_addr == "" && opcode!="RESW" && opcode!="RESB"){
                T_start_addr = locctr;
            }
            //if opcodes are RESW or RESB we have to generate new text record if needed
            //caret count is incremented by 3 because UNICODE 2038 takes 3 bytes
            if(T_record.size() + object_code.size()-caret_count<=60 && opcode!="RESW" && opcode!="RESB"){
                T_record+="\u2038"+object_code;
                caret_count+=3;
            }else{
                if(T_record == "") continue;
                string text_record = get_text_record(T_record,T_start_addr,caret_count);
                fw_object<<text_record<<endl;
                if(opcode == "RESW" || opcode == "RESB"){
                    T_record = "";
                    T_start_addr = "";
                    caret_count = 0;
                }else{
                    T_record = "\u2038"+object_code;
                    T_start_addr = locctr;
                    caret_count = 3;
                }
                
            }
        }
    }

    fw_listing.close();
    fw_object.close();
}

int main(){
    second_pass(first_pass());
}


