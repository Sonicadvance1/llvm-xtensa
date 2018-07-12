//===-- XtensaISelLowering.cpp - Xtensa DAG Lowering Implementation ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the XtensaTargetLowering class.
//
//===----------------------------------------------------------------------===//

#include "XtensaISelLowering.h"
#include "Xtensa.h"
#include "XtensaMachineFunctionInfo.h"
#include "XtensaRegisterInfo.h"
#include "XtensaSubtarget.h"
#include "XtensaTargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "xtensa-isellowering"

/// Value type used for condition codes.
static const MVT MVT_CC = MVT::i32;

const char *XtensaTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  case XtensaISD::RET_FLAG: return "RET_FLAG";
  case XtensaISD::ENTRY_FLAG: return "ENTRY_FLAG";
  default: return nullptr;
  }
}

XtensaTargetLowering::XtensaTargetLowering(const TargetMachine &TM,
                                             const XtensaSubtarget &STI)
  : TargetLowering(TM)
  , Subtarget(STI) {

  // Set up the register classes.
  addRegisterClass(MVT::i32, &Xtensa::GPRRegClass);
  addRegisterClass(MVT::f32, &Xtensa::FPRRegClass);

  setOperationAction(ISD::BR_CC, MVT::i32, Custom);

  setStackPointerRegisterToSaveRestore(Xtensa::a1);

  // Compute derived properties from the register classes
  computeRegisterProperties(Subtarget.getRegisterInfo());
}

//===----------------------------------------------------------------------===//
//                      Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "XtensaGenCallingConv.inc"
/// Selects the correct CCAssignFn for a given CallingConvention value.
CCAssignFn *XtensaTargetLowering::CCAssignFnForCall(CallingConv::ID CC,
                                                     bool IsVarArg) const {
  assert(!IsVarArg && "We don't support variadic arguments");
  return CC_Xtensa;
}

CCAssignFn *
XtensaTargetLowering::CCAssignFnForReturn(CallingConv::ID CC) const {
  return RetCC_Xtensa;
}

SDValue XtensaTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {

  LLVM_DEBUG(llvm::dbgs() << "ISelDAG: Lowering formal arguments\n");
  if (isVarArg) {
    LLVM_DEBUG(llvm::errs() << "ISelDAG: Can't lower variadic arguments\n");
    return SDValue();
  }

  MachineFunction &MF = DAG.getMachineFunction();

  // Assign locations to all of the incoming arguments
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CCAssignFnForCall(CallConv, isVarArg));

  //MF.addLiveIn(Xtensa::a1, &Xtensa::GPRRegClass);

  // XXX: This constant probably isn't correct in all cases
  Chain = DAG.getNode(XtensaISD::ENTRY_FLAG, DL,
      MVT::Other,
      {Chain, DAG.getRegister(Xtensa::a1, MVT::i32), DAG.getTargetConstant(4, DL, MVT::i32)});

  assert(ArgLocs.size() == Ins.size());
  for (auto &VA : ArgLocs) {
    assert(VA.needsCustom() == false && "Doesn't support custom argument lowering");
    if (VA.isRegLoc()) {
      // Only support arguments passed in as registers
      EVT RegVT = VA.getLocVT();

      const TargetRegisterClass *RC;
      if (RegVT == MVT::i32)
        RC = &Xtensa::GPRRegClass;
      else if (RegVT == MVT::f32)
        RC = &Xtensa::FPRRegClass;
      else
        llvm_unreachable("Unhandled simple type");

      unsigned Reg = MF.addLiveIn(VA.getLocReg(), RC);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, Reg, RegVT);
      switch (VA.getLocInfo()) {
      default: llvm_unreachable("Unknown loc info!");
      case CCValAssign::Full: break;
      case CCValAssign::BCvt:
        ArgValue = DAG.getNode(ISD::BITCAST, DL, VA.getValVT(), ArgValue);
        break;
      }
      InVals.push_back(ArgValue);
    }
    else {
      llvm::errs() << "ISelDAG: Too many arguments passed to function\n";
      return SDValue();
    }
  }

  return Chain;
}
SDValue
XtensaTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::OutputArg> &Outs,
                               const SmallVectorImpl<SDValue> &OutVals,
                               const SDLoc &DL, SelectionDAG &DAG) const {
  unsigned Opc = XtensaISD::RET_FLAG;
  MachineFunction &MF = DAG.getMachineFunction();

  if (MF.getFunction().getReturnType()->isAggregateType()) {
    llvm::errs() << "ISelDAG: Can't return aggregate types\n";
    llvm_unreachable("Unhandled aggregate type");
  }

  // CCValAssign - represent the assignment of the return value to a physical location
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, RVLocs, *DAG.getContext());
  CCInfo.AnalyzeReturn(Outs, CCAssignFnForReturn(CallConv));

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the results values in to the output physical registers
  for (unsigned i = 0; i < RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    SDValue Arg = OutVals[i];

    switch (VA.getLocInfo()) {
    default: llvm_unreachable("Unknown loc info!");
    case CCValAssign::Full: break;
    case CCValAssign::BCvt:
        Arg = DAG.getNode(ISD::BITCAST, DL, VA.getLocVT(), Arg);
      break;
    }

    assert(VA.isRegLoc() && "Can only support return in registers");

    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Arg, Flag);

    // Ensures the emitted copies are together on return
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain; // Update the chain

  if (Flag.getNode())
    RetOps.push_back(Flag);

  return DAG.getNode(Opc, DL, MVT::Other, RetOps);
}

SDValue XtensaTargetLowering::LowerBR_CC(SDValue Op, SelectionDAG &DAG) const {
  SDValue Chain = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS = Op.getOperand(2);
  SDValue RHS = Op.getOperand(3);
  SDValue Dest = Op.getOperand(4);
  SDLoc dl(Op);

  if (LHS.getValueType().isInteger()) {
    const ConstantSDNode *RHSC = dyn_cast<ConstantSDNode>(RHS);
    if (RHSC && RHSC->getZExtValue() == 0) {
      if (CC == ISD::SETEQ) {
          return DAG.getNode(XtensaISD::BEQZ, dl, MVT::Other, Chain, LHS,
                             Dest);
      }
    }

    llvm_unreachable("XXX:");
  }

  llvm_unreachable("XXX:");
}

SDValue XtensaTargetLowering::LowerOperation(SDValue Op,
                                              SelectionDAG &DAG) const {
  LLVM_DEBUG(dbgs() << "Custom lowering: ");
  LLVM_DEBUG(Op.dump());
  switch (Op.getOpcode()) {
  case ISD::BR_CC:
    return LowerBR_CC(Op, DAG);
  default:
    llvm_unreachable("unimplemented operand");
    return SDValue();
  }
}
