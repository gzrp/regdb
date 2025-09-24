#pragma once

#include "filesystem.hpp"
#include "regdb/core/common.hpp"
#include "regdb/registry/registry.hpp"
#include <fmt/format.h>

namespace regdb {

enum ConfigType {
	LOCAL,										// 临时内存模式
	GLOBAL										// 全局存储模式
};

class Config {
public:
	static duckdb::DatabaseInstance* local_db;													// 临时内存模式实例
	static duckdb::DatabaseInstance* global_db;													// 全局存储模式实例

 	static duckdb::Connection GetLocalConnection(duckdb::DatabaseInstance* db = nullptr);		// 获取当前临时内存实例连接
	static duckdb::Connection GetGlobalConnection();											// 获取全局存储模式实例连接

	static void Configure(duckdb::DatabaseInstance& db);										// 对实例进行配置
	static void ConfigureLocal(duckdb::DatabaseInstance& db);									// 对临时内存模式实例进行配置
	static void ConfigureGlobal();																// 对全局存储模式实例进行配置
	static void ConfigureTables(duckdb::Connection& con, ConfigType type);						// 对表进行配置

	static std::string get_schema_name();														// 获取 schema 名称
	static std::filesystem::path get_global_storage_path();										// 获取全局存储模型路径
	static std::string get_modelspace_table_name();												// 获取模型表名称
	static std::string get_regspace_table_name();												// 获取正则化表名称

private:
	static void SetupGlobalStorageLocation();																// 设置全局存储路径
	static void ConfigSchema(duckdb::Connection& con, std::string& schema_name);							// 配置 schema 名称
	static void ConfigModelSpaceTable(duckdb::Connection& con, std::string& schema_name, ConfigType type);	// 配置模型空间表
	static void ConfigRegSpaceTable(duckdb::Connection& con, std::string& schema_name, ConfigType type);	// 配置正则化空间表

}; // class Config

} // namespace regdb