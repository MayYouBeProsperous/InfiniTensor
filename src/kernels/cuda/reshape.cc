#include "cuda/cuda_kernel_wihtout_config.h"

namespace infini {
class CopyCuda : public CudaKernelWithoutConfig {
    void compute(const Operator &op,
                 const RuntimeObj *_context) const override {
        auto inData = op->getInputs(0)->getRawDataPtr<void *>();
        auto outData = op->getOutputs()[0]->getRawDataPtr<void *>();
        cudaMemcpyAsync(outData, inData, op->getInputs(0)->getBytes(),
                        cudaMemcpyDeviceToDevice);
    }
};
// reshape/flatten/identity all act as copying from input to output.
REGISTER_KERNEL(Device::CUDA, OpType::Reshape, DataType::Float32, CopyCuda,
                "Reshape_CUDA_Float32");
REGISTER_KERNEL(Device::CUDA, OpType::Reshape, DataType::Int64, CopyCuda,
                "Reshape_CUDA_Int64");
REGISTER_KERNEL(Device::CUDA, OpType::Reshape, DataType::Int32, CopyCuda,
                "Reshape_CUDA_Int32");
REGISTER_KERNEL(Device::CUDA, OpType::Flatten, DataType::Float32, CopyCuda,
                "Flatten_CUDA_Float32");
REGISTER_KERNEL(Device::CUDA, OpType::Identity, DataType::Float32, CopyCuda,
                "Identity_CUDA_Float32");

} // namespace infini
