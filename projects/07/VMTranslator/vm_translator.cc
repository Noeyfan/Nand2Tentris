#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <bitset>
using namespace std;

enum CommandType {
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL
};

// static
bool isCharSet(char c) {
    return isalpha(c) || isdigit(c);
}

bool isArithmeticOps(string& token) {
    return token == "eq"
        || token == "add"
        || token == "sub"
        || token == "lt"
        || token == "gt"
        || token == "neg"
        || token == "and"
        || token == "or"
        || token == "not";
}

bool isStackOps(string& token) {
    return token == "push" || token == "pop";
}

string getSegmentPointers(string& segment) {
    if (segment == "local") return "LCL";
    if (segment == "argument") return "ARG";
    if (segment == "this") return "THIS";
    if (segment == "that") return "THAT";
    if (segment == "temp") return "5";
    throw invalid_argument("segment: " + segment + " is invalid.");
}

string getThisOrThat(int index) {
    return index == 0 ? "THIS" : "THAT";
}

// Tokenizer
class Tokenizer {
private:
    ifstream& file;

public:
    Tokenizer(ifstream& file): file(file) {}

    string next() {
        string token = "";
        string discard;
        while(file) {
            char c = file.get();
            if (c == '/' && file.peek() == '/') {
                getline(file, discard);
                continue;
            }

            if (isCharSet(c)) {
                token += c;

                if (!isCharSet(file.peek())) {
                    return token;
                }
            }
        }

        return "EOF";
    }
};

// Parser
class Parser {
private:
    ifstream& file;
    Tokenizer tokenizer;

    bool moreCommands;
    CommandType cType;
    string firstArg;
    string secondArg;

public:
    Parser(ifstream& file): file(file), tokenizer(file) {
        if (!file.is_open()) {
            throw invalid_argument("file not found");
        }

        // find first command
        advance();
    }

    bool hasMoreCommands() {
        return moreCommands;
    }

    void advance() {
        moreCommands = false;
        while(true) {
            string token = tokenizer.next();
            if (token == "EOF") {
                return;
            }

            if (isArithmeticOps(token)) {
                cType = C_ARITHMETIC;
                moreCommands = true;
                firstArg = token;
                return;
            } else if (isStackOps(token)) {
                cType = token == "push" ? C_PUSH: C_POP;
                firstArg = tokenizer.next();
                secondArg = tokenizer.next();
                moreCommands = true;
                return;
            }
        }
    }

    int commandType() {
        return cType;
    }

    string arg1() {
        return firstArg;
    }

    int arg2() {
        return stoi(secondArg);
    }
};

class CodeWriter {
private:
  string filename;
  int counter;

public:
    CodeWriter(string filename): filename(filename), counter(0) {
    }

    void writeArithmetic(const string& command) {
        cout << "// " << command << "\n";
        if (command == "add") {
            // pop first
            cout << "@SP" << "\n";
            cout << "A=M-1" << "\n";
            cout << "D=M" << "\n";

            // pop second
            cout << "A=A-1" << "\n";

            // ops
            cout << "M=M+D" << "\n";

            cout << "@SP" << "\n";
            cout << "M=M-1" << "\n";
        } else if (command == "sub") {
            cout << "@SP" << "\n";
            cout << "A=M-1" << "\n";
            cout << "D=M" << "\n";

            cout << "A=A-1" << "\n";
            cout << "M=M-D" << "\n";

            cout << "@SP" << "\n";
            cout << "M=M-1" << "\n";
        } else if (command == "and") {
            cout << "@SP" << "\n";
            cout << "A=M-1" << "\n";
            cout << "D=M" << "\n";

            cout << "A=A-1" << "\n";
            cout << "M=M&D" << "\n";

            cout << "@SP" << "\n";
            cout << "M=M-1" << "\n";
        } else if (command == "or") {
            cout << "@SP" << "\n";
            cout << "A=M-1" << "\n";
            cout << "D=M" << "\n";

            cout << "A=A-1" << "\n";
            cout << "M=M|D" << "\n";

            cout << "@SP" << "\n";
            cout << "M=M-1" << "\n";
        } else if (command == "eq") {
            cout << "@SP" << "\n";
            cout << "A=M-1" << "\n";
            cout << "D=M" << "\n";

            cout << "A=A-1" << "\n";
            cout << "D=M-D" << "\n";
            cout << "M=0" << "\n";

            cout << "@EQ_FALSE." + to_string(counter) << "\n";
            cout << "D;JNE" << "\n";
            cout << "@2" << "\n";
            cout << "D=A" << "\n";
            cout << "@SP" << "\n";
            cout << "A=M-D" << "\n";
            cout << "M=-1" << "\n";

            cout << "(EQ_FALSE." + to_string(counter) + ")" << "\n";
            cout << "@SP" << "\n";
            cout << "M=M-1" << "\n";
        } else if (command == "gt") {
            cout << "@SP" << "\n";
            cout << "A=M-1" << "\n";
            cout << "D=M" << "\n";

            cout << "A=A-1" << "\n";
            cout << "D=M-D" << "\n";
            cout << "M=-1" << "\n";

            cout << "@GT_FALSE." + to_string(counter) << "\n";
            cout << "D;JGT" << "\n";
            cout << "@2" << "\n";
            cout << "D=A" << "\n";
            cout << "@SP" << "\n";
            cout << "A=M-D" << "\n";
            cout << "M=0" << "\n";

            cout << "(GT_FALSE." + to_string(counter) + ")" << "\n";
            cout << "@SP" << "\n";
            cout << "M=M-1" << "\n";
        } else if (command == "lt") {
            cout << "@SP" << "\n";
            cout << "A=M-1" << "\n";
            cout << "D=M" << "\n";

            cout << "A=A-1" << "\n";
            cout << "D=M-D" << "\n";
            cout << "M=-1" << "\n";

            cout << "@LT_FALSE." + to_string(counter) << "\n";
            cout << "D;JLT" << "\n";
            cout << "@2" << "\n";
            cout << "D=A" << "\n";
            cout << "@SP" << "\n";
            cout << "A=M-D" << "\n";
            cout << "M=0" << "\n";

            cout << "(LT_FALSE." + to_string(counter) + ")" << "\n";
            cout << "@SP" << "\n";
            cout << "M=M-1" << "\n";
        } else if (command == "neg") {
            cout << "@SP" << "\n";
            cout << "A=M-1" << "\n";
            cout << "M=-M" << "\n";
        } else if (command == "not") {
            cout << "@SP" << "\n";
            cout << "A=M-1" << "\n";
            cout << "M=!M" << "\n";
        }

        counter++;
    }

