#include "operators/split.h"
#include "utils/operator_utils.h"
#include <numeric>

namespace infini {
SplitObj::SplitObj(GraphObj *graph, Tensor input,
                   std::optional<TensorVec> outputs, int dim, int num)
    : OperatorObj(OpType::Split, {input},
                  ((!outputs) ? TensorVec(num, nullptr) : std::move(*outputs))),
      dim(get_real_axis(dim, input->getRank())), num(num), ratio({}) {
    int dimSize = input->getDims().at(this->dim);
    int pieceSize = dimSize / num;
    int lastSize = dimSize - pieceSize * num;

    if (lastSize > 0) {
        ratio = std::vector<int>(num - 1, pieceSize);
        ratio.emplace_back(lastSize + pieceSize);
    } else
        ratio = std::vector<int>(num, pieceSize);

    IT_ASSERT(checkValid(graph));
}

SplitObj::SplitObj(GraphObj *graph, Tensor input,
                   std::optional<TensorVec> outputs, int dim,
                   const vector<int> &ratio)
    : OperatorObj(OpType::Split, {input},
                  ((!outputs) ? TensorVec{nullptr} : (*outputs))),
      dim(get_real_axis(dim, input->getRank())), num(-1), ratio(ratio) {
    num = ratio.size();
    if (!outputs) {
        TensorVec tmp(num, nullptr);
        this->outputs = tmp;
    }
    IT_ASSERT(checkValid(graph));
}

optional<vector<Shape>> SplitObj::inferShape(const TensorVec &inputs) const {
    IT_ASSERT(num != -1 && ratio.size() != 0);
    auto inputDims = inputs[0]->getDims();
    int totalSize = inputDims.at(dim);
    int ratioSum = std::accumulate(ratio.begin(), ratio.end(), 0);
    IT_ASSERT(totalSize % ratioSum == 0);

    int pieceSize = totalSize / ratioSum;

    vector<Shape> ret;
    Shape outShape = inputDims;
    for (int i = 0; i < num; i++) {
        outShape[dim] = pieceSize * ratio.at(i);
        ret.push_back(outShape);
    }
    return {ret};
}

vector<int> SplitObj::getWorkloadVector() const {
    vector<int> ret = inputs[0]->getDims();
    ret.emplace(ret.begin(), type.underlying());
    ret.emplace_back(dim);
    ret.emplace_back(num);
    return ret;
}

vector<int> SplitObj::getOpAttrVector() const {
    return {type.underlying(), dim, num};
}

string SplitObj::toString() const {
    std::ostringstream os;
    os << "Split[" << getGuid() << "]";
    os << "(";
    os << vecToString(inputs[0]->getDims()) << ",";
    os << "dim=" << dim << ",";
    os << "num= " << num << ",";
    os << "ratio= " << vecToString(ratio) << ",";
    os << "input=" << inputs[0]->getGuid() << ",";
    os << "output=";
    for (auto i = 0; i < num; i++)
        os << outputs[i]->getGuid() << ",";
    os << ")";
    return os.str();
}

} // namespace infini
