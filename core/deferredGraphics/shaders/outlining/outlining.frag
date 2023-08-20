#version 450

#include "../__methods__/defines.glsl"

layout(set = 3, binding = 0) uniform sampler2D baseColorTexture;
layout(set = 3, binding = 1) uniform sampler2D metallicRoughnessTexture;
layout(set = 3, binding = 2) uniform sampler2D normalTexture;
layout(set = 3, binding = 3) uniform sampler2D occlusionTexture;
layout(set = 3, binding = 4) uniform sampler2D emissiveTexture;

layout(location = 0) in vec4 position;
layout(location = 1) in float depth;
layout(location = 2) in vec2 UV0;
layout(location = 3) in vec2 UV1;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outBaseColor;
layout(location = 3) out vec4 outEmissiveTexture;

layout(push_constant) uniform Stencil {
    vec4 color;
    float width;
} stencil;

void main() {
    outPosition = position;
    outBaseColor = vec4(0.0, 0.0, 0.0, 0.0);
    outNormal = vec4(0.0, 0.0, 0.0, 0.0);
    outEmissiveTexture = stencil.color;
}
