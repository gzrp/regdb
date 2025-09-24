#include "regdb/registry/registry.hpp"

namespace regdb {

void Registry::Register(duckdb::ExtensionLoader& loader) {
    RegisterScalarFunctions(loader);
}

void Registry::RegisterScalarFunctions(duckdb::ExtensionLoader& loader) {
    ScalarRegistry::Register(loader);
}

} // namespace regdb