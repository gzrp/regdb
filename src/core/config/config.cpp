#include "regdb/core/config.hpp"
#include "filesystem.hpp"
#include <fmt/format.h>

namespace regdb {

// 当前实例
duckdb::DatabaseInstance* Config::local_db;

// 获取 schema 名称
std::string Config::get_schema_name() {
    return "regdb";
}

// 获取存储路径
std::filesystem::path Config::get_global_storage_path() {
#ifdef _WIN32
    const char* homeDir = getenv("USERPROFILE");
#else
    const char* homeDir = getenv("HOME");
#endif
    if (homeDir == nullptr) {
        throw std::runtime_error("Could not find home directory");
    }
    return std::filesystem::path(homeDir) / ".duckdb" / "regdb_storage" / "regdb.db";
}

// 获取连接
duckdb::Connection Config::GetLocalConnection(duckdb::DatabaseInstance* db) {
    if (db) {
        Config::local_db = db;
    }
    // 构造连接对象并返回
    duckdb::Connection con(*Config::local_db);
    return con;
}

// 获取全局连接
duckdb::Connection Config::GetGlobalConnection() {
    const duckdb::DuckDB db(Config::get_global_storage_path().string());
    duckdb::Connection con(*db.instance);
    return con;
}

// 设置全局存储路径
void Config::SetupGlobalStorageLocation() {
    const auto regdb_global_path = get_global_storage_path();
    const auto regdb_dir = regdb_global_path.parent_path();
    if (!std::filesystem::exists(regdb_dir)) {
        try {
            std::filesystem::create_directory(regdb_dir);
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error creating directories: " << e.what() << std::endl;
        }
    }
}

// 配置 schema
void Config::ConfigSchema(duckdb::Connection& con, std::string& schema_name) {
    auto result = con.Query(duckdb_fmt::format(" SELECT * "
                                               " FROM information_schema.schemata "
                                               " WHERE schema_name = '{}'; ",
                                               schema_name));
    if (result->RowCount() == 0) {
        // schema 不存在, 创建 schema
        con.Query(duckdb_fmt::format("CREATE SCHEMA {};", schema_name));
    }
}

// 配置全局模式
void Config::ConfigureGlobal() {
    auto con = Config::GetGlobalConnection();
    ConfigureTables(con, ConfigType::GLOBAL);
}

// 配置本地模式, 同时也可以访问持久化
void Config::ConfigureLocal(duckdb::DatabaseInstance& db) {
    auto con = Config::GetLocalConnection(&db);
    ConfigureTables(con, ConfigType::LOCAL);
    con.Query(
        duckdb_fmt::format("ATTACH DATABASE '{}' AS regdb_storage;", Config::get_global_storage_path().string())
    );
}

void Config::ConfigureTables(duckdb::Connection& con, const ConfigType type) {
    con.BeginTransaction();
    std::string schema = Config::get_schema_name();
    ConfigSchema(con, schema);
    ConfigModelSpaceTable(con, schema, type);
    ConfigRegSpaceTable(con, schema, type);
    con.Commit();
}

std::string Config::get_modelspace_table_name() {
    return "REGDB_MODEL_SPACE_TABLE";
}

std::string Config::get_regspace_table_name() {
    return "REGDB_REG_SPACE_TABLE";
}

void Config::ConfigModelSpaceTable(duckdb::Connection& con, std::string& schema_name, const ConfigType type) {
    const std::string table_name = Config::get_modelspace_table_name();
    // 查询表是否存在
    auto result = con.Query(duckdb_fmt::format(" SELECT table_name "
                                               " FROM information_schema.tables "
                                               " WHERE table_schema = '{}' "
                                               " AND table_name = '{}'; ",
                                               schema_name, table_name));
    if (result->RowCount() == 0) {
        // 不存在则创建
        con.Query(duckdb_fmt::format(" INSTALL JSON; "
                                     " LOAD JSON; "
                                     " CREATE TABLE {}.{} ( "
                                     " model_name VARCHAR NOT NULL, "
                                     " model_type VARCHAR NOT NULL, "
                                     " model_args JSON NOT NULL, "
                                     " updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP , "
                                     " PRIMARY KEY model_name "
                                     " ); ",
                                     schema_name, table_name));

        // 测试使用
        if (type == ConfigType::GLOBAL) {
            con.Query(duckdb_fmt::format(" INSERT INTO {}.{} (model_name, model_type, model_args) "
                                         " VALUES ('default', 'MLP', '{{\"in_features\": 10, \"out_features\": 2, \"hidden_features\": [512, 512, 512, 512, 512, 512] }}'); ",
                                         schema_name, table_name));
        }
    }
}


void Config::ConfigRegSpaceTable(duckdb::Connection& con, std::string& schema_name, const ConfigType type) {
    const std::string table_name = Config::get_regspace_table_name();
    // 查询表是否存在
    auto result = con.Query(duckdb_fmt::format(" SELECT table_name "
                                               " FROM information_schema.tables "
                                               " WHERE table_schema = '{}' "
                                               " AND table_name = '{}'; ",
                                               schema_name, table_name));
    if (result->RowCount() == 0) {
        // 不存在则创建
        con.Query(duckdb_fmt::format(" INSTALL JSON; "
                                     " LOAD JSON; "
                                     " CREATE TABLE {}.{} ( "
                                     " reg_space VARCHAR NOT NULL, "
                                     " reg_args JSON NOT NULL, "
                                     " updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP , "
                                     " PRIMARY KEY reg_space "
                                     " ); ",
                                     schema_name, table_name));

        // 测试使用
        if (type == ConfigType::GLOBAL) {
            con.Query(duckdb_fmt::format(" INSERT INTO {}.{} (reg_space, reg_args) "
                                         " VALUES ('default', '{{\"use_weight_decay\": true, \"use_dropout\": true, \"use_bn\": true, \"use_ln\": true, \"use_skip\": true, \"use_data_augment\": true, \"use_swa\": true, \"use_lookahead\": true }}'); ",
                                         schema_name, table_name));
        }
    }
}

// 注册 db
void Config::Configure(duckdb::ExtensionLoader& loader) {
    Registry::Register(loader);
    auto& db = loader.GetDatabaseInstance();
    if (const auto db_path = db.config.options.database_path; db_path != get_global_storage_path().string()) {
        SetupGlobalStorageLocation();
        ConfigureGlobal();
        ConfigureLocal(db);
    }
}

} // namespace regdb