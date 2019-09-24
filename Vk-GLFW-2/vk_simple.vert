# version 450
# extension GL_ARB_separate_shader_objects : enable

// ========================================================================= \\

out gl_PerVertex
{
    vec4 gl_Position;
};

// ========================================================================= \\

// out VS_OUT
// {
//     vec3 N;
//     vec3 V;
// } vs_out;

// ========================================================================= \\

// layout (location = 0) in vec3 in_Position;
// layout (location = 1) in vec3 in_Normal;

// ========================================================================= \\

// uniform mat4 pr_matrix;
// uniform mat4 mv_matrix;

// ========================================================================= \\

// Clip Space Vertices
// Clip Space in Vulkan: -1 <-> +1 in X, -1 <-> +1 in Y
// Clip Space in Vulkan: Y - Axis is pointing down! (Gl is up)

const vec4 in_Position[3] = vec4[3](vec4( 0.0, -0.5, 0.0, 1.0),
                                    vec4( 0.5,  0.5, 0.0, 1.0),
                                    vec4(-0.5,  0.5, 0.0, 1.0));

void main(void)
{
    // // Calculate view-space coordinate
    // vec4 P = mv_matrix * vec4(in_Position, 1);

    // // Calculate normal in view-space
    // vs_out.N = (transpose(inverse(mv_matrix)) * vec4(in_Normal, 0)).xyz;

    // // Calculate view vector
    // vs_out.V = -1 * P.xyz;

    // // Calculate the clip-space position of each vertex
    // gl_Position = pr_matrix * P;

    // OpenGL: gl_VertexID Vulkan: gl_VertexIndex ??
    // No vs_out.gl_Position ?

    gl_Position = in_Position[gl_VertexIndex];
}

// ========================================================================= \\

