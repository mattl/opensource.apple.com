//===- SimplifyLibCalls.cpp - Optimize specific well-known library calls --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements a simple pass that applies a variety of small
// optimizations for calls to specific well-known function calls (e.g. runtime
// library functions).   Any optimization that takes the very simple form
// "replace call to library function with simpler code that provides the same
// result" belongs in this file.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "simplify-libcalls"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/BuildLibCalls.h"
#include "llvm/Intrinsics.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Target/TargetData.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Config/config.h"
using namespace llvm;

STATISTIC(NumSimplified, "Number of library calls simplified");
STATISTIC(NumAnnotated, "Number of attributes added to library functions");

//===----------------------------------------------------------------------===//
// Optimizer Base Class
//===----------------------------------------------------------------------===//

/// This class is the abstract base class for the set of optimizations that
/// corresponds to one library call.
namespace {
class LibCallOptimization {
protected:
  Function *Caller;
  const TargetData *TD;
  LLVMContext* Context;
public:
  LibCallOptimization() { }
  virtual ~LibCallOptimization() {}

  /// CallOptimizer - This pure virtual method is implemented by base classes to
  /// do various optimizations.  If this returns null then no transformation was
  /// performed.  If it returns CI, then it transformed the call and CI is to be
  /// deleted.  If it returns something else, replace CI with the new value and
  /// delete CI.
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B)
    =0;

  Value *OptimizeCall(CallInst *CI, const TargetData *TD, IRBuilder<> &B) {
    Caller = CI->getParent()->getParent();
    this->TD = TD;
    if (CI->getCalledFunction())
      Context = &CI->getCalledFunction()->getContext();
    return CallOptimizer(CI->getCalledFunction(), CI, B);
  }
};
} // End anonymous namespace.


//===----------------------------------------------------------------------===//
// Helper Functions
//===----------------------------------------------------------------------===//

/// IsOnlyUsedInZeroEqualityComparison - Return true if it only matters that the
/// value is equal or not-equal to zero.
static bool IsOnlyUsedInZeroEqualityComparison(Value *V) {
  for (Value::use_iterator UI = V->use_begin(), E = V->use_end();
       UI != E; ++UI) {
    if (ICmpInst *IC = dyn_cast<ICmpInst>(*UI))
      if (IC->isEquality())
        if (Constant *C = dyn_cast<Constant>(IC->getOperand(1)))
          if (C->isNullValue())
            continue;
    // Unknown instruction.
    return false;
  }
  return true;
}

//===----------------------------------------------------------------------===//
// String and Memory LibCall Optimizations
//===----------------------------------------------------------------------===//

//===---------------------------------------===//
// 'strcat' Optimizations
namespace {
struct StrCatOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    // Verify the "strcat" function prototype.
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 2 ||
        FT->getReturnType() != Type::getInt8PtrTy(*Context) ||
        FT->getParamType(0) != FT->getReturnType() ||
        FT->getParamType(1) != FT->getReturnType())
      return 0;

    // Extract some information from the instruction
    Value *Dst = CI->getOperand(1);
    Value *Src = CI->getOperand(2);

    // See if we can get the length of the input string.
    uint64_t Len = GetStringLength(Src);
    if (Len == 0) return 0;
    --Len;  // Unbias length.

    // Handle the simple, do-nothing case: strcat(x, "") -> x
    if (Len == 0)
      return Dst;

    // These optimizations require TargetData.
    if (!TD) return 0;

    EmitStrLenMemCpy(Src, Dst, Len, B);
    return Dst;
  }

  void EmitStrLenMemCpy(Value *Src, Value *Dst, uint64_t Len, IRBuilder<> &B) {
    // We need to find the end of the destination string.  That's where the
    // memory is to be moved to. We just generate a call to strlen.
    Value *DstLen = EmitStrLen(Dst, B, TD);

    // Now that we have the destination's length, we must index into the
    // destination's pointer to get the actual memcpy destination (end of
    // the string .. we're concatenating).
    Value *CpyDst = B.CreateGEP(Dst, DstLen, "endptr");

    // We have enough information to now generate the memcpy call to do the
    // concatenation for us.  Make a memcpy to copy the nul byte with align = 1.
    EmitMemCpy(CpyDst, Src,
               ConstantInt::get(TD->getIntPtrType(*Context), Len+1),
                                1, false, B, TD);
  }
};

//===---------------------------------------===//
// 'strncat' Optimizations

struct StrNCatOpt : public StrCatOpt {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    // Verify the "strncat" function prototype.
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 3 ||
        FT->getReturnType() != Type::getInt8PtrTy(*Context) ||
        FT->getParamType(0) != FT->getReturnType() ||
        FT->getParamType(1) != FT->getReturnType() ||
        !FT->getParamType(2)->isIntegerTy())
      return 0;

    // Extract some information from the instruction
    Value *Dst = CI->getOperand(1);
    Value *Src = CI->getOperand(2);
    uint64_t Len;

    // We don't do anything if length is not constant
    if (ConstantInt *LengthArg = dyn_cast<ConstantInt>(CI->getOperand(3)))
      Len = LengthArg->getZExtValue();
    else
      return 0;

    // See if we can get the length of the input string.
    uint64_t SrcLen = GetStringLength(Src);
    if (SrcLen == 0) return 0;
    --SrcLen;  // Unbias length.

    // Handle the simple, do-nothing cases:
    // strncat(x, "", c) -> x
    // strncat(x,  c, 0) -> x
    if (SrcLen == 0 || Len == 0) return Dst;

    // These optimizations require TargetData.
    if (!TD) return 0;

    // We don't optimize this case
    if (Len < SrcLen) return 0;

    // strncat(x, s, c) -> strcat(x, s)
    // s is constant so the strcat can be optimized further
    EmitStrLenMemCpy(Src, Dst, SrcLen, B);
    return Dst;
  }
};

//===---------------------------------------===//
// 'strchr' Optimizations

struct StrChrOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    // Verify the "strchr" function prototype.
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 2 ||
        FT->getReturnType() != Type::getInt8PtrTy(*Context) ||
        FT->getParamType(0) != FT->getReturnType())
      return 0;

    Value *SrcStr = CI->getOperand(1);

    // If the second operand is non-constant, see if we can compute the length
    // of the input string and turn this into memchr.
    ConstantInt *CharC = dyn_cast<ConstantInt>(CI->getOperand(2));
    if (CharC == 0) {
      // These optimizations require TargetData.
      if (!TD) return 0;

      uint64_t Len = GetStringLength(SrcStr);
      if (Len == 0 || !FT->getParamType(1)->isIntegerTy(32))// memchr needs i32.
        return 0;

      return EmitMemChr(SrcStr, CI->getOperand(2), // include nul.
                        ConstantInt::get(TD->getIntPtrType(*Context), Len),
                        B, TD);
    }

    // Otherwise, the character is a constant, see if the first argument is
    // a string literal.  If so, we can constant fold.
    std::string Str;
    if (!GetConstantStringInfo(SrcStr, Str))
      return 0;

    // strchr can find the nul character.
    Str += '\0';
    char CharValue = CharC->getSExtValue();

    // Compute the offset.
    uint64_t i = 0;
    while (1) {
      if (i == Str.size())    // Didn't find the char.  strchr returns null.
        return Constant::getNullValue(CI->getType());
      // Did we find our match?
      if (Str[i] == CharValue)
        break;
      ++i;
    }

    // strchr(s+n,c)  -> gep(s+n+i,c)
    Value *Idx = ConstantInt::get(Type::getInt64Ty(*Context), i);
    return B.CreateGEP(SrcStr, Idx, "strchr");
  }
};

//===---------------------------------------===//
// 'strcmp' Optimizations

