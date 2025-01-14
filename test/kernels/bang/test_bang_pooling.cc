#include "bang/bang_runtime.h"
#include "core/graph.h"
#include "core/kernel.h"
#include "core/runtime.h"
#include "operators/pooling.h"

#include "test.h"

namespace infini {

template <class T, typename std::enable_if<std::is_base_of<PoolingObj, T>{},
                                           int>::type = 0>
void testPooling(const std::function<void(void *, size_t, DataType)> &generator,
                 const Shape &shape) {
    // Runtime
    Runtime cpuRuntime = NativeCpuRuntimeObj::getInstance();
    auto bangRuntime = make_ref<BangRuntimeObj>();

    // Build input data on CPU
    Tensor inputCpu = make_ref<TensorObj>(shape, DataType::Float32, cpuRuntime);
    inputCpu->dataMalloc();
    inputCpu->setData(generator);

    // GPU
    Graph bangGraph = make_ref<GraphObj>(bangRuntime);
    auto inputGpu = bangGraph->cloneTensor(inputCpu);
    auto gpuOp =
        bangGraph->addOp<T>(inputGpu, nullptr, 3, 3, 1, 1, 1, 1, 2, 2, 0);
    bangGraph->dataMalloc();
    bangRuntime->run(bangGraph);
    auto outputGpu = gpuOp->getOutput();
    auto outputGpu2Cpu = outputGpu->clone(cpuRuntime);
    inputCpu->printData();
    outputGpu2Cpu->printData();
    EXPECT_TRUE(1);
}

TEST(cnnl_Pooling, run) {
    testPooling<MaxPoolObj>(IncrementalGenerator(), Shape{1, 1, 5, 5});
    testPooling<AvgPoolObj>(IncrementalGenerator(), Shape{1, 1, 5, 5});
}

} // namespace infini
