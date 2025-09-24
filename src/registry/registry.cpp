#include "regdb/registry/registry.hpp"

namespace regdb {

void Registry::Register(duckdb::DatabaseInstance& db) {
    RegisterScalarFunctions(db);
}

void Registry::RegisterScalarFunctions(duckdb::DatabaseInstance& db) {
    ScalarRegistry::Register(db);
}

} // namespace regdb