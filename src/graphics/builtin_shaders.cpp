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
    @location(1) normal: vec3<f32>,
    @location(2) uv: vec2<f32>,
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
struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
}

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
    @location(1) normal: vec3<f32>,
    @location(2) uv: vec2<f32>,
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

    constexpr std::string_view PBR_VERTEX = R"(
struct CameraUniforms {
    view: mat4x4<f32>,
    projection: mat4x4<f32>,
}

struct ModelUniforms {
    model: mat4x4<f32>,
}

struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) normal: vec3<f32>,
    @location(2) uv: vec2<f32>,
}

struct VertexOutput {
    @builtin(position) clip_position: vec4<f32>,
    @location(0) world_position: vec3<f32>,
    @location(1) world_normal: vec3<f32>,
    @location(2) uv: vec2<f32>,
}

@group(0) @binding(0) var<uniform> camera: CameraUniforms;
@group(1) @binding(0) var<uniform> model: ModelUniforms;

@vertex
fn main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    let world_pos = model.model * vec4<f32>(in.position, 1.0);
    out.world_position = world_pos.xyz;
    out.world_normal = normalize((model.model * vec4<f32>(in.normal, 0.0)).xyz);
    out.uv = in.uv;
    out.clip_position = camera.projection * camera.view * world_pos;
    return out;
}
)";

    constexpr std::string_view PBR_FRAGMENT = R"(
const PI: f32 = 3.14159265359;

struct DirectionalLight {
    direction: vec3<f32>,
    _pad1: f32,
    color: vec3<f32>,
    intensity: f32,
}

struct PointLight {
    position: vec3<f32>,
    range: f32,
    color: vec3<f32>,
    intensity: f32,
}

struct LightingData {
    ambient_color: vec4<f32>,
    camera_position: vec3<f32>,
    _pad1: f32,
    directional_lights: array<DirectionalLight, 4>,
    point_lights: array<PointLight, 8>,
    num_directional: u32,
    num_point: u32,
    _pad2: vec2<f32>,
}

struct PBRMaterial {
    albedo: vec4<f32>,
    metallic: f32,
    roughness: f32,
    ao: f32,
    _pad1: f32,
    emissive: vec3<f32>,
    emissive_intensity: f32,
}

struct VertexOutput {
    @builtin(position) clip_position: vec4<f32>,
    @location(0) world_position: vec3<f32>,
    @location(1) world_normal: vec3<f32>,
    @location(2) uv: vec2<f32>,
}

@group(2) @binding(0) var<uniform> material: PBRMaterial;
@group(3) @binding(0) var<uniform> lighting: LightingData;

fn distribution_ggx(N: vec3<f32>, H: vec3<f32>, roughness: f32) -> f32 {
    let a = roughness * roughness;
    let a2 = a * a;
    let NdotH = max(dot(N, H), 0.0);
    let NdotH2 = NdotH * NdotH;
    let nom = a2;
    var denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / denom;
}

fn geometry_schlick_ggx(NdotV: f32, roughness: f32) -> f32 {
    let r = (roughness + 1.0);
    let k = (r * r) / 8.0;
    let nom = NdotV;
    let denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}

fn geometry_smith(N: vec3<f32>, V: vec3<f32>, L: vec3<f32>, roughness: f32) -> f32 {
    let NdotV = max(dot(N, V), 0.0);
    let NdotL = max(dot(N, L), 0.0);
    let ggx2 = geometry_schlick_ggx(NdotV, roughness);
    let ggx1 = geometry_schlick_ggx(NdotL, roughness);
    return ggx1 * ggx2;
}

