// segments
string S_CONST = "constant";
string S_ARG = "arg";
string S_LOCAL = "local";
string S_STATIC = "static";
string S_THIS = "this";
string S_THAT = "that";
string S_POINTER = "pointer";
string S_TEMP = "temp";

// arithmetic
string A_ADD = "add";
string A_SUB = "sub";
string A_NEG = "neg";
string A_EQ = "eq";
string A_GT = "gt";
string A_LT = "lt";
string A_AND = "and";
string A_OR = "or";
string A_NOT = "not";

static unordered_map<char, string> ops = {
      {'+', A_ADD},
      {'-', A_SUB},
      {'=', A_EQ},
      {'>', A_GT},
      {'<', A_LT},
      {'&', A_AND},
      {'|', A_OR},
      {'*', "call Math.multiply 2"},
};

static unordered_map<char, string> uops = {
      {'~', A_NOT},
      {'-', A_NEG},
};

static unordered_map<int, string> kindsToSeg = {
      {KIND_STATIC, S_STATIC},
      {KIND_FIELD, S_THIS},
      {KIND_ARG, S_ARG},
      {KIND_VAR, S_LOCAL},
};

class VMWriter {
private:
  ofstream& output;

public:
    VMWriter(ofstream& output): output(output) { }

    void writePush(const string& segment, int index) {
        output << "push " << segment << " " << index << "\n";
    }

    void writePop(const string& segment, int index) {
        output << "pop " << segment << " " << index << "\n";
    }

    void writeArithmetic(const string& ari) {
        output << ari << "\n";
    }

    void writeLabel(const string& label) {
        output << "label "<< label << "\n";
    }

    void writeGoto(const string& label) {
        output << "goto "<< label << "\n";
    }

    void writeIf(const string& label) {
        output << "if-goto "<< label << "\n";
    }

    void writeCall(const string& name, int nArgs) {
        output << "call "<< name << " " << nArgs << "\n";
    }

    void writeFunction(const string& name, int nLocals) {
        output << "function " << name << " " << nLocals << "\n";
    }

    void writeReturn() {
        output << "return\n";
    }

    void writeVoid() {
        output << "pop temp 0\n";
        output << "push constant 0\n";
    }

    void close() {
        output.close();
    }
};
