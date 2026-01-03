#include <catch2/catch_all.hpp>
#include <game_engine/graphics/primitives.hpp>
#include <cmath>
#include <set>

using namespace game_engine::graphics;

TEST_CASE("Cube generation") {
    SECTION("given default size when generating cube then has 24 vertices") {
        auto cube = generateCube();
        REQUIRE(cube.vertices.size() == 24);
    }

    SECTION("given default size when generating cube then has 36 indices") {
        auto cube = generateCube();
        REQUIRE(cube.indices.size() == 36);
    }

    SECTION("given size 2 when generating cube then vertices span from -1 to 1") {
        auto cube = generateCube(2.0f);
        float minVal = std::numeric_limits<float>::max();
        float maxVal = std::numeric_limits<float>::lowest();
        for (const auto& v : cube.vertices) {
            minVal = std::min({minVal, v.x, v.y, v.z});
            maxVal = std::max({maxVal, v.x, v.y, v.z});
        }
        REQUIRE(minVal == Catch::Approx(-1.0f));
        REQUIRE(maxVal == Catch::Approx(1.0f));
    }

    SECTION("given cube when checking normals then all normals are unit length") {
        auto cube = generateCube();
        for (const auto& v : cube.vertices) {
            float length = std::sqrt(v.nx * v.nx + v.ny * v.ny + v.nz * v.nz);
            REQUIRE(length == Catch::Approx(1.0f).margin(0.001f));
        }
    }

    SECTION("given cube when checking indices then all indices are valid") {
        auto cube = generateCube();
        for (uint32_t idx : cube.indices) {
            REQUIRE(idx < cube.vertices.size());
        }
    }

    SECTION("given cube when checking UVs then all UVs are in 0-1 range") {
        auto cube = generateCube();
        for (const auto& v : cube.vertices) {
            REQUIRE(v.u >= 0.0f);
            REQUIRE(v.u <= 1.0f);
            REQUIRE(v.v >= 0.0f);
            REQUIRE(v.v <= 1.0f);
        }
    }

    SECTION("given size 0 when generating cube then produces degenerate cube") {
        auto cube = generateCube(0.0f);
        REQUIRE(cube.vertices.size() == 24);
        for (const auto& v : cube.vertices) {
            REQUIRE(v.x == 0.0f);
            REQUIRE(v.y == 0.0f);
            REQUIRE(v.z == 0.0f);
        }
    }
}

TEST_CASE("Sphere generation") {
    SECTION("given default parameters when generating sphere then has correct vertex count") {
        auto sphere = generateSphere();
        size_t expectedVertices = 16 * 32;
        REQUIRE(sphere.vertices.size() == expectedVertices);
    }

    SECTION("given default parameters when generating sphere then has correct index count") {
        auto sphere = generateSphere();
        size_t expectedIndices = (16 - 1) * (32 - 1) * 6;
        REQUIRE(sphere.indices.size() == expectedIndices);
    }

    SECTION("given radius 1 when generating sphere then all vertices at distance 1 from origin") {
        auto sphere = generateSphere(1.0f, 8, 16);
        for (const auto& v : sphere.vertices) {
            float dist = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
            REQUIRE(dist == Catch::Approx(1.0f).margin(0.001f));
        }
    }

    SECTION("given sphere when checking normals then normals point outward") {
        auto sphere = generateSphere(1.0f, 8, 16);
        for (const auto& v : sphere.vertices) {
            float dot = v.x * v.nx + v.y * v.ny + v.z * v.nz;
            REQUIRE(dot > 0.0f);
        }
    }

    SECTION("given sphere when checking indices then all indices valid") {
        auto sphere = generateSphere();
        for (uint32_t idx : sphere.indices) {
            REQUIRE(idx < sphere.vertices.size());
        }
    }

    SECTION("given minimum rings and sectors when generating sphere then produces valid mesh") {
        auto sphere = generateSphere(1.0f, 2, 3);
        REQUIRE(sphere.vertices.size() > 0);
        REQUIRE(sphere.indices.size() > 0);
    }
}

