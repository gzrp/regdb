#define DUCKDB_EXTENSION_MAIN

#include "regdb_extension.hpp"
#include "regdb/core/common.hpp"
#include "regdb/core/config.hpp"


/**
*  加载扩展 regdb_init  -> RegdbExtension::Load -> LoadInternal -> 将 func, DuckParserExtension 和 DuckOperatorExtension 塞入 DBConfig
*  解析阶段原生 parser 解析失败，会依次调用 parser_extensions 里的每个 parse_function, 即 duck_parse, 内部使用 regdb::QueryParse 翻译成普通 SQL, 再让原生 Parser 解析
*  计划阶段 回调 duck_plan，先把 parse_data 缓存到 Context.registered_state
*  绑定阶段 Binder 遇到 EXTENSION_STATEMENT 时轮询 operator_extensions，调用其中 bind（这里是 duck_bind）。
* duck_bind 取回刚才缓存的原生 SQLStatement，用内部 Binder 绑定成 BoundStatement 并返回。
* 绑定通过后，后续优化器 & 执行器就当它是一条普通 SQL 处理。
*/

namespace duckdb {

// 注册解析器
static void LoadInternal(ExtensionLoader& loader) {
    // 注册函数
    regdb::Config::Configure(loader);

    // Register the custom parser
 //   auto& config = DBConfig::GetConfig(loader.GetDatabaseInstance());
 //   DuckParserExtension duck_parser;
 //   config.parser_extensions.push_back(duck_parser);
 //   config.operator_extensions.push_back(make_uniq<DuckOperatorExtension>());
}

/*
ParserExtensionParseResult duck_parse(ParserExtensionInfo*, const std::string& query) {
    flock::QueryParser query_parser;

    // Translate and print SQL queries for each input query
    std::string sql_query = query_parser.ParseQuery(query);

    // Parse and return the statement using DuckDB's parser
    Parser parser;
    parser.ParseQuery(sql_query);
    auto statements = std::move(parser.statements);

    return ParserExtensionParseResult(
            make_uniq_base<ParserExtensionParseData, DuckParseData>(std::move(statements[0])));
}

ParserExtensionPlanResult duck_plan(ParserExtensionInfo*, ClientContext& context,
                                    unique_ptr<ParserExtensionParseData> parse_data) {
    if (auto state = context.registered_state->Get<DuckState>("duck")) {
        context.registered_state->Remove("duck");
    }
    context.registered_state->GetOrCreate<DuckState>("duck", std::move(parse_data));
    throw BinderException("Use duck_bind instead");
}

BoundStatement duck_bind(ClientContext& context, Binder& binder, OperatorExtensionInfo* info, SQLStatement& statement) {
    switch (statement.type) {
        case StatementType::EXTENSION_STATEMENT: {
            auto& extension_statement = dynamic_cast<ExtensionStatement&>(statement);
            if (extension_statement.extension.parse_function == duck_parse) {
                if (const auto duck_state = context.registered_state->Get<DuckState>("duck")) {
                    const auto duck_binder = Binder::CreateBinder(context, &binder);
                    const auto duck_parse_data = dynamic_cast<DuckParseData*>(duck_state->parse_data.get());
                    auto bound_statement = duck_binder->Bind(*(duck_parse_data->statement));
                    BoundStatement result;
                    return bound_statement;
                }
                throw BinderException("Registered state not found");
            }
        }
        default:
            // No-op empty
            return {};
    }
}

*/

void RegdbExtension::Load(ExtensionLoader &loader) {
    LoadInternal(loader);
}

std::string RegdbExtension::Name() {
    return "regdb";
}

std::string RegdbExtension::Version() const {
    return "v0.0.1";
}

} // namespace duckdb

extern "C" {

DUCKDB_CPP_EXTENSION_ENTRY(regdb, loader) {
    duckdb::LoadInternal(loader);
}
}