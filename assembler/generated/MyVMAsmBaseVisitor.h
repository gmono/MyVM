
// Generated from /media/phnics/WorkData/native-emit/老项目重建/MyVM/assembler/MyVMAsm.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "MyVMAsmVisitor.h"


/**
 * This class provides an empty implementation of MyVMAsmVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  MyVMAsmBaseVisitor : public MyVMAsmVisitor {
public:

  virtual std::any visitProgram(MyVMAsmParser::ProgramContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement(MyVMAsmParser::StatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSectionDirective(MyVMAsmParser::SectionDirectiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstantDefinition(MyVMAsmParser::ConstantDefinitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLegacyConstantDefinition(MyVMAsmParser::LegacyConstantDefinitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLabelDefinition(MyVMAsmParser::LabelDefinitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInstruction(MyVMAsmParser::InstructionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDataDirective(MyVMAsmParser::DataDirectiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperandList(MyVMAsmParser::OperandListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperand(MyVMAsmParser::OperandContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression(MyVMAsmParser::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitwiseOr(MyVMAsmParser::BitwiseOrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitwiseXor(MyVMAsmParser::BitwiseXorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitwiseAnd(MyVMAsmParser::BitwiseAndContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitShiftExpr(MyVMAsmParser::ShiftExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAdditiveExpr(MyVMAsmParser::AdditiveExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMultiplicativeExpr(MyVMAsmParser::MultiplicativeExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnaryExpr(MyVMAsmParser::UnaryExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimary(MyVMAsmParser::PrimaryContext *ctx) override {
    return visitChildren(ctx);
  }


};

