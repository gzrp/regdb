#include "regdb/functions/scalar/search_reg_args.hpp"
#include "regdb/registry/registry.hpp"

namespace regdb {

void ScalarRegistry::RegisterSearchRegArgs(duckdb::ExtensionLoader& loader) {
    loader.RegisterFunction(
        duckdb::ScalarFunction(
            "search_reg_args",
            {
                duckdb::LogicalType::VARCHAR,    // model
                duckdb::LogicalType::VARCHAR,    // regspace
                duckdb::LogicalType::VARCHAR,    // time threshold  120s/5m/1h
            },
            duckdb::LogicalType::VARCHAR,
            SearchRegArgs::Execute
        )
    );
}

} // namespace regdb