struct StrCmpOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    // Verify the "strcmp" function prototype.
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 2 ||
	!FT->getReturnType()->isIntegerTy(32) ||
        FT->getParamType(0) != FT->getParamType(1) ||
        FT->getParamType(0) != Type::getInt8PtrTy(*Context))
      return 0;

    Value *Str1P = CI->getOperand(1), *Str2P = CI->getOperand(2);
    if (Str1P == Str2P)      // strcmp(x,x)  -> 0
      return ConstantInt::get(CI->getType(), 0);

    std::string Str1, Str2;
    bool HasStr1 = GetConstantStringInfo(Str1P, Str1);
    bool HasStr2 = GetConstantStringInfo(Str2P, Str2);

    if (HasStr1 && Str1.empty()) // strcmp("", x) -> *x
      return B.CreateZExt(B.CreateLoad(Str2P, "strcmpload"), CI->getType());

    if (HasStr2 && Str2.empty()) // strcmp(x,"") -> *x
      return B.CreateZExt(B.CreateLoad(Str1P, "strcmpload"), CI->getType());

    // strcmp(x, y)  -> cnst  (if both x and y are constant strings)
    if (HasStr1 && HasStr2)
      return ConstantInt::get(CI->getType(),
                                     strcmp(Str1.c_str(),Str2.c_str()));

    // strcmp(P, "x") -> memcmp(P, "x", 2)
    uint64_t Len1 = GetStringLength(Str1P);
    uint64_t Len2 = GetStringLength(Str2P);
    if (Len1 && Len2) {
      // These optimizations require TargetData.
      if (!TD) return 0;

      return EmitMemCmp(Str1P, Str2P,
                        ConstantInt::get(TD->getIntPtrType(*Context),
                        std::min(Len1, Len2)), B, TD);
    }

    return 0;
  }
};

//===---------------------------------------===//
// 'strncmp' Optimizations

struct StrNCmpOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    // Verify the "strncmp" function prototype.
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 3 ||
	!FT->getReturnType()->isIntegerTy(32) ||
        FT->getParamType(0) != FT->getParamType(1) ||
        FT->getParamType(0) != Type::getInt8PtrTy(*Context) ||
        !FT->getParamType(2)->isIntegerTy())
      return 0;

    Value *Str1P = CI->getOperand(1), *Str2P = CI->getOperand(2);
    if (Str1P == Str2P)      // strncmp(x,x,n)  -> 0
      return ConstantInt::get(CI->getType(), 0);

    // Get the length argument if it is constant.
    uint64_t Length;
    if (ConstantInt *LengthArg = dyn_cast<ConstantInt>(CI->getOperand(3)))
      Length = LengthArg->getZExtValue();
    else
      return 0;

    if (Length == 0) // strncmp(x,y,0)   -> 0
      return ConstantInt::get(CI->getType(), 0);

    std::string Str1, Str2;
    bool HasStr1 = GetConstantStringInfo(Str1P, Str1);
    bool HasStr2 = GetConstantStringInfo(Str2P, Str2);

    if (HasStr1 && Str1.empty())  // strncmp("", x, n) -> *x
      return B.CreateZExt(B.CreateLoad(Str2P, "strcmpload"), CI->getType());

    if (HasStr2 && Str2.empty())  // strncmp(x, "", n) -> *x
      return B.CreateZExt(B.CreateLoad(Str1P, "strcmpload"), CI->getType());

    // strncmp(x, y)  -> cnst  (if both x and y are constant strings)
    if (HasStr1 && HasStr2)
      return ConstantInt::get(CI->getType(),
                              strncmp(Str1.c_str(), Str2.c_str(), Length));
    return 0;
  }
};


//===---------------------------------------===//
// 'strcpy' Optimizations

struct StrCpyOpt : public LibCallOptimization {
  bool OptChkCall;  // True if it's optimizing a __strcpy_chk libcall.

  StrCpyOpt(bool c) : OptChkCall(c) {}

  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    // Verify the "strcpy" function prototype.
    unsigned NumParams = OptChkCall ? 3 : 2;
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != NumParams ||
        FT->getReturnType() != FT->getParamType(0) ||
        FT->getParamType(0) != FT->getParamType(1) ||
        FT->getParamType(0) != Type::getInt8PtrTy(*Context))
      return 0;

    Value *Dst = CI->getOperand(1), *Src = CI->getOperand(2);
    if (Dst == Src)      // strcpy(x,x)  -> x
      return Src;

    // These optimizations require TargetData.
    if (!TD) return 0;

    // See if we can get the length of the input string.
    uint64_t Len = GetStringLength(Src);
    if (Len == 0) return 0;

    // We have enough information to now generate the memcpy call to do the
    // concatenation for us.  Make a memcpy to copy the nul byte with align = 1.
    if (OptChkCall)
      EmitMemCpyChk(Dst, Src,
                    ConstantInt::get(TD->getIntPtrType(*Context), Len),
                    CI->getOperand(3), B, TD);
    else
      EmitMemCpy(Dst, Src,
                 ConstantInt::get(TD->getIntPtrType(*Context), Len),
                                  1, false, B, TD);
    return Dst;
  }
};

//===---------------------------------------===//
// 'strncpy' Optimizations

struct StrNCpyOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 3 || FT->getReturnType() != FT->getParamType(0) ||
        FT->getParamType(0) != FT->getParamType(1) ||
        FT->getParamType(0) != Type::getInt8PtrTy(*Context) ||
        !FT->getParamType(2)->isIntegerTy())
      return 0;

    Value *Dst = CI->getOperand(1);
    Value *Src = CI->getOperand(2);
    Value *LenOp = CI->getOperand(3);

    // See if we can get the length of the input string.
    uint64_t SrcLen = GetStringLength(Src);
    if (SrcLen == 0) return 0;
    --SrcLen;

    if (SrcLen == 0) {
      // strncpy(x, "", y) -> memset(x, '\0', y, 1)
      EmitMemSet(Dst, ConstantInt::get(Type::getInt8Ty(*Context), '\0'),
                 LenOp, false, B, TD);
      return Dst;
    }

    uint64_t Len;
    if (ConstantInt *LengthArg = dyn_cast<ConstantInt>(LenOp))
      Len = LengthArg->getZExtValue();
    else
      return 0;

    if (Len == 0) return Dst; // strncpy(x, y, 0) -> x

    // These optimizations require TargetData.
    if (!TD) return 0;

    // Let strncpy handle the zero padding
    if (Len > SrcLen+1) return 0;

    // strncpy(x, s, c) -> memcpy(x, s, c, 1) [s and c are constant]
    EmitMemCpy(Dst, Src,
               ConstantInt::get(TD->getIntPtrType(*Context), Len),
                                1, false, B, TD);

    return Dst;
  }
};

//===---------------------------------------===//
// 'strlen' Optimizations

struct StrLenOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 1 ||
        FT->getParamType(0) != Type::getInt8PtrTy(*Context) ||
        !FT->getReturnType()->isIntegerTy())
      return 0;

    Value *Src = CI->getOperand(1);

    // Constant folding: strlen("xyz") -> 3
    if (uint64_t Len = GetStringLength(Src))
      return ConstantInt::get(CI->getType(), Len-1);

    // strlen(x) != 0 --> *x != 0
    // strlen(x) == 0 --> *x == 0
    if (IsOnlyUsedInZeroEqualityComparison(CI))
      return B.CreateZExt(B.CreateLoad(Src, "strlenfirst"), CI->getType());
    return 0;
  }
};

//===---------------------------------------===//
// 'strto*' Optimizations.  This handles strtol, strtod, strtof, strtoul, etc.

struct StrToOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    const FunctionType *FT = Callee->getFunctionType();
    if ((FT->getNumParams() != 2 && FT->getNumParams() != 3) ||
        !FT->getParamType(0)->isPointerTy() ||
        !FT->getParamType(1)->isPointerTy())
      return 0;

    Value *EndPtr = CI->getOperand(2);
    if (isa<ConstantPointerNull>(EndPtr)) {
      CI->setOnlyReadsMemory();
      CI->addAttribute(1, Attribute::NoCapture);
    }

    return 0;
  }
};

//===---------------------------------------===//
// 'strstr' Optimizations

