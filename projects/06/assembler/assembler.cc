#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <bitset>
using namespace std;

// static
bool isHackCharSet(char c) {
    return isalpha(c)
      || isdigit(c)
      || c == '+'
      || c == '-'
      || c == ';'
      || c == '='
      || c == '@'
      || c == '.'
      || c == '$'
      || c == '_'
      || c == '!'
      || c == '&'
      || c == '|'
      ;
}

unordered_map<string, string> destCode = {
      {"",    "000"},
      {"M",   "001"},
      {"D",   "010"},
      {"MD",  "011"},
      {"A",   "100"},
      {"AM",  "101"},
      {"AD",  "110"},
      {"AMD", "111"}
};

unordered_map<string, string> jumpCode = {
      {"",    "000"},
      {"JGT", "001"},
      {"JEQ", "010"},
      {"JGE", "011"},
      {"JLT", "100"},
      {"JNE", "101"},
      {"JLE", "110"},
      {"JMP", "111"}
};

unordered_map<string, string> compCode = {
      {"0",   "0101010"},
      {"1",   "0111111"},
      {"-1",  "0111010"},
      {"D",   "0001100"},
      {"A",   "0110000"},
      {"M",   "1110000"},
      {"!D",  "0001101"},
      {"!A",  "0110001"},
      {"!M",  "1110001"},
      {"-D",  "0001111"},
      {"-A",  "0110011"},
      {"-M",  "1110011"},
      {"D+1", "0011111"},
      {"A+1", "0110111"},
      {"M+1", "1110111"},
      {"D-1", "0001110"},
      {"A-1", "0110010"},
      {"M-1", "1110010"},
      {"D+A", "0000010"},
      {"D+M", "1000010"},
      {"D-A", "0010011"},
      {"D-M", "1010011"},
      {"A-D", "0000111"},
      {"M-D", "1000111"},
      {"D&A", "0000000"},
      {"D&M", "1000000"},
      {"D|A", "0010101"},
      {"D|M", "1010101"}
};

// class
class SymbolTable {
private:
  unordered_map<string,string> table = {
        {"R0",     "0"},
        {"R1",     "1"},
        {"R2",     "2"},
        {"R3",     "3"},
        {"R4",     "4"},
        {"R5",     "5"},
        {"R6",     "6"},
        {"R7",     "7"},
        {"R8",     "8"},
        {"R9",     "9"},
        {"R10",    "10"},
        {"R11",    "11"},
        {"R12",    "12"},
        {"R13",    "13"},
        {"R14",    "14"},
        {"R15",    "15"},
        {"SCREEN", "16384"},
        {"KBD",    "24576"},
        {"SP",     "0"},
        {"LCL",    "1"},
        {"ARG",    "2"},
        {"THIS",   "3"},
        {"THAT",   "4"}
  };

  int freeCounter = 16;

public:
  SymbolTable() {
  }

  bool isSymbol(const string& s) const {
      return (!s.empty() && isalpha(s[0]));
  }

  void put(string symbol, string value) {
      table[symbol] = value;
  }

  void malloc(string& symbol) {
      put(symbol, to_string(freeCounter));
      freeCounter += 1;
  }

  string getOrMalloc(string symbol) {
      if (isSymbol(symbol)) {
          if (!table.contains(symbol)) {
              malloc(symbol);
          }
          return table[symbol];
      } else {
          // return as literal
          return symbol;
      }
  }
};

class Instruction {
  bool isA; // is A instruction
  string addr;
  string dest;
  string comp;
  string jump;

  string IntTo15Bit(const string& integer) const {
      return bitset<15>(atoi(integer.c_str())).to_string();
  }

public:
    Instruction(string addr): isA(true), addr(addr) {}
    Instruction(string dest, string comp, string jump): isA(false), dest(dest), comp(comp), jump(jump) {}

    string toByteString(SymbolTable& st) const {
        if (isA) {
            string res = st.getOrMalloc(addr);
            // cout << "addr: " << addr << " res: " << res << "\n";
            return "0" + IntTo15Bit(res);
        } else {
            return "111" + compCode[comp] + destCode[dest] + jumpCode[jump];
        }
    }

    static Instruction parse(ifstream& file) {
        if (file.peek() == '@') {
            // pase A instruction
            file.get();
            string addr = "";
            while(isHackCharSet(file.peek())) {
                char c = file.get();
                addr += c;
            }
            return Instruction(addr);
        } else {
            vector<string> dcj= {"", "", ""}; // dest, comp, jump
            int p = 1;

            while(isHackCharSet(file.peek())) {
                char c = file.get();
                if (c == '=') {
                    dcj[0] = dcj[1];
                    dcj[1] = "";
                    continue;
                }
                if (c == ';') {
                    p = 2;
                    continue;
                }
                dcj[p] += c;
            }

            return Instruction(dcj[0], dcj[1], dcj[2]);
        }
    }

    friend ostream& operator<<(ostream &os, const Instruction& ins) {
        if (ins.isA) {
            os << "@" + ins.addr;
        } else {
            os << ins.dest + "=" + ins.comp + ";" + ins.jump;
        }
        return os;
    }
};

// main
int main(int argc, char* argv[]) {
    if (argc <= 1) {
        cout << "./a.out <filename> ";
        return 0;
    }

    // Symbol Table
    SymbolTable st = SymbolTable();

    // Phase 1 Parsing
    vector<Instruction> asms;
    std::ifstream file(argv[1]);
    // cout << "Parsing " << argv[1] << "\n";
    if (file.is_open()) {
        string discard;
        string line = "";
        while(file) {
          char c = file.peek();

          // assuming no divid character
          if (c == '/') {
              getline(file, discard);
              continue;
          }

          if (c == '(') {
              file.get();
              string symbol = "";
              while(file.peek() != ')') {
                  char sc = file.get();
                  if (!isHackCharSet(sc)) {
                      continue;
                  }
                  symbol += sc;
              }

              st.put(symbol, to_string(asms.size()));
              continue;
          }

          if (!isHackCharSet(c)) {
              file.get();
              continue;
          }

          asms.push_back(Instruction::parse(file));
        }

        for (const Instruction& a: asms) {
            // cout << a << "\n";
            cout << a.toByteString(st) << "\n";
        }

        // Phase 2
    } else {
        cout << "Error Opening File";
    }
}
