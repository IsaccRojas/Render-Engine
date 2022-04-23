#pragma once

#ifndef GLUTIL_HPP_
#define GLUTIL_HPP_

#include <GL/glew.h>
#include <cstring>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <memory>
#include <iostream>

enum StageType { GLP_COMPUTE, GLP_RENDER };

//representation of an execution stage of OpenGL; stores program and associated VAO
class GLStage {
    StageType _type;
    GLuint _program_h;
    GLuint _vao_h;
    bool _has_elements;
public:
    GLStage(StageType type);
    
    GLStage(const GLStage&) = delete;
    GLStage& operator=(const GLStage&) = delete;
    
    GLStage(GLStage &&other);
    GLStage& operator=(GLStage &&other);

    ~GLStage();
    void program(char *shader_srcs[], GLenum shader_types[], int count);
    void attrib(GLuint buf_h, GLuint index, GLint size, GLenum type, GLsizei bytestride, GLvoid *byteoffset);
    void uniform(GLuint index, GLint value);
    void uniform(GLuint index, GLfloat value);
    void element(GLuint buf_h);
    void storage(GLuint buf_h, GLuint index);
    void render(GLsizei count);
    void compute(GLuint groups_x, GLuint groups_y, GLuint groups_z);
};

//class to preserve state of a single OpenGL buffer with a fixed usage and byte size
class GLBuffer {
    GLuint _buf_h;
    GLenum _usage;
    GLuint _size;

public:
    //create new GLBuffer with fixed usage and byte size
    GLBuffer(GLenum buffer_usage, GLuint buffer_size);
    
    GLBuffer(const GLBuffer&) = delete;
    GLBuffer& operator=(const GLBuffer&) = delete;
    
    GLBuffer(GLBuffer &&other);
    GLBuffer& operator=(GLBuffer &&other);

    ~GLBuffer();

    //binds buffer handle of GLBuffer to target
    void bind(GLenum target);
    //binds buffer handle of GLBuffer to target at base
    void bindbase(GLenum target, GLuint index);

    //update sub data in GLBuffer
    void subdata(GLsizeiptr data_size, const void *data, GLsizeiptr offset);

    GLuint size();
    GLenum usage();
    GLuint handle();

    const char *copy_mem();
};

//class to preserve state of a single mutable OpenGL 2D texture array
class GLTexture2DArray {
    GLuint _tex_h;
    GLuint _size;

    GLenum _levels;
    GLenum _storeformat;
    GLenum _dataformat;
    GLenum _type;

    GLuint _width;
    GLuint _height;
    GLuint _depth;

    bool _allocated;

    void _init();
public:
    //create new empty GLTexture
    GLTexture2DArray();

    GLTexture2DArray(const GLTexture2DArray&) = delete;
    GLTexture2DArray& operator=(const GLTexture2DArray&) = delete;

    GLTexture2DArray(GLTexture2DArray &&other);
    GLTexture2DArray& operator=(GLTexture2DArray &&other);

    ~GLTexture2DArray();

    //binds texture handle of GLTexture2D to target
    void bind(GLenum target);

    void parameteri(GLenum param, GLint value);

    //allocate memory storage
    void alloc(GLint levels, GLenum storeformat, GLenum dataformat, GLenum type, GLsizei width, GLsizei height, GLsizei depth);

    //write sub image data into level of allocated storage
    void subimage(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, const void *data);

    void clear();

    GLuint size();
    GLuint width();
    GLuint height();
    GLuint depth();
};

//class to preserve state of a single mutable OpenGL 2D texture
class GLTexture2D {
    GLuint _tex_h;
    GLuint _size;
    
    void _init();

public:
    //create new GLTexture with data
    GLTexture2D(GLenum storeformat, GLenum dataformat, GLint writelevel, GLsizei width, GLsizei height, GLenum type, const void *data);
    //create new empty GLTexture
    GLTexture2D();

    GLTexture2D(const GLTexture2D&) = delete;
    GLTexture2D& operator=(const GLTexture2D&) = delete;

