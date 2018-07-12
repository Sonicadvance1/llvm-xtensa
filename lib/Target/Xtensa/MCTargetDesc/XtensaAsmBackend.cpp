//===-- XtensaAsmBackend.cpp - Xtensa Assembler Backend -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "XtensaSubtarget.h"
#include "MCTargetDesc/XtensaFixupKinds.h"
#include "MCTargetDesc/XtensaMCTargetDesc.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include <cassert>
#include <cstdint>

using namespace llvm;

namespace {

class XtensaAsmBackend final : public MCAsmBackend {
  static const unsigned PCRelFlagVal = MCFixupKindInfo::FKF_IsPCRel;

public:

  XtensaAsmBackend(const MCSubtargetInfo &STI)
    : MCAsmBackend(llvm::support::little)
    , STI{STI}
  {}
  ~XtensaAsmBackend() override = default;

  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;


  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override {
    return createXtensaELFObjectWriter(reinterpret_cast<XtensaSubtarget const*>(&STI)->getFamily());
  }


  // No instruction requires relaxation
  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const override {
    return false;
  }

  unsigned getNumFixupKinds() const override { return Xtensa::NumTargetFixupKinds; };

  bool mayNeedRelaxation(const MCInst &Inst, const MCSubtargetInfo &STI) const override { return false; }

  void relaxInstruction(const MCInst &Inst, const MCSubtargetInfo &STI,
                        MCInst &Res) const override {}

  bool writeNopData(raw_ostream &OS, uint64_t Count) const override;

  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override;
private:
  const MCSubtargetInfo &STI;
};

} // end anonymous namespace

bool XtensaAsmBackend::writeNopData(raw_ostream &OS, uint64_t Count) const {
  if ((Count % 3) != 0)
    return false;

  // XXX: Write real NOP
  uint32_t Hex = 0;
  for (uint64_t i = 0; i < Count; ++i) {
    support::endian::write<uint8_t>(OS, (Hex >>  0) & 0xff, llvm::support::little);
    support::endian::write<uint8_t>(OS, (Hex >>  8) & 0xff, llvm::support::little);
    support::endian::write<uint8_t>(OS, (Hex >> 16) & 0xff, llvm::support::little);
  }
  return true;
}

static unsigned getFixupKindNumBytes(unsigned Kind) {
  switch (Kind) {
  case FK_SecRel_1:
  case FK_Data_1:
    return 1;
  case FK_SecRel_2:
  case FK_Data_2:
    return 2;
  case FK_SecRel_4:
  case FK_Data_4:
  case FK_PCRel_4:
    return 4;
  case FK_SecRel_8:
  case FK_Data_8:
    return 8;
  case Xtensa::fixup_xtensa_jump_target:
    return 3;
  case Xtensa::fixup_xtensa_cond_branch12_target:
    return 2;
  default:
    llvm_unreachable("Unknown fixup kind!");
  }
}

static uint64_t adjustFixupValue(const MCFixup &Fixup, uint64_t Value,
                                 MCContext *Ctx) {
  switch (static_cast<unsigned>(Fixup.getKind())) {
  case FK_Data_1:
  case FK_Data_2:
  case FK_Data_4:
  case FK_Data_8:
  case FK_PCRel_4:
  case FK_SecRel_4:
    return Value;
  case Xtensa::fixup_xtensa_jump_target:
    return 0x03ffff & (Value - 4);
  case Xtensa::fixup_xtensa_cond_branch12_target:
    return 0x0fff & (Value - 4);
  default:
    llvm_unreachable("unhandled fixup kind");
  }
}

void XtensaAsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                               const MCValue &Target,
                               MutableArrayRef<char> Data, uint64_t Value,
                               bool IsResolved, const MCSubtargetInfo *STI) const {
  Value = adjustFixupValue(Fixup, Value, &Asm.getContext());
  if (!Value)
    return; // Doesn't change encoding.

  MCFixupKindInfo Info = getFixupKindInfo(Fixup.getKind());

  // Shift the value into position.
  Value <<= Info.TargetOffset;

  unsigned NumBytes = getFixupKindNumBytes(Fixup.getKind());
  uint32_t Offset = Fixup.getOffset();
  assert(Offset + NumBytes <= Data.size() && "Invalid fixup offset!");

  // For each byte of the fragment that the fixup touches, mask in the bits from
  // the fixup value.
  for (unsigned i = 0; i != NumBytes; ++i)
    Data[Offset + i] |= static_cast<uint8_t>((Value >> (i * 8)) & 0xff);
}

const MCFixupKindInfo &XtensaAsmBackend::getFixupKindInfo(
                                                       MCFixupKind Kind) const {
  const static MCFixupKindInfo Infos[Xtensa::NumTargetFixupKinds] = {
      // This table *must* be in the order that the fixup_* kinds are defined
      // in XtensaFixupKinds.h.
      //
      // Name                           Offset (bits) Size (bits)     Flags
      {"fixup_xtensa_ldst_imm4_scale2", 12, 4, 0},
      {"fixup_xtensa_jump_target", 6, 18, MCFixupKindInfo::FKF_IsPCRel},
      {"fixup_xtensa_cond_branch12_target", 12, 12, MCFixupKindInfo::FKF_IsPCRel},

  };

  if (Kind < FirstTargetFixupKind)
    return MCAsmBackend::getFixupKindInfo(Kind);

  assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() &&
         "Invalid kind!");
  return Infos[Kind - FirstTargetFixupKind];
}

MCAsmBackend *llvm::createXtensaAsmBackend(const Target &T,
                                              const MCSubtargetInfo &STI,
                                              const MCRegisterInfo &MRI,
                                              const MCTargetOptions &Options) {
  return new XtensaAsmBackend(STI);
}

