#include <fstream>
using namespace std;

class CompilationEngine {
private:
  JackTokenizer jt;
  ofstream& output;

  // helper
  void eatAndWriteToken(TokenTypes t) {
      output << jt.getXml(t) << "\n";
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

public:
  CompilationEngine(ifstream& in, ofstream& out): jt(in), output(out) {}

  void compileClass() {
      assert(jt.hasMoreTokens());
      assert(jt.keyWord() == CLASS);

      // class
      output << "<class>\n";
      eatAndWriteToken(KEYWORD);

      // class Name
      eatAndWriteToken(IDENTIFIER);

      // {
      eatAndWriteToken(SYMBOL);

      // classVarDec*
      compileClassVarDec();

      // subroutineDec*
      compileSubroutineDec();

      // }
      eatAndWriteToken(SYMBOL);
      output << "</class>\n";
  }

  /**
   * Contract of every compile(parsing) function is:
   * 1. Before enter the function, input is at first token.
   * 2. Before exit the function, input is at next token after last token.
   */
  void compileClassVarDec() {
      while (jt.keyWord() == STATIC || jt.keyWord() == FIELD) {
          output << "<classVarDec>\n";
          eatAndWriteToken(KEYWORD);

          // type
          if (jt.keyWord() == INT
              || jt.keyWord() == CHAR
              || jt.keyWord() == BOOLEAN
          ) {
              eatAndWriteToken(KEYWORD);
          } else {
              eatAndWriteToken(IDENTIFIER);
          }

          // varName
          eatAndWriteToken(IDENTIFIER);

          // , varName
          while (jt.tokenType() == SYMBOL && jt.symbol() == ',') {
              eatAndWriteToken(SYMBOL);
              eatAndWriteToken(IDENTIFIER);
          }

          // ;
          eatAndWriteToken(SYMBOL);

          output << "</classVarDec>\n";
      }
  }

  void compileSubroutineDec() {
      while (jt.keyWord() == CONSTRUCTOR
          || jt.keyWord() == FUNCTION
          || jt.keyWord() == METHOD
      ) {
          output << "<subroutineDec>\n";
          eatAndWriteToken(KEYWORD);

          // return type
          if (jt.keyWord() == VOID
              || jt.keyWord() == INT
              || jt.keyWord() == CHAR
              || jt.keyWord() == BOOLEAN
          ) {
              eatAndWriteToken(KEYWORD);
          } else {
              eatAndWriteToken(IDENTIFIER);
          }

          // subroutineName
          eatAndWriteToken(IDENTIFIER);

          // (
          eatAndWriteToken(SYMBOL);

          // parameterList
          compileParameterList();

          // )
          eatAndWriteToken(SYMBOL);

          compileSubroutineBody();

          output << "</subroutineDec>\n";
      }
  }

  void compileParameterList() {
      output << "<parameterList>\n";

      if (jt.keyWord() == INT
          || jt.keyWord() == CHAR
          || jt.keyWord() == BOOLEAN
      ) {
          eatAndWriteToken(KEYWORD);
      } else if (jt.tokenType() == IDENTIFIER) {
          eatAndWriteToken(IDENTIFIER);
      } else {
          output << "</parameterList>\n";
          return;
      }

      // varName
      eatAndWriteToken(IDENTIFIER);

      // , type varName
      while (jt.tokenType() == SYMBOL && jt.symbol() == ',') {
          eatAndWriteToken(SYMBOL);

          // type
          if (jt.keyWord() == INT
              || jt.keyWord() == CHAR
              || jt.keyWord() == BOOLEAN
          ) {
              eatAndWriteToken(KEYWORD);
          } else {
              eatAndWriteToken(IDENTIFIER);
          }

          // varName
          eatAndWriteToken(IDENTIFIER);
      }

      output << "</parameterList>\n";
  }

  void compileSubroutineBody() {
      output << "<subroutineBody>\n";

      // {
      eatAndWriteToken(SYMBOL);

      // varDec*
      compileVarDec();

      // statements
      compileStatements();

      // }
      eatAndWriteToken(SYMBOL);

      output << "</subroutineBody>\n";
  }

  void compileVarDec() {
      // var
      while (jt.keyWord() == VAR) {
          output << "<varDec>\n";
          eatAndWriteToken(KEYWORD);

          // type
          if (jt.keyWord() == INT
              || jt.keyWord() == CHAR
              || jt.keyWord() == BOOLEAN
          ) {
              eatAndWriteToken(KEYWORD);
          } else {
              eatAndWriteToken(IDENTIFIER);
          }

          // varName
          eatAndWriteToken(IDENTIFIER);

          // , varName
          while (jt.tokenType() == SYMBOL && jt.symbol() == ',') {
              eatAndWriteToken(SYMBOL);
              eatAndWriteToken(IDENTIFIER);
          }

          // ;
          eatAndWriteToken(SYMBOL);

          output << "</varDec>\n";
      }
  }

  void compileStatements() {
      output << "<statements>\n";
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
      output << "</statements>\n";
  }

  void compileLet() {
      output << "<letStatement>\n";
      eatAndWriteToken(KEYWORD);

      // varName
      eatAndWriteToken(IDENTIFIER);

      // [
      if (jt.tokenType() == SYMBOL && jt.symbol() == '[') {
          eatAndWriteToken(SYMBOL);

          // expression
          compileExpression();

          // ]
          eatAndWriteToken(SYMBOL);
      }

      // =
      eatAndWriteToken(SYMBOL);

      // expression
      compileExpression();

      // ;
      eatAndWriteToken(SYMBOL);

      output << "</letStatement>\n";
  }

  void compileIf() {
      output << "<ifStatement>\n";
      eatAndWriteToken(KEYWORD);

      // (
      eatAndWriteToken(SYMBOL);

      // expression
      compileExpression();

      // )
      eatAndWriteToken(SYMBOL);

      // {
      eatAndWriteToken(SYMBOL);

      // statements
      compileStatements();

      // }
      eatAndWriteToken(SYMBOL);

      if (jt.keyWord() == ELSE) {
          eatAndWriteToken(KEYWORD);

          // {
          eatAndWriteToken(SYMBOL);

          // statements
          compileStatements();

          // }
          eatAndWriteToken(SYMBOL);
      }

      output << "</ifStatement>\n";
  }

  void compileWhile() {
      output << "<whileStatement>\n";
      eatAndWriteToken(KEYWORD);

      // (
      eatAndWriteToken(SYMBOL);

      // expression
      compileExpression();

      // )
      eatAndWriteToken(SYMBOL);

      // {
      eatAndWriteToken(SYMBOL);

      // statements
      compileStatements();

      // }
      eatAndWriteToken(SYMBOL);

      output << "</whileStatement>\n";
  }

  void compileDo() {
      output << "<doStatement>\n";
      eatAndWriteToken(KEYWORD);

      // subroutineCall
      // subroutineName or (className | varName)
      eatAndWriteToken(IDENTIFIER);

      if (jt.tokenType() == SYMBOL && jt.symbol() == '.') {
          eatAndWriteToken(SYMBOL);
          // subroutineName
          eatAndWriteToken(IDENTIFIER);
      }

      // (
      eatAndWriteToken(SYMBOL);

      compileExpressionList();

      // )
      eatAndWriteToken(SYMBOL);

      //;
      eatAndWriteToken(SYMBOL);

      output << "</doStatement>\n";
  }

  void compileReturn() {
      output << "<returnStatement>\n";
      eatAndWriteToken(KEYWORD);

      // expression
      if (jt.tokenType() != SYMBOL) {
          compileExpression();
      }

      // ;
      eatAndWriteToken(SYMBOL);
      output << "</returnStatement>\n";
  }

  void compileExpression() {
      output << "<expression>\n";
      compileTerm();

      while (isOpSymbol(jt)) {
          eatAndWriteToken(SYMBOL);

          compileTerm();
      }
      output << "</expression>\n";
  }

  void compileTerm() {
      output << "<term>\n";
      if (jt.tokenType() == INT_CONST) {
          // intConstant
          eatAndWriteToken(INT_CONST);
      } else if (jt.tokenType() == STRING_CONST) {
          // stringConstant
          eatAndWriteToken(STRING_CONST);
      } else if (jt.tokenType() == KEYWORD) {
          // keywordConstant, true, false, null, this
          // (probably should do further check)
          eatAndWriteToken(KEYWORD);
      } else if (jt.tokenType() == SYMBOL && jt.symbol() == '(') {
          // (
          eatAndWriteToken(SYMBOL);

          compileExpression();

          // )
          eatAndWriteToken(SYMBOL);
      } else if (jt.tokenType() == SYMBOL && (jt.symbol() == '-' || jt.symbol() == '~')) {
          // unaryOp term
          eatAndWriteToken(SYMBOL);

          compileTerm();
      } else if (jt.tokenType() == IDENTIFIER) {
          // varName or subroutineCall
          string curToken = jt.getXml(IDENTIFIER);
          jt.advance();

          if (jt.tokenType() == SYMBOL && jt.symbol() == '.') {
              // Foo.Bar()
              output << curToken << "\n";
              eatAndWriteToken(SYMBOL);

              // subroutineName
              eatAndWriteToken(IDENTIFIER);

              // (
              eatAndWriteToken(SYMBOL);

              compileExpressionList();

              // )
              eatAndWriteToken(SYMBOL);
          } else if (jt.tokenType() == SYMBOL && jt.symbol() == '(') {
              output << curToken << "\n";

              // Foo()
              // subroutineName
              eatAndWriteToken(IDENTIFIER);

              // (
              eatAndWriteToken(SYMBOL);

              compileExpressionList();

              // )
              eatAndWriteToken(SYMBOL);
          } else if (jt.tokenType() == SYMBOL && jt.symbol() == '[') {
              output << curToken << "\n";

              // [
              eatAndWriteToken(SYMBOL);

              compileExpression();

              // ]
              eatAndWriteToken(SYMBOL);
          } else {
              // foo
              output << curToken << "\n";
          }
      }

      output << "</term>\n";
  }

  void compileExpressionList() {
      output << "<expressionList>\n";
      if (jt.tokenType() == SYMBOL
          && jt.symbol() != '-'
          && jt.symbol() != '~'
          && jt.symbol() != '('
      ) {
          // no expression
          output << "</expressionList>\n";
          return;
      }

      compileExpression();

      while (jt.tokenType() == SYMBOL && jt.symbol() == ',') {
          eatAndWriteToken(SYMBOL);

          compileExpression();
      }
      output << "</expressionList>\n";
  }
};
