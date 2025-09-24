#include "regdb/custom_parser/query/regspace_parser.hpp"

#include "regdb/core/config.hpp"
#include "regdb/core/common.hpp"

#include <sstream>
#include <stdexcept>

namespace regdb {

void RegSpaceParser::Parse(const std::string& query, std::unique_ptr<QueryStatement>& statement) {
    Tokenizer tokenizer(query);
    auto token = tokenizer.NextToken();
    auto value = duckdb::StringUtil::Upper(token.value);

    if (token.type == TokenType::KEYWORD) {
        if (value == "CREATE") {
            ParseCreateRegSpace(tokenizer, statement);
        } else if (value == "DELETE") {
            ParseDeleteRegSpace(tokenizer, statement);
        } else if (value == "UPDATE") {
            ParseUpdateRegSpace(tokenizer, statement);
        } else if (value == "GET") {
            ParseGetRegSpace(tokenizer, statement);
        } else {
            throw std::runtime_error("Unknown keyword: " + token.value);
        }
    } else {
        throw std::runtime_error("Unknown keyword: " + token.value);
    }
}

void RegSpaceParser::ParseCreateRegSpace(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement) {
    auto token = tokenizer.NextToken();
    auto value = duckdb::StringUtil::Upper(token.value);

    std::string catalog;
    // CREATE GLOBAL|LOCAL REGSPACE ( reg_space, reg_args);
    if (token.type == TokenType::KEYWORD && (value == "GLOBAL" || value == "LOCAL")) {
        if (value == "GLOBAL") {
            catalog = "regdb_storage.";
        }
        token = tokenizer.NextToken();
        value = duckdb::StringUtil::Upper(token.value);
    }

    if (token.type != TokenType::KEYWORD || value != "REGSPACE") {
        throw std::runtime_error("Unknown keyword: " + token.value);
    }

    token = tokenizer.NextToken();
    if (token.type != TokenType::PARENTHESIS || token.value != "(") {
        throw std::runtime_error("Expected opening parenthesis '(' after 'REGSPACE'.");
    }

    token = tokenizer.NextToken();
    if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
        throw std::runtime_error("Expected non-empty string literal for regspace.");
    }
    const auto reg_space = token.value;

    token = tokenizer.NextToken();
    if (token.type != TokenType::SYMBOL || token.value != ",") {
        throw std::runtime_error("Expected comma ',' after regspace.");
    }

    token = tokenizer.NextToken();
    if (token.type != TokenType::JSON || token.value.empty()) {
        throw std::runtime_error("Expected json value for the reg_args.");
    }
    auto reg_args = nlohmann::json::parse(token.value);
    const std::set<std::string> expected_keys = {"use_weight_decay", "use_dropout", "use_bn", "use_ln", "use_skip", "use_data_augment", "use_swa", "use_lookahead"};
    std::set<std::string> json_keys;
    for (auto it = reg_args.begin(); it != reg_args.end(); ++it) {
        json_keys.insert(it.key());
    }
    if (json_keys != expected_keys) {
        throw std::runtime_error("Expected keys: use_weight_decay, use_dropout, use_bn, use_ln, use_skip, use_data_augment, use_swa, use_lookahead in new_reg_args.");
    }
    token = tokenizer.NextToken();
    if (token.type != TokenType::PARENTHESIS || token.value != ")") {
        throw std::runtime_error("Expected closing parenthesis ')' after reg_args.");
    }
    token = tokenizer.NextToken();
    if (token.type == TokenType::END_OF_FILE) {
        auto create_statement = std::make_unique<CreateRegSpaceStatement>();
        create_statement->catalog = catalog;
        create_statement->reg_space = reg_space;
        create_statement->reg_args = reg_args;
        statement = std::move(create_statement);
    } else {
        throw std::runtime_error("Unexpected characters after the closing parenthesis. Only a semicolon is allowed.");
    }
}

