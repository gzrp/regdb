#pragma once

#include <any>
#include <nlohmann/json.hpp>
#include "regdb/core/common.hpp"

namespace geofer {

class ScalarFunctionBase {
public:
    ScalarFunctionBase() = delete;

    static void ValidateArguments(duckdb::DataChunk& args);
    static std::vector<std::any> Operation(duckdb::DataChunk& args);
    static void Execute(duckdb::DataChunk& args, duckdb::ExpressionState& state, duckdb::Vector& result);
};

} // namespace regdb