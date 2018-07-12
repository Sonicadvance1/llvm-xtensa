class InstXtensa24<dag outs, dag ins, string asmstr, list<dag> pattern>
  : Instruction {
  field bits<24> Inst;
  field bits<24> SoftFail = 0;
  let Size = 3;

  let Namespace = "Xtensa";
  let DecoderNamespace = "Xtensa";

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = asmstr;
  let Pattern = pattern;
}

class InstXtensa16<dag outs, dag ins, string asmstr, list<dag> pattern>
  : Instruction {
  field bits<16> Inst;
  field bits<16> SoftFail = 0;
  let Size = 2;

  let Namespace = "Xtensa";
  let DecoderNamespace = "Xtensa";

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = asmstr;
  let Pattern = pattern;
}