struct StrStrOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 2 ||
        !FT->getParamType(0)->isPointerTy() ||
        !FT->getParamType(1)->isPointerTy() ||
        !FT->getReturnType()->isPointerTy())
      return 0;

    // fold strstr(x, x) -> x.
    if (CI->getOperand(1) == CI->getOperand(2))
      return B.CreateBitCast(CI->getOperand(1), CI->getType());

    // See if either input string is a constant string.
    std::string SearchStr, ToFindStr;
    bool HasStr1 = GetConstantStringInfo(CI->getOperand(1), SearchStr);
    bool HasStr2 = GetConstantStringInfo(CI->getOperand(2), ToFindStr);

    // fold strstr(x, "") -> x.
    if (HasStr2 && ToFindStr.empty())
      return B.CreateBitCast(CI->getOperand(1), CI->getType());

    // If both strings are known, constant fold it.
    if (HasStr1 && HasStr2) {
      std::string::size_type Offset = SearchStr.find(ToFindStr);

      if (Offset == std::string::npos) // strstr("foo", "bar") -> null
        return Constant::getNullValue(CI->getType());

      // strstr("abcd", "bc") -> gep((char*)"abcd", 1)
      Value *Result = CastToCStr(CI->getOperand(1), B);
      Result = B.CreateConstInBoundsGEP1_64(Result, Offset, "strstr");
      return B.CreateBitCast(Result, CI->getType());
    }

    // fold strstr(x, "y") -> strchr(x, 'y').
    if (HasStr2 && ToFindStr.size() == 1)
      return B.CreateBitCast(EmitStrChr(CI->getOperand(1), ToFindStr[0], B, TD),
                             CI->getType());
    return 0;
  }
};


//===---------------------------------------===//
// 'memcmp' Optimizations

struct MemCmpOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 3 || !FT->getParamType(0)->isPointerTy() ||
        !FT->getParamType(1)->isPointerTy() ||
        !FT->getReturnType()->isIntegerTy(32))
      return 0;

    Value *LHS = CI->getOperand(1), *RHS = CI->getOperand(2);

    if (LHS == RHS)  // memcmp(s,s,x) -> 0
      return Constant::getNullValue(CI->getType());

    // Make sure we have a constant length.
    ConstantInt *LenC = dyn_cast<ConstantInt>(CI->getOperand(3));
    if (!LenC) return 0;
    uint64_t Len = LenC->getZExtValue();

    if (Len == 0) // memcmp(s1,s2,0) -> 0
      return Constant::getNullValue(CI->getType());

    if (Len == 1) { // memcmp(S1,S2,1) -> *LHS - *RHS
      Value *LHSV = B.CreateLoad(CastToCStr(LHS, B), "lhsv");
      Value *RHSV = B.CreateLoad(CastToCStr(RHS, B), "rhsv");
      return B.CreateSExt(B.CreateSub(LHSV, RHSV, "chardiff"), CI->getType());
    }

    // Constant folding: memcmp(x, y, l) -> cnst (all arguments are constant)
    std::string LHSStr, RHSStr;
    if (GetConstantStringInfo(LHS, LHSStr) &&
        GetConstantStringInfo(RHS, RHSStr)) {
      // Make sure we're not reading out-of-bounds memory.
      if (Len > LHSStr.length() || Len > RHSStr.length())
        return 0;
      uint64_t Ret = memcmp(LHSStr.data(), RHSStr.data(), Len);
      return ConstantInt::get(CI->getType(), Ret);
    }

    return 0;
  }
};

//===---------------------------------------===//
// 'memcpy' Optimizations

struct MemCpyOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    // These optimizations require TargetData.
    if (!TD) return 0;

    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 3 || FT->getReturnType() != FT->getParamType(0) ||
        !FT->getParamType(0)->isPointerTy() ||
        !FT->getParamType(1)->isPointerTy() ||
        FT->getParamType(2) != TD->getIntPtrType(*Context))
      return 0;

    // memcpy(x, y, n) -> llvm.memcpy(x, y, n, 1)
    EmitMemCpy(CI->getOperand(1), CI->getOperand(2),
               CI->getOperand(3), 1, false, B, TD);
    return CI->getOperand(1);
  }
};

//===---------------------------------------===//
// 'memmove' Optimizations

struct MemMoveOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    // These optimizations require TargetData.
    if (!TD) return 0;

    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 3 || FT->getReturnType() != FT->getParamType(0) ||
        !FT->getParamType(0)->isPointerTy() ||
        !FT->getParamType(1)->isPointerTy() ||
        FT->getParamType(2) != TD->getIntPtrType(*Context))
      return 0;

    // memmove(x, y, n) -> llvm.memmove(x, y, n, 1)
    EmitMemMove(CI->getOperand(1), CI->getOperand(2),
                CI->getOperand(3), 1, false, B, TD);
    return CI->getOperand(1);
  }
};

//===---------------------------------------===//
// 'memset' Optimizations

struct MemSetOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    // These optimizations require TargetData.
    if (!TD) return 0;

    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 3 || FT->getReturnType() != FT->getParamType(0) ||
        !FT->getParamType(0)->isPointerTy() ||
        !FT->getParamType(1)->isIntegerTy() ||
        FT->getParamType(2) != TD->getIntPtrType(*Context))
      return 0;

    // memset(p, v, n) -> llvm.memset(p, v, n, 1)
    Value *Val = B.CreateIntCast(CI->getOperand(2), Type::getInt8Ty(*Context),
                                 false);
    EmitMemSet(CI->getOperand(1), Val,  CI->getOperand(3), false, B, TD);
    return CI->getOperand(1);
  }
};

//===----------------------------------------------------------------------===//
// Math Library Optimizations
//===----------------------------------------------------------------------===//

//===---------------------------------------===//
// 'pow*' Optimizations

struct PowOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    const FunctionType *FT = Callee->getFunctionType();
    // Just make sure this has 2 arguments of the same FP type, which match the
    // result type.
    if (FT->getNumParams() != 2 || FT->getReturnType() != FT->getParamType(0) ||
        FT->getParamType(0) != FT->getParamType(1) ||
        !FT->getParamType(0)->isFloatingPointTy())
      return 0;

    Value *Op1 = CI->getOperand(1), *Op2 = CI->getOperand(2);
    if (ConstantFP *Op1C = dyn_cast<ConstantFP>(Op1)) {
      if (Op1C->isExactlyValue(1.0))  // pow(1.0, x) -> 1.0
        return Op1C;
      if (Op1C->isExactlyValue(2.0))  // pow(2.0, x) -> exp2(x)
        return EmitUnaryFloatFnCall(Op2, "exp2", B, Callee->getAttributes());
    }

    ConstantFP *Op2C = dyn_cast<ConstantFP>(Op2);
    if (Op2C == 0) return 0;

    if (Op2C->getValueAPF().isZero())  // pow(x, 0.0) -> 1.0
      return ConstantFP::get(CI->getType(), 1.0);

    if (Op2C->isExactlyValue(0.5)) {
      // Expand pow(x, 0.5) to (x == -infinity ? +infinity : fabs(sqrt(x))).
      // This is faster than calling pow, and still handles negative zero
      // and negative infinite correctly.
      // TODO: In fast-math mode, this could be just sqrt(x).
      // TODO: In finite-only mode, this could be just fabs(sqrt(x)).
      Value *Inf = ConstantFP::getInfinity(CI->getType());
      Value *NegInf = ConstantFP::getInfinity(CI->getType(), true);
      Value *Sqrt = EmitUnaryFloatFnCall(Op1, "sqrt", B,
                                         Callee->getAttributes());
      Value *FAbs = EmitUnaryFloatFnCall(Sqrt, "fabs", B,
                                         Callee->getAttributes());
      Value *FCmp = B.CreateFCmpOEQ(Op1, NegInf, "tmp");
      Value *Sel = B.CreateSelect(FCmp, Inf, FAbs, "tmp");
      return Sel;
    }

    if (Op2C->isExactlyValue(1.0))  // pow(x, 1.0) -> x
      return Op1;
    if (Op2C->isExactlyValue(2.0))  // pow(x, 2.0) -> x*x
      return B.CreateFMul(Op1, Op1, "pow2");
    if (Op2C->isExactlyValue(-1.0)) // pow(x, -1.0) -> 1.0/x
      return B.CreateFDiv(ConstantFP::get(CI->getType(), 1.0),
                          Op1, "powrecip");
    return 0;
  }
};

//===---------------------------------------===//
// 'exp2' Optimizations

