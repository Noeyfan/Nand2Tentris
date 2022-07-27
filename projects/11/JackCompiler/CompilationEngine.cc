class CompilationEngine {
private:
  JackTokenizer jt;
  VMWriter vw;
  string className;

  SymbolTable classSt;
  SymbolTable fnSt;
  int label;

  // helper
  void eat(TokenTypes t) {
      // disable xml writing
      // cout << jt.getXml(t) << "\n";
      jt.advance();
  }

  bool isOpSymbol(JackTokenizer& jt) {
      char s = jt.symbol();
      return (jt.tokenType() == SYMBOL)
        && (s == '+' || s == '-' || s == '*' || s == '/'
            || s == '&' || s == '|' || s == '<'
            || s == '>' || s == '='
      );
  }

  SymbolTable& getSymbolTable(const string& id) {
      if (fnSt.kindOf(id) != -1) {
          return fnSt;
      }

      if (classSt.kindOf(id) != -1) {
          return classSt;
      }

      // undefined symbol
      assert(false);
  }

  bool hasSymbol(const string& id) {
      return (classSt.kindOf(id) != -1) || (fnSt.kindOf(id) != -1);
  }

  string getSymbolType(const string& id) {
      SymbolTable& st = getSymbolTable(id);
      return st.typeOf(id);
  }

  void writePushVar(string& id) {
      SymbolTable& st = getSymbolTable(id);
      int kind = st.kindOf(id);
      int index = st.indexOf(id);
      vw.writePush(kindsToSeg[kind], index);
  }

  void writePopVar(string& id) {
      SymbolTable& st = getSymbolTable(id);
      int kind = st.kindOf(id);
      int index = st.indexOf(id);
      vw.writePop(kindsToSeg[kind], index);
  }

  void writeOp(char op) {
      vw.writeArithmetic(ops[op]);
  }

  void writeUOp(char op) {
      vw.writeArithmetic(uops[op]);
  }

  string nextLabel() {
      return "L" + to_string(label++);
  }

public:
  CompilationEngine(
    ifstream& in,
    ofstream& out,
    string fileName): jt(in), vw(out), className(fileName), label(0) {}

  void compileClass() {
      assert(jt.hasMoreTokens());
      assert(jt.keyWord() == CLASS);

      // class
      vw.writeComment("class " + className) ;
      eat(KEYWORD);

      // class Name
      eat(IDENTIFIER);

      // {
      eat(SYMBOL);

      // classVarDec*
      compileClassVarDec();

      // subroutineDec*
      compileSubroutineDec();

      // }
      eat(SYMBOL);

      vw.writeComment("class " + className) ;
  }

  /**
   * Contract of every compile(parsing) function is:
   * 1. Before enter the function, input is at first token.
   * 2. Before exit the function, input is at next token after last token.
   */
  void compileClassVarDec() {
      while (jt.keyWord() == STATIC || jt.keyWord() == FIELD) {
          Kind kind = (jt.keyWord() == STATIC) ? KIND_STATIC : KIND_FIELD;
          eat(KEYWORD);

          // type
          string type = jt.stringVal();
          if (jt.keyWord() == INT
              || jt.keyWord() == CHAR
              || jt.keyWord() == BOOLEAN
          ) {
              eat(KEYWORD);
          } else {
              eat(IDENTIFIER);
          }

          // varName
          string name = jt.stringVal();
          eat(IDENTIFIER);

          vw.writeComment("classVarDec: " + name + ", " + type);
          classSt.define(name, type, kind);

          // , varName
          while (jt.tokenType() == SYMBOL && jt.symbol() == ',') {
              eat(SYMBOL);

              // varName
              name = jt.stringVal();
              vw.writeComment("classVarDec: " + name + ", " + type);
              classSt.define(name, type, kind);
              eat(IDENTIFIER);
          }

          // ;
          eat(SYMBOL);
      }
  }

  // TODO, complete me next
  void compileSubroutineDec() {
      while (jt.keyWord() == CONSTRUCTOR
          || jt.keyWord() == FUNCTION
          || jt.keyWord() == METHOD
      ) {
          int fnType = jt.keyWord();
          eat(KEYWORD);

          // return type
          if (jt.keyWord() == VOID
              || jt.keyWord() == INT
              || jt.keyWord() == CHAR
              || jt.keyWord() == BOOLEAN
          ) {
              eat(KEYWORD);
          } else {
              eat(IDENTIFIER);
          }

          string fn;
          if (fnType == CONSTRUCTOR) {
            fn = className + ".new";
          } else {
            fn = className + "." + jt.stringVal();
          }

          // subroutineName
          eat(IDENTIFIER);

          // (
          eat(SYMBOL);

          // parameterList
          compileParameterList(fnType);

          // )
          eat(SYMBOL);

          compileSubroutineBody(fn, fnType);

          // clear symbol table
          fnSt.clear();
      }
  }

  void compileParameterList(int fnType) {
      if (fnType == METHOD) {
          // this pointer, always of type <className>
          fnSt.define("this", className, KIND_ARG);
      }

      string type = jt.stringVal();
      if (jt.keyWord() == INT
          || jt.keyWord() == CHAR
          || jt.keyWord() == BOOLEAN
      ) {
          eat(KEYWORD);
      } else if (jt.tokenType() == IDENTIFIER) {
          eat(IDENTIFIER);
      } else {
          return;
      }

      // varName
      vw.writeComment("parameter: " + jt.stringVal() + ", " + type) ;
      fnSt.define(jt.stringVal(), type, KIND_ARG);
      eat(IDENTIFIER);

      // , type varName
      while (jt.tokenType() == SYMBOL && jt.symbol() == ',') {
          eat(SYMBOL);

          // type
          type = jt.stringVal();;
          if (jt.keyWord() == INT
              || jt.keyWord() == CHAR
              || jt.keyWord() == BOOLEAN
          ) {
              eat(KEYWORD);
          } else {
              eat(IDENTIFIER);
          }

          // varName
          vw.writeComment("parameter: " + jt.stringVal() + ", " + type) ;
          fnSt.define(jt.stringVal(), type, KIND_ARG);
          eat(IDENTIFIER);
      }
  }

  void compileSubroutineBody(const string& fn, int fnType) {
      // {
      eat(SYMBOL);

      // varDec*
      // Can only write function name after
      // figure out how many local variables
      compileVarDec();
      vw.writeFunction(fn, fnSt.varCount(KIND_VAR));

      if (fnType == CONSTRUCTOR) {
          int nFields = classSt.varCount(KIND_FIELD);
          vw.writePush(S_CONST, nFields);
          vw.writeAlloc();
          vw.writePop(S_POINTER, 0);
      } else if (fnType == METHOD) {
          vw.writePush(S_ARG, 0);
          vw.writePop(S_POINTER, 0);
      }

      // statements
      compileStatements();

      // }
      eat(SYMBOL);
  }

  void compileVarDec() {
      // var
      while (jt.keyWord() == VAR) {
          eat(KEYWORD);

          // type
          string type = jt.stringVal();
          if (jt.keyWord() == INT
              || jt.keyWord() == CHAR
              || jt.keyWord() == BOOLEAN
          ) {
              eat(KEYWORD);
          } else {
              eat(IDENTIFIER);
          }

          // varName
          vw.writeComment("var: " + jt.stringVal() + ", " + type) ;
          fnSt.define(jt.stringVal(), type, KIND_VAR);
          eat(IDENTIFIER);

          // , varName
          while (jt.tokenType() == SYMBOL && jt.symbol() == ',') {
              // ,
              eat(SYMBOL);

              // varName with same type
              vw.writeComment("var: " + jt.stringVal() + ", " + type) ;
              fnSt.define(jt.stringVal(), type, KIND_VAR);
              eat(IDENTIFIER);
          }

          // ;
          eat(SYMBOL);
      }
  }

  void compileStatements() {
      while (jt.keyWord() == LET
             || jt.keyWord() == IF
             || jt.keyWord() == WHILE
             || jt.keyWord() == DO
             || jt.keyWord() == RETURN
      ) {
          if (jt.keyWord() == LET) {
              compileLet();
          } else if (jt.keyWord() == IF) {
              compileIf();
          } else if (jt.keyWord() == WHILE) {
              compileWhile();
          } else if (jt.keyWord() == DO) {
              compileDo();
          } else {
              compileReturn();
          }
      }
  }

  void compileLet() {
      vw.writeComment("Compile let");
      eat(KEYWORD);

      // varName
      string id = jt.stringVal();
      eat(IDENTIFIER);

      // [
      bool isArray = false;
      if (jt.tokenType() == SYMBOL && jt.symbol() == '[') {
          isArray = true;

          eat(SYMBOL);

          // push arr (base addr)
          writePushVar(id);

          // expression
          compileExpression();

          // add (offset from expression)
          vw.writeArithmetic(A_ADD);

          // ]
          eat(SYMBOL);
      }

      // =
      eat(SYMBOL);

      // expression
      compileExpression();
      // let <id> = <expression>;
      vw.writeComment("let: " + id) ;

      if (isArray) {
          // pop THAT
          vw.writePop(S_TEMP, 0);
          vw.writePop(S_POINTER, 1);
          vw.writePush(S_TEMP, 0);
          vw.writePop(S_THAT, 0);
      } else {
          writePopVar(id);
      }

      // ;
      eat(SYMBOL);
  }

  void compileIf() {
      vw.writeComment("Compile if");
      eat(KEYWORD);

      string ifLabel = nextLabel();
      string elseLabel = nextLabel();

      // (
      eat(SYMBOL);

      // expression
      compileExpression();
      // not expression
      vw.writeArithmetic(A_NOT);

      // if-goto L1
      vw.writeIf(ifLabel);

      // )
      eat(SYMBOL);

      // {
      eat(SYMBOL);

      // statements
      compileStatements();

      // }
      eat(SYMBOL);

      // goto L2
      vw.writeGoto(elseLabel);
      vw.writeLabel(ifLabel);

      if (jt.keyWord() == ELSE) {
          eat(KEYWORD);

          // {
          eat(SYMBOL);

          // statements
          compileStatements();

          // }
          eat(SYMBOL);
      }
      // label L2
      vw.writeLabel(elseLabel);
  }

  void compileWhile() {
      vw.writeComment("Compile while");

      eat(KEYWORD);

      // insert while label
      string enterLabel = nextLabel();
      string exitLabel = nextLabel();
      vw.writeLabel(enterLabel);

      // (
      eat(SYMBOL);

      // expression
      compileExpression();

      // )
      eat(SYMBOL);

      // not expression for easier computation
      vw.writeArithmetic(A_NOT);

      // if-goto L2
      vw.writeIf(exitLabel);

      // {
      eat(SYMBOL);

      // statements
      compileStatements();

      // }
      eat(SYMBOL);

      // goto L1
      vw.writeGoto(enterLabel);

      // exit Label
      vw.writeLabel(exitLabel);
  }

  void compileDo() {
      eat(KEYWORD);

      // subroutineCall
      // subroutineName or (className | varName)
      string id = jt.stringVal();
      eat(IDENTIFIER);

      string subroutineFullName = "";
      int expressionCounts = 0;
      if (jt.tokenType() == SYMBOL && jt.symbol() == '.') {
          eat(SYMBOL);
          // subroutineName
          // Foo.bar()
          if (hasSymbol(id)) {
              // call method
              // push callee obj
              writePushVar(id);
              expressionCounts = 1;
              id = getSymbolType(id);
          }
          subroutineFullName = id + "." + jt.stringVal();
          eat(IDENTIFIER);
      } else {
          // bar()
          vw.writePush(S_POINTER, 0);
          expressionCounts = 1;
          subroutineFullName = className + "." + id;
      }

      // (
      eat(SYMBOL);

      expressionCounts += compileExpressionList();

      // )
      eat(SYMBOL);

      //;
      eat(SYMBOL);

      vw.writeComment("call: " + subroutineFullName + ", " + to_string(expressionCounts));
      vw.writeCall(subroutineFullName, expressionCounts);
      vw.writePop(S_TEMP, 0);
  }

  void compileReturn() {
      eat(KEYWORD);

      // expression
      if (jt.tokenType() != SYMBOL) {
          compileExpression();
      } else {
          // void function just push a dummy value
          vw.writePush(S_CONST, 0);
      }

      vw.writeComment("return");
      vw.writeReturn();

      // ;
      eat(SYMBOL);
  }

  void compileExpression() {
      compileTerm();

      while (isOpSymbol(jt)) {
          char op = jt.symbol();
          eat(SYMBOL);

          compileTerm();
          writeOp(op);
      }
  }

  void compileTerm() {
      if (jt.tokenType() == INT_CONST) {
          // intConstant
          vw.writePush(S_CONST, jt.intVal());
          eat(INT_CONST);
      } else if (jt.tokenType() == STRING_CONST) {
          // stringConstant
          string str = jt.stringVal();
          int size = str.size();
          // alloc string size
          vw.writePush(S_CONST, size);
          vw.writeCall("String.new", 1);

          // setting each character in memory
          for (int i = 0; i < size; ++i) {
              vw.writePush(S_CONST, (int)(str[i]));
              vw.writeCall("String.appendChar", 2);
          }

          eat(STRING_CONST);
      } else if (jt.tokenType() == KEYWORD) {
          // keywordConstant, true, false, null, this
          if (jt.keyWord() == TRUE) {
              // should we write -1 or 1 for true?
              vw.writePush(S_CONST, 0);
              vw.writeArithmetic(A_NOT);
          } else if (jt.keyWord() == FALSE) {
              vw.writePush(S_CONST, 0);
          } else if (jt.keyWord() == JNULL) {
              vw.writePush(S_CONST, 0);
          } else if (jt.keyWord() == THIS) {
              vw.writePush(S_POINTER, 0);
          } else {
              // unknown keyword
              assert(false);
          }
          eat(KEYWORD);
      } else if (jt.tokenType() == SYMBOL && jt.symbol() == '(') {
          // (
          // TODO
          eat(SYMBOL);

          compileExpression();

          // )
          eat(SYMBOL);
      } else if (jt.tokenType() == SYMBOL && (jt.symbol() == '-' || jt.symbol() == '~')) {
          // unaryOp term
          char op = jt.symbol();
          eat(SYMBOL);

          compileTerm();

          writeUOp(op);
      } else if (jt.tokenType() == IDENTIFIER) {
          // varName or subroutineCall
          string id = jt.stringVal();
          string curToken = jt.getXml(IDENTIFIER);
          jt.advance();

          bool isArray = false;
          int expressionCounts = 0;
          if (jt.tokenType() == SYMBOL && jt.symbol() == '.') {
              eat(SYMBOL);

              // Foo.bar()
              if (hasSymbol(id)) {
                  writePushVar(id);
                  expressionCounts = 1;
                  id = getSymbolType(id);
              }

              // subroutineName
              string subroutineFullName = id + "." + jt.stringVal();
              eat(IDENTIFIER);

              // (
              eat(SYMBOL);

              expressionCounts += compileExpressionList();

              // )
              eat(SYMBOL);

              vw.writeComment("call: " + subroutineFullName +  + ", " + to_string(expressionCounts));
              vw.writeCall(subroutineFullName, expressionCounts);
          } else if (jt.tokenType() == SYMBOL && jt.symbol() == '(') {
              // push this
              vw.writePush(S_POINTER, 0);
              expressionCounts = 1;

              // bar()
              // (
              eat(SYMBOL);

              expressionCounts += compileExpressionList();

              // )
              eat(SYMBOL);

              string subroutineFullName = className + "." + id;
              vw.writeComment("call: " + id + ", " + to_string(expressionCounts));
              vw.writeCall(id, expressionCounts);
          } else if (jt.tokenType() == SYMBOL && jt.symbol() == '[') {
              isArray = true;

              eat(SYMBOL);

              // push arr (base addr)
              writePushVar(id);

              // expression
              compileExpression();

              // add (offset from expression)
              vw.writeArithmetic(A_ADD);

              // set THAT
              vw.writePop(S_POINTER, 1);

              // push value onto stack
              vw.writePush(S_THAT, 0);

              // ]
              eat(SYMBOL);
          } else {
              // foo
              writePushVar(id);
          }
      }
  }

  int compileExpressionList() {
      int nExps = 0;
      if (jt.tokenType() == SYMBOL
          && jt.symbol() != '-'
          && jt.symbol() != '~'
          && jt.symbol() != '('
      ) {
          // no expression
          return nExps;
      }

      compileExpression();
      ++nExps;

      while (jt.tokenType() == SYMBOL && jt.symbol() == ',') {
          eat(SYMBOL);

          compileExpression();
          ++nExps;
      }

      return nExps;
  }
};
