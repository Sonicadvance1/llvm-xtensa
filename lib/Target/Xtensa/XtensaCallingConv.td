//===-- XtensaCallingConv.td - Calling Conventions Xtensa --------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the Xtensa architecture.
//
//===----------------------------------------------------------------------===//

def RetCC_Xtensa : CallingConv<[
  // All arguments get passed in integer registers if there is space.
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,
  CCIfType<[f32], CCBitConvertToType<i32>>,
  CCIfType<[i32], CCAssignToReg<[ a2, a3, a4, a5, a6, a7 ]>>
]>;

def CC_Xtensa : CallingConv<[
  // All arguments get passed in integer registers if there is space.
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,
  CCIfType<[f32], CCBitConvertToType<i32>>,
  CCIfType<[i32], CCAssignToReg<[ a2, a3, a4, a5, a6, a7 ]>>
]>;

def RetCC_Xtensa_HF : CallingConv<[
  // All arguments get passed in integer registers if there is space.
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,
  CCIfType<[i32], CCAssignToReg<[ a2, a3, a4, a5, a6, a7 ]>>,
  CCIfType<[f32], CCAssignToReg<[ f0, f1, f2, f3, f4, f5, f6, f7 ]>>
]>;

def CC_Xtensa_HF : CallingConv<[
  // All arguments get passed in integer registers if there is space.
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,
  CCIfType<[i32], CCAssignToReg<[ a2, a3, a4, a5, a6, a7 ]>>,
  CCIfType<[f32], CCAssignToReg<[ f0, f1, f2, f3, f4, f5, f6, f7 ]>>
]>;


def CSR_Xtensa : CalleeSavedRegs<(add a0, a1, a12, a13, a14, a15)>;


