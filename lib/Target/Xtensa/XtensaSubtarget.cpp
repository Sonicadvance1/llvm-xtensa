//===-- XtensaSubtarget.cpp - Xtensa Subtarget Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Xtensa specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "XtensaSubtarget.h"
#include "Xtensa.h"
#include "llvm/Support/TargetRegistry.h"

#define DEBUG_TYPE "lx6-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "XtensaGenSubtargetInfo.inc"

using namespace llvm;

XtensaSubtarget &
XtensaSubtarget::initializeSubtargetDependencies(StringRef FS,
                                                  StringRef CPUString) {
  // Determine default and user-specified characteristics

  ParseSubtargetFeatures(CPUString, FS);

  return *this;
}

void XtensaSubtarget::anchor() {}

XtensaSubtarget::XtensaSubtarget(const Triple &TT, StringRef CPU, StringRef FS,
                           TargetMachine &TM)
    : XtensaGenSubtargetInfo(TT, CPU, FS),
      DL("e-m:e-p:32:32-i1:8:32-i8:8:32-i16:16:32-i64:32-f64:32-a:0:32-n32"),
      InstrInfo(initializeSubtargetDependencies(FS, CPU)), TLInfo(TM, *this), FrameLowering() {
}

