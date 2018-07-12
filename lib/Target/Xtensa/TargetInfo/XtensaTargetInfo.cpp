//===-- XtensaTargetInfo.cpp - Xtensa Target Implementation ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Xtensa.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

namespace llvm {
Target &getTheXtensaTarget() {
  static Target TheXtensaTarget;
  return TheXtensaTarget;
}
} // namespace llvm

extern "C" void LLVMInitializeXtensaTargetInfo() {
  RegisterTarget<Triple::xtensa, false> Xtensa(getTheXtensaTarget(), "xtensa",
                                           "Xtensa LX6", "Xtensa");

}
