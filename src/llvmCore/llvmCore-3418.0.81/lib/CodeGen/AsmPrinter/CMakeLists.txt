add_llvm_library(LLVMAsmPrinter
  ARMException.cpp
  AsmPrinter.cpp
  AsmPrinterDwarf.cpp
  AsmPrinterInlineAsm.cpp
  DIE.cpp
  DwarfAccelTable.cpp
  DwarfCFIException.cpp
  DwarfCompileUnit.cpp
  DwarfDebug.cpp
  DwarfException.cpp
  OcamlGCPrinter.cpp
  Win64Exception.cpp
  )

add_llvm_library_dependencies(LLVMAsmPrinter
  LLVMAnalysis
  LLVMCodeGen
  LLVMCore
  LLVMMC
  LLVMMCParser
  LLVMSupport
  LLVMTarget
  )
