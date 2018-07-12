//===-- XtensaMCTargetDesc.h - Xtensa Target Descriptions -------------*- C++ -*-===//
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

#pragma once

#include "llvm/Config/config.h"
#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCTargetOptions;
class StringRef;
class Target;
class Triple;
class raw_ostream;
class raw_pwrite_stream;

Target &getTheXtensaTarget();

MCCodeEmitter *createXtensaMCCodeEmitter(const MCInstrInfo &MCII,
                                      const MCRegisterInfo &MRI,
                                      MCContext &Ctx);

MCAsmBackend *createXtensaAsmBackend(const Target &T,
                                              const MCSubtargetInfo &STI,
                                              const MCRegisterInfo &MRI,
                                              const MCTargetOptions &Options);
std::unique_ptr<MCObjectTargetWriter> createXtensaELFObjectWriter(uint8_t OSABI);
}

// Defines symbolic names for Xtensa registers.  This defines a mapping from
// register name to register number.

#define GET_REGINFO_ENUM
#include "XtensaGenRegisterInfo.inc"

// Defines symbolic names for the Xtensa instructions.
//
#define GET_INSTRINFO_ENUM
#include "XtensaGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "XtensaGenSubtargetInfo.inc"
