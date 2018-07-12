//===-- XtensaISelLowering.h - Xtensa DAG Lowering Interface ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Xtensa uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "Xtensa.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

// Forward delcarations
class XtensaSubtarget;
class XtensaTargetMachine;

namespace XtensaISD {
enum NodeType {
  // Start the numbering where the builtin ops and target ops leave off.
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  RET_FLAG,
  ENTRY_FLAG,
  BEQZ,
};
}

//===--------------------------------------------------------------------===//
// TargetLowering Implementation
//===--------------------------------------------------------------------===//
class XtensaTargetLowering : public TargetLowering {
public:
  explicit XtensaTargetLowering(const TargetMachine &TM, const XtensaSubtarget &STI);

  /// Selects the correct CCAssignFn for a given CallingConvention value.
  CCAssignFn *CCAssignFnForCall(CallingConv::ID CC, bool IsVarArg) const;

  /// Selects the correct CCAssignFn for a given CallingConvention value.
  CCAssignFn *CCAssignFnForReturn(CallingConv::ID CC) const;

  /// getTargetNodeName - This method returns the name of a target specific
  //  DAG node.
  virtual const char *getTargetNodeName(unsigned Opcode) const override;

  /// Provide custom lowering hooks for some operations.
  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

private:
  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool isVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;

  SDValue LowerBR_CC(SDValue Op, SelectionDAG &DAG) const;

  const XtensaSubtarget &Subtarget;
};
}