struct Exp2Opt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    const FunctionType *FT = Callee->getFunctionType();
    // Just make sure this has 1 argument of FP type, which matches the
    // result type.
    if (FT->getNumParams() != 1 || FT->getReturnType() != FT->getParamType(0) ||
        !FT->getParamType(0)->isFloatingPointTy())
      return 0;

    Value *Op = CI->getOperand(1);
    // Turn exp2(sitofp(x)) -> ldexp(1.0, sext(x))  if sizeof(x) <= 32
    // Turn exp2(uitofp(x)) -> ldexp(1.0, zext(x))  if sizeof(x) < 32
    Value *LdExpArg = 0;
    if (SIToFPInst *OpC = dyn_cast<SIToFPInst>(Op)) {
      if (OpC->getOperand(0)->getType()->getPrimitiveSizeInBits() <= 32)
        LdExpArg = B.CreateSExt(OpC->getOperand(0),
				Type::getInt32Ty(*Context), "tmp");
    } else if (UIToFPInst *OpC = dyn_cast<UIToFPInst>(Op)) {
      if (OpC->getOperand(0)->getType()->getPrimitiveSizeInBits() < 32)
        LdExpArg = B.CreateZExt(OpC->getOperand(0),
				Type::getInt32Ty(*Context), "tmp");
    }

    if (LdExpArg) {
      const char *Name;
      if (Op->getType()->isFloatTy())
        Name = "ldexpf";
      else if (Op->getType()->isDoubleTy())
        Name = "ldexp";
      else
        Name = "ldexpl";

      Constant *One = ConstantFP::get(*Context, APFloat(1.0f));
      if (!Op->getType()->isFloatTy())
        One = ConstantExpr::getFPExtend(One, Op->getType());

      Module *M = Caller->getParent();
      Value *Callee = M->getOrInsertFunction(Name, Op->getType(),
                                             Op->getType(),
                                             Type::getInt32Ty(*Context),NULL);
      CallInst *CI = B.CreateCall2(Callee, One, LdExpArg);
      if (const Function *F = dyn_cast<Function>(Callee->stripPointerCasts()))
        CI->setCallingConv(F->getCallingConv());

      return CI;
    }
    return 0;
  }
};

//===---------------------------------------===//
// Double -> Float Shrinking Optimizations for Unary Functions like 'floor'

struct UnaryDoubleFPOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 1 || !FT->getReturnType()->isDoubleTy() ||
        !FT->getParamType(0)->isDoubleTy())
      return 0;

    // If this is something like 'floor((double)floatval)', convert to floorf.
    FPExtInst *Cast = dyn_cast<FPExtInst>(CI->getOperand(1));
    if (Cast == 0 || !Cast->getOperand(0)->getType()->isFloatTy())
      return 0;

    // floor((double)floatval) -> (double)floorf(floatval)
    Value *V = Cast->getOperand(0);
    V = EmitUnaryFloatFnCall(V, Callee->getName().data(), B,
                             Callee->getAttributes());
    return B.CreateFPExt(V, Type::getDoubleTy(*Context));
  }
};

//===----------------------------------------------------------------------===//
// Integer Optimizations
//===----------------------------------------------------------------------===//

//===---------------------------------------===//
// 'ffs*' Optimizations

struct FFSOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    const FunctionType *FT = Callee->getFunctionType();
    // Just make sure this has 2 arguments of the same FP type, which match the
    // result type.
    if (FT->getNumParams() != 1 ||
	!FT->getReturnType()->isIntegerTy(32) ||
        !FT->getParamType(0)->isIntegerTy())
      return 0;

    Value *Op = CI->getOperand(1);

    // Constant fold.
    if (ConstantInt *CI = dyn_cast<ConstantInt>(Op)) {
      if (CI->getValue() == 0)  // ffs(0) -> 0.
        return Constant::getNullValue(CI->getType());
      return ConstantInt::get(Type::getInt32Ty(*Context), // ffs(c) -> cttz(c)+1
                              CI->getValue().countTrailingZeros()+1);
    }

    // ffs(x) -> x != 0 ? (i32)llvm.cttz(x)+1 : 0
    const Type *ArgType = Op->getType();
    Value *F = Intrinsic::getDeclaration(Callee->getParent(),
                                         Intrinsic::cttz, &ArgType, 1);
    Value *V = B.CreateCall(F, Op, "cttz");
    V = B.CreateAdd(V, ConstantInt::get(V->getType(), 1), "tmp");
    V = B.CreateIntCast(V, Type::getInt32Ty(*Context), false, "tmp");

    Value *Cond = B.CreateICmpNE(Op, Constant::getNullValue(ArgType), "tmp");
    return B.CreateSelect(Cond, V,
			  ConstantInt::get(Type::getInt32Ty(*Context), 0));
  }
};

//===---------------------------------------===//
// 'isdigit' Optimizations

struct IsDigitOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    const FunctionType *FT = Callee->getFunctionType();
    // We require integer(i32)
    if (FT->getNumParams() != 1 || !FT->getReturnType()->isIntegerTy() ||
        !FT->getParamType(0)->isIntegerTy(32))
      return 0;

    // isdigit(c) -> (c-'0') <u 10
    Value *Op = CI->getOperand(1);
    Op = B.CreateSub(Op, ConstantInt::get(Type::getInt32Ty(*Context), '0'),
                     "isdigittmp");
    Op = B.CreateICmpULT(Op, ConstantInt::get(Type::getInt32Ty(*Context), 10),
                         "isdigit");
    return B.CreateZExt(Op, CI->getType());
  }
};

//===---------------------------------------===//
// 'isascii' Optimizations

struct IsAsciiOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    const FunctionType *FT = Callee->getFunctionType();
    // We require integer(i32)
    if (FT->getNumParams() != 1 || !FT->getReturnType()->isIntegerTy() ||
        !FT->getParamType(0)->isIntegerTy(32))
      return 0;

    // isascii(c) -> c <u 128
    Value *Op = CI->getOperand(1);
    Op = B.CreateICmpULT(Op, ConstantInt::get(Type::getInt32Ty(*Context), 128),
                         "isascii");
    return B.CreateZExt(Op, CI->getType());
  }
};

//===---------------------------------------===//
// 'abs', 'labs', 'llabs' Optimizations

struct AbsOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    const FunctionType *FT = Callee->getFunctionType();
    // We require integer(integer) where the types agree.
    if (FT->getNumParams() != 1 || !FT->getReturnType()->isIntegerTy() ||
        FT->getParamType(0) != FT->getReturnType())
      return 0;

    // abs(x) -> x >s -1 ? x : -x
    Value *Op = CI->getOperand(1);
    Value *Pos = B.CreateICmpSGT(Op,
                             Constant::getAllOnesValue(Op->getType()),
                                 "ispos");
    Value *Neg = B.CreateNeg(Op, "neg");
    return B.CreateSelect(Pos, Op, Neg);
  }
};


//===---------------------------------------===//
// 'toascii' Optimizations

struct ToAsciiOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    const FunctionType *FT = Callee->getFunctionType();
    // We require i32(i32)
    if (FT->getNumParams() != 1 || FT->getReturnType() != FT->getParamType(0) ||
        !FT->getParamType(0)->isIntegerTy(32))
      return 0;

    // isascii(c) -> c & 0x7f
    return B.CreateAnd(CI->getOperand(1),
                       ConstantInt::get(CI->getType(),0x7F));
  }
};

//===----------------------------------------------------------------------===//
// Formatting and IO Optimizations
//===----------------------------------------------------------------------===//

//===---------------------------------------===//
// 'printf' Optimizations

struct PrintFOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    // Require one fixed pointer argument and an integer/void result.
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() < 1 || !FT->getParamType(0)->isPointerTy() ||
        !(FT->getReturnType()->isIntegerTy() ||
          FT->getReturnType()->isVoidTy()))
      return 0;

    // Check for a fixed format string.
    std::string FormatStr;
    if (!GetConstantStringInfo(CI->getOperand(1), FormatStr))
      return 0;

    // Empty format string -> noop.
    if (FormatStr.empty())  // Tolerate printf's declared void.
      return CI->use_empty() ? (Value*)CI :
                               ConstantInt::get(CI->getType(), 0);

    // printf("x") -> putchar('x'), even for '%'.  Return the result of putchar
    // in case there is an error writing to stdout.
    if (FormatStr.size() == 1) {
      Value *Res = EmitPutChar(ConstantInt::get(Type::getInt32Ty(*Context),
                                                FormatStr[0]), B, TD);
      if (CI->use_empty()) return CI;
      return B.CreateIntCast(Res, CI->getType(), true);
    }

    // printf("foo\n") --> puts("foo")
    if (FormatStr[FormatStr.size()-1] == '\n' &&
        FormatStr.find('%') == std::string::npos) {  // no format characters.
      // Create a string literal with no \n on it.  We expect the constant merge
      // pass to be run after this pass, to merge duplicate strings.
      FormatStr.erase(FormatStr.end()-1);
      Constant *C = ConstantArray::get(*Context, FormatStr, true);
      C = new GlobalVariable(*Callee->getParent(), C->getType(), true,
                             GlobalVariable::InternalLinkage, C, "str");
      EmitPutS(C, B, TD);
      return CI->use_empty() ? (Value*)CI :
                    ConstantInt::get(CI->getType(), FormatStr.size()+1);
    }

    // Optimize specific format strings.
    // printf("%c", chr) --> putchar(*(i8*)dst)
    if (FormatStr == "%c" && CI->getNumOperands() > 2 &&
        CI->getOperand(2)->getType()->isIntegerTy()) {
      Value *Res = EmitPutChar(CI->getOperand(2), B, TD);

      if (CI->use_empty()) return CI;
      return B.CreateIntCast(Res, CI->getType(), true);
    }

    // printf("%s\n", str) --> puts(str)
    if (FormatStr == "%s\n" && CI->getNumOperands() > 2 &&
        CI->getOperand(2)->getType()->isPointerTy() &&
        CI->use_empty()) {
      EmitPutS(CI->getOperand(2), B, TD);
      return CI;
    }
    return 0;
  }
};