fn fresnel_schlick(cos_theta: f32, F0: vec3<f32>) -> vec3<f32> {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

fn calculate_light(
    light_dir: vec3<f32>,
    light_color: vec3<f32>,
    light_intensity: f32,
    N: vec3<f32>,
    V: vec3<f32>,
    F0: vec3<f32>,
    albedo: vec3<f32>,
    metallic: f32,
    roughness: f32
) -> vec3<f32> {
    let L = normalize(light_dir);
    let H = normalize(V + L);
    let radiance = light_color * light_intensity;

    let NDF = distribution_ggx(N, H, roughness);
    let G = geometry_smith(N, V, L, roughness);
    let F = fresnel_schlick(max(dot(H, V), 0.0), F0);

    let numerator = NDF * G * F;
    let denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    let specular = numerator / denominator;

    let kS = F;
    var kD = vec3<f32>(1.0) - kS;
    kD = kD * (1.0 - metallic);

    let NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

@fragment
fn main(in: VertexOutput) -> @location(0) vec4<f32> {
    let albedo = material.albedo.rgb;
    let metallic = material.metallic;
    let roughness = material.roughness;
    let ao = material.ao;

    let N = normalize(in.world_normal);
    let V = normalize(lighting.camera_position - in.world_position);

    var F0 = vec3<f32>(0.04);
    F0 = mix(F0, albedo, metallic);

    var Lo = vec3<f32>(0.0);

    for (var i: u32 = 0u; i < lighting.num_directional; i = i + 1u) {
        let light = lighting.directional_lights[i];
        Lo = Lo + calculate_light(
            -light.direction,
            light.color,
            light.intensity,
            N, V, F0, albedo, metallic, roughness
        );
    }

    for (var i: u32 = 0u; i < lighting.num_point; i = i + 1u) {
        let light = lighting.point_lights[i];
        let light_vec = light.position - in.world_position;
        let distance = length(light_vec);
        let attenuation = clamp(1.0 - distance / light.range, 0.0, 1.0);
        let atten_squared = attenuation * attenuation;
        Lo = Lo + calculate_light(
            light_vec,
            light.color,
            light.intensity * atten_squared,
            N, V, F0, albedo, metallic, roughness
        );
    }

    let ambient = lighting.ambient_color.rgb * albedo * ao;
    var color = ambient + Lo;

    color = color + material.emissive * material.emissive_intensity;

    color = color / (color + vec3<f32>(1.0));
    color = pow(color, vec3<f32>(1.0 / 2.2));

    return vec4<f32>(color, material.albedo.a);
}
)";

    constexpr std::string_view GLASS_VERTEX = R"(
struct CameraUniforms {
    view: mat4x4<f32>,
    projection: mat4x4<f32>,
}

struct ModelUniforms {
    model: mat4x4<f32>,
}

struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) normal: vec3<f32>,
    @location(2) uv: vec2<f32>,
}

struct VertexOutput {
    @builtin(position) clip_position: vec4<f32>,
    @location(0) world_position: vec3<f32>,
    @location(1) world_normal: vec3<f32>,
    @location(2) uv: vec2<f32>,
}

@group(0) @binding(0) var<uniform> camera: CameraUniforms;
@group(1) @binding(0) var<uniform> model: ModelUniforms;

@vertex
fn main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    let world_pos = model.model * vec4<f32>(in.position, 1.0);
    out.world_position = world_pos.xyz;
    out.world_normal = normalize((model.model * vec4<f32>(in.normal, 0.0)).xyz);
    out.uv = in.uv;
    out.clip_position = camera.projection * camera.view * world_pos;
    return out;
}
)";

    constexpr std::string_view GLASS_FRAGMENT = R"(
struct DirectionalLight {
    direction: vec3<f32>,
    _pad1: f32,
    color: vec3<f32>,
    intensity: f32,
}

struct PointLight {
    position: vec3<f32>,
    range: f32,
    color: vec3<f32>,
    intensity: f32,
}

struct LightingData {
    ambient_color: vec4<f32>,
    camera_position: vec3<f32>,
    _pad1: f32,
    directional_lights: array<DirectionalLight, 4>,
    point_lights: array<PointLight, 8>,
    num_directional: u32,
    num_point: u32,
    _pad2: vec2<f32>,
}

struct GlassMaterial {
    tint: vec4<f32>,
    ior: f32,
    roughness: f32,
    fresnel_power: f32,
    reflectivity: f32,
    thickness: f32,
    dispersion: f32,
    _pad1: f32,
    _pad2: f32,
}

struct VertexOutput {
    @builtin(position) clip_position: vec4<f32>,
    @location(0) world_position: vec3<f32>,
    @location(1) world_normal: vec3<f32>,
    @location(2) uv: vec2<f32>,
}

@group(2) @binding(0) var<uniform> material: GlassMaterial;
@group(3) @binding(0) var<uniform> lighting: LightingData;

fn fresnel_schlick(cos_theta: f32, F0: f32, power: f32) -> f32 {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), power);
}

