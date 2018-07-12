def Xtensa_retflag : SDNode<"XtensaISD::RET_FLAG", SDTNone, [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
def Xtensa_entryflag : SDNode<"XtensaISD::ENTRY_FLAG", SDTypeProfile<0,-1,[]>, [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def SDT_Xtensabeqz : SDTypeProfile<0, 2, [SDTCisInt<0>, SDTCisVT<1, OtherVT>]>;
def Xtensabeqz           : SDNode<"XtensaISD::BEQZ", SDT_Xtensabeqz,
                                [SDNPHasChain]>;
def jumptarget : Operand<OtherVT> {
  let PrintMethod = "printJumpTargetOperand";
  let EncoderMethod = "getJumpBranchTargetOpValue";
  let OperandType = "OPERAND_PCREL";
}

def cbranch12target : Operand<OtherVT> {
  let PrintMethod = "printJumpTargetOperand";
  let EncoderMethod = "getCondBranch12TargetOpValue";
  let OperandType = "OPERAND_PCREL";
}
