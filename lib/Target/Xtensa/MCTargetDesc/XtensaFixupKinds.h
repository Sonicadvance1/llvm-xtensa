//===-- XtensaFixupKinds.h - Xtensa Specific Fixup Entries ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace Xtensa {

enum Fixups {
  fixup_xtensa_ldst_imm4_scale2 = FirstTargetFixupKind,
  fixup_xtensa_jump_target,
  fixup_xtensa_cond_branch12_target,
  fixup_xtensa_shift,
  // Marker
  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};

} // end namespace Xtensa
} // end namespace llvm

