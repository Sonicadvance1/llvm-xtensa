def NOP : InstXtensa24<(outs variable_ops), (ins variable_ops), "nop", [/* No Pattern */]> {
  let Inst{23-0} = 0b000000000010000011110000;
}

def MOV_r : InstXtensa16<(outs GPR:$rt), (ins GPR:$rs),
                         "mov.n $rt, $rs", [/* No Pattern */]> {
  bits<4> rt;
  bits<4> rs;
  let Inst{3-0} = 0b1101;
  let Inst{7-4} = rt;
  let Inst{11-8} = rs;
}

def ADD_rr : InstXtensa16<(outs GPR:$rr), (ins GPR:$rs, GPR:$rt),
                          "add.n $rr, $rs, $rt", [(set i32:$rr, (add i32:$rs, i32:$rt))]> {
  bits<4> rr;
  bits<4> rt;
  bits<4> rs;
  let Inst{3-0} = 0b1010;
  let Inst{7-4} = rt;
  let Inst{11-8} = rs;
  let Inst{15-12} = rr;
}

def SUB_rr : InstXtensa24<(outs GPR:$rr), (ins GPR:$rs, GPR:$rt),
                          "sub.n $rr, $rs, $rt", [(set i32:$rr, (sub i32:$rs, i32:$rt))]> {
  bits<4> rr;
  bits<4> rt;
  bits<4> rs;
  let Inst{3-0} = 0b0000;
  let Inst{7-4} = rt;
  let Inst{11-8} = rs;
  let Inst{15-12} = rr;
  let Inst{23-16} = 0b11000000;
}

def ADDMI_ri : InstXtensa24<(outs GPR:$rt), (ins GPR:$rs, i32imm:$imm8),
                            "addmi $rt, $rs, ($imm8 << 8)", [(set i32:$rt, (add i32:$rs, (shl i32:$imm8, 8)))]> {
  bits<4> rt;
  bits<4> rs;
  bits<8> imm8;
  let Inst{3-0} = 0b0010;
  let Inst{7-4} = rt;
  let Inst{11-8} = rs;
  let Inst{15-12} = 0b1101;
  let Inst{23-16} = imm8;
}

class UImm4OffsetOperand<int Scale> : AsmOperandClass {
  let Name = "UImm4Offset" # Scale;
  let RenderMethod = "addUImm4OffsetOperands<" # Scale # ">";
  let PredicateMethod = "isUImm4Offset<" # Scale # ">";
  let DiagnosticType = "InvalidMemoryIndexed" # Scale;
}

def UImm4OffsetScale2Operand : UImm4OffsetOperand<2>;

class uimm4_scaled<int Scale> : Operand<i32> {
  let ParserMatchClass
   = !cast<AsmOperandClass>("UImm4OffsetScale" # Scale # "Operand");
  let EncoderMethod
   = "getLdStUImm4OpValue<Xtensa::fixup_xtensa_ldst_imm4_scale" # Scale # ">";
  let PrintMethod = "printUImm4Offset<" # Scale # ">";
}

def uimm4s2 : uimm4_scaled<2>;
def am_indexed32 : ComplexPattern<i32, 2, "SelectAddrModeIndexed16", []>;

def shift_imm_XFORM: SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant(32 - N->getZExtValue(), SDLoc(N), MVT::i32);
}]>;

def ShiftImmAsmOperand : AsmOperandClass {
  let Name = "ShiftImm";
  let ParserMethod = "parseShiftImm";
}
def shift_imm : Operand<i32>, PatLeaf<(i32 imm), [{
    int32_t v = N->getZExtValue();
    return v != 0; }],
    shift_imm_XFORM> {

  let PrintMethod = "printShiftImmOperand";
  let ParserMatchClass = ShiftImmAsmOperand;
}

def S32I_r : InstXtensa16<(outs), (ins GPR:$rt, GPR:$rs, uimm4s2:$imm4),
                          "s32i.n $rt, $rs, $imm4", [(store i32:$rt, (am_indexed32 i32:$rs, uimm4s2:$imm4))]> {
  bits<4> rt;
  bits<4> rs;
  bits<4> imm4;

  let Inst{3-0} = 0b1001;
  let Inst{7-4} = rt;
  let Inst{11-8} = rs;
  let Inst{15-12} = imm4;
}

def L32I_r : InstXtensa16<(outs GPR:$rt), (ins GPR:$rs, uimm4s2:$imm4),
                          "l32i.n $rt, $rs, $imm4", [(set i32:$rt, (load (am_indexed32 i32:$rs, uimm4s2:$imm4)))]> {
  bits<4> rt;
  bits<4> rs;
  bits<4> imm4;

  let Inst{3-0} = 0b1000;
  let Inst{7-4} = rt;
  let Inst{11-8} = rs;
  let Inst{15-12} = imm4;
}

def SLLI : InstXtensa24<(outs GPR:$rr), (ins GPR:$rs, shift_imm:$sa),
                        "ssli $rr, $rs, $sa", [(set i32:$rr, (shl i32:$rs, shift_imm:$sa))]> {
  bits<4> rr;
  bits<4> rs;
  bits<5> sa;

  let Inst{3-0} = 0b0000;
  let Inst{7-4} = sa{3-0};
  let Inst{11-8} = rs;
  let Inst{15-12} = rr;
  let Inst{19-16} = 0b0001;
  let Inst{20} = sa{4};
}

let isReturn = 1, isTerminator = 1, hasDelaySlot = 0, isBarrier = 1, isNotDuplicable = 1 in {
  def RETWN : InstXtensa16<(outs), (ins), "ret.w.n", [(Xtensa_retflag)]> {
    let Inst{15-0} = 0b1111000000011101;
  }

  def RETW : InstXtensa24<(outs), (ins), "retw", [(Xtensa_retflag)]> {
    let Inst{23-0} = 0b000000000000000010010000;
  }
}

let isBarrier = 1, isNotDuplicable = 1, hasSideEffects = 1 in {
  def ENTRY : InstXtensa24<(outs), (ins GPR:$rs, i32imm:$imm12),
                         "entry $rs, $imm12", [(Xtensa_entryflag i32:$rs, i32:$imm12)]> {
    bits<4> rs;
    bits<12> imm12;

    let Inst{7-0} = 0b00110110;
    let Inst{11-8} = rs;
    let Inst{23-12} = imm12;
  }
}

let isBranch = 1, isTerminator = 1, hasDelaySlot = 0, isBarrier = 1 in {
  def J : InstXtensa24<(outs), (ins jumptarget:$dst), "j $dst", [(br bb:$dst)]> {
    bits<18> dst;
    let Inst{5-0} = 0b000110;
    let Inst{23-6} = dst;
  }

  def BEQZ : InstXtensa24<(outs), (ins GPR:$rs, cbranch12target:$dst),
                          "beqz $rs, $dst", [(Xtensabeqz GPR:$rs, bb:$dst)]> {
    bits<12> dst;
    bits<4> rs;

    let Inst{7-0} = 0b00010110;
    let Inst{11-8} = rs;
    let Inst{23-12} = dst;
  }
}
