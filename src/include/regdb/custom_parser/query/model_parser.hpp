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

// 创建模型语句
class CreateModelStatement : public QueryStatement {
public:
    CreateModelStatement() { type = StatementType::CREATE_MODEL; }
    std::string catalog;
    std::string model_name;
    std::string model_type;
    nlohmann::json model_args;
};

// 删除模型语句
class DeleteModelStatement : public QueryStatement {
public:
    DeleteModelStatement() { type = StatementType::DELETE_MODEL; }
    std::string model_name;
};

// 更新模型Scope语句
class UpdateModelScopeStatement : public QueryStatement {
public:
    UpdateModelScopeStatement() { type = StatementType::UPDATE_MODEL_SCOPE; }
    std::string model_name;
    std::string catalog;
};

class UpdateModelStatement : public QueryStatement {
public:
    UpdateModelStatement() { type = StatementType::UPDATE_MODEL; }
    std::string model_name;
    std::string model_type;
    nlohmann::json new_model_args;
};

class GetModelStatement : public QueryStatement {
public:
    GetModelStatement() { type = StatementType::GET_MODEL; }
    std::string model_name;
    std::string model_type;
};

class GetAllModelStatement : public QueryStatement {
public:
    GetAllModelStatement() { type = StatementType::GET_ALL_MODEL; }
};

class ModelParser {
public:
    void Parse(const std::string& query, std::unique_ptr<QueryStatement>& statement);
    std::string ToSQL(const QueryStatement& statement) const;

private:
    void ParseCreateModel(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement);
    void ParseDeleteModel(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement);
    void ParseUpdateModel(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement);
    void ParseGetModel(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement);
};

} // namespace regdb