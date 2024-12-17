#define CATCH_CONFIG_MAIN
#include <cstdint>
#include <iostream>

#include <fstream>
#include <vector>
#include <filesystem>

#include "catch2/catch_all.hpp"
#include "nvm/span.h"

using namespace nvm;

bool CreateTestFile(const std::string& filename, size_t expected_size) {
    namespace fs = std::filesystem;

    // Check if file exists
    if (fs::exists(filename)) {
        // Check file size
        auto file_size = fs::file_size(filename);
        if (file_size == expected_size) {
            return true; // File exists and size matches
        }
    }

    // If file doesn't exist or size is incorrect, recreate it
    std::vector<uint8_t> data(expected_size, 0xAB); // Fill with 0xAB
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        return false; // Failed to create the file
    }

    outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
    return true; // File successfully created or updated
}

std::pair<bool,size_t> PrepareTestFile(const std::string& filename, size_t expected_size){
    
    return std::make_pair(CreateTestFile(filename,expected_size),expected_size);
}

void api_function(uint8_t* data, size_t size) {
    Span<uint8_t> span(data, size);
    std::fill(span.Begin(), span.End(), 0x00); // Clear data
}

TEST_CASE("Span: Process Large Data Buffers", "[Span]") {
    std::vector<uint8_t> buffer(1024, 0xFF); // Simulate a 1KB buffer filled with 0xFF
    Span<uint8_t> span(buffer);

    REQUIRE(span.Size() == 1024);
    REQUIRE(span[0] == 0xFF);

    SECTION("Modify Part of Buffer") {
        auto subspan = span.Subspan(512, 256); // Slice middle part
        std::fill(subspan.Begin(), subspan.End(), 0x00); // Clear 256 bytes

        REQUIRE(buffer[512] == 0x00); // Ensure original buffer is updated
        REQUIRE(buffer[767] == 0x00); // Verify end of slice
        REQUIRE(buffer[768] == 0xFF); // Ensure data outside slice is unchanged
    }

    SECTION("Read Only Access") {
        Span< uint8_t> readOnlySpan(buffer);
        REQUIRE(readOnlySpan[100] == 0xFF);
    }
}


TEST_CASE("Span: Binary File Streaming", "[Span]") {
    const std::string filename = "test_binary.dat";
    const size_t expected_size = 1024;
    

    auto test_file = PrepareTestFile(filename,expected_size);

    REQUIRE(test_file.first == true );
    REQUIRE(test_file.second == expected_size);


    REQUIRE(std::filesystem::exists(filename));

    SECTION("Read and Process File in Chunks") {
        std::ifstream inFile(filename, std::ios::binary);
        REQUIRE(inFile.is_open());

        std::vector<uint8_t> chunk(512);
        inFile.read(reinterpret_cast<char*>(chunk.data()), chunk.size());
        Span<uint8_t> span(chunk);

        REQUIRE(span.Size() == 512);
        REQUIRE(span[0] == 0xAB);

        // Example processing
        std::transform(span.Begin(), span.End(), span.Begin(), [](uint8_t val) { return val ^ 0xFF; });
        REQUIRE(span[0] == 0x54); // Validate transformation
    }

    std::filesystem::remove(filename); // Cleanup
}


TEST_CASE("Span: Parse Network Packet", "[Span]") {
    struct Packet {
        uint16_t header;
        uint8_t payload[10];
    } packet = {0x1234, {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A}};

    Span<uint8_t> packetSpan(reinterpret_cast<uint8_t*>(&packet), sizeof(Packet));

    REQUIRE(packetSpan.Size() == sizeof(Packet));

    SECTION("Extract Header and Payload") {
        auto headerSpan = packetSpan.Subspan(0, sizeof(packet.header));
        auto payloadSpan = packetSpan.Subspan(sizeof(packet.header), sizeof(packet.payload));

        REQUIRE(headerSpan.Size() == sizeof(packet.header));
        REQUIRE(payloadSpan.Size() == sizeof(packet.payload));

        uint16_t header = *reinterpret_cast<const uint16_t*>(headerSpan.Data());
        REQUIRE(header == 0x1234);

        REQUIRE(payloadSpan[0] == 0x01);
        REQUIRE(payloadSpan[9] == 0x0A);
    }
}


TEST_CASE("Span: Interoperability with Raw API", "[Span]") {
    uint8_t rawData[16] = {1, 2, 3, 4, 5};
    Span<uint8_t> span(rawData, 16);

    REQUIRE(span[0] == 1);

    SECTION("Modify via API") {
        api_function(rawData, 16);
        REQUIRE(span[0] == 0x00);
        REQUIRE(span[4] == 0x00);
    }
}


TEST_CASE("Span: Serialize and Deserialize", "[Span]") {
    struct Data {
        int32_t id;
        float value;
    } data = {42, 3.14f};

    std::vector<uint8_t> buffer(sizeof(Data));
    Span<uint8_t> span(buffer);

    REQUIRE(span.Size() == sizeof(Data));

    // Serialize
    std::memcpy(span.Data(), &data, sizeof(Data));
    REQUIRE(span[0] != 0); // Check serialized data

    // Deserialize
    Data deserialized;
    std::memcpy(&deserialized, span.Data(), sizeof(Data));
    REQUIRE(deserialized.id == 42);
    REQUIRE(deserialized.value == Catch::Approx(3.14f));
}