//===---------------------------------------===//
// 'sprintf' Optimizations

struct SPrintFOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    // Require two fixed pointer arguments and an integer result.
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 2 || !FT->getParamType(0)->isPointerTy() ||
        !FT->getParamType(1)->isPointerTy() ||
        !FT->getReturnType()->isIntegerTy())
      return 0;

    // Check for a fixed format string.
    std::string FormatStr;
    if (!GetConstantStringInfo(CI->getOperand(2), FormatStr))
      return 0;

    // If we just have a format string (nothing else crazy) transform it.
    if (CI->getNumOperands() == 3) {
      // Make sure there's no % in the constant array.  We could try to handle
      // %% -> % in the future if we cared.
      for (unsigned i = 0, e = FormatStr.size(); i != e; ++i)
        if (FormatStr[i] == '%')
          return 0; // we found a format specifier, bail out.

      // These optimizations require TargetData.
      if (!TD) return 0;

      // sprintf(str, fmt) -> llvm.memcpy(str, fmt, strlen(fmt)+1, 1)
      EmitMemCpy(CI->getOperand(1), CI->getOperand(2), // Copy the nul byte.
                 ConstantInt::get(TD->getIntPtrType(*Context),
                 FormatStr.size()+1), 1, false, B, TD);
      return ConstantInt::get(CI->getType(), FormatStr.size());
    }

    // The remaining optimizations require the format string to be "%s" or "%c"
    // and have an extra operand.
    if (FormatStr.size() != 2 || FormatStr[0] != '%' || CI->getNumOperands() <4)
      return 0;

    // Decode the second character of the format string.
    if (FormatStr[1] == 'c') {
      // sprintf(dst, "%c", chr) --> *(i8*)dst = chr; *((i8*)dst+1) = 0
      if (!CI->getOperand(3)->getType()->isIntegerTy()) return 0;
      Value *V = B.CreateTrunc(CI->getOperand(3),
                               Type::getInt8Ty(*Context), "char");
      Value *Ptr = CastToCStr(CI->getOperand(1), B);
      B.CreateStore(V, Ptr);
      Ptr = B.CreateGEP(Ptr, ConstantInt::get(Type::getInt32Ty(*Context), 1),
                        "nul");
      B.CreateStore(Constant::getNullValue(Type::getInt8Ty(*Context)), Ptr);

      return ConstantInt::get(CI->getType(), 1);
    }

    if (FormatStr[1] == 's') {
      // These optimizations require TargetData.
      if (!TD) return 0;

      // sprintf(dest, "%s", str) -> llvm.memcpy(dest, str, strlen(str)+1, 1)
      if (!CI->getOperand(3)->getType()->isPointerTy()) return 0;

      Value *Len = EmitStrLen(CI->getOperand(3), B, TD);
      Value *IncLen = B.CreateAdd(Len,
                                  ConstantInt::get(Len->getType(), 1),
                                  "leninc");
      EmitMemCpy(CI->getOperand(1), CI->getOperand(3), IncLen, 1, false, B, TD);

      // The sprintf result is the unincremented number of bytes in the string.
      return B.CreateIntCast(Len, CI->getType(), false);
    }
    return 0;
  }
};

//===---------------------------------------===//
// 'fwrite' Optimizations

struct FWriteOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    // Require a pointer, an integer, an integer, a pointer, returning integer.
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 4 || !FT->getParamType(0)->isPointerTy() ||
        !FT->getParamType(1)->isIntegerTy() ||
        !FT->getParamType(2)->isIntegerTy() ||
        !FT->getParamType(3)->isPointerTy() ||
        !FT->getReturnType()->isIntegerTy())
      return 0;

    // Get the element size and count.
    ConstantInt *SizeC = dyn_cast<ConstantInt>(CI->getOperand(2));
    ConstantInt *CountC = dyn_cast<ConstantInt>(CI->getOperand(3));
    if (!SizeC || !CountC) return 0;
    uint64_t Bytes = SizeC->getZExtValue()*CountC->getZExtValue();

    // If this is writing zero records, remove the call (it's a noop).
    if (Bytes == 0)
      return ConstantInt::get(CI->getType(), 0);

    // If this is writing one byte, turn it into fputc.
    if (Bytes == 1) {  // fwrite(S,1,1,F) -> fputc(S[0],F)
      Value *Char = B.CreateLoad(CastToCStr(CI->getOperand(1), B), "char");
      EmitFPutC(Char, CI->getOperand(4), B, TD);
      return ConstantInt::get(CI->getType(), 1);
    }

    return 0;
  }
};

//===---------------------------------------===//
// 'fputs' Optimizations

struct FPutsOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    // These optimizations require TargetData.
    if (!TD) return 0;

    // Require two pointers.  Also, we can't optimize if return value is used.
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 2 || !FT->getParamType(0)->isPointerTy() ||
        !FT->getParamType(1)->isPointerTy() ||
        !CI->use_empty())
      return 0;

    // fputs(s,F) --> fwrite(s,1,strlen(s),F)
    uint64_t Len = GetStringLength(CI->getOperand(1));
    if (!Len) return 0;
    EmitFWrite(CI->getOperand(1),
               ConstantInt::get(TD->getIntPtrType(*Context), Len-1),
               CI->getOperand(2), B, TD);
    return CI;  // Known to have no uses (see above).
  }
};

//===---------------------------------------===//
// 'fprintf' Optimizations

struct FPrintFOpt : public LibCallOptimization {
  virtual Value *CallOptimizer(Function *Callee, CallInst *CI, IRBuilder<> &B) {
    // Require two fixed paramters as pointers and integer result.
    const FunctionType *FT = Callee->getFunctionType();
    if (FT->getNumParams() != 2 || !FT->getParamType(0)->isPointerTy() ||
        !FT->getParamType(1)->isPointerTy() ||
        !FT->getReturnType()->isIntegerTy())
      return 0;

    // All the optimizations depend on the format string.
    std::string FormatStr;
    if (!GetConstantStringInfo(CI->getOperand(2), FormatStr))
      return 0;

    // fprintf(F, "foo") --> fwrite("foo", 3, 1, F)
    if (CI->getNumOperands() == 3) {
      for (unsigned i = 0, e = FormatStr.size(); i != e; ++i)
        if (FormatStr[i] == '%')  // Could handle %% -> % if we cared.
          return 0; // We found a format specifier.

      // These optimizations require TargetData.
      if (!TD) return 0;

      EmitFWrite(CI->getOperand(2),
                 ConstantInt::get(TD->getIntPtrType(*Context),
                                  FormatStr.size()),
                 CI->getOperand(1), B, TD);
      return ConstantInt::get(CI->getType(), FormatStr.size());
    }

    // The remaining optimizations require the format string to be "%s" or "%c"
    // and have an extra operand.
    if (FormatStr.size() != 2 || FormatStr[0] != '%' || CI->getNumOperands() <4)
      return 0;

    // Decode the second character of the format string.
    if (FormatStr[1] == 'c') {
      // fprintf(F, "%c", chr) --> *(i8*)dst = chr
      if (!CI->getOperand(3)->getType()->isIntegerTy()) return 0;
      EmitFPutC(CI->getOperand(3), CI->getOperand(1), B, TD);
      return ConstantInt::get(CI->getType(), 1);
    }

    if (FormatStr[1] == 's') {
      // fprintf(F, "%s", str) -> fputs(str, F)
      if (!CI->getOperand(3)->getType()->isPointerTy() || !CI->use_empty())
        return 0;
      EmitFPutS(CI->getOperand(3), CI->getOperand(1), B, TD);
      return CI;
    }
    return 0;
  }
};

} // end anonymous namespace.

