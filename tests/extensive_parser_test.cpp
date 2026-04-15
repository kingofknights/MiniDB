#include <gtest/gtest.h>
#include "src/parser/lexer.h"
#include "src/parser/parser.h"

namespace minidb {

class ExtensiveParserTest : public ::testing::Test {
protected:
    std::unique_ptr<Statement> ParseSQL(const std::string& sql, Status& status) {
        Lexer lexer(sql);
        auto tokens = lexer.Tokenize();
        Parser parser(tokens);
        return parser.Parse(status);
    }
};

// --- LEXER TESTS (10 Tests) ---

TEST_F(ExtensiveParserTest, LexerBasicKeywords) {
    Lexer lexer("CREATE TABLE INSERT SELECT DELETE WHERE INDEX ON INT TEXT");
    auto tokens = lexer.Tokenize();
    EXPECT_EQ(tokens[0].type, TokenType::CREATE);
    EXPECT_EQ(tokens[1].type, TokenType::TABLE);
    EXPECT_EQ(tokens[2].type, TokenType::INSERT);
    EXPECT_EQ(tokens[3].type, TokenType::SELECT);
    EXPECT_EQ(tokens[4].type, TokenType::DELETE);
    EXPECT_EQ(tokens[5].type, TokenType::WHERE);
    EXPECT_EQ(tokens[6].type, TokenType::INDEX);
    EXPECT_EQ(tokens[7].type, TokenType::ON);
    EXPECT_EQ(tokens[8].type, TokenType::INT);
    EXPECT_EQ(tokens[9].type, TokenType::TEXT);
}

TEST_F(ExtensiveParserTest, LexerIdentifiers) {
    Lexer lexer("users employees _temp_table table123");
    auto tokens = lexer.Tokenize();
    EXPECT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].text, "USERS");
    EXPECT_EQ(tokens[1].text, "EMPLOYEES");
    EXPECT_EQ(tokens[2].text, "_TEMP_TABLE");
    EXPECT_EQ(tokens[3].text, "TABLE123");
}

TEST_F(ExtensiveParserTest, LexerLiterals) {
    Lexer lexer("123 'string' \"quoted\"");
    auto tokens = lexer.Tokenize();
    EXPECT_EQ(tokens[0].type, TokenType::INTEGER_LITERAL);
    EXPECT_EQ(tokens[0].text, "123");
    EXPECT_EQ(tokens[1].type, TokenType::STRING_LITERAL);
    EXPECT_EQ(tokens[1].text, "string");
    EXPECT_EQ(tokens[2].type, TokenType::STRING_LITERAL);
    EXPECT_EQ(tokens[2].text, "quoted");
}

TEST_F(ExtensiveParserTest, LexerPunctuation) {
    Lexer lexer("(), * ; =");
    auto tokens = lexer.Tokenize();
    EXPECT_EQ(tokens[0].type, TokenType::LPAREN);
    EXPECT_EQ(tokens[1].type, TokenType::RPAREN);
    EXPECT_EQ(tokens[2].type, TokenType::COMMA);
    EXPECT_EQ(tokens[3].type, TokenType::STAR);
    EXPECT_EQ(tokens[4].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[5].type, TokenType::EQUAL);
}

TEST_F(ExtensiveParserTest, LexerEmpty) {
    Lexer lexer("");
    auto tokens = lexer.Tokenize();
    EXPECT_EQ(tokens[0].type, TokenType::END_OF_FILE);
}

TEST_F(ExtensiveParserTest, LexerWhitespace) {
    Lexer lexer("  \n \t CREATE   \r\n  TABLE ");
    auto tokens = lexer.Tokenize();
    EXPECT_EQ(tokens[0].type, TokenType::CREATE);
    EXPECT_EQ(tokens[1].type, TokenType::TABLE);
}

TEST_F(ExtensiveParserTest, LexerCaseInsensitivity) {
    Lexer lexer("creAtE TaBlE");
    auto tokens = lexer.Tokenize();
    EXPECT_EQ(tokens[0].type, TokenType::CREATE);
    EXPECT_EQ(tokens[1].type, TokenType::TABLE);
}

TEST_F(ExtensiveParserTest, LexerUnderlineIdentifier) {
    Lexer lexer("my_column_name");
    auto tokens = lexer.Tokenize();
    EXPECT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].text, "MY_COLUMN_NAME");
}

