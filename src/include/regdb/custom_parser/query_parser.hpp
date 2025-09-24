#pragma once

#include "regdb/core/common.hpp"
#include "regdb/custom_parser/query/model_parser.hpp"
#include "regdb/custom_parser/query/regspace_parser.hpp"
#include "regdb/custom_parser/query_statements.hpp"
#include "regdb/custom_parser/tokenizer.hpp"

#include "fmt/format.h"
#include <memory>
#include <string>
#include <vector>

namespace regdb {

class QueryParser {
public:
    std::string ParseQuery(const std::string& query);
    std::string ParseRegSpaceOrModel(Tokenizer tokenizer, const std::string& query);

private:
    std::unique_ptr<QueryStatement> statement;
};

} // namespace regdb