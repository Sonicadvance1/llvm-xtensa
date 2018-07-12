//===-- XtensaInstPrinter.cpp - Convert Xtensa MCInst to asm syntax -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an Xtensa MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "XtensaInstPrinter.h"
#include "Xtensa.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

// Include the auto-generated portion of the assembly writer.
#define GET_INSTRUCTION_NAME
#include "XtensaGenAsmWriter.inc"

void XtensaInstPrinter::printInst(const MCInst *MI, raw_ostream &O,
                               StringRef Annot, const MCSubtargetInfo &STI) {
  printInstruction(MI, O);
  printAnnotation(O, Annot);
}

void XtensaInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                  raw_ostream &O, const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    O << getRegisterName(Op.getReg());
  } else if (Op.isImm()) {
    O << (int32_t)Op.getImm();
  } else {
    llvm_unreachable("XXX:");
  }
}

void XtensaInstPrinter::printMemOperand(const MCInst *MI, int OpNo, raw_ostream &O,
                                     const char *Modifier) {
  llvm_unreachable("XXX:");
}

void XtensaInstPrinter::printJumpTargetOperand(const MCInst *MI, unsigned OpNo,
                                  raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isImm()) {
    int32_t Imm = Op.getImm();
    O << ((Imm >= 0) ? "+" : "") << Imm;
  }
  else if (Op.isExpr()) {
    Op.getExpr()->print(O, &MAI);
  }
  else {
    O << Op;
  }
}

void XtensaInstPrinter::printUImm4Offset(const MCInst *MI, unsigned OpNum,
                                           unsigned Scale, raw_ostream &O) {
  const MCOperand MO = MI->getOperand(OpNum);
  if (MO.isImm()) {
    O << "#" << formatImm(MO.getImm() * Scale);
  } else {
    assert(MO.isExpr() && "Unexpected operand type!");
    MO.getExpr()->print(O, &MAI);
  }
}

void XtensaInstPrinter::printShiftImmOperand(const MCInst *MI, unsigned OpNum, raw_ostream &O) {
  const MCOperand MO = MI->getOperand(OpNum);
  if (MO.isImm()) {
    O << "#" << formatImm(32 - MO.getImm());
  } else {
    assert(MO.isExpr() && "Unexpected operand type!");
    MO.getExpr()->print(O, &MAI);
  }

}