TEST_F(ExtensiveParserTest, LexerUnknownChar) {
    Lexer lexer("$");
    auto tokens = lexer.Tokenize();
    EXPECT_EQ(tokens[0].type, TokenType::UNKNOWN);
}

TEST_F(ExtensiveParserTest, LexerMixed) {
    Lexer lexer("SELECT * FROM users;");
    auto tokens = lexer.Tokenize();
    ASSERT_GE(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, TokenType::SELECT);
    EXPECT_EQ(tokens[1].type, TokenType::STAR);
    EXPECT_EQ(tokens[2].type, TokenType::FROM);
    EXPECT_EQ(tokens[3].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[4].type, TokenType::SEMICOLON);
}

// --- PARSER TESTS (25 Tests) ---

TEST_F(ExtensiveParserTest, ParseCreateBasic) {
    Status s = Status::OK();
    auto stmt = ParseSQL("CREATE TABLE t1 (c1 INT);", s);
    ASSERT_TRUE(s.ok());
    auto c = static_cast<CreateTableStatement*>(stmt.get());
    EXPECT_EQ(c->table_name, "T1");
    EXPECT_EQ(c->columns.size(), 1);
}

TEST_F(ExtensiveParserTest, ParseCreateMultiColumn) {
    Status s = Status::OK();
    auto stmt = ParseSQL("CREATE TABLE t1 (c1 INT, c2 TEXT, c3 INT);", s);
    ASSERT_TRUE(s.ok());
    auto c = static_cast<CreateTableStatement*>(stmt.get());
    EXPECT_EQ(c->columns.size(), 3);
}

TEST_F(ExtensiveParserTest, ParseCreateInvalidNoParen) {
    Status s = Status::OK();
    auto stmt = ParseSQL("CREATE TABLE t1 c1 INT;", s);
    EXPECT_FALSE(s.ok());
}

TEST_F(ExtensiveParserTest, ParseCreateInvalidNoType) {
    Status s = Status::OK();
    auto stmt = ParseSQL("CREATE TABLE t1 (c1);", s);
    EXPECT_FALSE(s.ok());
}

TEST_F(ExtensiveParserTest, ParseInsertBasic) {
    Status s = Status::OK();
    auto stmt = ParseSQL("INSERT INTO t1 VALUES (1, 'a');", s);
    ASSERT_TRUE(s.ok());
    auto i = static_cast<InsertStatement*>(stmt.get());
    EXPECT_EQ(i->table_name, "T1");
    EXPECT_EQ(i->raw_values.size(), 2);
}

TEST_F(ExtensiveParserTest, ParseInsertNoValuesKeyword) {
    Status s = Status::OK();
    auto stmt = ParseSQL("INSERT INTO t1 (1);", s);
    EXPECT_FALSE(s.ok());
}

TEST_F(ExtensiveParserTest, ParseSelectStar) {
    Status s = Status::OK();
    auto stmt = ParseSQL("SELECT * FROM t1;", s);
    ASSERT_TRUE(s.ok());
    auto sel = static_cast<SelectStatement*>(stmt.get());
    EXPECT_EQ(sel->table_name, "T1");
}

TEST_F(ExtensiveParserTest, ParseSelectWhereInt) {
    Status s = Status::OK();
    auto stmt = ParseSQL("SELECT * FROM t1 WHERE id = 100;", s);
    ASSERT_TRUE(s.ok());
    auto sel = static_cast<SelectStatement*>(stmt.get());
    ASSERT_NE(sel->where, nullptr);
    EXPECT_EQ(sel->where->column_name, "ID");
    EXPECT_EQ(sel->where->value, "100");
}

TEST_F(ExtensiveParserTest, ParseSelectWhereString) {
    Status s = Status::OK();
    auto stmt = ParseSQL("SELECT * FROM t1 WHERE name = 'Alice';", s);
    ASSERT_TRUE(s.ok());
    auto sel = static_cast<SelectStatement*>(stmt.get());
    EXPECT_EQ(sel->where->value, "Alice");
}

TEST_F(ExtensiveParserTest, ParseDeleteBasic) {
    Status s = Status::OK();
    auto stmt = ParseSQL("DELETE FROM t1;", s);
    ASSERT_TRUE(s.ok());
    auto del = static_cast<DeleteStatement*>(stmt.get());
    EXPECT_EQ(del->table_name, "T1");
    EXPECT_EQ(del->where, nullptr);
}

