#version 460

layout(location = 1) in vec4 v_pos;
layout(location = 2) in vec3 v_texcoords;

layout(location = 4) uniform mat4 u_view;
layout(location = 5) uniform mat4 u_proj;

out vec3 f_texcoords;

void main() {
    f_texcoords = v_texcoords;
    gl_Position = u_proj * u_view * v_pos;
}