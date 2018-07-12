//===-- XtensaTargetMachine.cpp - Define TargetMachine for Xtensa ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements the info about Xtensa target spec.
//
//===----------------------------------------------------------------------===//

#include "Xtensa.h"
#include "XtensaTargetMachine.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

extern "C" void LLVMInitializeXtensaTarget() {
  // Register the target.
  RegisterTargetMachine<XtensaTargetMachine> Z(getTheXtensaTarget());
}

// DataLayout: little or big endian
static std::string computeDataLayout(const Triple &TT) {
    return "e-m:e-p:32:32-i32:32-n32:32-S128";
}

static Reloc::Model getEffectiveRelocModel(Optional<Reloc::Model> RM) {
  if (!RM.hasValue())
    return Reloc::PIC_;
  return *RM;
}

static CodeModel::Model getEffectiveCodeModel(Optional<CodeModel::Model> CM) {
  if (CM)
    return *CM;
  return CodeModel::Small;
}

XtensaTargetMachine::XtensaTargetMachine(const Target &T, const Triple &TT,
                                   StringRef CPU, StringRef FS,
                                   const TargetOptions &Options,
                                   Optional<Reloc::Model> RM,
                                   Optional<CodeModel::Model> CM,
                                   CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(TT), TT, CPU, FS, Options,
                        getEffectiveRelocModel(RM), getEffectiveCodeModel(CM),
                        OL)
      , TLOF(make_unique<TargetLoweringObjectFileELF>())
      , Subtarget(TT, CPU, FS, *this)
{
  initAsmInfo();
}

namespace {
// Xtensa Code Generator Pass Configuration Options.
class XtensaPassConfig final : public TargetPassConfig {
public:
  XtensaPassConfig(XtensaTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  XtensaTargetMachine &getXtensaTargetMachine() const {
    return getTM<XtensaTargetMachine>();
  }

  bool addInstSelector() override;
};
}

TargetPassConfig *XtensaTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new XtensaPassConfig(*this, PM);
}

bool XtensaPassConfig::addInstSelector() {
  addPass(createXtensaISelDag(getXtensaTargetMachine(), getOptLevel()));
  return false;
}
