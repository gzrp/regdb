#pragma once

#include "regdb/core/common.hpp"
#include "regdb/registry/scalar.hpp"

namespace regdb {

// 标量函数统一注册入口
class ScalarRegistry {
public:
    static void Register(duckdb::DatabaseInstance& db);

private:
    static void RegisterQuack(duckdb::DatabaseInstance& db);
};

} // namesapce regdb


