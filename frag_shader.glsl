#version 460

layout(location = 6) uniform sampler2DArray texsamplerarray;

in vec3 f_texcoords;
in float f_texdepth;

out vec4 fragcolor;

void main() {
    vec4 texel = texelFetch(
        texsamplerarray,
        ivec3(round(f_texcoords.x), round(f_texcoords.y), round(f_texcoords.z)),
        0
    );
    
    if (texel.xyz == vec3(230.0/255.0, 85.0/255.0, 230.0/255.0))
        discard;
    else
        fragcolor = texel;
}