//===----------------------------------------------------------------------===//
// SimplifyLibCalls Pass Implementation
//===----------------------------------------------------------------------===//

namespace {
  /// This pass optimizes well known library functions from libc and libm.
  ///
  class SimplifyLibCalls : public FunctionPass {
    StringMap<LibCallOptimization*> Optimizations;
    // String and Memory LibCall Optimizations
    StrCatOpt StrCat; StrNCatOpt StrNCat; StrChrOpt StrChr; StrCmpOpt StrCmp;
    StrNCmpOpt StrNCmp; StrCpyOpt StrCpy; StrCpyOpt StrCpyChk;
    StrNCpyOpt StrNCpy; StrLenOpt StrLen;
    StrToOpt StrTo; StrStrOpt StrStr;
    MemCmpOpt MemCmp; MemCpyOpt MemCpy; MemMoveOpt MemMove; MemSetOpt MemSet;
    // Math Library Optimizations
    PowOpt Pow; Exp2Opt Exp2; UnaryDoubleFPOpt UnaryDoubleFP;
    // Integer Optimizations
    FFSOpt FFS; AbsOpt Abs; IsDigitOpt IsDigit; IsAsciiOpt IsAscii;
    ToAsciiOpt ToAscii;
    // Formatting and IO Optimizations
    SPrintFOpt SPrintF; PrintFOpt PrintF;
    FWriteOpt FWrite; FPutsOpt FPuts; FPrintFOpt FPrintF;

    bool Modified;  // This is only used by doInitialization.
  public:
    static char ID; // Pass identification
    SimplifyLibCalls() : FunctionPass(&ID), StrCpy(false), StrCpyChk(true) {}
    void InitOptimizations();
    bool runOnFunction(Function &F);

    void setDoesNotAccessMemory(Function &F);
    void setOnlyReadsMemory(Function &F);
    void setDoesNotThrow(Function &F);
    void setDoesNotCapture(Function &F, unsigned n);
    void setDoesNotAlias(Function &F, unsigned n);
    bool doInitialization(Module &M);

    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    }
  };
  char SimplifyLibCalls::ID = 0;
} // end anonymous namespace.

static RegisterPass<SimplifyLibCalls>
X("simplify-libcalls", "Simplify well-known library calls");

// Public interface to the Simplify LibCalls pass.
FunctionPass *llvm::createSimplifyLibCallsPass() {
  return new SimplifyLibCalls();
}

/// Optimizations - Populate the Optimizations map with all the optimizations
/// we know.
void SimplifyLibCalls::InitOptimizations() {
  // String and Memory LibCall Optimizations
  Optimizations["strcat"] = &StrCat;
  Optimizations["strncat"] = &StrNCat;
  Optimizations["strchr"] = &StrChr;
  Optimizations["strcmp"] = &StrCmp;
  Optimizations["strncmp"] = &StrNCmp;
  Optimizations["strcpy"] = &StrCpy;
  Optimizations["strncpy"] = &StrNCpy;
  Optimizations["strlen"] = &StrLen;
  Optimizations["strtol"] = &StrTo;
  Optimizations["strtod"] = &StrTo;
  Optimizations["strtof"] = &StrTo;
  Optimizations["strtoul"] = &StrTo;
  Optimizations["strtoll"] = &StrTo;
  Optimizations["strtold"] = &StrTo;
  Optimizations["strtoull"] = &StrTo;
  Optimizations["strstr"] = &StrStr;
  Optimizations["memcmp"] = &MemCmp;
  Optimizations["memcpy"] = &MemCpy;
  Optimizations["memmove"] = &MemMove;
  Optimizations["memset"] = &MemSet;

  // _chk variants of String and Memory LibCall Optimizations.
  Optimizations["__strcpy_chk"] = &StrCpyChk;

  // Math Library Optimizations
  Optimizations["powf"] = &Pow;
  Optimizations["pow"] = &Pow;
  Optimizations["powl"] = &Pow;
  Optimizations["llvm.pow.f32"] = &Pow;
  Optimizations["llvm.pow.f64"] = &Pow;
  Optimizations["llvm.pow.f80"] = &Pow;
  Optimizations["llvm.pow.f128"] = &Pow;
  Optimizations["llvm.pow.ppcf128"] = &Pow;
  Optimizations["exp2l"] = &Exp2;
  Optimizations["exp2"] = &Exp2;
  Optimizations["exp2f"] = &Exp2;
  Optimizations["llvm.exp2.ppcf128"] = &Exp2;
  Optimizations["llvm.exp2.f128"] = &Exp2;
  Optimizations["llvm.exp2.f80"] = &Exp2;
  Optimizations["llvm.exp2.f64"] = &Exp2;
  Optimizations["llvm.exp2.f32"] = &Exp2;

#ifdef HAVE_FLOORF
  Optimizations["floor"] = &UnaryDoubleFP;
#endif
#ifdef HAVE_CEILF
  Optimizations["ceil"] = &UnaryDoubleFP;
#endif
#ifdef HAVE_ROUNDF
  Optimizations["round"] = &UnaryDoubleFP;
#endif
#ifdef HAVE_RINTF
  Optimizations["rint"] = &UnaryDoubleFP;
#endif
#ifdef HAVE_NEARBYINTF
  Optimizations["nearbyint"] = &UnaryDoubleFP;
#endif

  // Integer Optimizations
  Optimizations["ffs"] = &FFS;
  Optimizations["ffsl"] = &FFS;
  Optimizations["ffsll"] = &FFS;
  Optimizations["abs"] = &Abs;
  Optimizations["labs"] = &Abs;
  Optimizations["llabs"] = &Abs;
  Optimizations["isdigit"] = &IsDigit;
  Optimizations["isascii"] = &IsAscii;
  Optimizations["toascii"] = &ToAscii;

  // Formatting and IO Optimizations
  Optimizations["sprintf"] = &SPrintF;
  Optimizations["printf"] = &PrintF;
  Optimizations["fwrite"] = &FWrite;
  Optimizations["fputs"] = &FPuts;
  Optimizations["fprintf"] = &FPrintF;
}


/// runOnFunction - Top level algorithm.
///
bool SimplifyLibCalls::runOnFunction(Function &F) {
  if (Optimizations.empty())
    InitOptimizations();

  const TargetData *TD = getAnalysisIfAvailable<TargetData>();

  IRBuilder<> Builder(F.getContext());

  bool Changed = false;
  for (Function::iterator BB = F.begin(), E = F.end(); BB != E; ++BB) {
    for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ) {
      // Ignore non-calls.
      CallInst *CI = dyn_cast<CallInst>(I++);
      if (!CI) continue;

      // Ignore indirect calls and calls to non-external functions.
      Function *Callee = CI->getCalledFunction();
      if (Callee == 0 || !Callee->isDeclaration() ||
          !(Callee->hasExternalLinkage() || Callee->hasDLLImportLinkage()))
        continue;

      // Ignore unknown calls.
      LibCallOptimization *LCO = Optimizations.lookup(Callee->getName());
      if (!LCO) continue;

      // Set the builder to the instruction after the call.
      Builder.SetInsertPoint(BB, I);

      // Try to optimize this call.
      Value *Result = LCO->OptimizeCall(CI, TD, Builder);
      if (Result == 0) continue;

      DEBUG(dbgs() << "SimplifyLibCalls simplified: " << *CI;
            dbgs() << "  into: " << *Result << "\n");

      // Something changed!
      Changed = true;
      ++NumSimplified;

      // Inspect the instruction after the call (which was potentially just
      // added) next.
      I = CI; ++I;

      if (CI != Result && !CI->use_empty()) {
        CI->replaceAllUsesWith(Result);
        if (!Result->hasName())
          Result->takeName(CI);
      }
      CI->eraseFromParent();
    }
  }
  return Changed;
}

// Utility methods for doInitialization.

