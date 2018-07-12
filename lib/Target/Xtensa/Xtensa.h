//===-- Xtensa.h - Top-level interface for Xtensa representation ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "MCTargetDesc/XtensaMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Target/TargetIntrinsicInfo.h"

namespace llvm {
class XtensaRegisterBankInfo;
class XtensaSubtarget;
class XtensaTargetMachine;

FunctionPass *createXtensaISelDag(XtensaTargetMachine &TM,
                                 CodeGenOpt::Level OptLevel);

}