TEST_F(ExtensiveParserTest, ParseDeleteWhere) {
    Status s = Status::OK();
    auto stmt = ParseSQL("DELETE FROM t1 WHERE id = 5;", s);
    ASSERT_TRUE(s.ok());
    auto del = static_cast<DeleteStatement*>(stmt.get());
    ASSERT_NE(del->where, nullptr);
    EXPECT_EQ(del->where->column_name, "ID");
}

TEST_F(ExtensiveParserTest, ParseCreateIndex) {
    Status s = Status::OK();
    auto stmt = ParseSQL("CREATE INDEX idx ON t1 (col);", s);
    ASSERT_TRUE(s.ok());
    auto idx = static_cast<CreateIndexStatement*>(stmt.get());
    EXPECT_EQ(idx->index_name, "IDX");
    EXPECT_EQ(idx->table_name, "T1");
    ASSERT_EQ(idx->column_names.size(), 1);
    EXPECT_EQ(idx->column_names[0], "COL");
}

TEST_F(ExtensiveParserTest, ParseInvalidStatement) {
    Status s = Status::OK();
    auto stmt = ParseSQL("DROP TABLE t1;", s);
    EXPECT_FALSE(s.ok());
}

TEST_F(ExtensiveParserTest, ParseUnterminatedString) {
    Status s = Status::OK();
    auto stmt = ParseSQL("INSERT INTO t1 VALUES ('unterm);", s);
    // Lexer will just finish the string at end of file
    // So parser might see a string and then fail at RPAREN
    EXPECT_FALSE(s.ok());
}

TEST_F(ExtensiveParserTest, ParseSelectNoFrom) {
    Status s = Status::OK();
    auto stmt = ParseSQL("SELECT * users;", s);
    EXPECT_FALSE(s.ok());
}

TEST_F(ExtensiveParserTest, ParseCreateEmptyCols) {
    Status s = Status::OK();
    auto stmt = ParseSQL("CREATE TABLE t1 ();", s);
    EXPECT_FALSE(s.ok());
}

TEST_F(ExtensiveParserTest, ParseInsertTrailingComma) {
    Status s = Status::OK();
    auto stmt = ParseSQL("INSERT INTO t1 VALUES (1, );", s);
    EXPECT_FALSE(s.ok());
}

TEST_F(ExtensiveParserTest, ParseWhereNoEqual) {
    Status s = Status::OK();
    auto stmt = ParseSQL("SELECT * FROM t1 WHERE id 5;", s);
    EXPECT_FALSE(s.ok());
}

TEST_F(ExtensiveParserTest, ParseWhereNoValue) {
    Status s = Status::OK();
    auto stmt = ParseSQL("SELECT * FROM t1 WHERE id = ;", s);
    EXPECT_FALSE(s.ok());
}

TEST_F(ExtensiveParserTest, ParseCreateIndexNoOn) {
    Status s = Status::OK();
    auto stmt = ParseSQL("CREATE INDEX idx t1 (c);", s);
    EXPECT_FALSE(s.ok());
}

TEST_F(ExtensiveParserTest, ParseMultiSpaceAndNewlines) {
    Status s = Status::OK();
    auto stmt = ParseSQL("  SELECT \n * \n FROM \n t1 \n WHERE \n id \n = \n 1 ; ", s);
    EXPECT_TRUE(s.ok());
}

TEST_F(ExtensiveParserTest, ParseInsertMultiRowAttempt) {
    Status s = Status::OK();
    // Our parser only supports single row VALUES (v1)
    auto stmt = ParseSQL("INSERT INTO t1 VALUES (1), (2);", s);
    // Should fail after first RPAREN because it doesn't expect another LPAREN/COMMA
    EXPECT_FALSE(s.ok());
}

TEST_F(ExtensiveParserTest, ParseExtraSemicolons) {
    Status s = Status::OK();
    auto stmt = ParseSQL("SELECT * FROM t1;;;", s);
    // Our new strict parser rejects extra tokens after first valid statement+semicolon
    EXPECT_FALSE(s.ok());
}

TEST_F(ExtensiveParserTest, ParseSelectStarNoSemicolon) {
    Status s = Status::OK();
    auto stmt = ParseSQL("SELECT * FROM t1", s);
    EXPECT_TRUE(s.ok()); // We don't strictly require semicolon at end of string in v1
}

TEST_F(ExtensiveParserTest, ParseDeleteAllUpperCase) {
    Status s = Status::OK();
    auto stmt = ParseSQL("DELETE FROM USERS WHERE ID = 10", s);
    EXPECT_TRUE(s.ok());
}

} // namespace minidb