    GLTexture2D(GLTexture2D &&other);
    GLTexture2D& operator=(GLTexture2D &&other);

    ~GLTexture2D();

    //binds texture handle of GLTexture2D to target
    void bind(GLenum target);

    void parameteri(GLenum param, GLint value);

    void image(GLint level, GLenum storeformat, GLsizei width, GLsizei height, GLenum dataformat, GLenum type, const void *data);

    //write sub image data into level of allocated storage
    void subimage(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum dataformat, GLenum type, const void *data);

    GLuint size();
};

//class to wrap vertex/index data with GLBuffers
class Model {
    //vec4's of model data
    std::unique_ptr<float> _mod;
    //unsigned ints as elements of model data (draw order)
    std::unique_ptr<unsigned> _elem;
    unsigned _modsize;
    unsigned _elemsize;

    //number of vertices in model
    unsigned _numpts;

    //buffer references and offsets in bytes
    GLBuffer *_mbuf;
    unsigned _moff;
    GLBuffer *_ebuf;
    unsigned _eoff;

public:
    Model(
        float *model, unsigned *elements, unsigned model_size, unsigned elements_size, unsigned num_points, 
        GLBuffer *mbuf = NULL, unsigned moffset = 0, GLBuffer *ebuf = NULL, unsigned eoffset = 0
    );
    Model(
        const Model &baseModel, unsigned id,
        GLBuffer *mbuf = NULL, unsigned moffset = 0, GLBuffer *ebuf = NULL, unsigned eoffset = 0
    );
    Model(const Model &other);
    Model();
    ~Model();
    Model& operator=(const Model &other);

    float* copymod() const;
    unsigned* copyelem() const;
    unsigned modsize() const;
    unsigned elemsize() const;
    unsigned numpts() const;

    //set data ID of vec4's
    static float* setModelID(float *data, unsigned size, unsigned new_id);
    //set data ID of elements (unsigned ints)
    static unsigned* setElementsID(unsigned *data, unsigned size, unsigned num_points, unsigned new_id, unsigned old_id = 0);

    unsigned moff() const;
    unsigned eoff() const;

    void updatemod();
    void updateelem();
};

class Matrix {
    glm::mat4 _mat;

public:
    glm::vec3 pos;
    glm::vec3 scale;
    Matrix(glm::vec3 matpos, glm::vec3 matscale);
    Matrix();

    float* data();
};

//class to wrap a glm matrix with a GLBuffer
class Transform {
public:
    Matrix mat;

private:
    //buffer reference and offset in bytes
    GLBuffer *_tbuf;
    unsigned _toff;

public:
    Transform(GLBuffer *tbuf, unsigned offset);
    Transform(const Transform &other) = default;
    Transform();
    Transform& operator=(const Transform &other) = default;

    unsigned toff() const;

    //update transform buffer's data
    void updatet();
};

//representation of quad of texture, used to transform TexCoords
class TexFrame {
    int _xo;
    int _yo;
    int _zo;
    int _w;
    int _h;
public:
    TexFrame(int xoff, int yoff, int zoff, int width, int height);
    TexFrame();
    TexFrame& set(int xoff, int yoff, int zoff, int width, int height);
    void zero();
    int xoff();
    int yoff();
    int zoff();
    int width();
    int height();
    float* data();
    bool is_zero();
};

//class to wrap texture UV data with a GLBuffer
class TexCoords {
    //UV-depth coordinates (with 4th component padding)
    std::unique_ptr<float> _tc;
    unsigned _tcsize;

    //number of coordinates
    unsigned _numcoords;

public:
    TexFrame frame;

private:
    //buffer reference and offset in bytes
    GLBuffer *_tcbuf;
    unsigned _tcoff;

public:
    TexCoords(
        unsigned num_coords, 
        GLBuffer *tcbuf = NULL, unsigned tcoffset = 0
    );
    TexCoords(const TexCoords &other);
    TexCoords();
    ~TexCoords();
    TexCoords& operator=(const TexCoords &other);

    float* copytc() const;
    unsigned tcsize() const;

    unsigned tcoff() const;

    void updatetc();
};

#endif