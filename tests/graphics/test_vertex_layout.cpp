#include <catch2/catch_all.hpp>
#include <game_engine/graphics/vertex_layout.hpp>

using namespace game_engine::graphics;

TEST_CASE("VertexLayoutType enum") {
    SECTION("given Position type when cast to int then returns 0") {
        REQUIRE(static_cast<int>(VertexLayoutType::Position) == 0);
    }

    SECTION("given PositionColor type when cast to int then returns 1") {
        REQUIRE(static_cast<int>(VertexLayoutType::PositionColor) == 1);
    }

    SECTION("given PositionUV type when cast to int then returns 2") {
        REQUIRE(static_cast<int>(VertexLayoutType::PositionUV) == 2);
    }

    SECTION("given PositionNormalUV type when cast to int then returns 3") {
        REQUIRE(static_cast<int>(VertexLayoutType::PositionNormalUV) == 3);
    }
}

TEST_CASE("VertexFormat enum values") {
    SECTION("given Float32x2 when getting value then matches expected") {
        REQUIRE(static_cast<uint32_t>(VertexFormat::Float32x2) == 0x00000003);
    }

    SECTION("given Float32x3 when getting value then matches expected") {
        REQUIRE(static_cast<uint32_t>(VertexFormat::Float32x3) == 0x00000004);
    }

    SECTION("given Float32x4 when getting value then matches expected") {
        REQUIRE(static_cast<uint32_t>(VertexFormat::Float32x4) == 0x00000005);
    }
}

TEST_CASE("VertexLayout Position") {
    SECTION("given Position layout when getting stride then equals sizeof VertexPosition") {
        auto layout = getVertexLayout(VertexLayoutType::Position);
        REQUIRE(layout.stride == sizeof(VertexPosition));
    }

    SECTION("given Position layout when getting attributes then has 1 attribute") {
        auto layout = getVertexLayout(VertexLayoutType::Position);
        REQUIRE(layout.attributes.size() == 1);
    }

    SECTION("given Position layout when getting first attribute then location is 0") {
        auto layout = getVertexLayout(VertexLayoutType::Position);
        REQUIRE(layout.attributes[0].location == 0);
    }

    SECTION("given Position layout when getting first attribute then offset is 0") {
        auto layout = getVertexLayout(VertexLayoutType::Position);
        REQUIRE(layout.attributes[0].offset == 0);
    }

    SECTION("given Position layout when getting first attribute then format is Float32x3") {
        auto layout = getVertexLayout(VertexLayoutType::Position);
        REQUIRE(layout.attributes[0].format == VertexFormat::Float32x3);
    }
}

TEST_CASE("VertexLayout PositionColor") {
    SECTION("given PositionColor layout when getting stride then equals sizeof VertexPositionColor") {
        auto layout = getVertexLayout(VertexLayoutType::PositionColor);
        REQUIRE(layout.stride == sizeof(VertexPositionColor));
    }

    SECTION("given PositionColor layout when getting attributes then has 2 attributes") {
        auto layout = getVertexLayout(VertexLayoutType::PositionColor);
        REQUIRE(layout.attributes.size() == 2);
    }

    SECTION("given PositionColor layout when getting position attribute then correct format") {
        auto layout = getVertexLayout(VertexLayoutType::PositionColor);
        REQUIRE(layout.attributes[0].location == 0);
        REQUIRE(layout.attributes[0].offset == 0);
        REQUIRE(layout.attributes[0].format == VertexFormat::Float32x3);
    }

    SECTION("given PositionColor layout when getting color attribute then correct format") {
        auto layout = getVertexLayout(VertexLayoutType::PositionColor);
        REQUIRE(layout.attributes[1].location == 1);
        REQUIRE(layout.attributes[1].offset == sizeof(float) * 3);
        REQUIRE(layout.attributes[1].format == VertexFormat::Float32x4);
    }
}

