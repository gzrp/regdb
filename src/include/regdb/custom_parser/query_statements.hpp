#pragma once

#include "regdb/core/common.hpp"

#include <memory>
#include <string>
#include <vector>

namespace regdb {

// 语句类型
enum class StatementType {
    CREATE_MODEL,
    DELETE_MODEL,
    UPDATE_MODEL,
    UPDATE_MODEL_SCOPE,
    GET_MODEL,
    GET_ALL_MODEL,
    CREATE_REGSPACE,
    DELETE_REGSPACE,
    UPDATE_REGSPACE,
    UPDATE_REGSPACE_SCOPE,
    GET_REGSPACE,
    GET_ALL_REGSPACE,
};

// 语句抽象基类
class QueryStatement {
public:
    virtual ~QueryStatement() = default;
    StatementType type;
};

// CREATE DUCK
class CreateDuckStatement: public QueryStatement {
public:
    std::string duck_string;
};

} // namespace regdb
