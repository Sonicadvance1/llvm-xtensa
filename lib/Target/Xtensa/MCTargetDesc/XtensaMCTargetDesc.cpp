//===-- XtensaMCTargetDesc.cpp - Xtensa Target Descriptions ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides Xtensa specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/XtensaMCTargetDesc.h"
#include "Xtensa.h"
#include "InstPrinter/XtensaInstPrinter.h"
#include "MCTargetDesc/XtensaMCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "XtensaGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "XtensaGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "XtensaGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createXtensaMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitXtensaMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createXtensaMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitXtensaMCRegisterInfo(X, Xtensa::a11 /* RAReg doesn't exist */);
  return X;
}

static MCSubtargetInfo *createXtensaMCSubtargetInfo(const Triple &TT,
                                                 StringRef CPU, StringRef FS) {
  return createXtensaMCSubtargetInfoImpl(TT, CPU, FS);
}

static MCStreamer *createXtensaMCStreamer(const Triple &T, MCContext &Ctx,
                                       std::unique_ptr<MCAsmBackend> &&MAB,
                                       std::unique_ptr<MCObjectWriter> &&OW,
                                       std::unique_ptr<MCCodeEmitter> &&Emitter,
                                       bool RelaxAll) {
  return createELFStreamer(Ctx, std::move(MAB), std::move(OW), std::move(Emitter),
                           RelaxAll);
}

static MCInstPrinter *createXtensaMCInstPrinter(const Triple &T,
                                             unsigned SyntaxVariant,
                                             const MCAsmInfo &MAI,
                                             const MCInstrInfo &MII,
                                             const MCRegisterInfo &MRI) {
  return new XtensaInstPrinter(MAI, MII, MRI);
}

extern "C" void LLVMInitializeXtensaTargetMC() {
  for (Target *T :
       {&getTheXtensaTarget()}) {
    // Register the MC asm info.
    RegisterMCAsmInfo<XtensaMCAsmInfo> X(*T);

    // Register the MC instruction info.
    TargetRegistry::RegisterMCInstrInfo(*T, createXtensaMCInstrInfo);

    // Register the MC register info.
    TargetRegistry::RegisterMCRegInfo(*T, createXtensaMCRegisterInfo);

    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(*T,
                                            createXtensaMCSubtargetInfo);

    // Register the object streamer
    TargetRegistry::RegisterELFStreamer(*T, createXtensaMCStreamer);

    // Register the MCInstPrinter.
    TargetRegistry::RegisterMCInstPrinter(*T, createXtensaMCInstPrinter);
    TargetRegistry::RegisterMCCodeEmitter(getTheXtensaTarget(),
                                          createXtensaMCCodeEmitter);
    TargetRegistry::RegisterMCAsmBackend(getTheXtensaTarget(),
                                         createXtensaAsmBackend);

  }
}
