set(LLVM_TARGET_DEFINITIONS Xtensa.td)

tablegen(LLVM XtensaGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM XtensaGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM XtensaGenCallingConv.inc -gen-callingconv)
tablegen(LLVM XtensaGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM XtensaGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM XtensaGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM XtensaGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM XtensaGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM XtensaGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(XtensaCommonTableGen)

add_llvm_target(XtensaCodeGen
  XtensaAsmPrinter.cpp
  XtensaFrameLowering.cpp
  XtensaInstrInfo.cpp
  XtensaISelDAGToDAG.cpp
  XtensaISelLowering.cpp
  XtensaMCInstLower.cpp
  XtensaRegisterInfo.cpp
  XtensaSubtarget.cpp
  XtensaTargetMachine.cpp
  )

add_subdirectory(TargetInfo)
add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
add_subdirectory(Disassembler)