void RegSpaceParser::ParseDeleteRegSpace(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement) {
    auto token = tokenizer.NextToken();
    auto value = duckdb::StringUtil::Upper(token.value);
    // DELETE REGSPACE regspace;
    if (token.type != TokenType::KEYWORD || value != "REGSPACE") {
        throw std::runtime_error("Unknown keyword: " + token.value);
    }

    token = tokenizer.NextToken();
    if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
      throw std::runtime_error("Expected non-empty string literal for regspace name.");
    }
    auto reg_space = token.value;

    token = tokenizer.NextToken();
    if (token.type == TokenType::SYMBOL || token.value == ";") {
        auto delete_statement = std::make_unique<DeleteRegSpaceStatement>();
        delete_statement->reg_space = reg_space;
        statement = std::move(delete_statement);
    } else {
        throw std::runtime_error("Unexpected characters after the closing parenthesis. Only a semicolon is allowed.");
    }
}

void RegSpaceParser::ParseUpdateRegSpace(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement) {
    auto token = tokenizer.NextToken();
    auto value = duckdb::StringUtil::Upper(token.value);
    // UPDATE REGSPACE regspace TO GLOBAL | LOCAL;
    // UPDATE REGSPACE(regspace, new_reg_args);
    if (token.type != TokenType::KEYWORD || value != "REGSPACE") {
        throw std::runtime_error("Unknown keyword: " + token.value);
    }
    token = tokenizer.NextToken();
    if (token.type == TokenType::STRING_LITERAL) {
        auto reg_space = token.value;
        token = tokenizer.NextToken();
        if (token.type != TokenType::KEYWORD || duckdb::StringUtil::Upper(token.value) != "TO") {
            throw std::runtime_error("Expected 'TO' after regspace name.");
        }

        token = tokenizer.NextToken();
        value = duckdb::StringUtil::Upper(token.value);
        if (token.type != TokenType::KEYWORD || (value != "GLOBAL" && value != "LOCAL")) {
            throw std::runtime_error("Expected 'GLOBAL' or 'LOCAL' after 'TO'.");
        }
        auto catalog = value == "GLOBAL" ? "regdb_storage." : "";

        token = tokenizer.NextToken();
        if (token.type == TokenType::SYMBOL || token.value == ";") {
            auto update_statement = std::make_unique<UpdateRegSpaceScopeStatement>();
            update_statement->reg_space = reg_space;
            update_statement->catalog = catalog;
            statement = std::move(update_statement);
        } else {
            throw std::runtime_error("Unexpected characters after the closing parenthesis. Only a semicolon is allowed.");
        }
    } else {
        if (token.type != TokenType::PARENTHESIS || token.value != "(") {
            throw std::runtime_error("Expected opening parenthesis '(' after 'REGSPACE'.");
        }

        token = tokenizer.NextToken();
        if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
            throw std::runtime_error("Expected non-empty string literal for regspace.");
        }
        auto reg_space = token.value;

        token = tokenizer.NextToken();
        if (token.type != TokenType::SYMBOL || token.value != ",") {
            throw std::runtime_error("Expected comma ',' after regspace.");
        }

        token = tokenizer.NextToken();
        if (token.type != TokenType::JSON || token.value.empty()) {
            throw std::runtime_error("Expected json value for the new_reg_args.");
        }
        auto new_reg_args = nlohmann::json::parse(token.value);
        const std::set<std::string> expected_keys = {"use_weight_decay", "use_dropout", "use_bn", "use_ln", "use_skip", "use_data_augment", "use_swa", "use_lookahead"};
        std::set<std::string> json_keys;
        for (auto it = new_reg_args.begin(); it != new_reg_args.end(); ++it) {
            json_keys.insert(it.key());
        }
        if (json_keys != expected_keys) {
            throw std::runtime_error("Expected keys: use_weight_decay, use_dropout, use_bn, use_ln, use_skip, use_data_augment, use_swa, use_lookahead in new_reg_args.");
        }

        token = tokenizer.NextToken();
        if (token.type != TokenType::PARENTHESIS || token.value != ")") {
            throw std::runtime_error("Expected closing parenthesis ')' after new_reg_args.");
        }
        token = tokenizer.NextToken();
        if (token.type == TokenType::END_OF_FILE) {
            auto update_statement = std::make_unique<UpdateRegSpaceStatement>();
            update_statement->reg_space = reg_space;
            update_statement->new_reg_args = new_reg_args;
            statement = std::move(update_statement);
        } else {
            throw std::runtime_error("Unexpected characters after the closing parenthesis. Only a semicolon is allowed.");
        }
    }
}

