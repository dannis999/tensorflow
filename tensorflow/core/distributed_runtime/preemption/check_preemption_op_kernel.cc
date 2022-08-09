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

#include <string>

#include "absl/strings/string_view.h"
#include "tensorflow/core/distributed_runtime/coordination/coordination_service_agent.h"
#include "tensorflow/core/distributed_runtime/error_payloads.h"
#include "tensorflow/core/framework/op_kernel.h"
#include "tensorflow/core/framework/op_requires.h"
#include "tensorflow/core/framework/types.h"
#include "tensorflow/core/platform/errors.h"

namespace tensorflow {

// Kernel that gets preemption notice using coordination service.
class CheckPreemptionOp : public OpKernel {
 public:
  explicit CheckPreemptionOp(OpKernelConstruction* ctx) : OpKernel(ctx) {
    OP_REQUIRES_OK(ctx, ctx->GetAttr("preemption_key", &preemption_key_));
  }

  void Compute(OpKernelContext* ctx) override {
    auto status_or_task =
        ctx->coordination_service_agent()->TryGetKeyValue(preemption_key_);

    // No-op if preemption key is not found.
    if (errors::IsNotFound(status_or_task.status())) {
      return;
    }

    // Raise if TryGetKeyValue returns other error status.
    OP_REQUIRES_OK(ctx, status_or_task.status());

    // Preemption key is set, meaning a task has been preempted.
    const std::string& preempted_task = status_or_task.ValueOrDie();
    LOG(INFO) << "Preemption reported by task: " << preempted_task;
    OP_REQUIRES_OK(ctx,
                   errors::AbortedWithPayloads(
                       absl::StrCat("Task ", preempted_task, " was preempted."),
                       {{kWorkerPreemption, preempted_task}}));
  }

 private:
  std::string preemption_key_;
};

REGISTER_KERNEL_BUILDER(Name("CheckPreemption").Device(DEVICE_CPU),
                        CheckPreemptionOp);

}  // namespace tensorflow