@fragment
fn main(in: VertexOutput) -> @location(0) vec4<f32> {
    let N = normalize(in.world_normal);
    let V = normalize(lighting.camera_position - in.world_position);

    let NdotV = max(dot(N, V), 0.0);
    let fresnel = fresnel_schlick(NdotV, material.reflectivity, material.fresnel_power);

    var reflection_color = vec3<f32>(0.0);
    let R = reflect(-V, N);

    for (var i: u32 = 0u; i < lighting.num_directional; i = i + 1u) {
        let light = lighting.directional_lights[i];
        let L = normalize(-light.direction);
        let H = normalize(V + L);
        let NdotH = max(dot(N, H), 0.0);

        let shininess = (1.0 - material.roughness) * 256.0 + 8.0;
        let spec = pow(NdotH, shininess) * light.intensity;
        reflection_color = reflection_color + light.color * spec;
    }

    for (var i: u32 = 0u; i < lighting.num_point; i = i + 1u) {
        let light = lighting.point_lights[i];
        let L = normalize(light.position - in.world_position);
        let H = normalize(V + L);
        let NdotH = max(dot(N, H), 0.0);
        let distance = length(light.position - in.world_position);
        let attenuation = clamp(1.0 - distance / light.range, 0.0, 1.0);

        let shininess = (1.0 - material.roughness) * 256.0 + 8.0;
        let spec = pow(NdotH, shininess) * light.intensity * attenuation * attenuation;
        reflection_color = reflection_color + light.color * spec;
    }

    let refraction_color = material.tint.rgb * lighting.ambient_color.rgb;

    let edge_glow = pow(1.0 - NdotV, 3.0) * 0.3;
    let edge_color = material.tint.rgb * edge_glow;

    var final_color = mix(refraction_color, reflection_color, fresnel);
    final_color = final_color + edge_color;
    final_color = final_color * material.tint.rgb;

    final_color = final_color / (final_color + vec3<f32>(1.0));
    final_color = pow(final_color, vec3<f32>(1.0 / 2.2));

    let alpha = mix(material.tint.a, 1.0, fresnel * 0.5);

    return vec4<f32>(final_color, alpha);
}
)";

    constexpr std::string_view GLOW_VERTEX = R"(
struct CameraUniforms {
    view: mat4x4<f32>,
    projection: mat4x4<f32>,
}

struct ModelUniforms {
    model: mat4x4<f32>,
}

struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) normal: vec3<f32>,
    @location(2) uv: vec2<f32>,
}

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
    @location(1) world_position: vec3<f32>,
}

@group(0) @binding(0) var<uniform> camera: CameraUniforms;
@group(1) @binding(0) var<uniform> model: ModelUniforms;

@vertex
fn main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    let world_pos = model.model * vec4<f32>(in.position, 1.0);
    out.world_position = world_pos.xyz;
    out.position = camera.projection * camera.view * world_pos;
    out.uv = in.uv;
    return out;
}
)";

    constexpr std::string_view GLOW_FRAGMENT = R"(
struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
    @location(1) world_position: vec3<f32>,
}

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
fn main(in: VertexOutput) -> @location(0) vec4<f32> {
    let center = vec2<f32>(0.5, 0.5);
    let dist = distance(in.uv, center) * 2.0;

    let core_glow = 1.0 - smoothstep(0.0, 0.3, dist);
    let mid_glow = (1.0 - smoothstep(0.0, 0.6, dist)) * 0.6;
    let outer_glow = (1.0 - smoothstep(0.0, 1.0, dist)) * 0.3;

    let glow_intensity = core_glow + mid_glow + outer_glow;

    let final_color = material.color.rgb * glow_intensity;
    let final_alpha = glow_intensity * material.color.a;

    if (final_alpha < 0.01) {
        discard;
    }

    return vec4<f32>(final_color, final_alpha);
}
)";

    static const ShaderDefinition SHADER_DEFINITIONS[] = {
        {
            UNLIT_VERTEX,
            UNLIT_FRAGMENT,
            VertexLayoutType::PositionNormalUV,
            true,
            false
        },
        {
            UNLIT_TEXTURED_VERTEX,
            UNLIT_TEXTURED_FRAGMENT,
            VertexLayoutType::PositionNormalUV,
            true,
            true
        },
        {
            SPRITE_VERTEX,
            SPRITE_FRAGMENT,
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
        },
        {
            PBR_VERTEX,
            PBR_FRAGMENT,
            VertexLayoutType::PositionNormalUV,
            true,
            false
        },
        {
            GLASS_VERTEX,
            GLASS_FRAGMENT,
            VertexLayoutType::PositionNormalUV,
            true,
            false
        },
        {
            GLOW_VERTEX,
            GLOW_FRAGMENT,
            VertexLayoutType::PositionNormalUV,
            true,
            false
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
