cmake_minimum_required(VERSION 3.14)

project(AnalyzerLibrary LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(AnalyzerLibrary SHARED
  analyzer.h
  analyzer_global.h
  src/analyzer.cpp
  src/BasicBlock.cpp
  src/ControlFlowGraph.cpp
  src/ExpressionParser.cpp
  src/ExpressionTokenizer.cpp
  src/FileReader.cpp
  src/InstrParser.cpp
  src/Logger.cpp
  src/Stmt.cpp
  src/SymbolTable.cpp
  src/Tokenizer.cpp
  src/Traverser.cpp
  src/Visitor.cpp
  src/ComputeReachingDefsVisitor.cpp
  src/Utils.cpp
  src/Expr.h
  src/BasicBlock.h
  src/ControlFlowGraph.h
  src/ExpressionParser.h
  src/ExpressionTokenizer.h
  src/FileReader.h
  src/InstrParser.h
  src/Logger.h
  src/Stmt.h
  src/SymbolTable.h
  src/Tokenizer.h
  src/Traverser.h
  src/Visitor.h
  src/ComputeReachingDefsVisitor.h
  src/Utils.h
  src/Results.h
)

target_compile_definitions(AnalyzerLibrary PRIVATE ANALYZERLIBRARY_LIBRARY)
