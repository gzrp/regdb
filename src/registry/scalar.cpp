#include "regdb/registry/scalar.hpp"

namespace regdb {

// Register 方法实现，注册所有的私有函数
void ScalarRegistry::Register(duckdb::DatabaseInstance& db) {
    RegisterQuack(db);
}

} // namespace regdb