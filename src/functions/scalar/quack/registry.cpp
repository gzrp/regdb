#include "regdb/functions/scalar/quack.hpp"
#include "regdb/registry/registry.hpp"

namespace regdb {

void ScalarRegistry::RegisterQuack(duckdb::ExtensionLoader& loader) {
    loader::RegisterFunction(
        duckdb::ScalarFunction(
            "quack",
            {
                duckdb::LogicalType::VARCHAR
            },
            duckdb::LogicalType::VARCHAR,
            Quack::Execute
        )
    );
}

} // namespace regdb