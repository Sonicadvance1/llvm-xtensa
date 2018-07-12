//===-- XtensaInstrInfo.cpp - Xtensa Instruction Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Xtensa implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "XtensaInstrInfo.h"
#include "Xtensa.h"
#include "XtensaSubtarget.h"
#include "MCTargetDesc/XtensaBaseInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/MC/MCContext.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "XtensaGenInstrInfo.inc"

using namespace llvm;

// Pin the vtable to this file.
void XtensaInstrInfo::anchor() {}

XtensaInstrInfo::XtensaInstrInfo(const XtensaSubtarget &STI)
  : XtensaGenInstrInfo(),
    RI(), Subtarget(STI) {
}

//===----------------------------------------------------------------------===//
// Branch Analysis
//===----------------------------------------------------------------------===//
//
/// AnalyzeBranch - Analyze the branching code at the end of MBB, returning
/// true if it cannot be understood (e.g. it's a switch dispatch or isn't
/// implemented for a target).  Upon success, this returns false and returns
/// with the following information in various cases:
///
/// 1. If this block ends with no branches (it just falls through to its succ)
///    just return false, leaving TBB/FBB null.
/// 2. If this block ends with only an unconditional branch, it sets TBB to be
///    the destination block.
/// 3. If this block ends with an conditional branch and it falls through to
///    an successor block, it sets TBB to be the branch destination block and a
///    list of operands that evaluate the condition. These
///    operands can be passed to other TargetInstrInfo methods to create new
///    branches.
/// 4. If this block ends with an conditional branch and an unconditional
///    block, it returns the 'true' destination in TBB, the 'false' destination
///    in FBB, and a list of operands that evaluate the condition. These
///    operands can be passed to other TargetInstrInfo methods to create new
///    branches.
///
/// Note that RemoveBranch and InsertBranch must be implemented to support
/// cases where this method returns success.
///
bool XtensaInstrInfo::analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                   MachineBasicBlock *&FBB,
                   SmallVectorImpl<MachineOperand> &Cond,
                   bool AllowModify) const {

  // Start from the bottom of the block and work upwards, examining the terminator instructions
  MachineBasicBlock::iterator I = MBB.getLastNonDebugInstr();

  // Are we already at the end? Early exist
  if (I == MBB.end())
    return false;

  if (!isUnpredicatedTerminator(*I))
    return false;

  while (I != MBB.begin()) {
    --I;

    // We don't care about debug values
    if (I->isDebugValue())
      continue;

    // When we see a non-terminator instruction, we're done
    if (!isUnpredicatedTerminator(*I))
      break;

    // A terminator that isn't a branch can't be handled here
    if (!I->isBranch())
      return true;

    // XXX: Check for unconditional branch here

    // Conditional Branch, don't handle these
    return true;
  }

  return true;
}

/// RemoveBranch - Remove the branching code at the end of the specific MBB.
/// This is only invoked in cases where AnalyzeBranch returns success. It
/// returns the number of instructions that were removed.
unsigned XtensaInstrInfo::removeBranch(MachineBasicBlock &MBB,
                      int *BytesRemoved) const {
  // XXX:
  llvm_unreachable("Unimplemented operand");
}

/// InsertBranch - Insert branch code into the end of the specified
/// MachineBasicBlock.  The operands to this method are the same as those
/// returned by AnalyzeBranch.  This is only invoked in cases where
/// AnalyzeBranch returns success. It returns the number of instructions
/// inserted.
///
/// It is also invoked by tail merging to add unconditional branches in
/// cases where AnalyzeBranch doesn't apply because there was no original
/// branch to analyze.  At least this much must be implemented, else tail
/// merging needs to be disabled.
unsigned XtensaInstrInfo::insertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                    MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond,
                      const DebugLoc &DL,
                      int *BytesAdded) const {
  // XXX:
  llvm_unreachable("Unimplemented operand");
}

void XtensaInstrInfo::copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                 const DebugLoc &DL, unsigned DestReg, unsigned SrcReg,
                 bool KillSrc) const {
  if (Xtensa::GPRRegClass.contains(DestReg, SrcReg)) {
    BuildMI(MBB, I, DL, get(Xtensa::MOV_r), DestReg)
      .addReg(SrcReg);
  }
  else {
    llvm_unreachable("Impossible reg-to-reg copy");
  }
}

void XtensaInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                         MachineBasicBlock::iterator I,
                                         unsigned SrcReg, bool isKill,
                                         int FrameIndex,
                                         const TargetRegisterClass *RC,
                                         const TargetRegisterInfo *TRI) const
{
  // XXX:
  llvm_unreachable("Unimplemented operand");
}

void XtensaInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator I,
                                          unsigned DestReg, int FrameIndex,
                                          const TargetRegisterClass *RC,
                                          const TargetRegisterInfo *TRI) const
{
  // XXX:
  llvm_unreachable("Unimplemented operand");
}

bool XtensaInstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
  switch (MI.getOpcode())
  {
  default:
    return false;
  }
}

void XtensaInstrInfo::insertNoop(MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator MI) const {
  // XXX:
  llvm_unreachable("Unimplemented operand");
}
bool XtensaInstrInfo::isCopyInstr(const MachineInstr &MI,
                                   const MachineOperand *&Src,
                                   const MachineOperand *&Dest) const {
  if (!MI.isMoveReg())
    return false;
  Dest = &MI.getOperand(0);
  Src = &MI.getOperand(1);
  return true;
}

