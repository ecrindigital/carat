#include <game_engine/graphics/builtin_shaders.hpp>
#include <stdexcept>

namespace game_engine::graphics {

    constexpr std::string_view UNLIT_VERTEX = R"(
struct CameraUniforms {
    view: mat4x4<f32>,
    projection: mat4x4<f32>,
}

struct ModelUniforms {
    model: mat4x4<f32>,
}

struct VertexInput {
    @location(0) position: vec3<f32>,
}

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
}

@group(0) @binding(0) var<uniform> camera: CameraUniforms;
@group(1) @binding(0) var<uniform> model: ModelUniforms;

@vertex
fn main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.position = camera.projection * camera.view * model.model * vec4<f32>(in.position, 1.0);
    return out;
}
)";

    constexpr std::string_view UNLIT_FRAGMENT = R"(
struct MaterialUniforms {
    color: vec4<f32>,
    uv_offset: vec2<f32>,
    uv_scale: vec2<f32>,
    flip_x: u32,
    flip_y: u32,
    _padding: vec2<f32>,
}

@group(2) @binding(0) var<uniform> material: MaterialUniforms;

@fragment
fn main() -> @location(0) vec4<f32> {
    return material.color;
}
)";

    constexpr std::string_view UNLIT_TEXTURED_VERTEX = R"(
struct CameraUniforms {
    view: mat4x4<f32>,
    projection: mat4x4<f32>,
}

struct ModelUniforms {
    model: mat4x4<f32>,
}

struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) uv: vec2<f32>,
}

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
}

@group(0) @binding(0) var<uniform> camera: CameraUniforms;
@group(1) @binding(0) var<uniform> model: ModelUniforms;

@vertex
fn main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.position = camera.projection * camera.view * model.model * vec4<f32>(in.position, 1.0);
    out.uv = in.uv;
    return out;
}
)";

    constexpr std::string_view UNLIT_TEXTURED_FRAGMENT = R"(
struct MaterialUniforms {
    color: vec4<f32>,
    uv_offset: vec2<f32>,
    uv_scale: vec2<f32>,
    flip_x: u32,
    flip_y: u32,
    _padding: vec2<f32>,
}

@group(2) @binding(0) var<uniform> material: MaterialUniforms;
@group(2) @binding(1) var tex: texture_2d<f32>;
@group(2) @binding(2) var tex_sampler: sampler;

@fragment
fn main(in: VertexOutput) -> @location(0) vec4<f32> {
    var uv = in.uv * material.uv_scale + material.uv_offset;

    if (material.flip_x != 0u) {
        uv.x = 1.0 - uv.x;
    }
    if (material.flip_y != 0u) {
        uv.y = 1.0 - uv.y;
    }

    let tex_color = textureSample(tex, tex_sampler, uv);
    return tex_color * material.color;
}

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
}
)";

    constexpr std::string_view SPRITE_VERTEX = R"(
struct CameraUniforms {
    view: mat4x4<f32>,
    projection: mat4x4<f32>,
}

struct ModelUniforms {
    model: mat4x4<f32>,
}

struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) uv: vec2<f32>,
}

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
}

@group(0) @binding(0) var<uniform> camera: CameraUniforms;
@group(1) @binding(0) var<uniform> model: ModelUniforms;

@vertex
fn main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.position = camera.projection * camera.view * model.model * vec4<f32>(in.position, 1.0);
    out.uv = in.uv;
    return out;
}
)";

    constexpr std::string_view SPRITE_FRAGMENT = R"(
struct MaterialUniforms {
    color: vec4<f32>,
    uv_offset: vec2<f32>,
    uv_scale: vec2<f32>,
    flip_x: u32,
    flip_y: u32,
    _padding: vec2<f32>,
}

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
}

@group(2) @binding(0) var<uniform> material: MaterialUniforms;
@group(2) @binding(1) var tex: texture_2d<f32>;
@group(2) @binding(2) var tex_sampler: sampler;

@fragment
fn main(in: VertexOutput) -> @location(0) vec4<f32> {
    var uv = in.uv * material.uv_scale + material.uv_offset;

    if (material.flip_x != 0u) {
        uv.x = 1.0 - uv.x;
    }
    if (material.flip_y != 0u) {
        uv.y = 1.0 - uv.y;
    }

    let tex_color = textureSample(tex, tex_sampler, uv);
    let final_color = tex_color * material.color;

    if (final_color.a < 0.01) {
        discard;
    }

    return final_color;
}
)";

    static const ShaderDefinition SHADER_DEFINITIONS[] = {
        {
            UNLIT_VERTEX,
            UNLIT_FRAGMENT,
            VertexLayoutType::Position,
            true,
            false
        },
        {
            UNLIT_TEXTURED_VERTEX,
            UNLIT_TEXTURED_FRAGMENT,
            VertexLayoutType::PositionUV,
            true,
            true
        },
        {
            SPRITE_VERTEX,
            SPRITE_FRAGMENT,
            VertexLayoutType::PositionUV,
            true,
            true
        },
        {
            UNLIT_VERTEX,
            UNLIT_FRAGMENT,
            VertexLayoutType::PositionNormalUV,
            true,
            true
        },
        {
            UNLIT_VERTEX,
            UNLIT_FRAGMENT,
            VertexLayoutType::PositionNormalUV,
            true,
            true
        }
    };

    const ShaderDefinition& getShaderDefinition(BuiltinShader shader) {
        auto index = static_cast<size_t>(shader);
        if (index >= sizeof(SHADER_DEFINITIONS) / sizeof(SHADER_DEFINITIONS[0])) {
            throw std::out_of_range("Invalid BuiltinShader enum value");
        }
        return SHADER_DEFINITIONS[index];
    }

}
