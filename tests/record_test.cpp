#include <gtest/gtest.h>
#include "src/storage/record.h"

namespace minidb {

TEST(RecordTest, RoundTrip) {
    Schema schema({
        {"id", DataType::INT},
        {"name", DataType::TEXT},
        {"age", DataType::INT}
    });

    Record record(std::vector<Value>{Value(1), Value("Alice"), Value(25)});

    auto buffer = Record::Serialize(schema, record);
    
    // Expected size: 4 (INT) + 4 (LEN) + 5 ("Alice") + 4 (INT) = 17 bytes
    EXPECT_EQ(buffer.size(), 17);

    size_t bytes_read = 0;
    Record deserialized = Record::Deserialize(schema, buffer.data(), bytes_read);

    EXPECT_EQ(bytes_read, 17);
    EXPECT_EQ(deserialized.GetValue(0).AsInt(), 1);
    EXPECT_EQ(deserialized.GetValue(1).AsString(), "Alice");
    EXPECT_EQ(deserialized.GetValue(2).AsInt(), 25);
}

} // namespace minidb
