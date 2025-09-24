#pragma once

#include "regdb/core/common.hpp"
#include "regdb/custom_parser/query_statements.hpp"
#include "regdb/custom_parser/tokenizer.hpp"

#include "fmt/format.h"
#include <memory>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace regdb {

// 创建 regspace 语句
class CreateRegSpaceStatement : public QueryStatement {
public:
    CreateRegSpaceStatement() { type = StatementType::CREATE_REGSPACE; };
    std::string catalog;
    std::string reg_space;
    nlohmann::json reg_args;
};

class DeleteRegSpaceStatement : public QueryStatement {
public:
    DeleteRegSpaceStatement() { type = StatementType::DELETE_REGSPACE; };
    std::string reg_space;
};

class UpdateRegSpaceScopeStatement : public QueryStatement {
public:
    UpdateRegSpaceScopeStatement() { type = StatementType::UPDATE_REGSPACE_SCOPE; };
    std::string catalog;
    std::string reg_space;
};

class UpdateRegSpaceStatement : public QueryStatement {
public:
    UpdateRegSpaceStatement() { type = StatementType::UPDATE_REGSPACE; };
    std::string reg_space;
    std::string new_reg_args;
};

class GetRegSpaceStatement : public QueryStatement {
public:
    GetRegSpaceStatement() { type = StatementType::GET_REGSPACE; };
    std::string reg_space;
};

class GetAllRegSpaceStatement : public QueryStatement {
public:
    GetAllRegSpaceStatement() { type = StatementType::GET_ALL_REGSPACE; };
};

class RegSpaceParser {
public:
    void Parse(const std::string& query, std::unique_ptr<QueryStatement>& query_statement);
    std::string ToSQL(const QueryStatement& statement) const;

private:
    void ParseCreateRegSpace(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement);
    void ParseDeleteRegSpace(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement);
    void ParseUpdateRegSpace(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement);
    void ParseGetRegSpace(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement);
};

} // namespace regdb