    void writePushPop(int cType, string segment, int index) {
        cout << "// " << (cType == C_PUSH ? "push " : "pop ") << segment << " " << index << "\n";
        if (segment == "constant") {
            if (cType == C_PUSH) {
                cout << "@" + to_string(index) << "\n";
                cout << "D=A" << "\n";
                cout << "@SP" << "\n";
                cout << "AM=M+1" << "\n";
                cout << "A=A-1" << "\n";
                cout << "M=D" << "\n";
            } else {
                throw invalid_argument("pop constant not supported");
            }
        } else if (segment == "local"
                   || segment == "argument"
                   || segment == "this"
                   || segment == "that"
                   || segment == "temp"
        ) {
            string segmentPointer = getSegmentPointers(segment);
            if (cType == C_POP) {
                // compute segP+index
                cout << "@" << to_string(index) << "\n";
                cout << "D=A" << "\n";
                cout << "@" << segmentPointer << "\n";

                if (segment == "temp") {
                    // Base address is hard coded instead of stored in memory
                  cout << "D=A+D" << "\n";
                } else {
                  cout << "D=M+D" << "\n";
                }

                // store segP+index in R13
                cout << "@R13" << "\n";
                cout << "M=D" << "\n";

                // SP--
                cout << "@SP" << "\n";
                cout << "M=M-1" << "\n";

                // store *SP in D
                cout << "A=M" << "\n";
                cout << "D=M" << "\n";

                // *(segP+index) = *SP
                cout << "@R13" << "\n";
                cout << "A=M" << "\n";
                cout << "M=D" << "\n";
            } else {
                // compute segP+index
                cout << "@" + to_string(index) << "\n";
                cout << "D=A" << "\n";
                cout << "@" + segmentPointer << "\n";
                if (segment == "temp") {
                    // Base address is hard coded instead of stored in memory
                  cout << "A=A+D" << "\n";
                } else {
                  cout << "A=M+D" << "\n";
                }
                cout << "D=M" << "\n";

                // *SP = *(segP+index)
                cout << "@SP" << "\n";
                cout << "A=M" << "\n";
                cout << "M=D" << "\n";

                // SP++
                cout << "@SP" << "\n";
                cout << "M=M+1" << "\n";
            }
        } else if (segment == "static") {
            if (cType == C_POP) {
                // SP--
                cout << "@SP" << "\n";
                cout << "M=M-1" << "\n";

                // store *SP in D
                cout << "A=M" << "\n";
                cout << "D=M" << "\n";

                // *static.index = *SP
                cout << "@" << filename + "." + to_string(index) << "\n";
                cout << "M=D" << "\n";
            } else {
                // store *static.index in D
                cout << "@" << filename + "." + to_string(index) << "\n";
                cout << "D=M" << "\n";

                // *SP = D
                cout << "@SP" << "\n";
                cout << "A=M" << "\n";
                cout << "M=D" << "\n";

                // SP++
                cout << "@SP" << "\n";
                cout << "M=M+1" << "\n";
            }
        } else if (segment == "pointer") {
            string thisOrThat = getThisOrThat(index);
            if (cType == C_POP) {
                // SP--
                cout << "@SP" << "\n";
                cout << "M=M-1" << "\n";

                // THIS/THAT = *SP
                cout << "@SP" << "\n";
                cout << "A=M" << "\n";
                cout << "D=M" << "\n";
                cout << "@" << thisOrThat << "\n";
                cout << "M=D" << "\n";
            } else {
                // *SP = THIS/THAT
                cout << "@" << thisOrThat << "\n";
                cout << "D=M" << "\n";
                cout << "@SP" << "\n";
                cout << "A=M" << "\n";
                cout << "M=D" << "\n";

                // SP++
                cout << "@SP" << "\n";
                cout << "M=M+1" << "\n";
            }
        }
        counter++;
    }
};

// main
int main(int argc, char* argv[]) {
    if (argc <= 1) {
        cout << "./a.out <filename> ";
        return 0;
    }

    // Some crappy filename parsing
    string path = argv[1];
    size_t lastDot = path.find_last_of(".");
    size_t lastSlash = path.find_last_of("/");
    size_t start = 0;
    if (lastSlash == -1) {
        start = 0;
    } else {
        start = lastSlash+1;
    }

    // Phase 1 Parsing
    std::ifstream file(path);
    Parser p = Parser(file);
    string filename = path.substr(start, lastDot - start);
    CodeWriter cw(filename);

    while (p.hasMoreCommands()) {
        if (p.commandType() == C_ARITHMETIC) {
            cw.writeArithmetic(p.arg1());
        } else if (p.commandType() == C_PUSH || p.commandType() == C_POP) {
            cw.writePushPop(p.commandType(), p.arg1(), p.arg2());
        }
        p.advance();
    }
}