TEST_CASE("VertexLayout PositionUV") {
    SECTION("given PositionUV layout when getting stride then equals sizeof VertexPositionUV") {
        auto layout = getVertexLayout(VertexLayoutType::PositionUV);
        REQUIRE(layout.stride == sizeof(VertexPositionUV));
    }

    SECTION("given PositionUV layout when getting attributes then has 2 attributes") {
        auto layout = getVertexLayout(VertexLayoutType::PositionUV);
        REQUIRE(layout.attributes.size() == 2);
    }

    SECTION("given PositionUV layout when getting position attribute then correct format") {
        auto layout = getVertexLayout(VertexLayoutType::PositionUV);
        REQUIRE(layout.attributes[0].location == 0);
        REQUIRE(layout.attributes[0].format == VertexFormat::Float32x3);
    }

    SECTION("given PositionUV layout when getting uv attribute then correct format") {
        auto layout = getVertexLayout(VertexLayoutType::PositionUV);
        REQUIRE(layout.attributes[1].location == 1);
        REQUIRE(layout.attributes[1].offset == sizeof(float) * 3);
        REQUIRE(layout.attributes[1].format == VertexFormat::Float32x2);
    }
}

TEST_CASE("VertexLayout PositionNormalUV") {
    SECTION("given PositionNormalUV layout when getting stride then equals sizeof VertexPositionNormalUV") {
        auto layout = getVertexLayout(VertexLayoutType::PositionNormalUV);
        REQUIRE(layout.stride == sizeof(VertexPositionNormalUV));
    }

    SECTION("given PositionNormalUV layout when getting attributes then has 3 attributes") {
        auto layout = getVertexLayout(VertexLayoutType::PositionNormalUV);
        REQUIRE(layout.attributes.size() == 3);
    }

    SECTION("given PositionNormalUV layout when getting position attribute then correct") {
        auto layout = getVertexLayout(VertexLayoutType::PositionNormalUV);
        REQUIRE(layout.attributes[0].location == 0);
        REQUIRE(layout.attributes[0].offset == 0);
        REQUIRE(layout.attributes[0].format == VertexFormat::Float32x3);
    }

    SECTION("given PositionNormalUV layout when getting normal attribute then correct") {
        auto layout = getVertexLayout(VertexLayoutType::PositionNormalUV);
        REQUIRE(layout.attributes[1].location == 1);
        REQUIRE(layout.attributes[1].offset == sizeof(float) * 3);
        REQUIRE(layout.attributes[1].format == VertexFormat::Float32x3);
    }

    SECTION("given PositionNormalUV layout when getting uv attribute then correct") {
        auto layout = getVertexLayout(VertexLayoutType::PositionNormalUV);
        REQUIRE(layout.attributes[2].location == 2);
        REQUIRE(layout.attributes[2].offset == sizeof(float) * 6);
        REQUIRE(layout.attributes[2].format == VertexFormat::Float32x2);
    }
}

TEST_CASE("Vertex struct sizes") {
    SECTION("given VertexPosition then size is 12 bytes") {
        REQUIRE(sizeof(VertexPosition) == 12);
    }

    SECTION("given VertexPositionColor then size is 28 bytes") {
        REQUIRE(sizeof(VertexPositionColor) == 28);
    }

    SECTION("given VertexPositionUV then size is 20 bytes") {
        REQUIRE(sizeof(VertexPositionUV) == 20);
    }

    SECTION("given VertexPositionNormalUV then size is 32 bytes") {
        REQUIRE(sizeof(VertexPositionNormalUV) == 32);
    }
}

TEST_CASE("VertexAttribute structure") {
    SECTION("given VertexAttribute when created then can store location") {
        VertexAttribute attr{1, 0, VertexFormat::Float32x3};
        REQUIRE(attr.location == 1);
    }

    SECTION("given VertexAttribute when created then can store offset") {
        VertexAttribute attr{0, 12, VertexFormat::Float32x3};
        REQUIRE(attr.offset == 12);
    }

    SECTION("given VertexAttribute when created then can store format") {
        VertexAttribute attr{0, 0, VertexFormat::Float32x4};
        REQUIRE(attr.format == VertexFormat::Float32x4);
    }
}

TEST_CASE("VertexLayout structure") {
    SECTION("given VertexLayout when created then attributes vector is accessible") {
        VertexLayout layout;
        layout.attributes.push_back({0, 0, VertexFormat::Float32x3});
        REQUIRE(layout.attributes.size() == 1);
    }

    SECTION("given VertexLayout when created then stride is modifiable") {
        VertexLayout layout;
        layout.stride = 32;
        REQUIRE(layout.stride == 32);
    }
}
