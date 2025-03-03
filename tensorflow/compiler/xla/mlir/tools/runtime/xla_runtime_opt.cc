/* Copyright 2022 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "mlir/InitAllDialects.h"  // from @llvm-project
#include "mlir/Tools/mlir-opt/MlirOptMain.h"  // from @llvm-project
#include "tensorflow/compiler/xla/mlir/transforms/math/passes.h"
#include "tensorflow/compiler/xla/mlir/transforms/memref/passes.h"
#include "tensorflow/compiler/xla/mlir/transforms/runtime/rt_passes.h"

int main(int argc, char **argv) {
  mlir::DialectRegistry registry;
  mlir::registerAllDialects(registry);

  registry.insert<xla::runtime::RuntimeDialect>();
  xla::runtime::registerMathTransformsPasses();
  xla::runtime::registerMemrefTransformsPasses();
  xla::runtime::registerRuntimeTransformsPasses();

  return failed(MlirOptMain(argc, argv, "Xla Runtime Pass Driver\n", registry));
}
