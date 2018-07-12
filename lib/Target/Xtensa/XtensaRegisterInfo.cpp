//===-- XtensaRegisterInfo.cpp - Xtensa Register Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Xtensa implementation of the MRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "XtensaRegisterInfo.h"
#include "Xtensa.h"
#include "XtensaFrameLowering.h"
#include "XtensaInstrInfo.h"
#include "XtensaMachineFunctionInfo.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#define GET_REGINFO_TARGET_DESC
#include "XtensaGenRegisterInfo.inc"

using namespace llvm;

XtensaRegisterInfo::XtensaRegisterInfo() : XtensaGenRegisterInfo(Xtensa::INVALID) {}

const uint16_t *
XtensaRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
    return CSR_Xtensa_SaveList;
}

BitVector XtensaRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  return Reserved;
}

const uint32_t *XtensaRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                                      CallingConv::ID) const {
  return 0;
}

bool
XtensaRegisterInfo::requiresRegisterScavenging(const MachineFunction &MF) const {
  return true;
}

bool
XtensaRegisterInfo::trackLivenessAfterRegAlloc(const MachineFunction &MF) const {
  return true;
}

bool XtensaRegisterInfo::useFPForScavengingIndex(const MachineFunction &MF) const {
  return false;
}

void XtensaRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj, unsigned FIOperandNum,
                                          RegScavenger *RS) const {
  MachineInstr &MI = *II;

  // Determine if we can eliminate the index from this kind of instruction.
  switch (MI.getOpcode()) {
  default:
    // Not supported yet.
    llvm_unreachable("Couldn't reach here");
    return;
  }
}

unsigned XtensaRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return Xtensa::INVALID;
}