void RegSpaceParser::ParseGetRegSpace(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement) {
    auto token = tokenizer.NextToken();
    auto value = duckdb::StringUtil::Upper(token.value);
    // GET REGSPACES;
    // GET REGSPACE regspace;
    if (token.type != TokenType::KEYWORD || (value != "REGSPACE" && value != "REGSPACES")) {
        throw std::runtime_error("Unknown keyword: " + token.value);
    }

    token = tokenizer.NextToken();
    if ((token.type == TokenType::SYMBOL || token.value == ";") && value == "REGSPACES") {
        auto get_all_statement = duckdb::make_uniq<GetAllRegSpaceStatement>();
        statement = std::move(get_all_statement);
    } else {
        if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
            throw std::runtime_error("Expected non-empty string literal for regspace name.");
        }
        auto reg_space = token.value;

        token = tokenizer.NextToken();
        if (token.type == TokenType::SYMBOL || token.value == ";") {
            auto get_statement = duckdb::make_uniq<GetRegSpaceStatement>();
            get_statement->reg_space = reg_space;
            statement = std::move(get_statement);
        } else {
          throw std::runtime_error("Unexpected characters after the regspace name. Only a semicolon is allowed.");
        }
    }
}

std::string RegSpaceParser::ToSQL(const QueryStatement& statement) const {
    std::string query;
    switch (statement.type) {
    case StatementType::CREATE_REGSPACE: {
        const auto& create_stmt = static_cast<const CreateRegSpaceStatement&>(statement);
        auto con = Config::GetLocalConnection();
        auto result = con.Query(duckdb_fmt::format(" SELECT reg_space "
                                                   " FROM {}regdb_config.REGDB_REG_SPACE_TABLE"
                                                   " WHERE reg_space = '{}';",
                                                   create_stmt.catalog.empty() ? "regdb_storage." : "",
                                                   create_stmt.reg_space));
        if (result->RowCount() != 0) {
            throw std::runtime_error(duckdb_fmt::format("RegSpace '{}' already exist.", create_stmt.reg_space));
        }
        query = duckdb_fmt::format(" INSERT INTO {}regdb_config.REGDB_REG_SPACE_TABLE "
                                   " (reg_space, reg_args) "
                                   " VALUES ('{}', '{}'); ",
                                   create_stmt.catalog, create_stmt.reg_space, create_stmt.reg_args.dump());
        break;
    }
    case StatementType::DELETE_REGSPACE: {
        const auto& delete_stmt = static_cast<const DeleteRegSpaceStatement&>(statement);
        auto con = Config::GetLocalConnection();
        auto result = con.Query(duckdb_fmt::format(" DELETE FROM regdb_config.REGDB_REG_SPACE_TABLE "
                                                   " WHERE reg_space = '{}'; ",
                                                   delete_stmt.reg_space));

        query = duckdb_fmt::format(" DELETE FROM regdb_storage.regdb_config.REGDB_REG_SPACE_TABLE "
                                   " WHERE reg_space = '{}'; ",
                                   delete_stmt.reg_space);
        break;
    }
    case StatementType::UPDATE_REGSPACE: {
        const auto& update_stmt = static_cast<const UpdateRegSpaceStatement&>(statement);
        auto con = Config::GetLocalConnection();
        auto result = con.Query(duckdb_fmt::format( " SELECT reg_space, 'global' AS scope "
                                                    " FROM regdb_storage.regdb_config.REGDB_REG_SPACE_TABLE "
                                                    " WHERE reg_space = '{}' "
                                                    " UNION ALL "
                                                    " SELECT reg_space, 'local' AS	scope "
                                                    " FROM regdb_config.REGDB_REG_SPACE_TABLE "
                                                    " WHERE reg_space = '{}';",
                                                   update_stmt.reg_space, update_stmt.reg_space));
        if (result->RowCount() == 0) {
            throw std::runtime_error(duckdb_fmt::format("RegSpace '{}' doesn't exist.", update_stmt.reg_space));
        }

        auto catalog = result->GetValue(1, 0).ToString() == "global" ? "regdb_storage." : "";
        query = duckdb_fmt::format(" UPDATE {}regdb_config.REGDB_REG_SPACE_TABLE "
                                   " SET reg_args= '{}' "
                                   " WHERE reg_space = '{}'; ",
                                   catalog, update_stmt.new_reg_args.dump(), update_stmt.reg_space);
        break;
    }
    case StatementType::UPDATE_REGSPACE_SCOPE: {
        const auto& update_stmt = static_cast<const UpdateRegSpaceScopeStatement&>(statement);
        auto con = Config::GetLocalConnection();
        auto result = con.Query(duckdb_fmt::format(" SELECT reg_space "
                                                   " FROM {}regdb_config.REGDB_REG_SPACE_TABLE"
                                                   " WHERE reg_space = '{}';",
                                                   update_stmt.catalog, update_stmt.reg_space));
        if (result->RowCount() != 0) {
            throw std::runtime_error(
            duckdb_fmt::format("RegSpace '{}' already exist in {} storage.", update_stmt.reg_space,
                               update_stmt.catalog == "regdb_storage." ? "global" : "local"));
        }

        con.Query(duckdb_fmt::format(" INSERT INTO {}regdb_config.REGDB_REG_SPACE_TABLE "
                                   " (reg_space, reg_args) "
                                   " SELECT reg_space, reg_args "
                                   " FROM {}regdb_config.REGDB_REG_SPACE_TABLE "
                                   " WHERE reg_space = '{}';",
                                   update_stmt.catalog,
                                   update_stmt.catalog == "regdb_storage." ? "" : "regdb_storage.",
                                   update_stmt.reg_space));

        query = duckdb_fmt::format(" DELETE FROM {}regdb_config.REGDB_REG_SPACE_TABLE "
                                 " WHERE reg_space = '{}'; ",
                                 update_stmt.catalog == "regdb_storage." ? "" : "regdb_storage.",
                                 update_stmt.reg_space);
        break;
    }
    case StatementType::GET_REGSPACE: {
        const auto& get_stmt = static_cast<const GetRegSpaceStatement&>(statement);
        query = duckdb_fmt::format(" SELECT 'global' AS scope, * "
                                 " FROM regdb_storage.regdb_config.REGDB_REG_SPACE_TABLE "
                                 " WHERE reg_space = '{}' "
                                 " UNION ALL "
                                 " SELECT 'local' AS scope, * "
                                 " FROM regdb_config.REGDB_REG_SPACE_TABLE "
                                 " WHERE reg_space = '{}'; ",
                                 get_stmt.reg_space, get_stmt.reg_space);

        break;
    }
    case StatementType::GET_ALL_REGSPACE: {
        query = duckdb_fmt::format(" SELECT 'global' as scope, * "
                                   " FROM regdb_storage.regdb_config.REGDB_REG_SPACE_TABLE "
                                   " UNION ALL "
                                   " SELECT 'local' as scope, * "
                                   " FROM regdb_config.REGDB_REG_SPACE_TABLE;"
                                   );
        break;
    }
    default:
      throw std::runtime_error("Unknown statement type.");
    }

    return query;
}

} // namespace regdb