void SimplifyLibCalls::setDoesNotAccessMemory(Function &F) {
  if (!F.doesNotAccessMemory()) {
    F.setDoesNotAccessMemory();
    ++NumAnnotated;
    Modified = true;
  }
}
void SimplifyLibCalls::setOnlyReadsMemory(Function &F) {
  if (!F.onlyReadsMemory()) {
    F.setOnlyReadsMemory();
    ++NumAnnotated;
    Modified = true;
  }
}
void SimplifyLibCalls::setDoesNotThrow(Function &F) {
  if (!F.doesNotThrow()) {
    F.setDoesNotThrow();
    ++NumAnnotated;
    Modified = true;
  }
}
void SimplifyLibCalls::setDoesNotCapture(Function &F, unsigned n) {
  if (!F.doesNotCapture(n)) {
    F.setDoesNotCapture(n);
    ++NumAnnotated;
    Modified = true;
  }
}
void SimplifyLibCalls::setDoesNotAlias(Function &F, unsigned n) {
  if (!F.doesNotAlias(n)) {
    F.setDoesNotAlias(n);
    ++NumAnnotated;
    Modified = true;
  }
}

/// doInitialization - Add attributes to well-known functions.
///
bool SimplifyLibCalls::doInitialization(Module &M) {
  Modified = false;
  for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I) {
    Function &F = *I;
    if (!F.isDeclaration())
      continue;

    if (!F.hasName())
      continue;

    const FunctionType *FTy = F.getFunctionType();

    StringRef Name = F.getName();
    switch (Name[0]) {
      case 's':
        if (Name == "strlen") {
          if (FTy->getNumParams() != 1 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setOnlyReadsMemory(F);
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        } else if (Name == "strcpy" ||
                   Name == "stpcpy" ||
                   Name == "strcat" ||
                   Name == "strtol" ||
                   Name == "strtod" ||
                   Name == "strtof" ||
                   Name == "strtoul" ||
                   Name == "strtoll" ||
                   Name == "strtold" ||
                   Name == "strncat" ||
                   Name == "strncpy" ||
                   Name == "strtoull") {
          if (FTy->getNumParams() < 2 ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 2);
        } else if (Name == "strxfrm") {
          if (FTy->getNumParams() != 3 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        } else if (Name == "strcmp" ||
                   Name == "strspn" ||
                   Name == "strncmp" ||
                   Name ==" strcspn" ||
                   Name == "strcoll" ||
                   Name == "strcasecmp" ||
                   Name == "strncasecmp") {
          if (FTy->getNumParams() < 2 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setOnlyReadsMemory(F);
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        } else if (Name == "strstr" ||
                   Name == "strpbrk") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setOnlyReadsMemory(F);
          setDoesNotThrow(F);
          setDoesNotCapture(F, 2);
        } else if (Name == "strtok" ||
                   Name == "strtok_r") {
          if (FTy->getNumParams() < 2 ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 2);
        } else if (Name == "scanf" ||
                   Name == "setbuf" ||
                   Name == "setvbuf") {
          if (FTy->getNumParams() < 1 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        } else if (Name == "strdup" ||
                   Name == "strndup") {
          if (FTy->getNumParams() < 1 ||
              !FTy->getReturnType()->isPointerTy() ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotAlias(F, 0);
          setDoesNotCapture(F, 1);
        } else if (Name == "stat" ||
                   Name == "sscanf" ||
                   Name == "sprintf" ||
                   Name == "statvfs") {
          if (FTy->getNumParams() < 2 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        } else if (Name == "snprintf") {
          if (FTy->getNumParams() != 3 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(2)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 3);
        } else if (Name == "setitimer") {
          if (FTy->getNumParams() != 3 ||
              !FTy->getParamType(1)->isPointerTy() ||
              !FTy->getParamType(2)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 2);
          setDoesNotCapture(F, 3);
        } else if (Name == "system") {
          if (FTy->getNumParams() != 1 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          // May throw; "system" is a valid pthread cancellation point.
          setDoesNotCapture(F, 1);
        }
        break;
      case 'm':
        if (Name == "malloc") {
          if (FTy->getNumParams() != 1 ||
              !FTy->getReturnType()->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotAlias(F, 0);
        } else if (Name == "memcmp") {
          if (FTy->getNumParams() != 3 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setOnlyReadsMemory(F);
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        } else if (Name == "memchr" ||
                   Name == "memrchr") {
          if (FTy->getNumParams() != 3)
            continue;
          setOnlyReadsMemory(F);
          setDoesNotThrow(F);
        } else if (Name == "modf" ||
                   Name == "modff" ||
                   Name == "modfl" ||
                   Name == "memcpy" ||
                   Name == "memccpy" ||
                   Name == "memmove") {
          if (FTy->getNumParams() < 2 ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 2);
        } else if (Name == "memalign") {
          if (!FTy->getReturnType()->isPointerTy())
            continue;
          setDoesNotAlias(F, 0);
        } else if (Name == "mkdir" ||
                   Name == "mktime") {
          if (FTy->getNumParams() == 0 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        }
        break;
      case 'r':
        if (Name == "realloc") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getReturnType()->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotAlias(F, 0);
          setDoesNotCapture(F, 1);
        } else if (Name == "read") {
          if (FTy->getNumParams() != 3 ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          // May throw; "read" is a valid pthread cancellation point.
          setDoesNotCapture(F, 2);
        } else if (Name == "rmdir" ||
                   Name == "rewind" ||
                   Name == "remove" ||
                   Name == "realpath") {
          if (FTy->getNumParams() < 1 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        } else if (Name == "rename" ||
                   Name == "readlink") {
          if (FTy->getNumParams() < 2 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        }
        break;
      case 'w':
        if (Name == "write") {
          if (FTy->getNumParams() != 3 ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          // May throw; "write" is a valid pthread cancellation point.
          setDoesNotCapture(F, 2);
        }
        break;
      case 'b':
        if (Name == "bcopy") {
          if (FTy->getNumParams() != 3 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        } else if (Name == "bcmp") {
          if (FTy->getNumParams() != 3 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setOnlyReadsMemory(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        } else if (Name == "bzero") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        }
        break;
      case 'c':
        if (Name == "calloc") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getReturnType()->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotAlias(F, 0);
        } else if (Name == "chmod" ||
                   Name == "chown" ||
                   Name == "ctermid" ||
                   Name == "clearerr" ||
                   Name == "closedir") {
          if (FTy->getNumParams() == 0 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        }
        break;
      case 'a':
        if (Name == "atoi" ||
            Name == "atol" ||
            Name == "atof" ||
            Name == "atoll") {
          if (FTy->getNumParams() != 1 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setOnlyReadsMemory(F);
          setDoesNotCapture(F, 1);
        } else if (Name == "access") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        }
        break;
      case 'f':
        if (Name == "fopen") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getReturnType()->isPointerTy() ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotAlias(F, 0);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        } else if (Name == "fdopen") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getReturnType()->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotAlias(F, 0);
          setDoesNotCapture(F, 2);
        } else if (Name == "feof" ||
                   Name == "free" ||
                   Name == "fseek" ||
                   Name == "ftell" ||
                   Name == "fgetc" ||
                   Name == "fseeko" ||
                   Name == "ftello" ||
                   Name == "fileno" ||
                   Name == "fflush" ||
                   Name == "fclose" ||
                   Name == "fsetpos" ||
                   Name == "flockfile" ||
                   Name == "funlockfile" ||
                   Name == "ftrylockfile") {
          if (FTy->getNumParams() == 0 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        } else if (Name == "ferror") {
          if (FTy->getNumParams() != 1 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setOnlyReadsMemory(F);
        } else if (Name == "fputc" ||
                   Name == "fstat" ||
                   Name == "frexp" ||
                   Name == "frexpf" ||
                   Name == "frexpl" ||
                   Name == "fstatvfs") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 2);
        } else if (Name == "fgets") {
          if (FTy->getNumParams() != 3 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(2)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 3);
        } else if (Name == "fread" ||
                   Name == "fwrite") {
          if (FTy->getNumParams() != 4 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(3)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 4);
        } else if (Name == "fputs" ||
                   Name == "fscanf" ||
                   Name == "fprintf" ||
                   Name == "fgetpos") {
          if (FTy->getNumParams() < 2 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        }
        break;
      case 'g':
        if (Name == "getc" ||
            Name == "getlogin_r" ||
            Name == "getc_unlocked") {
          if (FTy->getNumParams() == 0 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        } else if (Name == "getenv") {
          if (FTy->getNumParams() != 1 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setOnlyReadsMemory(F);
          setDoesNotCapture(F, 1);
        } else if (Name == "gets" ||
                   Name == "getchar") {
          setDoesNotThrow(F);
        } else if (Name == "getitimer") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 2);
        } else if (Name == "getpwnam") {
          if (FTy->getNumParams() != 1 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        }
        break;
      case 'u':
        if (Name == "ungetc") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 2);
        } else if (Name == "uname" ||
                   Name == "unlink" ||
                   Name == "unsetenv") {
          if (FTy->getNumParams() != 1 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        } else if (Name == "utime" ||
                   Name == "utimes") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        }
        break;
      case 'p':
        if (Name == "putc") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 2);
        } else if (Name == "puts" ||
                   Name == "printf" ||
                   Name == "perror") {
          if (FTy->getNumParams() != 1 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        } else if (Name == "pread" ||
                   Name == "pwrite") {
          if (FTy->getNumParams() != 4 ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          // May throw; these are valid pthread cancellation points.
          setDoesNotCapture(F, 2);
        } else if (Name == "putchar") {
          setDoesNotThrow(F);
        } else if (Name == "popen") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getReturnType()->isPointerTy() ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotAlias(F, 0);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        } else if (Name == "pclose") {
          if (FTy->getNumParams() != 1 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        }
        break;
      case 'v':
        if (Name == "vscanf") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        } else if (Name == "vsscanf" ||
                   Name == "vfscanf") {
          if (FTy->getNumParams() != 3 ||
              !FTy->getParamType(1)->isPointerTy() ||
              !FTy->getParamType(2)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        } else if (Name == "valloc") {
          if (!FTy->getReturnType()->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotAlias(F, 0);
        } else if (Name == "vprintf") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        } else if (Name == "vfprintf" ||
                   Name == "vsprintf") {
          if (FTy->getNumParams() != 3 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        } else if (Name == "vsnprintf") {
          if (FTy->getNumParams() != 4 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(2)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 3);
        }
        break;
      case 'o':
        if (Name == "open") {
          if (FTy->getNumParams() < 2 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          // May throw; "open" is a valid pthread cancellation point.
          setDoesNotCapture(F, 1);
        } else if (Name == "opendir") {
          if (FTy->getNumParams() != 1 ||
              !FTy->getReturnType()->isPointerTy() ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotAlias(F, 0);
          setDoesNotCapture(F, 1);
        }
        break;
      case 't':
        if (Name == "tmpfile") {
          if (!FTy->getReturnType()->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotAlias(F, 0);
        } else if (Name == "times") {
          if (FTy->getNumParams() != 1 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        }
        break;
      case 'h':
        if (Name == "htonl" ||
            Name == "htons") {
          setDoesNotThrow(F);
          setDoesNotAccessMemory(F);
        }
        break;
      case 'n':
        if (Name == "ntohl" ||
            Name == "ntohs") {
          setDoesNotThrow(F);
          setDoesNotAccessMemory(F);
        }
        break;
      case 'l':
        if (Name == "lstat") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        } else if (Name == "lchown") {
          if (FTy->getNumParams() != 3 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        }
        break;
      case 'q':
        if (Name == "qsort") {
          if (FTy->getNumParams() != 4 ||
              !FTy->getParamType(3)->isPointerTy())
            continue;
          // May throw; places call through function pointer.
          setDoesNotCapture(F, 4);
        }
        break;
      case '_':
        if (Name == "__strdup" ||
            Name == "__strndup") {
          if (FTy->getNumParams() < 1 ||
              !FTy->getReturnType()->isPointerTy() ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotAlias(F, 0);
          setDoesNotCapture(F, 1);
        } else if (Name == "__strtok_r") {
          if (FTy->getNumParams() != 3 ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 2);
        } else if (Name == "_IO_getc") {
          if (FTy->getNumParams() != 1 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        } else if (Name == "_IO_putc") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 2);
        }
        break;
      case 1:
        if (Name == "\1__isoc99_scanf") {
          if (FTy->getNumParams() < 1 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        } else if (Name == "\1stat64" ||
                   Name == "\1lstat64" ||
                   Name == "\1statvfs64" ||
                   Name == "\1__isoc99_sscanf") {
          if (FTy->getNumParams() < 1 ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        } else if (Name == "\1fopen64") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getReturnType()->isPointerTy() ||
              !FTy->getParamType(0)->isPointerTy() ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotAlias(F, 0);
          setDoesNotCapture(F, 1);
          setDoesNotCapture(F, 2);
        } else if (Name == "\1fseeko64" ||
                   Name == "\1ftello64") {
          if (FTy->getNumParams() == 0 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 1);
        } else if (Name == "\1tmpfile64") {
          if (!FTy->getReturnType()->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotAlias(F, 0);
        } else if (Name == "\1fstat64" ||
                   Name == "\1fstatvfs64") {
          if (FTy->getNumParams() != 2 ||
              !FTy->getParamType(1)->isPointerTy())
            continue;
          setDoesNotThrow(F);
          setDoesNotCapture(F, 2);
        } else if (Name == "\1open64") {
          if (FTy->getNumParams() < 2 ||
              !FTy->getParamType(0)->isPointerTy())
            continue;
          // May throw; "open" is a valid pthread cancellation point.
          setDoesNotCapture(F, 1);
        }
        break;
    }
  }
  return Modified;
}

// TODO:
//   Additional cases that we need to add to this file:
//
// cbrt:
//   * cbrt(expN(X))  -> expN(x/3)
//   * cbrt(sqrt(x))  -> pow(x,1/6)
//   * cbrt(sqrt(x))  -> pow(x,1/9)
//
// cos, cosf, cosl:
//   * cos(-x)  -> cos(x)
//
// exp, expf, expl:
//   * exp(log(x))  -> x
//
// log, logf, logl:
//   * log(exp(x))   -> x
//   * log(x**y)     -> y*log(x)
//   * log(exp(y))   -> y*log(e)
//   * log(exp2(y))  -> y*log(2)
//   * log(exp10(y)) -> y*log(10)
//   * log(sqrt(x))  -> 0.5*log(x)
//   * log(pow(x,y)) -> y*log(x)
//
// lround, lroundf, lroundl:
//   * lround(cnst) -> cnst'
//
// pow, powf, powl:
//   * pow(exp(x),y)  -> exp(x*y)
//   * pow(sqrt(x),y) -> pow(x,y*0.5)
//   * pow(pow(x,y),z)-> pow(x,y*z)
//
// puts:
//   * puts("") -> putchar("\n")
//
// round, roundf, roundl:
//   * round(cnst) -> cnst'
//
// signbit:
//   * signbit(cnst) -> cnst'
//   * signbit(nncst) -> 0 (if pstv is a non-negative constant)
//
// sqrt, sqrtf, sqrtl:
//   * sqrt(expN(x))  -> expN(x*0.5)
//   * sqrt(Nroot(x)) -> pow(x,1/(2*N))
//   * sqrt(pow(x,y)) -> pow(|x|,y*0.5)
//
// stpcpy:
//   * stpcpy(str, "literal") ->
//           llvm.memcpy(str,"literal",strlen("literal")+1,1)
// strrchr:
//   * strrchr(s,c) -> reverse_offset_of_in(c,s)
//      (if c is a constant integer and s is a constant string)
//   * strrchr(s1,0) -> strchr(s1,0)
//
// strpbrk:
//   * strpbrk(s,a) -> offset_in_for(s,a)
//      (if s and a are both constant strings)
//   * strpbrk(s,"") -> 0
//   * strpbrk(s,a) -> strchr(s,a[0]) (if a is constant string of length 1)
//
// strspn, strcspn:
//   * strspn(s,a)   -> const_int (if both args are constant)
//   * strspn("",a)  -> 0
//   * strspn(s,"")  -> 0
//   * strcspn(s,a)  -> const_int (if both args are constant)
//   * strcspn("",a) -> 0
//   * strcspn(s,"") -> strlen(a)
//
// tan, tanf, tanl:
//   * tan(atan(x)) -> x
//
// trunc, truncf, truncl:
//   * trunc(cnst) -> cnst'
//
//
