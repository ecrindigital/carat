#include <game_engine/graphics/primitives.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

namespace game_engine::graphics {

    PrimitiveData generateCube(float size) {
        PrimitiveData data;

        float h = size * 0.5f;

        auto addFace = [&](glm::vec3 normal, glm::vec3 up, glm::vec3 right) {
            uint32_t baseIndex = static_cast<uint32_t>(data.vertices.size());

            glm::vec3 center = normal * h;
            glm::vec3 v0 = center - right * h - up * h;
            glm::vec3 v1 = center + right * h - up * h;
            glm::vec3 v2 = center + right * h + up * h;
            glm::vec3 v3 = center - right * h + up * h;

            data.vertices.push_back({v0.x, v0.y, v0.z, normal.x, normal.y, normal.z, 0.0f, 0.0f});
            data.vertices.push_back({v1.x, v1.y, v1.z, normal.x, normal.y, normal.z, 1.0f, 0.0f});
            data.vertices.push_back({v2.x, v2.y, v2.z, normal.x, normal.y, normal.z, 1.0f, 1.0f});
            data.vertices.push_back({v3.x, v3.y, v3.z, normal.x, normal.y, normal.z, 0.0f, 1.0f});

            data.indices.push_back(baseIndex + 0);
            data.indices.push_back(baseIndex + 1);
            data.indices.push_back(baseIndex + 2);
            data.indices.push_back(baseIndex + 0);
            data.indices.push_back(baseIndex + 2);
            data.indices.push_back(baseIndex + 3);
        };

        addFace({0, 0, 1}, {0, 1, 0}, {1, 0, 0});
        addFace({0, 0, -1}, {0, 1, 0}, {-1, 0, 0});
        addFace({1, 0, 0}, {0, 1, 0}, {0, 0, -1});
        addFace({-1, 0, 0}, {0, 1, 0}, {0, 0, 1});
        addFace({0, 1, 0}, {0, 0, -1}, {1, 0, 0});
        addFace({0, -1, 0}, {0, 0, 1}, {1, 0, 0});

        return data;
    }

    PrimitiveData generateSphere(float radius, int rings, int sectors) {
        PrimitiveData data;

        float const R = 1.0f / static_cast<float>(rings - 1);
        float const S = 1.0f / static_cast<float>(sectors - 1);

        for (int r = 0; r < rings; ++r) {
            for (int s = 0; s < sectors; ++s) {
                float const y = std::sin(-glm::half_pi<float>() + glm::pi<float>() * r * R);
                float const x = std::cos(2.0f * glm::pi<float>() * s * S) * std::sin(glm::pi<float>() * r * R);
                float const z = std::sin(2.0f * glm::pi<float>() * s * S) * std::sin(glm::pi<float>() * r * R);

                float u = s * S;
                float v = r * R;

                data.vertices.push_back({
                    x * radius, y * radius, z * radius,
                    x, y, z,
                    u, v
                });
            }
        }

        for (int r = 0; r < rings - 1; ++r) {
            for (int s = 0; s < sectors - 1; ++s) {
                uint32_t cur = r * sectors + s;
                uint32_t next = cur + sectors;

                data.indices.push_back(cur);
                data.indices.push_back(next);
                data.indices.push_back(cur + 1);

                data.indices.push_back(cur + 1);
                data.indices.push_back(next);
                data.indices.push_back(next + 1);
            }
        }

        return data;
    }

    PrimitiveData generatePlane(float width, float height) {
        PrimitiveData data;

        float hw = width * 0.5f;
        float hh = height * 0.5f;

        data.vertices.push_back({-hw, 0.0f, -hh, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f});
        data.vertices.push_back({ hw, 0.0f, -hh, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f});
        data.vertices.push_back({ hw, 0.0f,  hh, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f});
        data.vertices.push_back({-hw, 0.0f,  hh, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f});

        data.indices = {0, 1, 2, 0, 2, 3};

        return data;
    }

    PrimitiveData generateQuad(float width, float height) {
        PrimitiveData data;

        float hw = width * 0.5f;
        float hh = height * 0.5f;

        data.vertices.push_back({-hw, -hh, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f});
        data.vertices.push_back({ hw, -hh, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f});
        data.vertices.push_back({ hw,  hh, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
        data.vertices.push_back({-hw,  hh, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f});

        data.indices = {0, 1, 2, 0, 2, 3};

        return data;
    }

}
