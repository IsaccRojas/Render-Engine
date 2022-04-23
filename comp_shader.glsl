#version 430

//input model data for transformation
layout(std430, binding = 0) buffer model_buf {
    vec4 model_verts[];
};

//input transform data
layout(std430, binding = 1) buffer transform_buf {
    mat4 transforms[];
};

//input texture coordinate data
layout(std430, binding = 2) buffer texcoords_buf {
    vec4 texcoords[];
};

//flag for drawing or skipping data
layout(std430, binding = 3) buffer draw_buf {
    float draw[];
};

//output vertex data
struct vertexData {
    vec4 vertex;
    vec4 texcoords;
};
layout(std430, binding = 4) buffer vertex_buf {
    vertexData vertices[];
};

//define group size (invocations; note that this therefore does not have to correspond to data size)
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main() {
    //invocation
    uint i = gl_WorkGroupID.x;
    //entity ID
    int id = int(model_verts[i].w);

    //compute transformation and store into output buffer (first vector) (zeroed if draw is 0)
    vertices[i].vertex = (vec4(model_verts[i].xyz, 1.0) * transpose(transforms[id])) * draw[id];
    
    //set first three elements of second vector to texture coords/depth
    vertices[i].texcoords = texcoords[i];
    return;
}
