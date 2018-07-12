//===-- XtensaMachineFuctionInfo.h - Xtensa machine function info -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares Xtensa-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

// Forward declarations
class Function;

/// XtensaFunctionInfo - This class is derived from MachineFunction private
/// Xtensa target-specific information for each MachineFunction.
class XtensaFunctionInfo : public MachineFunctionInfo {
public:
  XtensaFunctionInfo() {}

  ~XtensaFunctionInfo() {}
};
} // End llvm namespace
