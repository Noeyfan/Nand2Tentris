#include <fstream>
#include <unordered_map>
#include <unordered_set>
using namespace std;
// enums
enum KeyWords {
    CLASS, METHOD, FUNCTION, CONSTRUCTOR, INT, BOOLEAN, CHAR, VOID, VAR, STATIC,
    FIELD, LET, DO, IF, ELSE, WHILE, RETURN, TRUE, FALSE, JNULL, THIS
};

enum TokenTypes {
    KEYWORD,
    SYMBOL,
    IDENTIFIER,
    INT_CONST,
    STRING_CONST,

    // bypass check
    NO_CHECK,
};

static unordered_map<int, string> keywordsRevrse = {
      {KeyWords::CLASS,       "class"},
      {KeyWords::CONSTRUCTOR, "constructor"},
      {KeyWords::FUNCTION,    "function"},
      {KeyWords::METHOD,      "method"},
      {KeyWords::FIELD,       "field"},
      {KeyWords::STATIC,      "static"},
      {KeyWords::VAR,         "var"},
      {KeyWords::INT,         "int"},
      {KeyWords::CHAR,        "char"},
      {KeyWords::BOOLEAN,     "boolean"},
      {KeyWords::VOID,        "void"},
      {KeyWords::TRUE,        "true"},
      {KeyWords::FALSE,       "false"},
      {KeyWords::JNULL,       "null"},
      {KeyWords::THIS,        "this"},
      {KeyWords::LET,         "let"},
      {KeyWords::DO,          "do"},
      {KeyWords::IF,          "if"},
      {KeyWords::ELSE,        "else"},
      {KeyWords::WHILE,       "while"},
      {KeyWords::RETURN,      "return"},
};

class JackTokenizer {
private:
  // static
  unordered_map<string, KeyWords> keywords = {
        {"class", KeyWords::CLASS},
        {"constructor", KeyWords::CONSTRUCTOR},
        {"function", KeyWords::FUNCTION},
        {"method", KeyWords::METHOD},
        {"field", KeyWords::FIELD},
        {"static", KeyWords::STATIC},
        {"var", KeyWords::VAR},
        {"int", KeyWords::INT},
        {"char", KeyWords::CHAR},
        {"boolean", KeyWords::BOOLEAN},
        {"void", KeyWords::VOID},
        {"true", KeyWords::TRUE},
        {"false", KeyWords::FALSE},
        {"null", KeyWords::JNULL},
        {"this", KeyWords::THIS},
        {"let", KeyWords::LET},
        {"do", KeyWords::DO},
        {"if", KeyWords::IF},
        {"else", KeyWords::ELSE},
        {"while", KeyWords::WHILE},
        {"return", KeyWords::RETURN},
  };

  unordered_set<char> symbols = {
      '{', '}', '(', ')', '[', ']', '.', ',', ';', '+',
      '-', '*', '/', '&', '|', '<', '>', '=', '~'
  };

  char identifierConcat = '_';
  char stringBegin = '"', stringEnd = '"';

  // member
  ifstream& file;
  string token;
  TokenTypes tokenT;
  bool moreToken;

  // helper
  void consume(ifstream& file, string& token) {
      char c = file.get();
      token += c;
  }

  string escapeSymbol(char c) {
      if (c == '<') return "&lt;";
      if (c == '>') return "&gt;";
      if (c == '&') return "&amp;";
      string result = "";
      result += c;
      return result;
  }

public:
  JackTokenizer(ifstream& file):
    file(file),
    token("") {
        advance();
    }

  bool hasMoreTokens() {
      return moreToken;
  }

  void advance() {
      token = "";
      moreToken = false;
      string discard;

      while(file) {
          char c = file.peek();
          if (isalpha(c) || c == identifierConcat) {
              consume(file, token);

              while(isalpha(file.peek())
                    || isdigit(file.peek())
                    || file.peek() == identifierConcat) {
                  consume(file, token);
              }

              tokenT = keywords.contains(token) ? KEYWORD: IDENTIFIER;
              moreToken = true;
              return;
          } else if (isdigit(c)) {
              consume(file, token);

              while(isdigit(file.peek())) {
                  consume(file, token);
              }

              tokenT = TokenTypes::INT_CONST;
              moreToken = true;
              return;
          } else if (c == stringBegin) {
              file.get(); // skip "

              while(file.peek() != stringEnd) {
                  if (c == '\n') {
                      file.get(); // skip newline
                      continue;
                  }
                  consume(file, token);
              }
              file.get(); // skip "

              tokenT = TokenTypes::STRING_CONST;
              moreToken = true;
              return;
          } else if (symbols.contains(c)) {
              consume(file, token);

              if (c == '/' && file.peek() == '/') {
                  getline(file, discard);
                  token = "";
                  continue;
              } else if (c == '/' && file.peek() == '*') {
                  while(true) {
                      c = file.get();
                      if (c == '*' && file.peek() == '/') {
                          file.get();
                          break;
                      }
                  }
                  token = "";
                  continue;
              }

              tokenT = SYMBOL;
              moreToken = true;
              return;
          } else {
              file.get(); // skip unknown character
          }
      }
  }

  int tokenType() {
      return tokenT;
  }

  int keyWord() {
      if (keywords.contains(token)) return keywords[token];
      return -1;
  }

  char symbol() {
      return token[0];
  }

  string identifier() {
      return token;
  }

  int intVal() {
      return stoi(token);
  }

  string stringVal() {
      return token;
  }

  string getXml(TokenTypes type) {
      if (type != NO_CHECK && type != tokenType()) {
          cerr << "expected: "<< type << ", " << "actual: " << tokenType()  <<"\n";
          cerr << "token xml: " << getXml(NO_CHECK) <<"\n";
          assert(false);
      }

      switch(type) {
        case KEYWORD:
          return "<keyword>" + stringVal() + "</keyword>";
        case SYMBOL:
          return "<symbol>" + escapeSymbol(symbol()) + "</symbol>";
        case IDENTIFIER:
          return "<identifier>" + identifier() + "</identifier>";
        case INT_CONST:
          return "<integerConstant>" + stringVal() + "</integerConstant>";
        case STRING_CONST:
          return "<stringConstant>" + stringVal() + "</stringConstant>";
        case NO_CHECK:
          return getXml(tokenT);
      }
  }
};
