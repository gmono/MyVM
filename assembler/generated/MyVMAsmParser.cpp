
// Generated from /media/phnics/WorkData/native-emit/老项目重建/MyVM/assembler/MyVMAsm.g4 by ANTLR 4.13.2


#include "MyVMAsmVisitor.h"

#include "MyVMAsmParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct MyVMAsmParserStaticData final {
  MyVMAsmParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  MyVMAsmParserStaticData(const MyVMAsmParserStaticData&) = delete;
  MyVMAsmParserStaticData(MyVMAsmParserStaticData&&) = delete;
  MyVMAsmParserStaticData& operator=(const MyVMAsmParserStaticData&) = delete;
  MyVMAsmParserStaticData& operator=(MyVMAsmParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag myvmasmParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<MyVMAsmParserStaticData> myvmasmParserStaticData = nullptr;

void myvmasmParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (myvmasmParserStaticData != nullptr) {
    return;
  }
#else
  assert(myvmasmParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<MyVMAsmParserStaticData>(
    std::vector<std::string>{
      "program", "statement", "sectionDirective", "constantDefinition", 
      "legacyConstantDefinition", "labelDefinition", "instruction", "dataDirective", 
      "operandList", "operand", "expression", "bitwiseOr", "bitwiseXor", 
      "bitwiseAnd", "shiftExpr", "additiveExpr", "multiplicativeExpr", "unaryExpr", 
      "primary"
    },
    std::vector<std::string>{
      "", "'#symbol'", "'#code'", "'#end'", "'.code'", "'.data'", "'.const'", 
      "'.byte'", "'.word'", "'.dword'", "'.qword'", "'.ascii'", "'.space'", 
      "'.align'", "", "", "", "'$'", "':'", "'='", "','", "'#'", "'+'", 
      "'-'", "'*'", "'/'", "'%'", "'~'", "'|'", "'^'", "'&'", "'<<'", "'>>'", 
      "'('", "')'"
    },
    std::vector<std::string>{
      "", "HASH_SYMBOL", "HASH_CODE", "HASH_END", "DOT_CODE", "DOT_DATA", 
      "DOT_CONST", "DOT_BYTE", "DOT_WORD", "DOT_DWORD", "DOT_QWORD", "DOT_ASCII", 
      "DOT_SPACE", "DOT_ALIGN", "NUMBER", "IDENT", "STRING", "DOLLAR", "COLON", 
      "EQUAL", "COMMA", "HASH", "PLUS", "MINUS", "STAR", "SLASH", "PERCENT", 
      "TILDE", "PIPE", "CARET", "AMP", "LSHIFT", "RSHIFT", "LPAREN", "RPAREN", 
      "NEWLINE", "LINE_COMMENT", "WS", "INVALID"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,38,170,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,1,0,3,0,40,8,0,1,0,5,0,43,
  	8,0,10,0,12,0,46,9,0,1,0,3,0,49,8,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,3,
  	1,59,8,1,1,2,1,2,1,3,1,3,1,3,1,3,1,3,1,4,1,4,1,4,1,4,1,5,1,5,1,5,1,6,
  	1,6,3,6,77,8,6,1,7,1,7,1,7,1,7,3,7,83,8,7,3,7,85,8,7,1,8,1,8,3,8,89,8,
  	8,1,8,5,8,92,8,8,10,8,12,8,95,9,8,1,9,3,9,98,8,9,1,9,1,9,1,10,1,10,1,
  	11,1,11,1,11,5,11,107,8,11,10,11,12,11,110,9,11,1,12,1,12,1,12,5,12,115,
  	8,12,10,12,12,12,118,9,12,1,13,1,13,1,13,5,13,123,8,13,10,13,12,13,126,
  	9,13,1,14,1,14,1,14,5,14,131,8,14,10,14,12,14,134,9,14,1,15,1,15,1,15,
  	5,15,139,8,15,10,15,12,15,142,9,15,1,16,1,16,1,16,5,16,147,8,16,10,16,
  	12,16,150,9,16,1,17,5,17,153,8,17,10,17,12,17,156,9,17,1,17,1,17,1,18,
  	1,18,1,18,1,18,1,18,1,18,1,18,1,18,3,18,168,8,18,1,18,0,0,19,0,2,4,6,
  	8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,0,6,1,0,1,5,2,0,7,10,12,13,
  	1,0,31,32,1,0,22,23,1,0,24,26,2,0,22,23,27,27,174,0,44,1,0,0,0,2,58,1,
  	0,0,0,4,60,1,0,0,0,6,62,1,0,0,0,8,67,1,0,0,0,10,71,1,0,0,0,12,74,1,0,
  	0,0,14,84,1,0,0,0,16,86,1,0,0,0,18,97,1,0,0,0,20,101,1,0,0,0,22,103,1,
  	0,0,0,24,111,1,0,0,0,26,119,1,0,0,0,28,127,1,0,0,0,30,135,1,0,0,0,32,
  	143,1,0,0,0,34,154,1,0,0,0,36,167,1,0,0,0,38,40,3,2,1,0,39,38,1,0,0,0,
  	39,40,1,0,0,0,40,41,1,0,0,0,41,43,5,35,0,0,42,39,1,0,0,0,43,46,1,0,0,
  	0,44,42,1,0,0,0,44,45,1,0,0,0,45,48,1,0,0,0,46,44,1,0,0,0,47,49,3,2,1,
  	0,48,47,1,0,0,0,48,49,1,0,0,0,49,50,1,0,0,0,50,51,5,0,0,1,51,1,1,0,0,
  	0,52,59,3,4,2,0,53,59,3,6,3,0,54,59,3,8,4,0,55,59,3,10,5,0,56,59,3,14,
  	7,0,57,59,3,12,6,0,58,52,1,0,0,0,58,53,1,0,0,0,58,54,1,0,0,0,58,55,1,
  	0,0,0,58,56,1,0,0,0,58,57,1,0,0,0,59,3,1,0,0,0,60,61,7,0,0,0,61,5,1,0,
  	0,0,62,63,5,6,0,0,63,64,5,15,0,0,64,65,5,19,0,0,65,66,3,20,10,0,66,7,
  	1,0,0,0,67,68,5,15,0,0,68,69,5,18,0,0,69,70,3,20,10,0,70,9,1,0,0,0,71,
  	72,5,15,0,0,72,73,5,18,0,0,73,11,1,0,0,0,74,76,5,15,0,0,75,77,3,16,8,
  	0,76,75,1,0,0,0,76,77,1,0,0,0,77,13,1,0,0,0,78,79,5,11,0,0,79,85,5,16,
  	0,0,80,82,7,1,0,0,81,83,3,16,8,0,82,81,1,0,0,0,82,83,1,0,0,0,83,85,1,
  	0,0,0,84,78,1,0,0,0,84,80,1,0,0,0,85,15,1,0,0,0,86,93,3,18,9,0,87,89,
  	5,20,0,0,88,87,1,0,0,0,88,89,1,0,0,0,89,90,1,0,0,0,90,92,3,18,9,0,91,
  	88,1,0,0,0,92,95,1,0,0,0,93,91,1,0,0,0,93,94,1,0,0,0,94,17,1,0,0,0,95,
  	93,1,0,0,0,96,98,5,21,0,0,97,96,1,0,0,0,97,98,1,0,0,0,98,99,1,0,0,0,99,
  	100,3,20,10,0,100,19,1,0,0,0,101,102,3,22,11,0,102,21,1,0,0,0,103,108,
  	3,24,12,0,104,105,5,28,0,0,105,107,3,24,12,0,106,104,1,0,0,0,107,110,
  	1,0,0,0,108,106,1,0,0,0,108,109,1,0,0,0,109,23,1,0,0,0,110,108,1,0,0,
  	0,111,116,3,26,13,0,112,113,5,29,0,0,113,115,3,26,13,0,114,112,1,0,0,
  	0,115,118,1,0,0,0,116,114,1,0,0,0,116,117,1,0,0,0,117,25,1,0,0,0,118,
  	116,1,0,0,0,119,124,3,28,14,0,120,121,5,30,0,0,121,123,3,28,14,0,122,
  	120,1,0,0,0,123,126,1,0,0,0,124,122,1,0,0,0,124,125,1,0,0,0,125,27,1,
  	0,0,0,126,124,1,0,0,0,127,132,3,30,15,0,128,129,7,2,0,0,129,131,3,30,
  	15,0,130,128,1,0,0,0,131,134,1,0,0,0,132,130,1,0,0,0,132,133,1,0,0,0,
  	133,29,1,0,0,0,134,132,1,0,0,0,135,140,3,32,16,0,136,137,7,3,0,0,137,
  	139,3,32,16,0,138,136,1,0,0,0,139,142,1,0,0,0,140,138,1,0,0,0,140,141,
  	1,0,0,0,141,31,1,0,0,0,142,140,1,0,0,0,143,148,3,34,17,0,144,145,7,4,
  	0,0,145,147,3,34,17,0,146,144,1,0,0,0,147,150,1,0,0,0,148,146,1,0,0,0,
  	148,149,1,0,0,0,149,33,1,0,0,0,150,148,1,0,0,0,151,153,7,5,0,0,152,151,
  	1,0,0,0,153,156,1,0,0,0,154,152,1,0,0,0,154,155,1,0,0,0,155,157,1,0,0,
  	0,156,154,1,0,0,0,157,158,3,36,18,0,158,35,1,0,0,0,159,168,5,14,0,0,160,
  	168,5,15,0,0,161,162,5,17,0,0,162,168,5,15,0,0,163,164,5,33,0,0,164,165,
  	3,20,10,0,165,166,5,34,0,0,166,168,1,0,0,0,167,159,1,0,0,0,167,160,1,
  	0,0,0,167,161,1,0,0,0,167,163,1,0,0,0,168,37,1,0,0,0,18,39,44,48,58,76,
  	82,84,88,93,97,108,116,124,132,140,148,154,167
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  myvmasmParserStaticData = std::move(staticData);
}

}

MyVMAsmParser::MyVMAsmParser(TokenStream *input) : MyVMAsmParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

MyVMAsmParser::MyVMAsmParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  MyVMAsmParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *myvmasmParserStaticData->atn, myvmasmParserStaticData->decisionToDFA, myvmasmParserStaticData->sharedContextCache, options);
}

