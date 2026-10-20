
// Generated from /media/phnics/WorkData/native-emit/老项目重建/MyVM/assembler/MyVMAsm.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  MyVMAsmLexer : public antlr4::Lexer {
public:
  enum {
    HASH_SYMBOL = 1, HASH_CODE = 2, HASH_END = 3, DOT_CODE = 4, DOT_DATA = 5, 
    DOT_CONST = 6, DOT_BYTE = 7, DOT_WORD = 8, DOT_DWORD = 9, DOT_QWORD = 10, 
    DOT_ASCII = 11, DOT_SPACE = 12, DOT_ALIGN = 13, NUMBER = 14, IDENT = 15, 
    STRING = 16, DOLLAR = 17, COLON = 18, EQUAL = 19, COMMA = 20, HASH = 21, 
    PLUS = 22, MINUS = 23, STAR = 24, SLASH = 25, PERCENT = 26, TILDE = 27, 
    PIPE = 28, CARET = 29, AMP = 30, LSHIFT = 31, RSHIFT = 32, LPAREN = 33, 
    RPAREN = 34, NEWLINE = 35, LINE_COMMENT = 36, WS = 37, INVALID = 38
  };

  explicit MyVMAsmLexer(antlr4::CharStream *input);

  ~MyVMAsmLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

