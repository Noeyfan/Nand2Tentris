class CompilationEngine {
private:
  JackTokenizer jt;
  VMWriter vw;
  SymbolTable classSt;
  SymbolTable fnSt;
  string className;

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
      if (classSt.kindOf(id) != -1) {
          return classSt;
      }

      if (fnSt.kindOf(id) != -1) {
          return fnSt;
      }

      // undefined symbol
      assert(false);
  }

  void writePushVar(string& id) {
      SymbolTable& st = getSymbolTable(id);
      int kind = st.kindOf(id);
      int index = st.indexOf(id);
      vw.writePush(kindsToSeg[kind], index);
  }

  void writeOp(char op) {
      vw.writeArithmetic(ops[op]);
  }

  void writeUOp(char op) {
      vw.writeArithmetic(uops[op]);
  }

public:
  CompilationEngine(
    ifstream& in,
    ofstream& out,
    string fileName): jt(in), vw(out), className(fileName) {}

  void compileClass() {
      assert(jt.hasMoreTokens());
      assert(jt.keyWord() == CLASS);

      // class
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

          cout << "class->";
          classSt.define(name, type, kind);

          // , varName
          while (jt.tokenType() == SYMBOL && jt.symbol() == ',') {
              eat(SYMBOL);
              eat(IDENTIFIER);
          }

          // ;
          eat(SYMBOL);
      }
  }

  void compileSubroutineDec() {
      while (jt.keyWord() == CONSTRUCTOR
          || jt.keyWord() == FUNCTION
          || jt.keyWord() == METHOD
      ) {
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

          // subroutineName
          vw.writeFunction(className + "." + jt.stringVal(), 0); // TODO nlocal
          eat(IDENTIFIER);

          // (
          eat(SYMBOL);

          // parameterList
          compileParameterList();

          // )
          eat(SYMBOL);

          compileSubroutineBody();

          // clear symbol table
          fnSt.clear();
      }
  }

  void compileParameterList() {
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
      cout << "fn->";
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
          cout << "fn->";
          fnSt.define(jt.stringVal(), type, KIND_ARG);
          eat(IDENTIFIER);
      }
  }

  void compileSubroutineBody() {
      // {
      eat(SYMBOL);

      // varDec*
      compileVarDec();

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
          cout << "fn->";
          fnSt.define(jt.stringVal(), type, KIND_VAR);
          eat(IDENTIFIER);

          // , varName
          while (jt.tokenType() == SYMBOL && jt.symbol() == ',') {
              // ,
              eat(SYMBOL);

              // varName with same type
              cout << "fn->";
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
      eat(KEYWORD);

      // varName
      cout << "using: " << jt.stringVal() << "\n";
      eat(IDENTIFIER);

      // [
      if (jt.tokenType() == SYMBOL && jt.symbol() == '[') {
          eat(SYMBOL);

          // expression
          compileExpression();

          // ]
          eat(SYMBOL);
      }

      // =
      eat(SYMBOL);

      // expression
      compileExpression();

      // ;
      eat(SYMBOL);
  }

  void compileIf() {
      eat(KEYWORD);

      // (
      eat(SYMBOL);

      // expression
      compileExpression();

      // )
      eat(SYMBOL);

      // {
      eat(SYMBOL);

      // statements
      compileStatements();

      // }
      eat(SYMBOL);

      if (jt.keyWord() == ELSE) {
          eat(KEYWORD);

          // {
          eat(SYMBOL);

          // statements
          compileStatements();

          // }
          eat(SYMBOL);
      }
  }

  void compileWhile() {
      eat(KEYWORD);

      // (
      eat(SYMBOL);

      // expression
      compileExpression();

      // )
      eat(SYMBOL);

      // {
      eat(SYMBOL);

      // statements
      compileStatements();

      // }
      eat(SYMBOL);
  }

  void compileDo() {
      eat(KEYWORD);

      // subroutineCall
      // subroutineName or (className | varName)
      string id = jt.stringVal();
      if (classSt.kindOf(id) != -1 || fnSt.kindOf(id) != -1) {
          cout << "using: " << id << "\n";
      }
      eat(IDENTIFIER);

      string subroutineFullName = id;
      if (jt.tokenType() == SYMBOL && jt.symbol() == '.') {
          eat(SYMBOL);
          // subroutineName
          subroutineFullName += ("." + jt.stringVal());
          eat(IDENTIFIER);
      }

      // (
      eat(SYMBOL);

      int expressionCounts = compileExpressionList();

      // )
      eat(SYMBOL);

      vw.writeCall(subroutineFullName, expressionCounts);

      //;
      eat(SYMBOL);
  }

  void compileReturn() {
      eat(KEYWORD);

      // expression
      if (jt.tokenType() != SYMBOL) {
          compileExpression();
      } else {
          vw.writeVoid();
      }

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
          // TODO
          eat(STRING_CONST);
      } else if (jt.tokenType() == KEYWORD) {
          // keywordConstant, true, false, null, this
          // (probably should do further check)
          // TODO
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

          if (jt.tokenType() == SYMBOL && jt.symbol() == '.') {
              // Foo.Bar()
              eat(SYMBOL);

              // subroutineName
              eat(IDENTIFIER);

              // (
              eat(SYMBOL);

              compileExpressionList();

              // )
              eat(SYMBOL);
          } else if (jt.tokenType() == SYMBOL && jt.symbol() == '(') {
              // Foo()
              // subroutineName
              eat(IDENTIFIER);

              // (
              eat(SYMBOL);

              compileExpressionList();

              // )
              eat(SYMBOL);
          } else if (jt.tokenType() == SYMBOL && jt.symbol() == '[') {
              // [
              eat(SYMBOL);

              compileExpression();

              // ]
              eat(SYMBOL);
          } else {
              // foo
              writePushVar(id);
              cout << "using: " << id << "\n";
          }
      }
  }

  int compileExpressionList() {
      int expressionCount = 0;
      if (jt.tokenType() == SYMBOL
          && jt.symbol() != '-'
          && jt.symbol() != '~'
          && jt.symbol() != '('
      ) {
          // no expression
          return expressionCount;
      }

      compileExpression();
      ++expressionCount;

      while (jt.tokenType() == SYMBOL && jt.symbol() == ',') {
          eat(SYMBOL);

          compileExpression();
          ++expressionCount;
      }

      return expressionCount;
  }
};
