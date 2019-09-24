# version 450 core
# extension GL_ARB_separate_shader_objects : enable

// ========================================================================= \\

// in VS_OUT
// {
//     vec3 N;
//     vec3 V;
// } fs_in;

// ========================================================================= \\
// MRT Outputs
layout (location = 0) out vec4 frag_0;
// layout (location = 1) out vec4 frag_1;
// layout (location = 2) out vec4 frag_2;
// layout (location = 3) out vec4 frag_3;

// ========================================================================= \\

// uniform int     Shading_Mode;
// uniform float   Metalness;
// uniform float   Roughness;
// uniform float   Transparency;

// // Todo: PBS Material UBO here
// uniform FrontMaterial
// {
//     vec4 ambient, diffuse, specular;
//     float exponent;
// };

// ========================================================================= \\

void main(void)
{
    // NVidia Green: 118, 185, 0
    frag_0 = vec4(0.4627, 0.7254, 0.0, 1.0);
}