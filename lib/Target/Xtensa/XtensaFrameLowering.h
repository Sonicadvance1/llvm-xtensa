//===-- XtensaFrameLowering.h - Frame info for Xtensa Target ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains Xtensa frame information that doesn't fit anywhere else
// cleanly...
//
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {
class XtensaSubtarget;

class XtensaFrameLowering : public TargetFrameLowering {
public:
  XtensaFrameLowering();

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF,
                    MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator I) const override;

  bool hasFP(const MachineFunction &MF) const override;

  //! Stack slot size (4 bytes)
  static int stackSlotSize() { return 4; }

private:
  uint64_t computeStackSize(MachineFunction &MF) const;
};
}
