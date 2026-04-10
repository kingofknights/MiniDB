#include <gtest/gtest.h>
#include "src/catalog/catalog.h"
#include <memory>

namespace minidb {

TEST(CatalogTest, SerializationRoundTrip) {
    Catalog catalog;
    catalog.CreateTable("users", Schema({
        {"id", DataType::INT},
        {"name", DataType::TEXT}
    }));
    catalog.CreateTable("orders", Schema({
        {"order_id", DataType::INT},
        {"amount", DataType::INT}
    }));

    std::vector<uint8_t> buffer;
    catalog.Serialize(buffer);

    auto deserialized = Catalog::Deserialize(buffer.data());
    
    ASSERT_TRUE(deserialized->TableExists("USERS"));
    ASSERT_TRUE(deserialized->TableExists("ORDERS"));

    const auto& user_schema = deserialized->GetSchema("USERS");
    EXPECT_EQ(user_schema.GetColumnCount(), 2);
    EXPECT_EQ(user_schema.GetColumn(0).GetName(), "ID");
    EXPECT_EQ(user_schema.GetColumn(0).GetType(), DataType::INT);
    EXPECT_EQ(user_schema.GetColumn(1).GetName(), "NAME");
    EXPECT_EQ(user_schema.GetColumn(1).GetType(), DataType::TEXT);
}

} // namespace minidb
