//===-- XtensaRegisterInfo.td - Xtensa Register defs -------------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the Xtensa register file
//===----------------------------------------------------------------------===//

class XtensaReg<bits<16> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "Xtensa";
}
foreach Index = 0-15 in {
  def a#Index : XtensaReg<Index, "a"#Index>, DwarfRegNum<[Index]>;
}
def INVALID : XtensaReg<16, "Invalid">, DwarfRegNum<[16]>;


foreach Index = 0-15 in {
  def f#Index : XtensaReg<Index, "f"#Index>, DwarfRegNum<[Index]>;
}

// Register classes
def GPR : RegisterClass<"Xtensa", [i32], 32,
  (sequence "a%u", 0, 15)>;
def FPR : RegisterClass<"Xtensa", [f32], 32,
  (sequence "f%u", 0, 15)>;
