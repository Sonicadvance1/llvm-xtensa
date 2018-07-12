//===-- XtensaELFObjectWriter.cpp - Xtensa ELF Writer ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/XtensaFixupKinds.h"
#include "MCTargetDesc/XtensaMCTargetDesc.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"
#include <cstdint>

using namespace llvm;

namespace {

class XtensaELFObjectWriter : public MCELFObjectTargetWriter {
public:
  XtensaELFObjectWriter(uint8_t OSABI);
  ~XtensaELFObjectWriter() override = default;

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};

} // end anonymous namespace

XtensaELFObjectWriter::XtensaELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/*Is64Bit*/ false, OSABI, ELF::EM_XTENSA,
                              /*HasRelocationAddend*/ false) {}

unsigned XtensaELFObjectWriter::getRelocType(MCContext &Ctx, const MCValue &Target,
                                          const MCFixup &Fixup,
                                          bool IsPCRel) const {
  // determine the type of the relocation
  switch ((unsigned)Fixup.getKind()) {
  default:
    llvm_unreachable("invalid fixup kind!");
  case FK_SecRel_8:
  case FK_PCRel_4:
  case FK_SecRel_4:
  case FK_Data_8:
  case FK_Data_4:
    return ELF::R_XTENSA_NONE;
  case Xtensa::fixup_xtensa_jump_target:
      return ELF::R_XTENSA_JUMP18;
  case Xtensa::fixup_xtensa_cond_branch12_target:
      return ELF::R_XTENSA_CBRANCH12;
  }
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createXtensaELFObjectWriter(uint8_t OSABI) {
  return llvm::make_unique<XtensaELFObjectWriter>(OSABI);
}
