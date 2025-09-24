#include "regdb/custom_parser/query_parser.hpp"

#include "regdb/core/common.hpp"

#include <sstream>
#include <stdexcept>

namespace regdb {

std::string QueryParser::ParseQuery(const std::string& query  ) {
    Tokenizer tokenizer(query);
    auto token = tokenizer.NextToken();
    auto value = duckdb::StringUtil::Upper(token.value);
    if (token.type != TokenType::KEYWORD ||
        (value != "CREATE" && value != "DELETE" && value != "UPDATE" && value != "GET")) {
        throw std::runtime_error(duckdb_fmt::format("Unknown keyword: {}", token.value));
    }
    return ParseRegSpaceOrModel(tokenizer, query);
}

inline std::string QueryParser::ParseRegSpaceOrModel(Tokenizer tokenizer, const std::string& query) {
    auto token = tokenizer.NextToken();
    auto value = duckdb::StringUtil::Upper(token.value);
    if (token.type == TokenType::KEYWORD && (value == "MODEL" || value == "MODELS")) {
        ModelParser model_parser;
        model_parser.Parse(query, statement);
        return model_parser.ToSQL(*statement);
    } else if (token.type == TokenType::KEYWORD && (value == "REGSPACE" || value == "REGSPACES")) {
        RegSpaceParser regspace_parser;
        regspace_parser.Parse(query, statement);
        return regspace_parser.ToSQL(*statement);
    } else if (token.type == TokenType::KEYWORD && (value == "GLOBAL" || value == "LOCAL")) {
        return ParseRegSpaceOrModel(tokenizer, query);
    } else {
        throw std::runtime_error(duckdb_fmt::format("Unknown keyword: {}", token.value));
    }
}

} // namespace regdb