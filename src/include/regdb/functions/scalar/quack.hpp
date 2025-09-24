#pragma once

#include "regdb/functions/scalar/scalar.hpp"

namespace regdb {

class Quack : public ScalarFunctionBase {
public:
    static void ValidateArguments(duckdb::DataChunk& args);
    static std::vector<std::string> Operation(duckdb::DataChunk& args);
    static void Execute(duckdb::DataChunk& args, duckdb::ExpressionState& state, duckdb::Vector& result);
};

} // namespace regdb