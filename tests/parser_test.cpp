#include <gtest/gtest.h>
#include "src/parser/lexer.h"
#include "src/parser/parser.h"

namespace minidb {

TEST(ParserTest, CreateTable) {
    Lexer lexer("CREATE TABLE users (id INT, name TEXT);");
    auto tokens = lexer.Tokenize();
    Parser parser(tokens);
    Status status = Status::OK();
    auto stmt = parser.Parse(status);
    
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(stmt->GetType(), StatementType::CREATE_TABLE);
    auto create = static_cast<CreateTableStatement*>(stmt.get());
    EXPECT_EQ(create->table_name, "USERS");
    ASSERT_EQ(create->columns.size(), 2);
    EXPECT_EQ(create->columns[0].GetName(), "ID");
    EXPECT_EQ(create->columns[0].GetType(), DataType::INT);
    EXPECT_EQ(create->columns[1].GetName(), "NAME");
    EXPECT_EQ(create->columns[1].GetType(), DataType::TEXT);
}

TEST(ParserTest, Insert) {
    Lexer lexer("INSERT INTO users VALUES (1, 'Alice');");
    auto tokens = lexer.Tokenize();
    Parser parser(tokens);
    Status status = Status::OK();
    auto stmt = parser.Parse(status);
    
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(stmt->GetType(), StatementType::INSERT);
    auto insert = static_cast<InsertStatement*>(stmt.get());
    EXPECT_EQ(insert->table_name, "USERS");
    ASSERT_EQ(insert->raw_values.size(), 2);
    EXPECT_EQ(insert->raw_values[0], "1");
    EXPECT_EQ(insert->raw_values[1], "Alice");
}

TEST(ParserTest, Select) {
    Lexer lexer("SELECT * FROM users;");
    auto tokens = lexer.Tokenize();
    Parser parser(tokens);
    Status status = Status::OK();
    auto stmt = parser.Parse(status);
    
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(stmt->GetType(), StatementType::SELECT);
    auto select = static_cast<SelectStatement*>(stmt.get());
    EXPECT_EQ(select->table_name, "USERS");
}

} // namespace minidb
