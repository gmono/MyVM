
// Generated from /media/phnics/WorkData/native-emit/老项目重建/MyVM/assembler/MyVMAsm.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "MyVMAsmParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by MyVMAsmParser.
 */
class  MyVMAsmVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by MyVMAsmParser.
   */
    virtual std::any visitProgram(MyVMAsmParser::ProgramContext *context) = 0;

    virtual std::any visitStatement(MyVMAsmParser::StatementContext *context) = 0;

    virtual std::any visitSectionDirective(MyVMAsmParser::SectionDirectiveContext *context) = 0;

    virtual std::any visitConstantDefinition(MyVMAsmParser::ConstantDefinitionContext *context) = 0;

    virtual std::any visitLegacyConstantDefinition(MyVMAsmParser::LegacyConstantDefinitionContext *context) = 0;

    virtual std::any visitLabelDefinition(MyVMAsmParser::LabelDefinitionContext *context) = 0;

    virtual std::any visitInstruction(MyVMAsmParser::InstructionContext *context) = 0;

    virtual std::any visitDataDirective(MyVMAsmParser::DataDirectiveContext *context) = 0;

    virtual std::any visitOperandList(MyVMAsmParser::OperandListContext *context) = 0;

    virtual std::any visitOperand(MyVMAsmParser::OperandContext *context) = 0;

    virtual std::any visitExpression(MyVMAsmParser::ExpressionContext *context) = 0;

    virtual std::any visitBitwiseOr(MyVMAsmParser::BitwiseOrContext *context) = 0;

    virtual std::any visitBitwiseXor(MyVMAsmParser::BitwiseXorContext *context) = 0;

    virtual std::any visitBitwiseAnd(MyVMAsmParser::BitwiseAndContext *context) = 0;

    virtual std::any visitShiftExpr(MyVMAsmParser::ShiftExprContext *context) = 0;

    virtual std::any visitAdditiveExpr(MyVMAsmParser::AdditiveExprContext *context) = 0;

    virtual std::any visitMultiplicativeExpr(MyVMAsmParser::MultiplicativeExprContext *context) = 0;

    virtual std::any visitUnaryExpr(MyVMAsmParser::UnaryExprContext *context) = 0;

    virtual std::any visitPrimary(MyVMAsmParser::PrimaryContext *context) = 0;


};

