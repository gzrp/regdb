#pragma once

#include "regdb/core/common.hpp"
#include "regdb/registry/scalar.hpp"

namespace regdb {

// 标量函数统一注册入口
class ScalarRegistry {
public:
    static void Register(duckdb::ExtensionLoader& loader);

private:
    static void RegisterQuack(duckdb::ExtensionLoader& loader);
    static void RegisterSearchRegArgs(duckdb::ExtensionLoader& loader);
};

} // namesapce regdb


