#include "regdb/functions/scalar/quack.hpp"

namespace regdb {

// 参数校验
void Quack::ValidateArguments(duckdb::DataChunk& args) {
    if (args.ColumnCount() != 1) {
        throw std::runtime_error("Quack expects exactly one argument.");
    }
    if (args.data[0].GetType().id() != duckdb::LogicalTypeId::VARCHAR) {
        throw std::runtime_error("Argument must be of type VARCHAR.");
    }
}

// 逻辑实现
std::vector<std::string> Quack::Operation(duckdb::DataChunk& args) {
    ValidateArguments(args);

    std::vector<std::string> results;
    results.reserve(args.size());

    auto &name_vector = args.data[0];

    duckdb::UnifiedVectorFormat format;
    name_vector.ToUnifiedFormat(args.size(), format);
    auto data = reinterpret_cast<duckdb::string_t *>(format.data);

    for (duckdb::idx_t row = 0; row < args.size(); ++row) {
        if (!format.validity.RowIsValid(row)) {
            results.emplace_back("");                      // 也可以直接写入 "NULL"
            continue;
        }
        auto idx   = format.sel->get_index(row);
        auto name  = data[idx].GetString();
        results.emplace_back("Quack " + name + " Quack");
    }
    return results;
}

void Quack::Execute(duckdb::DataChunk& args, duckdb::ExpressionState& state, duckdb::Vector& result) {
    // valid -> operation
    const auto responses = Quack::Operation(args);
    duckdb::idx_t pos = 0;
    for (const auto &res : responses) {
        result.SetValue(pos++, duckdb::Value(res));
    }
}

} // namespace regdb