//===-- Xtensa.td - Describe the Xtensa Target Machine -----------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

include "XtensaRegisterInfo.td"
include "XtensaInstrOperators.td"

include "XtensaInstrFormats.td"
include "XtensaInstrInfo.td"
include "XtensaInstrInfoFP.td"
include "XtensaCallingConv.td"

def XtensaInstrInfo : InstrInfo;

class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, NoItineraries, Features>;

def : Proc<"generic", []>;

def Xtensa : Target {
  let InstructionSet = XtensaInstrInfo;
}
