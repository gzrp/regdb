#include "regdb/functions/scalar/quack.hpp"
#include "regdb/registry/registry.hpp"

namespace regdb {

void ScalarRegistry::RegisterQuack(duckdb::DatabaseInstance& db) {
    duckdb::ExtensionUtil::RegisterFunction(
        db,
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