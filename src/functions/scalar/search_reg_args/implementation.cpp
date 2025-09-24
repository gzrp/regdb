#include "regdb/functions/scalar/search_reg_args.hpp"

namespace regdb {

// 参数校验
void SearchRegArgs::ValidateArguments(duckdb::DataChunk& args) {
    if (args.ColumnCount() != 3) {
        throw std::runtime_error("SearchRegArgs expects exactly three arguments.");
    }
    if (args.data[0].GetType().id() != duckdb::LogicalTypeId::VARCHAR) {
        throw std::runtime_error("Argument 0 must be of type VARCHAR.");
    }
    if (args.data[1].GetType().id() != duckdb::LogicalTypeId::VARCHAR) {
        throw std::runtime_error("Argument 1 must be of type VARCHAR.");
    }
    if (args.data[2].GetType().id() != duckdb::LogicalTypeId::VARCHAR) {
        throw std::runtime_error("Argument 2 must be of type VARCHAR.");
    }
}

// 逻辑实现
std::vector<std::string> SearchRegArgs::Operation(duckdb::DataChunk& args) {
    ValidateArguments(args);

    // 取第一行的三个参数
    auto v1 = args.data[0].GetValue(0).ToString();
    auto v2 = args.data[1].GetValue(0).ToString();
    auto v3 = args.data[2].GetValue(0).ToString();

    std::vector<std::string> results;
    results.emplace_back(v1 + "_" + v2 + "_" + v3);
    return results;
}

void SearchRegArgs::Execute(duckdb::DataChunk& args, duckdb::ExpressionState& state, duckdb::Vector& result) {
    // valid -> operation
    const auto responses = Quack::Operation(args);
    duckdb::idx_t pos = 0;
    for (const auto &res : responses) {
        result.SetValue(pos++, duckdb::Value(res));
    }
}

} // namespace regdb