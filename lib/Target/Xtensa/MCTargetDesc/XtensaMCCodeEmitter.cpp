//===-- XtensaMCCodeEmitter.cpp - Convert Xtensa code to machine code -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the XtensaMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "InstPrinter/XtensaInstPrinter.h"
#include "MCTargetDesc/XtensaFixupKinds.h"
#include "MCTargetDesc/XtensaMCTargetDesc.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/EndianStream.h"
#include <cassert>
#include <cstdint>

using namespace llvm;

#define DEBUG_TYPE "mccodeemitter"

namespace {

class XtensaMCCodeEmitter : public MCCodeEmitter {
  const MCInstrInfo &MCII;
  const MCRegisterInfo &MRI;

public:
  XtensaMCCodeEmitter(const MCInstrInfo &mcii, const MCRegisterInfo &mri)
      : MCII(mcii), MRI(mri) {}
  XtensaMCCodeEmitter(const XtensaMCCodeEmitter &) = delete;
  void operator=(const XtensaMCCodeEmitter &) = delete;
  ~XtensaMCCodeEmitter() override = default;

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;
  // getMachineOpValue - Return binary encoding of operand. If the machin
  // operand requires relocation, record the relocation and return zero.
  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  void encodeInstruction(const MCInst &MI, raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;


  template <uint32_t FixupKind>
  uint32_t getLdStUImm4OpValue(const MCInst &MI, unsigned OpIdx,
                                SmallVectorImpl<MCFixup> &Fixups,
                                const MCSubtargetInfo &STI) const;

  /// getJumpBranchTargetOpValue - Return encoding info for 18-bit immediate
  /// branch target.
  uint32_t getJumpBranchTargetOpValue(const MCInst &MI, unsigned OpIdx,
                                     SmallVectorImpl<MCFixup> &Fixups,
                                     const MCSubtargetInfo &STI) const;
  /// getCondBranch12TargetOpValue - Return encoding info for 18-bit immediate
  /// branch target.
  uint32_t getCondBranch12TargetOpValue(const MCInst &MI, unsigned OpIdx,
                                     SmallVectorImpl<MCFixup> &Fixups,
                                     const MCSubtargetInfo &STI) const;


private:
  uint64_t computeAvailableFeatures(const FeatureBitset &FB) const;
  void verifyInstructionPredicates(const MCInst &MI,
                                   uint64_t AvailableFeatures) const;
};

} // end anonymous namespace

unsigned XtensaMCCodeEmitter::getMachineOpValue(const MCInst &MI,
                                             const MCOperand &MO,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  if (MO.isReg())
    return MRI.getEncodingValue(MO.getReg());
  if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());

  assert(MO.isExpr());

  const MCExpr *Expr = MO.getExpr();

  assert(Expr->getKind() == MCExpr::SymbolRef);

  // bb label
  llvm::errs() << "Crap\n";
  return 0;
}
void XtensaMCCodeEmitter::encodeInstruction(const MCInst &MI, raw_ostream &OS,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  uint64_t Hex = getBinaryCodeForInstr(MI, Fixups, STI);


  support::endian::write<uint8_t>(OS, (Hex >>  0) & 0xff, llvm::support::little);
  support::endian::write<uint8_t>(OS, (Hex >>  8) & 0xff, llvm::support::little);
  if (!(Hex & 8))
    support::endian::write<uint8_t>(OS, (Hex >> 16) & 0xff, llvm::support::little);
}
template<unsigned FixupKind> uint32_t
XtensaMCCodeEmitter::getLdStUImm4OpValue(const MCInst &MI, unsigned OpIdx,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpIdx);
  uint32_t ImmVal = 0;

  if (MO.isImm())
    ImmVal = static_cast<uint32_t>(MO.getImm());
  else {
    assert(MO.isExpr() && "unable to encode load/store imm operand");
    MCFixupKind Kind = MCFixupKind(FixupKind);
    Fixups.push_back(MCFixup::create(0, MO.getExpr(), Kind, MI.getLoc()));
  }

  if (ImmVal & 1)
    llvm_unreachable("Can't handle unaligned loadstore\n");
  return ImmVal >> 1;
}

/// getBranchTargetOpValue - Helper function to get the branch target operand,
/// which is either an immediate or requires a fixup.
static uint32_t getBranchTargetOpValue(const MCInst &MI, unsigned OpIdx,
                                       unsigned FixupKind,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) {
  const MCOperand &MO = MI.getOperand(OpIdx);

  // If the destination is an immediate, we have nothing to do.
  if (MO.isImm()) return MO.getImm();
  assert(MO.isExpr() && "Unexpected branch target type!");
  const MCExpr *Expr = MO.getExpr();
  MCFixupKind Kind = MCFixupKind(FixupKind);
  Fixups.push_back(MCFixup::create(0, Expr, Kind, MI.getLoc()));

  // All of the information is in the fixup.
  return 0;
}
uint32_t
XtensaMCCodeEmitter::getJumpBranchTargetOpValue(
  const MCInst &MI, unsigned OpIdx,
  SmallVectorImpl<MCFixup> &Fixups,
  const MCSubtargetInfo &STI) const {
  const MCOperand MO = MI.getOperand(OpIdx);
  if (MO.isExpr()) {
    return ::getBranchTargetOpValue(MI, OpIdx,
                                    Xtensa::fixup_xtensa_jump_target, Fixups, STI);
  }

  return MO.getImm() - 4;
}

uint32_t
XtensaMCCodeEmitter::getCondBranch12TargetOpValue(
  const MCInst &MI, unsigned OpIdx,
  SmallVectorImpl<MCFixup> &Fixups,
  const MCSubtargetInfo &STI) const {
  const MCOperand MO = MI.getOperand(OpIdx);
  if (MO.isExpr()) {
    return ::getBranchTargetOpValue(MI, OpIdx,
                                    Xtensa::fixup_xtensa_cond_branch12_target, Fixups, STI);
  }

  return MO.getImm() - 4;
}



#include "XtensaGenMCCodeEmitter.inc"

MCCodeEmitter *llvm::createXtensaMCCodeEmitter(const MCInstrInfo &MCII,
                                            const MCRegisterInfo &MRI,
                                            MCContext &Ctx) {
  return new XtensaMCCodeEmitter(MCII, MRI);
}
