#pragma once

#include "regdb/core/common.hpp"
#include "regdb/registry/scalar.hpp"

namespace regdb {

// 注册函数统一入口
class Registry {
public:
    static void Register(duckdb::ExtensionLoader& loader);

private:
    static void RegisterScalarFunctions(duckdb::ExtensionLoader& loader);
};

} // namesapce regdb