MyVMAsmParser::~MyVMAsmParser() {
  delete _interpreter;
}

const atn::ATN& MyVMAsmParser::getATN() const {
  return *myvmasmParserStaticData->atn;
}

std::string MyVMAsmParser::getGrammarFileName() const {
  return "MyVMAsm.g4";
}

const std::vector<std::string>& MyVMAsmParser::getRuleNames() const {
  return myvmasmParserStaticData->ruleNames;
}

const dfa::Vocabulary& MyVMAsmParser::getVocabulary() const {
  return myvmasmParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView MyVMAsmParser::getSerializedATN() const {
  return myvmasmParserStaticData->serializedATN;
}


//----------------- ProgramContext ------------------------------------------------------------------

MyVMAsmParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MyVMAsmParser::ProgramContext::EOF() {
  return getToken(MyVMAsmParser::EOF, 0);
}

std::vector<tree::TerminalNode *> MyVMAsmParser::ProgramContext::NEWLINE() {
  return getTokens(MyVMAsmParser::NEWLINE);
}

tree::TerminalNode* MyVMAsmParser::ProgramContext::NEWLINE(size_t i) {
  return getToken(MyVMAsmParser::NEWLINE, i);
}

std::vector<MyVMAsmParser::StatementContext *> MyVMAsmParser::ProgramContext::statement() {
  return getRuleContexts<MyVMAsmParser::StatementContext>();
}

MyVMAsmParser::StatementContext* MyVMAsmParser::ProgramContext::statement(size_t i) {
  return getRuleContext<MyVMAsmParser::StatementContext>(i);
}


size_t MyVMAsmParser::ProgramContext::getRuleIndex() const {
  return MyVMAsmParser::RuleProgram;
}


std::any MyVMAsmParser::ProgramContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitProgram(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::ProgramContext* MyVMAsmParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 0, MyVMAsmParser::RuleProgram);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(44);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(39);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 49150) != 0)) {
          setState(38);
          statement();
        }
        setState(41);
        match(MyVMAsmParser::NEWLINE); 
      }
      setState(46);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx);
    }
    setState(48);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 49150) != 0)) {
      setState(47);
      statement();
    }
    setState(50);
    match(MyVMAsmParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatementContext ------------------------------------------------------------------

MyVMAsmParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MyVMAsmParser::SectionDirectiveContext* MyVMAsmParser::StatementContext::sectionDirective() {
  return getRuleContext<MyVMAsmParser::SectionDirectiveContext>(0);
}

MyVMAsmParser::ConstantDefinitionContext* MyVMAsmParser::StatementContext::constantDefinition() {
  return getRuleContext<MyVMAsmParser::ConstantDefinitionContext>(0);
}

MyVMAsmParser::LegacyConstantDefinitionContext* MyVMAsmParser::StatementContext::legacyConstantDefinition() {
  return getRuleContext<MyVMAsmParser::LegacyConstantDefinitionContext>(0);
}

MyVMAsmParser::LabelDefinitionContext* MyVMAsmParser::StatementContext::labelDefinition() {
  return getRuleContext<MyVMAsmParser::LabelDefinitionContext>(0);
}

MyVMAsmParser::DataDirectiveContext* MyVMAsmParser::StatementContext::dataDirective() {
  return getRuleContext<MyVMAsmParser::DataDirectiveContext>(0);
}

MyVMAsmParser::InstructionContext* MyVMAsmParser::StatementContext::instruction() {
  return getRuleContext<MyVMAsmParser::InstructionContext>(0);
}


size_t MyVMAsmParser::StatementContext::getRuleIndex() const {
  return MyVMAsmParser::RuleStatement;
}


std::any MyVMAsmParser::StatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitStatement(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::StatementContext* MyVMAsmParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 2, MyVMAsmParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(58);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(52);
      sectionDirective();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(53);
      constantDefinition();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(54);
      legacyConstantDefinition();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(55);
      labelDefinition();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(56);
      dataDirective();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(57);
      instruction();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SectionDirectiveContext ------------------------------------------------------------------

MyVMAsmParser::SectionDirectiveContext::SectionDirectiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MyVMAsmParser::SectionDirectiveContext::HASH_SYMBOL() {
  return getToken(MyVMAsmParser::HASH_SYMBOL, 0);
}

tree::TerminalNode* MyVMAsmParser::SectionDirectiveContext::HASH_CODE() {
  return getToken(MyVMAsmParser::HASH_CODE, 0);
}

tree::TerminalNode* MyVMAsmParser::SectionDirectiveContext::HASH_END() {
  return getToken(MyVMAsmParser::HASH_END, 0);
}

tree::TerminalNode* MyVMAsmParser::SectionDirectiveContext::DOT_CODE() {
  return getToken(MyVMAsmParser::DOT_CODE, 0);
}

tree::TerminalNode* MyVMAsmParser::SectionDirectiveContext::DOT_DATA() {
  return getToken(MyVMAsmParser::DOT_DATA, 0);
}


size_t MyVMAsmParser::SectionDirectiveContext::getRuleIndex() const {
  return MyVMAsmParser::RuleSectionDirective;
}


std::any MyVMAsmParser::SectionDirectiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitSectionDirective(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::SectionDirectiveContext* MyVMAsmParser::sectionDirective() {
  SectionDirectiveContext *_localctx = _tracker.createInstance<SectionDirectiveContext>(_ctx, getState());
  enterRule(_localctx, 4, MyVMAsmParser::RuleSectionDirective);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(60);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 62) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstantDefinitionContext ------------------------------------------------------------------

MyVMAsmParser::ConstantDefinitionContext::ConstantDefinitionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MyVMAsmParser::ConstantDefinitionContext::DOT_CONST() {
  return getToken(MyVMAsmParser::DOT_CONST, 0);
}

tree::TerminalNode* MyVMAsmParser::ConstantDefinitionContext::IDENT() {
  return getToken(MyVMAsmParser::IDENT, 0);
}

tree::TerminalNode* MyVMAsmParser::ConstantDefinitionContext::EQUAL() {
  return getToken(MyVMAsmParser::EQUAL, 0);
}

MyVMAsmParser::ExpressionContext* MyVMAsmParser::ConstantDefinitionContext::expression() {
  return getRuleContext<MyVMAsmParser::ExpressionContext>(0);
}


size_t MyVMAsmParser::ConstantDefinitionContext::getRuleIndex() const {
  return MyVMAsmParser::RuleConstantDefinition;
}


std::any MyVMAsmParser::ConstantDefinitionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitConstantDefinition(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::ConstantDefinitionContext* MyVMAsmParser::constantDefinition() {
  ConstantDefinitionContext *_localctx = _tracker.createInstance<ConstantDefinitionContext>(_ctx, getState());
  enterRule(_localctx, 6, MyVMAsmParser::RuleConstantDefinition);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(62);
    match(MyVMAsmParser::DOT_CONST);
    setState(63);
    match(MyVMAsmParser::IDENT);
    setState(64);
    match(MyVMAsmParser::EQUAL);
    setState(65);
    expression();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LegacyConstantDefinitionContext ------------------------------------------------------------------

MyVMAsmParser::LegacyConstantDefinitionContext::LegacyConstantDefinitionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MyVMAsmParser::LegacyConstantDefinitionContext::IDENT() {
  return getToken(MyVMAsmParser::IDENT, 0);
}

tree::TerminalNode* MyVMAsmParser::LegacyConstantDefinitionContext::COLON() {
  return getToken(MyVMAsmParser::COLON, 0);
}

MyVMAsmParser::ExpressionContext* MyVMAsmParser::LegacyConstantDefinitionContext::expression() {
  return getRuleContext<MyVMAsmParser::ExpressionContext>(0);
}


size_t MyVMAsmParser::LegacyConstantDefinitionContext::getRuleIndex() const {
  return MyVMAsmParser::RuleLegacyConstantDefinition;
}


std::any MyVMAsmParser::LegacyConstantDefinitionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitLegacyConstantDefinition(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::LegacyConstantDefinitionContext* MyVMAsmParser::legacyConstantDefinition() {
  LegacyConstantDefinitionContext *_localctx = _tracker.createInstance<LegacyConstantDefinitionContext>(_ctx, getState());
  enterRule(_localctx, 8, MyVMAsmParser::RuleLegacyConstantDefinition);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(67);
    match(MyVMAsmParser::IDENT);
    setState(68);
    match(MyVMAsmParser::COLON);
    setState(69);
    expression();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LabelDefinitionContext ------------------------------------------------------------------

MyVMAsmParser::LabelDefinitionContext::LabelDefinitionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MyVMAsmParser::LabelDefinitionContext::IDENT() {
  return getToken(MyVMAsmParser::IDENT, 0);
}

tree::TerminalNode* MyVMAsmParser::LabelDefinitionContext::COLON() {
  return getToken(MyVMAsmParser::COLON, 0);
}


size_t MyVMAsmParser::LabelDefinitionContext::getRuleIndex() const {
  return MyVMAsmParser::RuleLabelDefinition;
}


std::any MyVMAsmParser::LabelDefinitionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitLabelDefinition(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::LabelDefinitionContext* MyVMAsmParser::labelDefinition() {
  LabelDefinitionContext *_localctx = _tracker.createInstance<LabelDefinitionContext>(_ctx, getState());
  enterRule(_localctx, 10, MyVMAsmParser::RuleLabelDefinition);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(71);
    match(MyVMAsmParser::IDENT);
    setState(72);
    match(MyVMAsmParser::COLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InstructionContext ------------------------------------------------------------------

MyVMAsmParser::InstructionContext::InstructionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MyVMAsmParser::InstructionContext::IDENT() {
  return getToken(MyVMAsmParser::IDENT, 0);
}

MyVMAsmParser::OperandListContext* MyVMAsmParser::InstructionContext::operandList() {
  return getRuleContext<MyVMAsmParser::OperandListContext>(0);
}


size_t MyVMAsmParser::InstructionContext::getRuleIndex() const {
  return MyVMAsmParser::RuleInstruction;
}


std::any MyVMAsmParser::InstructionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitInstruction(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::InstructionContext* MyVMAsmParser::instruction() {
  InstructionContext *_localctx = _tracker.createInstance<InstructionContext>(_ctx, getState());
  enterRule(_localctx, 12, MyVMAsmParser::RuleInstruction);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(74);
    match(MyVMAsmParser::IDENT);
    setState(76);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 8739012608) != 0)) {
      setState(75);
      operandList();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DataDirectiveContext ------------------------------------------------------------------

MyVMAsmParser::DataDirectiveContext::DataDirectiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MyVMAsmParser::DataDirectiveContext::DOT_ASCII() {
  return getToken(MyVMAsmParser::DOT_ASCII, 0);
}

tree::TerminalNode* MyVMAsmParser::DataDirectiveContext::STRING() {
  return getToken(MyVMAsmParser::STRING, 0);
}

tree::TerminalNode* MyVMAsmParser::DataDirectiveContext::DOT_BYTE() {
  return getToken(MyVMAsmParser::DOT_BYTE, 0);
}

tree::TerminalNode* MyVMAsmParser::DataDirectiveContext::DOT_WORD() {
  return getToken(MyVMAsmParser::DOT_WORD, 0);
}

tree::TerminalNode* MyVMAsmParser::DataDirectiveContext::DOT_DWORD() {
  return getToken(MyVMAsmParser::DOT_DWORD, 0);
}

tree::TerminalNode* MyVMAsmParser::DataDirectiveContext::DOT_QWORD() {
  return getToken(MyVMAsmParser::DOT_QWORD, 0);
}

tree::TerminalNode* MyVMAsmParser::DataDirectiveContext::DOT_SPACE() {
  return getToken(MyVMAsmParser::DOT_SPACE, 0);
}

tree::TerminalNode* MyVMAsmParser::DataDirectiveContext::DOT_ALIGN() {
  return getToken(MyVMAsmParser::DOT_ALIGN, 0);
}

MyVMAsmParser::OperandListContext* MyVMAsmParser::DataDirectiveContext::operandList() {
  return getRuleContext<MyVMAsmParser::OperandListContext>(0);
}


size_t MyVMAsmParser::DataDirectiveContext::getRuleIndex() const {
  return MyVMAsmParser::RuleDataDirective;
}


std::any MyVMAsmParser::DataDirectiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitDataDirective(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::DataDirectiveContext* MyVMAsmParser::dataDirective() {
  DataDirectiveContext *_localctx = _tracker.createInstance<DataDirectiveContext>(_ctx, getState());
  enterRule(_localctx, 14, MyVMAsmParser::RuleDataDirective);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(84);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case MyVMAsmParser::DOT_ASCII: {
        enterOuterAlt(_localctx, 1);
        setState(78);
        match(MyVMAsmParser::DOT_ASCII);
        setState(79);
        match(MyVMAsmParser::STRING);
        break;
      }

      case MyVMAsmParser::DOT_BYTE:
      case MyVMAsmParser::DOT_WORD:
      case MyVMAsmParser::DOT_DWORD:
      case MyVMAsmParser::DOT_QWORD:
      case MyVMAsmParser::DOT_SPACE:
      case MyVMAsmParser::DOT_ALIGN: {
        enterOuterAlt(_localctx, 2);
        setState(80);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 14208) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(82);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 8739012608) != 0)) {
          setState(81);
          operandList();
        }
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OperandListContext ------------------------------------------------------------------

MyVMAsmParser::OperandListContext::OperandListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<MyVMAsmParser::OperandContext *> MyVMAsmParser::OperandListContext::operand() {
  return getRuleContexts<MyVMAsmParser::OperandContext>();
}

MyVMAsmParser::OperandContext* MyVMAsmParser::OperandListContext::operand(size_t i) {
  return getRuleContext<MyVMAsmParser::OperandContext>(i);
}

std::vector<tree::TerminalNode *> MyVMAsmParser::OperandListContext::COMMA() {
  return getTokens(MyVMAsmParser::COMMA);
}

tree::TerminalNode* MyVMAsmParser::OperandListContext::COMMA(size_t i) {
  return getToken(MyVMAsmParser::COMMA, i);
}


size_t MyVMAsmParser::OperandListContext::getRuleIndex() const {
  return MyVMAsmParser::RuleOperandList;
}


std::any MyVMAsmParser::OperandListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitOperandList(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::OperandListContext* MyVMAsmParser::operandList() {
  OperandListContext *_localctx = _tracker.createInstance<OperandListContext>(_ctx, getState());
  enterRule(_localctx, 16, MyVMAsmParser::RuleOperandList);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(86);
    operand();
    setState(93);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 8740061184) != 0)) {
      setState(88);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == MyVMAsmParser::COMMA) {
        setState(87);
        match(MyVMAsmParser::COMMA);
      }
      setState(90);
      operand();
      setState(95);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OperandContext ------------------------------------------------------------------

MyVMAsmParser::OperandContext::OperandContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MyVMAsmParser::ExpressionContext* MyVMAsmParser::OperandContext::expression() {
  return getRuleContext<MyVMAsmParser::ExpressionContext>(0);
}

tree::TerminalNode* MyVMAsmParser::OperandContext::HASH() {
  return getToken(MyVMAsmParser::HASH, 0);
}


size_t MyVMAsmParser::OperandContext::getRuleIndex() const {
  return MyVMAsmParser::RuleOperand;
}


std::any MyVMAsmParser::OperandContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitOperand(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::OperandContext* MyVMAsmParser::operand() {
  OperandContext *_localctx = _tracker.createInstance<OperandContext>(_ctx, getState());
  enterRule(_localctx, 18, MyVMAsmParser::RuleOperand);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(97);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == MyVMAsmParser::HASH) {
      setState(96);
      match(MyVMAsmParser::HASH);
    }
    setState(99);
    expression();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionContext ------------------------------------------------------------------

MyVMAsmParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MyVMAsmParser::BitwiseOrContext* MyVMAsmParser::ExpressionContext::bitwiseOr() {
  return getRuleContext<MyVMAsmParser::BitwiseOrContext>(0);
}


size_t MyVMAsmParser::ExpressionContext::getRuleIndex() const {
  return MyVMAsmParser::RuleExpression;
}


std::any MyVMAsmParser::ExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitExpression(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::ExpressionContext* MyVMAsmParser::expression() {
  ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, getState());
  enterRule(_localctx, 20, MyVMAsmParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(101);
    bitwiseOr();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BitwiseOrContext ------------------------------------------------------------------

MyVMAsmParser::BitwiseOrContext::BitwiseOrContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<MyVMAsmParser::BitwiseXorContext *> MyVMAsmParser::BitwiseOrContext::bitwiseXor() {
  return getRuleContexts<MyVMAsmParser::BitwiseXorContext>();
}

MyVMAsmParser::BitwiseXorContext* MyVMAsmParser::BitwiseOrContext::bitwiseXor(size_t i) {
  return getRuleContext<MyVMAsmParser::BitwiseXorContext>(i);
}

std::vector<tree::TerminalNode *> MyVMAsmParser::BitwiseOrContext::PIPE() {
  return getTokens(MyVMAsmParser::PIPE);
}

tree::TerminalNode* MyVMAsmParser::BitwiseOrContext::PIPE(size_t i) {
  return getToken(MyVMAsmParser::PIPE, i);
}


size_t MyVMAsmParser::BitwiseOrContext::getRuleIndex() const {
  return MyVMAsmParser::RuleBitwiseOr;
}


std::any MyVMAsmParser::BitwiseOrContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitBitwiseOr(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::BitwiseOrContext* MyVMAsmParser::bitwiseOr() {
  BitwiseOrContext *_localctx = _tracker.createInstance<BitwiseOrContext>(_ctx, getState());
  enterRule(_localctx, 22, MyVMAsmParser::RuleBitwiseOr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(103);
    bitwiseXor();
    setState(108);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == MyVMAsmParser::PIPE) {
      setState(104);
      match(MyVMAsmParser::PIPE);
      setState(105);
      bitwiseXor();
      setState(110);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BitwiseXorContext ------------------------------------------------------------------

MyVMAsmParser::BitwiseXorContext::BitwiseXorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<MyVMAsmParser::BitwiseAndContext *> MyVMAsmParser::BitwiseXorContext::bitwiseAnd() {
  return getRuleContexts<MyVMAsmParser::BitwiseAndContext>();
}

MyVMAsmParser::BitwiseAndContext* MyVMAsmParser::BitwiseXorContext::bitwiseAnd(size_t i) {
  return getRuleContext<MyVMAsmParser::BitwiseAndContext>(i);
}

std::vector<tree::TerminalNode *> MyVMAsmParser::BitwiseXorContext::CARET() {
  return getTokens(MyVMAsmParser::CARET);
}

tree::TerminalNode* MyVMAsmParser::BitwiseXorContext::CARET(size_t i) {
  return getToken(MyVMAsmParser::CARET, i);
}


size_t MyVMAsmParser::BitwiseXorContext::getRuleIndex() const {
  return MyVMAsmParser::RuleBitwiseXor;
}


std::any MyVMAsmParser::BitwiseXorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitBitwiseXor(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::BitwiseXorContext* MyVMAsmParser::bitwiseXor() {
  BitwiseXorContext *_localctx = _tracker.createInstance<BitwiseXorContext>(_ctx, getState());
  enterRule(_localctx, 24, MyVMAsmParser::RuleBitwiseXor);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(111);
    bitwiseAnd();
    setState(116);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == MyVMAsmParser::CARET) {
      setState(112);
      match(MyVMAsmParser::CARET);
      setState(113);
      bitwiseAnd();
      setState(118);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BitwiseAndContext ------------------------------------------------------------------

MyVMAsmParser::BitwiseAndContext::BitwiseAndContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<MyVMAsmParser::ShiftExprContext *> MyVMAsmParser::BitwiseAndContext::shiftExpr() {
  return getRuleContexts<MyVMAsmParser::ShiftExprContext>();
}

MyVMAsmParser::ShiftExprContext* MyVMAsmParser::BitwiseAndContext::shiftExpr(size_t i) {
  return getRuleContext<MyVMAsmParser::ShiftExprContext>(i);
}

std::vector<tree::TerminalNode *> MyVMAsmParser::BitwiseAndContext::AMP() {
  return getTokens(MyVMAsmParser::AMP);
}

tree::TerminalNode* MyVMAsmParser::BitwiseAndContext::AMP(size_t i) {
  return getToken(MyVMAsmParser::AMP, i);
}


size_t MyVMAsmParser::BitwiseAndContext::getRuleIndex() const {
  return MyVMAsmParser::RuleBitwiseAnd;
}


std::any MyVMAsmParser::BitwiseAndContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitBitwiseAnd(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::BitwiseAndContext* MyVMAsmParser::bitwiseAnd() {
  BitwiseAndContext *_localctx = _tracker.createInstance<BitwiseAndContext>(_ctx, getState());
  enterRule(_localctx, 26, MyVMAsmParser::RuleBitwiseAnd);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(119);
    shiftExpr();
    setState(124);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == MyVMAsmParser::AMP) {
      setState(120);
      match(MyVMAsmParser::AMP);
      setState(121);
      shiftExpr();
      setState(126);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ShiftExprContext ------------------------------------------------------------------

MyVMAsmParser::ShiftExprContext::ShiftExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<MyVMAsmParser::AdditiveExprContext *> MyVMAsmParser::ShiftExprContext::additiveExpr() {
  return getRuleContexts<MyVMAsmParser::AdditiveExprContext>();
}

MyVMAsmParser::AdditiveExprContext* MyVMAsmParser::ShiftExprContext::additiveExpr(size_t i) {
  return getRuleContext<MyVMAsmParser::AdditiveExprContext>(i);
}

std::vector<tree::TerminalNode *> MyVMAsmParser::ShiftExprContext::LSHIFT() {
  return getTokens(MyVMAsmParser::LSHIFT);
}

tree::TerminalNode* MyVMAsmParser::ShiftExprContext::LSHIFT(size_t i) {
  return getToken(MyVMAsmParser::LSHIFT, i);
}

std::vector<tree::TerminalNode *> MyVMAsmParser::ShiftExprContext::RSHIFT() {
  return getTokens(MyVMAsmParser::RSHIFT);
}

tree::TerminalNode* MyVMAsmParser::ShiftExprContext::RSHIFT(size_t i) {
  return getToken(MyVMAsmParser::RSHIFT, i);
}


size_t MyVMAsmParser::ShiftExprContext::getRuleIndex() const {
  return MyVMAsmParser::RuleShiftExpr;
}


std::any MyVMAsmParser::ShiftExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitShiftExpr(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::ShiftExprContext* MyVMAsmParser::shiftExpr() {
  ShiftExprContext *_localctx = _tracker.createInstance<ShiftExprContext>(_ctx, getState());
  enterRule(_localctx, 28, MyVMAsmParser::RuleShiftExpr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(127);
    additiveExpr();
    setState(132);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == MyVMAsmParser::LSHIFT

    || _la == MyVMAsmParser::RSHIFT) {
      setState(128);
      _la = _input->LA(1);
      if (!(_la == MyVMAsmParser::LSHIFT

      || _la == MyVMAsmParser::RSHIFT)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(129);
      additiveExpr();
      setState(134);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AdditiveExprContext ------------------------------------------------------------------

MyVMAsmParser::AdditiveExprContext::AdditiveExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<MyVMAsmParser::MultiplicativeExprContext *> MyVMAsmParser::AdditiveExprContext::multiplicativeExpr() {
  return getRuleContexts<MyVMAsmParser::MultiplicativeExprContext>();
}

MyVMAsmParser::MultiplicativeExprContext* MyVMAsmParser::AdditiveExprContext::multiplicativeExpr(size_t i) {
  return getRuleContext<MyVMAsmParser::MultiplicativeExprContext>(i);
}

std::vector<tree::TerminalNode *> MyVMAsmParser::AdditiveExprContext::PLUS() {
  return getTokens(MyVMAsmParser::PLUS);
}

tree::TerminalNode* MyVMAsmParser::AdditiveExprContext::PLUS(size_t i) {
  return getToken(MyVMAsmParser::PLUS, i);
}

std::vector<tree::TerminalNode *> MyVMAsmParser::AdditiveExprContext::MINUS() {
  return getTokens(MyVMAsmParser::MINUS);
}

tree::TerminalNode* MyVMAsmParser::AdditiveExprContext::MINUS(size_t i) {
  return getToken(MyVMAsmParser::MINUS, i);
}


size_t MyVMAsmParser::AdditiveExprContext::getRuleIndex() const {
  return MyVMAsmParser::RuleAdditiveExpr;
}


std::any MyVMAsmParser::AdditiveExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitAdditiveExpr(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::AdditiveExprContext* MyVMAsmParser::additiveExpr() {
  AdditiveExprContext *_localctx = _tracker.createInstance<AdditiveExprContext>(_ctx, getState());
  enterRule(_localctx, 30, MyVMAsmParser::RuleAdditiveExpr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(135);
    multiplicativeExpr();
    setState(140);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(136);
        _la = _input->LA(1);
        if (!(_la == MyVMAsmParser::PLUS

        || _la == MyVMAsmParser::MINUS)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(137);
        multiplicativeExpr(); 
      }
      setState(142);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MultiplicativeExprContext ------------------------------------------------------------------

MyVMAsmParser::MultiplicativeExprContext::MultiplicativeExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<MyVMAsmParser::UnaryExprContext *> MyVMAsmParser::MultiplicativeExprContext::unaryExpr() {
  return getRuleContexts<MyVMAsmParser::UnaryExprContext>();
}

MyVMAsmParser::UnaryExprContext* MyVMAsmParser::MultiplicativeExprContext::unaryExpr(size_t i) {
  return getRuleContext<MyVMAsmParser::UnaryExprContext>(i);
}

std::vector<tree::TerminalNode *> MyVMAsmParser::MultiplicativeExprContext::STAR() {
  return getTokens(MyVMAsmParser::STAR);
}

tree::TerminalNode* MyVMAsmParser::MultiplicativeExprContext::STAR(size_t i) {
  return getToken(MyVMAsmParser::STAR, i);
}

std::vector<tree::TerminalNode *> MyVMAsmParser::MultiplicativeExprContext::SLASH() {
  return getTokens(MyVMAsmParser::SLASH);
}

tree::TerminalNode* MyVMAsmParser::MultiplicativeExprContext::SLASH(size_t i) {
  return getToken(MyVMAsmParser::SLASH, i);
}

std::vector<tree::TerminalNode *> MyVMAsmParser::MultiplicativeExprContext::PERCENT() {
  return getTokens(MyVMAsmParser::PERCENT);
}

tree::TerminalNode* MyVMAsmParser::MultiplicativeExprContext::PERCENT(size_t i) {
  return getToken(MyVMAsmParser::PERCENT, i);
}


size_t MyVMAsmParser::MultiplicativeExprContext::getRuleIndex() const {
  return MyVMAsmParser::RuleMultiplicativeExpr;
}


std::any MyVMAsmParser::MultiplicativeExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitMultiplicativeExpr(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::MultiplicativeExprContext* MyVMAsmParser::multiplicativeExpr() {
  MultiplicativeExprContext *_localctx = _tracker.createInstance<MultiplicativeExprContext>(_ctx, getState());
  enterRule(_localctx, 32, MyVMAsmParser::RuleMultiplicativeExpr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(143);
    unaryExpr();
    setState(148);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 117440512) != 0)) {
      setState(144);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 117440512) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(145);
      unaryExpr();
      setState(150);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryExprContext ------------------------------------------------------------------

MyVMAsmParser::UnaryExprContext::UnaryExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MyVMAsmParser::PrimaryContext* MyVMAsmParser::UnaryExprContext::primary() {
  return getRuleContext<MyVMAsmParser::PrimaryContext>(0);
}

std::vector<tree::TerminalNode *> MyVMAsmParser::UnaryExprContext::PLUS() {
  return getTokens(MyVMAsmParser::PLUS);
}

tree::TerminalNode* MyVMAsmParser::UnaryExprContext::PLUS(size_t i) {
  return getToken(MyVMAsmParser::PLUS, i);
}

std::vector<tree::TerminalNode *> MyVMAsmParser::UnaryExprContext::MINUS() {
  return getTokens(MyVMAsmParser::MINUS);
}

tree::TerminalNode* MyVMAsmParser::UnaryExprContext::MINUS(size_t i) {
  return getToken(MyVMAsmParser::MINUS, i);
}

std::vector<tree::TerminalNode *> MyVMAsmParser::UnaryExprContext::TILDE() {
  return getTokens(MyVMAsmParser::TILDE);
}

tree::TerminalNode* MyVMAsmParser::UnaryExprContext::TILDE(size_t i) {
  return getToken(MyVMAsmParser::TILDE, i);
}


size_t MyVMAsmParser::UnaryExprContext::getRuleIndex() const {
  return MyVMAsmParser::RuleUnaryExpr;
}


std::any MyVMAsmParser::UnaryExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitUnaryExpr(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::UnaryExprContext* MyVMAsmParser::unaryExpr() {
  UnaryExprContext *_localctx = _tracker.createInstance<UnaryExprContext>(_ctx, getState());
  enterRule(_localctx, 34, MyVMAsmParser::RuleUnaryExpr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(154);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 146800640) != 0)) {
      setState(151);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 146800640) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(156);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(157);
    primary();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PrimaryContext ------------------------------------------------------------------

MyVMAsmParser::PrimaryContext::PrimaryContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MyVMAsmParser::PrimaryContext::NUMBER() {
  return getToken(MyVMAsmParser::NUMBER, 0);
}

tree::TerminalNode* MyVMAsmParser::PrimaryContext::IDENT() {
  return getToken(MyVMAsmParser::IDENT, 0);
}

tree::TerminalNode* MyVMAsmParser::PrimaryContext::DOLLAR() {
  return getToken(MyVMAsmParser::DOLLAR, 0);
}

tree::TerminalNode* MyVMAsmParser::PrimaryContext::LPAREN() {
  return getToken(MyVMAsmParser::LPAREN, 0);
}

MyVMAsmParser::ExpressionContext* MyVMAsmParser::PrimaryContext::expression() {
  return getRuleContext<MyVMAsmParser::ExpressionContext>(0);
}

tree::TerminalNode* MyVMAsmParser::PrimaryContext::RPAREN() {
  return getToken(MyVMAsmParser::RPAREN, 0);
}


size_t MyVMAsmParser::PrimaryContext::getRuleIndex() const {
  return MyVMAsmParser::RulePrimary;
}


std::any MyVMAsmParser::PrimaryContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MyVMAsmVisitor*>(visitor))
    return parserVisitor->visitPrimary(this);
  else
    return visitor->visitChildren(this);
}

MyVMAsmParser::PrimaryContext* MyVMAsmParser::primary() {
  PrimaryContext *_localctx = _tracker.createInstance<PrimaryContext>(_ctx, getState());
  enterRule(_localctx, 36, MyVMAsmParser::RulePrimary);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(167);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case MyVMAsmParser::NUMBER: {
        enterOuterAlt(_localctx, 1);
        setState(159);
        match(MyVMAsmParser::NUMBER);
        break;
      }

      case MyVMAsmParser::IDENT: {
        enterOuterAlt(_localctx, 2);
        setState(160);
        match(MyVMAsmParser::IDENT);
        break;
      }

      case MyVMAsmParser::DOLLAR: {
        enterOuterAlt(_localctx, 3);
        setState(161);
        match(MyVMAsmParser::DOLLAR);
        setState(162);
        match(MyVMAsmParser::IDENT);
        break;
      }

      case MyVMAsmParser::LPAREN: {
        enterOuterAlt(_localctx, 4);
        setState(163);
        match(MyVMAsmParser::LPAREN);
        setState(164);
        expression();
        setState(165);
        match(MyVMAsmParser::RPAREN);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void MyVMAsmParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  myvmasmParserInitialize();
#else
  ::antlr4::internal::call_once(myvmasmParserOnceFlag, myvmasmParserInitialize);
#endif
}
