#pragma once

#include "regdb/core/common.hpp"
#include "regdb/registry/scalar.hpp"

namespace regdb {

// 注册函数统一入口
class Registry {
public:
    static void Register(duckdb::DatabaseInstance& db);

private:
    static void RegisterScalarFunctions(duckdb::DatabaseInstance& db);
};

} // namesapce regdb