TEST_CASE("Plane generation") {
    SECTION("given default parameters when generating plane then has 4 vertices") {
        auto plane = generatePlane();
        REQUIRE(plane.vertices.size() == 4);
    }

    SECTION("given default parameters when generating plane then has 6 indices") {
        auto plane = generatePlane();
        REQUIRE(plane.indices.size() == 6);
    }

    SECTION("given 2x2 plane when generating then vertices span -1 to 1") {
        auto plane = generatePlane(2.0f, 2.0f);
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::lowest();
        for (const auto& v : plane.vertices) {
            minX = std::min(minX, v.x);
            maxX = std::max(maxX, v.x);
            minZ = std::min(minZ, v.z);
            maxZ = std::max(maxZ, v.z);
        }
        REQUIRE(minX == Catch::Approx(-1.0f));
        REQUIRE(maxX == Catch::Approx(1.0f));
        REQUIRE(minZ == Catch::Approx(-1.0f));
        REQUIRE(maxZ == Catch::Approx(1.0f));
    }

    SECTION("given plane when checking normals then all normals point up") {
        auto plane = generatePlane();
        for (const auto& v : plane.vertices) {
            REQUIRE(v.nx == Catch::Approx(0.0f));
            REQUIRE(v.ny == Catch::Approx(1.0f));
            REQUIRE(v.nz == Catch::Approx(0.0f));
        }
    }

    SECTION("given plane when checking Y coordinate then all vertices at Y=0") {
        auto plane = generatePlane();
        for (const auto& v : plane.vertices) {
            REQUIRE(v.y == 0.0f);
        }
    }
}

TEST_CASE("Quad generation") {
    SECTION("given default parameters when generating quad then has 4 vertices") {
        auto quad = generateQuad();
        REQUIRE(quad.vertices.size() == 4);
    }

    SECTION("given default parameters when generating quad then has 6 indices") {
        auto quad = generateQuad();
        REQUIRE(quad.indices.size() == 6);
    }

    SECTION("given quad when checking Z coordinate then all vertices at Z=0") {
        auto quad = generateQuad();
        for (const auto& v : quad.vertices) {
            REQUIRE(v.z == 0.0f);
        }
    }

    SECTION("given 2x2 quad when generating then vertices span -1 to 1 in X and Y") {
        auto quad = generateQuad(2.0f, 2.0f);
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        for (const auto& v : quad.vertices) {
            minX = std::min(minX, v.x);
            maxX = std::max(maxX, v.x);
            minY = std::min(minY, v.y);
            maxY = std::max(maxY, v.y);
        }
        REQUIRE(minX == Catch::Approx(-1.0f));
        REQUIRE(maxX == Catch::Approx(1.0f));
        REQUIRE(minY == Catch::Approx(-1.0f));
        REQUIRE(maxY == Catch::Approx(1.0f));
    }

    SECTION("given quad when checking normals then all normals point toward camera") {
        auto quad = generateQuad();
        for (const auto& v : quad.vertices) {
            REQUIRE(v.nx == Catch::Approx(0.0f));
            REQUIRE(v.ny == Catch::Approx(0.0f));
            REQUIRE(v.nz == Catch::Approx(1.0f));
        }
    }

    SECTION("given quad when checking UVs then corners have expected values") {
        auto quad = generateQuad();
        std::set<std::pair<float, float>> uvs;
        for (const auto& v : quad.vertices) {
            uvs.insert({v.u, v.v});
        }
        REQUIRE(uvs.count({0.0f, 0.0f}) == 1);
        REQUIRE(uvs.count({1.0f, 0.0f}) == 1);
        REQUIRE(uvs.count({0.0f, 1.0f}) == 1);
        REQUIRE(uvs.count({1.0f, 1.0f}) == 1);
    }
}

TEST_CASE("Primitive consistency") {
    SECTION("given any primitive when checking triangle winding then indices form valid triangles") {
        auto cube = generateCube();
        REQUIRE(cube.indices.size() % 3 == 0);

        auto sphere = generateSphere();
        REQUIRE(sphere.indices.size() % 3 == 0);

        auto plane = generatePlane();
        REQUIRE(plane.indices.size() % 3 == 0);

        auto quad = generateQuad();
        REQUIRE(quad.indices.size() % 3 == 0);
    }
}
