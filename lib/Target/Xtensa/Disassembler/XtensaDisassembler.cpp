//===- XtensaDisassembler.cpp - Disassembler for Xtensa ---------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is part of the Xtensa Disassembler.
//
//===----------------------------------------------------------------------===//

#include "Xtensa.h"
#include "XtensaSubtarget.h"
#include "MCTargetDesc/XtensaMCTargetDesc.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/TargetRegistry.h"
#include <cstdint>

using namespace llvm;

#define DEBUG_TYPE "xtensa-disassembler"

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {

/// A disassembler class for Xtensa.
class XtensaDisassembler : public MCDisassembler {
public:
  XtensaDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
      : MCDisassembler(STI, Ctx) {}
  ~XtensaDisassembler() override = default;

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &VStream,
                              raw_ostream &CStream) const override;
};

} // end anonymous namespace

static MCDisassembler *createXtensaDisassembler(const Target &T,
                                             const MCSubtargetInfo &STI,
                                             MCContext &Ctx) {
  return new XtensaDisassembler(STI, Ctx);
}


extern "C" void LLVMInitializeXtensaDisassembler() {
  // Register the disassembler.
  TargetRegistry::RegisterMCDisassembler(getTheXtensaTarget(),
                                         createXtensaDisassembler);
}

static DecodeStatus DecodeGPRRegisterClass(MCInst &Inst, unsigned RegNo,
                                           uint64_t /*Address*/,
                                           const void * /*Decoder*/) {
  if (RegNo > 15)
    return MCDisassembler::Fail;

  unsigned Reg;

  if (RegNo < 16)
    Reg = Xtensa::a0 + RegNo;

  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeFPRRegisterClass(MCInst &Inst, unsigned RegNo,
                                           uint64_t /*Address*/,
                                           const void * /*Decoder*/) {
  if (RegNo > 15)
    return MCDisassembler::Fail;

  unsigned Reg;

  if (RegNo < 16)
    Reg = Xtensa::f0 + RegNo;

  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

#include "XtensaGenDisassemblerTables.inc"

DecodeStatus XtensaDisassembler::getInstruction(MCInst &Instr, uint64_t &Size,
                                             ArrayRef<uint8_t> Bytes,
                                             uint64_t Address,
                                             raw_ostream &VStream,
                                             raw_ostream &CStream) const {
  Size = 0;
  size_t MinInstructionSize = 2;

  if (Bytes.size() < MinInstructionSize)
    return Fail;

  uint8_t firstByte = reinterpret_cast<const uint8_t*>(Bytes.data())[0];

  if (firstByte & 8) { // 16bit instruction
    Size = 2;
  }
  else {
    if (Bytes.size() < 3)
      return Fail;
    Size = 3;
  }

  uint32_t Insn{0};
  Insn |= firstByte << 0;
  Insn |= reinterpret_cast<const uint8_t*>(Bytes.data())[1] << 8;
  if (Size == 3) {
    Insn |= reinterpret_cast<const uint8_t*>(Bytes.data())[2] << 16;
    // Call the auto-generated decoder function.
    return decodeInstruction(DecoderTableXtensa24, Instr, Insn, Address, this, STI);
  }
  else {
    // Call the auto-generated decoder function.
    return decodeInstruction(DecoderTableXtensa16, Instr, Insn, Address, this, STI);
  }
}


