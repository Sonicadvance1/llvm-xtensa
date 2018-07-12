//===-- XtensaTargetMachine.h - Define TargetMachine for Xtensa --- C++ ---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Xtensa specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "XtensaSubtarget.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Analysis/TargetTransformInfo.h"

namespace llvm {
class XtensaTargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  XtensaSubtarget Subtarget;

public:
  XtensaTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                   StringRef FS, const TargetOptions &Options,
                   Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
                   CodeGenOpt::Level OL, bool JIT);

  const XtensaSubtarget *getSubtargetImpl() const { return &Subtarget; }
  const XtensaSubtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

};
}

