#include "regdb/custom_parser/tokenizer.hpp"

#include <cctype>
#include <iostream>
#include <stdexcept>

namespace regdb {

// 构造器
Tokenizer::Tokenizer(const std::string& query) : query_(query), position_(0) {}

// 跳过空格
void Tokenizer::SkipWhitespace() {
    while (position_ < static_cast<int>(query_.size()) && std::isspace(query_[position_])) {
        ++position_;
    }
}

std::string Tokenizer::GetQuery() {
    return query_;
}

// 解析一个字符串
Token Tokenizer::ParseStringLiteral() {
    if (query_[position_] != '\'') {
        throw std::runtime_error("String literal should start with a single quote.");
    }
    ++position_;
    int start = position_;
    while (position_ < static_cast<int>(query_.size()) && query_[position_] != '\'') {
        ++position_;
    }
    if (position_ == static_cast<int>(query_.size())) {
        throw std::runtime_error("Unterminated string literal.");
    }
    std::string value = query_.substr(start, position_ - start);
    ++position_;
    return {TokenType::STRING_LITERAL, value};
}

// 解析 json
Token Tokenizer::ParseJson() {
    if (query_[position_] != '{') {
        throw std::runtime_error("JSON should start with a curly brace.");
    }
    auto start = position_++;
    auto brace_count = 1;
    while (position_ < static_cast<int>(query_.size()) && brace_count > 0) {
        if (query_[position_] == '{') {
            ++brace_count;
        } else if (query_[position_] == '}') {
            --brace_count;
        }
        ++position_;
    }
    if (brace_count > 0) {
        throw std::runtime_error("Unterminated JSON.");
    }
    auto value = query_.substr(start, position_ - start);
    return {TokenType::JSON, value};
}

// 解析关键词
Token Tokenizer::ParseKeyword() {
    auto start = position_;
    while (position_ < static_cast<int>(query_.size()) && (std::isalpha(query_[position_]) || query_[position_] == '_')) {
        ++position_;
    }
    auto value = query_.substr(start, position_ - start);
    return {TokenType::KEYWORD, value};
}

// 解析单个字符
Token Tokenizer::ParseSymbol() {
    auto ch = query_[position_];
    ++position_;
    return {TokenType::SYMBOL, std::string(1, ch)};
}

// 解析一个数
Token Tokenizer::ParseNumber() {
    auto start = position_;
    while (position_ < static_cast<int>(query_.size()) && std::isdigit(query_[position_])) {
        ++position_;
    }
    auto value = query_.substr(start, position_ - start);
    return {TokenType::NUMBER, value};
}

// 解析圆括号
Token Tokenizer::ParseParenthesis() {
    auto ch = query_[position_];
    ++position_;
    return {TokenType::PARENTHESIS, std::string(1, ch)};
}

// 获取下一个 token
Token Tokenizer::GetNextToken() {
    SkipWhitespace();
    if (position_ >= static_cast<int>(query_.size())) {
        return {TokenType::END_OF_FILE, ""};
    }
    auto ch = query_[position_];
    if (ch == '\'') {
        return ParseStringLiteral();
    } else if (ch == '{') {
        return ParseJson();
    } else if (std::isalpha(ch)) {
        return ParseKeyword();
    } else if (ch == ';' || ch == ',') {
        return ParseSymbol();
    } else if (ch == '(' || ch == ')') {
        return ParseParenthesis();
    } else if (std::isdigit(ch)) {
        return ParseNumber();
    } else {
        return {TokenType::UNKNOWN, std::string(1, ch)};
    }
}

// 获取下一个 Token
Token Tokenizer::NextToken() {
    return GetNextToken();
}

// 转化 TokenType 到字符串
std::string TokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD:
            return "KEYWORD";
        case TokenType::STRING_LITERAL:
            return "STRING_LITERAL";
        case TokenType::JSON:
            return "JSON";
        case TokenType::SYMBOL:
            return "SYMBOL";
        case TokenType::NUMBER:
            return "NUMBER";
        case TokenType::PARENTHESIS:
            return "PARENTHESIS";
        case TokenType::END_OF_FILE:
            return "END_OF_FILE";
        case TokenType::UNKNOWN:
            return "UNKNOWN";
        default:
            return "UNKNOWN";
    }
}

} // namespace regdb