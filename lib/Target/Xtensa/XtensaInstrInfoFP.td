def ADDS_rr : InstXtensa24<(outs FPR:$fr), (ins FPR:$fs, FPR:$ft),
                           "add.s $fr, $fs, $ft", [(set f32:$fr, (fadd f32:$fs, f32:$ft))]> {

  bits<4> fr;
  bits<4> fs;
  bits<4> ft;

  let Inst{3-0} = 0b0000;
  let Inst{7-4} = ft;
  let Inst{11-8} = fs;
  let Inst{15-12} = fr;
  let Inst{23-16} = 0b00001010;
}

let isMoveReg = 1 in {
def WFR_rr : InstXtensa24<(outs FPR:$fr), (ins GPR:$rs),
                          "wfr $fr, $rs", [(set FPR:$fr, (bitconvert GPR:$rs))]> {
  bits<4> fr;
  bits<4> rs;
  let Inst{7-0} = 0b01010000;
  let Inst{11-8} = rs;
  let Inst{15-12} = fr;
  let Inst{23-16} = 0b11111010;
}
def RFR_rr : InstXtensa24<(outs GPR:$rr), (ins FPR:$fs),
                          "rfr $rr, $fs", [(set GPR:$rr, (bitconvert FPR:$fs))]> {
  bits<4> rr;
  bits<4> fs;
  let Inst{7-0} = 0b01000000;
  let Inst{11-8} = fs;
  let Inst{15-12} = rr;
  let Inst{23-16} = 0b11111010;
}
}
