#include "glutil.hpp"

// _______________________________________ GLStage _______________________________________

//representation of an execution stage of OpenGL; stores program and associated VAO

GLStage::GLStage(StageType type) : _type(type), _program_h(0), _has_elements(false) {
    glGenVertexArrays(1, &_vao_h);
}

GLStage::GLStage(GLStage &&other) :
    _type(other._type),
    _program_h(other._program_h),
    _vao_h(other._vao_h),
    _has_elements(other._has_elements)
{
    other._program_h = 0;
    other._vao_h = 0;
    other._has_elements = false;
}

GLStage& GLStage::operator=(GLStage &&other) {
    if (this != &other) {
        glDeleteVertexArrays(1, &_vao_h);
        glDeleteProgram(_program_h);
        _vao_h = other._vao_h;
        _program_h = other._program_h;
        _has_elements = other._has_elements;
        other._vao_h = 0;
        other._program_h = 0;
        other._has_elements = false;
    }
    return *this;
}

GLStage::~GLStage() {
    glDeleteVertexArrays(1, &_vao_h);
    glDeleteProgram(_program_h);
    _has_elements = false;
};

void GLStage::program(char *shader_srcs[], GLenum shader_types[], int count) {
    //create shaders
    GLuint *shaders = new GLuint[count];
    for (int i = 0; i < count; i++) {
        shaders[i] = glCreateShader(shader_types[i]);
        glShaderSource(shaders[i], 1, &shader_srcs[i], NULL);
        glCompileShader(shaders[i]);
    }

    //attach to program and link
    _program_h = glCreateProgram();
    for (int i = 0; i < count; i++)
        glAttachShader(_program_h, shaders[i]);
    glLinkProgram(_program_h);

    //detach and delete shaders
    for (int i = 0; i < count; i++) {
        glDetachShader(_program_h, shaders[i]);
        glDeleteShader(shaders[i]);
    }
    delete[] shaders;
}

void GLStage::attrib(GLuint buf_h, GLuint index, GLint size, GLenum type, GLsizei bytestride, GLvoid *byteoffset) {
    glUseProgram(_program_h);
    glBindVertexArray(_vao_h);
    glBindBuffer(GL_ARRAY_BUFFER, buf_h);

    glVertexAttribPointer(index, size, type, false, bytestride, byteoffset);
    glEnableVertexAttribArray(index);
}

void GLStage::uniform(GLuint index, GLint value) {
    glUseProgram(_program_h);
    glUniform1i(index, value);
}
void GLStage::uniform(GLuint index, GLfloat value) {
    glUseProgram(_program_h);
    glUniform1f(index, value);
}

void GLStage::element(GLuint buf_h) {
    glBindVertexArray(_vao_h);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf_h);
    _has_elements = true;
}

void GLStage::storage(GLuint buf_h, GLuint index) {
    glBindVertexArray(_vao_h);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buf_h);
}

void GLStage::render(GLsizei count) {
    glBindVertexArray(_vao_h);
    glUseProgram(_program_h);

    if (_has_elements)
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, NULL);
    else
        glDrawArrays(GL_TRIANGLES, 0, count);
}

void GLStage::compute(GLuint groups_x, GLuint groups_y, GLuint groups_z) {
    glBindVertexArray(_vao_h);
    glUseProgram(_program_h);

    glDispatchCompute(groups_x, groups_y, groups_z);    
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

// _______________________________________ GLBuffer _______________________________________

GLBuffer::GLBuffer(GLenum buffer_usage, GLuint buffer_size) :
    _usage(buffer_usage),
    _size(buffer_size)
{
    //create empty data space
    glCreateBuffers(1, &_buf_h);
    glNamedBufferData(_buf_h, _size, NULL, _usage);
}

GLBuffer::GLBuffer(GLBuffer &&other) : 
    _buf_h(other._buf_h),
    _usage(other._usage),
    _size(other._size)
{
    other._buf_h = 0;
}

GLBuffer& GLBuffer::operator=(GLBuffer &&other) {
    if (this != &other) {
        glDeleteBuffers(1, &_buf_h);
        _buf_h = other._buf_h;
        other._buf_h = 0;
    }
    return *this;
}

GLBuffer::~GLBuffer() {
    glDeleteBuffers(1, &_buf_h);
}

void GLBuffer::bind(GLenum target) {
    glBindBuffer(target, _buf_h);
}

void GLBuffer::bindbase(GLenum target, GLuint index) {
    glBindBufferBase(target, index, _buf_h); 
}

void GLBuffer::subdata(GLsizeiptr data_size, const void *data, GLsizeiptr offset) {
    if (offset + data_size >= _size) {
        //TODO: throw exception
    }

    glNamedBufferSubData(_buf_h, offset, data_size, data);
}

GLuint GLBuffer::size() { return _size; }
GLenum GLBuffer::usage() { return _usage; }
GLuint GLBuffer::handle() { return _buf_h; }

const char *GLBuffer::copy_mem() {
    void *buf = glMapNamedBuffer(_buf_h, GL_READ_ONLY);
    char *mem = new char[_size];
    std::memcpy(mem, buf, _size);
    bool res = glUnmapNamedBuffer(_buf_h);

    if (res)
        return mem;
    else {
        delete mem;
        return nullptr;
    }
}

// _______________________________________ GLTexture2DArray _______________________________________

void GLTexture2DArray::_init() {
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &_tex_h);
    parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    parameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

//create new empty GLTexture2D
GLTexture2DArray::GLTexture2DArray() :
    _size(0),
    _levels(0),
    _storeformat(0),
    _dataformat(0),
    _type(0),
    _width(0),
    _height(0),
    _depth(0),
    _allocated(false)
{
    _init();
}

GLTexture2DArray::GLTexture2DArray(GLTexture2DArray &&other) : 
    _tex_h(other._tex_h),
    _size(other._size),
    _levels(other._levels),
    _storeformat(other._storeformat),
    _dataformat(other._dataformat),
    _type(other._type),
    _width(other._width),
    _height(other._height),
    _depth(other._depth),
    _allocated(other._allocated)
{
    other._tex_h = 0;
    other._size = 0;
    other._levels = 0;
    other._storeformat = 0;
    other._dataformat = 0;
    other._type = 0;
    other._width = 0;
    other._height = 0;
    other._depth = 0;
    other._allocated = false;
}

GLTexture2DArray& GLTexture2DArray::operator=(GLTexture2DArray &&other) {
    if (this != &other) {
        glDeleteTextures(1, &_tex_h);
        _tex_h = other._tex_h;
        _size = other._size;
        _levels = other._levels;
        _storeformat = other._storeformat;
        _dataformat = other._dataformat;
        _type = other._type;
        _width = other._width;
        _height = other._height;
        _depth = other._depth;
        _allocated = other._allocated;
        other._tex_h = 0;
        other._size = 0;
        other._levels = 0;
        other._storeformat = 0;
        other._dataformat = 0;
        other._type = 0;
        other._width = 0;
        other._height = 0;
        other._depth = 0;
        other._allocated = false;
    }
    return *this;
}

GLTexture2DArray::~GLTexture2DArray() {
    glDeleteTextures(1, &_tex_h);
}

void GLTexture2DArray::bind(GLenum target) {
    glBindTexture(target, _tex_h);
}

void GLTexture2DArray::parameteri(GLenum param, GLint value) {
    glTextureParameteri(_tex_h, param, value);
}

void GLTexture2DArray::alloc(GLint levels, GLenum storeformat, GLenum dataformat, GLenum type, GLsizei width, GLsizei height, GLsizei depth) {
    if (_allocated)
        return;
    
    _levels = levels;
    _storeformat = storeformat;
    _dataformat = dataformat;
    _type = type;

    _width = width;
    _height = height;
    _depth = depth;

    bind(GL_TEXTURE_2D_ARRAY);
    GLsizei levelwidth;
    GLsizei levelheight;
    for (int i = 0; i < levels; i++) {
        levelwidth = _width / glm::exp(i);
        levelheight = _height / glm::exp(i);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, i, _storeformat, levelwidth, levelheight, _depth, 0, _dataformat, _type, NULL);
    }
    _size = _width * _height * _depth;

    _allocated = true;
}

void GLTexture2DArray::subimage(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, const void *data) {
    glTextureSubImage3D(_tex_h, level, xoffset, yoffset, zoffset, width, height, 1, _dataformat, _type, data);
}

void GLTexture2DArray::clear() {
    glDeleteTextures(1, &_tex_h);
    _size = 0;
    _levels = 0;
    _storeformat = 0;
    _dataformat = 0;
    _type = 0;
    _width = 0;
    _height = 0;
    _depth = 0;
    _allocated = false;
    _init();
}

GLuint GLTexture2DArray::size() { return _size; }
GLuint GLTexture2DArray::width() { return _width; }
GLuint GLTexture2DArray::height() { return _height; }
GLuint GLTexture2DArray::depth() { return _depth; }

// _______________________________________ GLTexture2D _______________________________________

void GLTexture2D::_init() {
    glCreateTextures(GL_TEXTURE_2D, 1, &_tex_h);
    parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    parameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

GLTexture2D::GLTexture2D(GLenum storeformat, GLenum dataformat, GLint writelevel, GLsizei width, GLsizei height, GLenum type, const void *data) {
    _init();
    image(writelevel, storeformat, width, height, dataformat, type, data);
}

//create new empty GLTexture2D
GLTexture2D::GLTexture2D() {
    _init();
    _size = 0;
}

GLTexture2D::GLTexture2D(GLTexture2D &&other) : 
    _tex_h(other._tex_h),
    _size(other._size)
{
    other._tex_h = 0;
}

GLTexture2D& GLTexture2D::operator=(GLTexture2D &&other) {
    if (this != &other) {
        glDeleteTextures(1, &_tex_h);
        _tex_h = other._tex_h;
        other._tex_h = 0;
    }
    return *this;
}

GLTexture2D::~GLTexture2D() {
    glDeleteTextures(1, &_tex_h);
}

void GLTexture2D::bind(GLenum target) {
    glBindTexture(target, _tex_h);
}

void GLTexture2D::parameteri(GLenum param, GLint value) {
    glTextureParameteri(_tex_h, param, value);
}

void GLTexture2D::image(GLint level, GLenum storeformat, GLsizei width, GLsizei height, GLenum dataformat, GLenum type, const void *data) {
    bind(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, level, storeformat, width, height, 0, dataformat, type, data);
    _size = width * height;
}

void GLTexture2D::subimage(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum dataformat, GLenum type, const void *data) {
    glTextureSubImage2D(_tex_h, level, xoffset, yoffset, width, height, dataformat, type, data);
}

GLuint GLTexture2D::size() { return _size; }

// _______________________________________ Model _______________________________________

//class to wrap vertex/index data with GLBuffers
Model::Model(
    float *model, unsigned *elements, unsigned model_size, unsigned elements_size, unsigned num_points, 
    GLBuffer *mbuf, unsigned moffset, GLBuffer *ebuf, unsigned eoffset
) : 
    _mod(model), 
    _elem(elements), 
    _modsize(model_size), 
    _elemsize(elements_size),
    _numpts(num_points),
    _mbuf(mbuf),
    _moff(moffset),
    _ebuf(ebuf),
    _eoff(eoffset)
{}

Model::Model(
    const Model &baseModel, unsigned id,
    GLBuffer *mbuf, unsigned moffset, GLBuffer *ebuf, unsigned eoffset
) : 
    _mod(baseModel.copymod()), 
    _elem(baseModel.copyelem()), 
    _modsize(baseModel._modsize), 
    _elemsize(baseModel._elemsize),
    _numpts(baseModel._numpts),
    _mbuf(mbuf),
    _moff(moffset),
    _ebuf(ebuf),
    _eoff(eoffset)
{
    Model::setModelID(_mod.get(), _modsize, id);
    Model::setElementsID(_elem.get(), _elemsize, _numpts, id);
}

Model::Model(const Model &other) :
    _mod(other.copymod()),
    _elem(other.copyelem()),
    _modsize(other._modsize),
    _elemsize(other._elemsize),
    _numpts(other._numpts),
    _mbuf(other._mbuf),
    _moff(other._moff),
    _ebuf(other._ebuf),
    _eoff(other._eoff)
{}

Model::Model() :
    _modsize(0),
    _elemsize(0),
    _numpts(0),
    _mbuf(NULL),
    _moff(0),
    _ebuf(NULL),
    _eoff(0)
{}

Model::~Model() {}

Model& Model::operator=(const Model &other) {
    _mod = std::unique_ptr<float>(other.copymod());
    _elem = std::unique_ptr<unsigned>(other.copyelem());
    _modsize = other._modsize;
    _elemsize = other._elemsize;
    _numpts = other._numpts;
    _mbuf = other._mbuf;
    _moff = other._moff;
    _ebuf = other._ebuf;
    _eoff = other._eoff;
    return *this;
}

float* Model::copymod() const {
    float *copy = new float[_modsize];
    return (float*)memcpy((void*)copy, (void*)_mod.get(), _modsize * sizeof(float));
}
unsigned* Model::copyelem() const {
    unsigned *copy = new unsigned[_elemsize];
    return (unsigned*)memcpy((void*)copy, (void*)_elem.get(), _elemsize * sizeof(unsigned));
}
unsigned Model::modsize() const { return _modsize; }
unsigned Model::elemsize() const { return _elemsize; }
unsigned Model::numpts() const { return _numpts; }

//set data ID of vec4's
float* Model::setModelID(float *data, unsigned size, unsigned new_id) {
    if (size % 4 != 0) {
        //throw exception
    }
    for (unsigned i = 0; i < size / 4; i++)
        data[(i * 4) + 3] = new_id;
    return data;
}

//set data ID of elements (unsigned ints)
unsigned* Model::setElementsID(unsigned *data, unsigned size, unsigned num_points, unsigned new_id, unsigned old_id) {
    int shift = (new_id * num_points) - (old_id * num_points);
    for (unsigned i = 0; i < size; i++)
        data[i] = data[i] + shift;
    return data;
}

unsigned Model::moff() const { return _moff; }
unsigned Model::eoff() const { return _eoff; }

void Model::updatemod() {
    if (_mbuf == NULL)
        return;
    _mbuf->subdata(_modsize * sizeof(float), _mod.get(), _moff);
}

void Model::updateelem() {
    if (_ebuf == NULL)
        return;
    _ebuf->subdata(_elemsize * sizeof(float), _elem.get(), _eoff);
}

// _______________________________________ Matrix _______________________________________

Matrix::Matrix(glm::vec3 matpos, glm::vec3 matscale) : pos(matpos), scale(matscale) {}

Matrix::Matrix() { 
    pos = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
};

float* Matrix::data() {
    _mat = glm::scale(
        glm::translate(
            glm::mat4(1.0f),
            pos
        ),
        scale
    );
    return glm::value_ptr(_mat);
}

// _______________________________________ Transform _______________________________________

Transform::Transform(GLBuffer *tbuf, unsigned offset) : _tbuf(tbuf), _toff(offset) {}
Transform::Transform() : _tbuf(NULL), _toff(0) {}

unsigned Transform::toff() const {
    return _toff;
}

//update transform buffer's data
void Transform::updatet() {
    if (_tbuf == NULL)
        return;
    _tbuf->subdata(16 * sizeof(float), mat.data(), _toff);
}

// _______________________________________ TexFrame _______________________________________

//representation of quad of texture, used to transform TexCoords
TexFrame::TexFrame(int xoff, int yoff, int zoff, int width, int height) :
    _xo(xoff), _yo(yoff), _zo(zoff), _w(width), _h(height)
{}
TexFrame::TexFrame() :
    _xo(0), _yo(0), _zo(0), _w(0), _h(0)
{}
TexFrame& TexFrame::set(int xoff, int yoff, int zoff, int width, int height) {
    _xo = xoff;
    _yo = yoff;
    _zo = zoff;
    _w = width;
    _h = height;
    return *this;
}
void TexFrame::zero() {
    _xo = 0;
    _yo = 0;
    _zo = 0;
    _w = 0;
    _h = 0;
}
int TexFrame::xoff() { return _xo; }
int TexFrame::yoff() { return _yo; }
int TexFrame::zoff() { return _zo; }
int TexFrame::width() { return _w; }
int TexFrame::height() { return _h; }
//TODO: remove hardcoding; change frame data to perform mapping based on number of points of model
float* TexFrame::data() {
    float *data = new float[16];
    data[0] = _xo;       data[1] = _yo;       data[2] = _zo;  data[3] = 0;
    data[4] = _xo + _w;  data[5] = _yo;       data[6] = _zo;  data[7] = 0;
    data[8] = _xo;       data[9] = _yo + _h;  data[10] = _zo; data[11] = 0;
    data[12] = _xo + _w; data[13] = _yo + _h; data[14] = _zo; data[15] = 0;
    return data;
}
bool TexFrame::is_zero() {
    if (_xo == 0 && _yo == 0 && _w == 0 && _h == 0)
        return true;
    return false;
}

// _______________________________________ TexCoords _______________________________________

//class to wrap texture UV data with a GLBuffer
TexCoords::TexCoords(
    unsigned num_coords, 
    GLBuffer *tcbuf, unsigned tcoffset
) : 
    _numcoords(num_coords),
    _tcbuf(tcbuf),
    _tcoff(tcoffset)
{
    _tcsize = num_coords * 4;
    float *tmp = new float[_tcsize];
    for (unsigned i = 0; i < num_coords * 4; i++)
        tmp[i] = -1.0f;
    _tc = std::unique_ptr<float>(tmp);
}

TexCoords::TexCoords(const TexCoords &other) :
    _tc(other.copytc()),
    _tcsize(other._tcsize),
    _numcoords(other._numcoords),
    frame(other.frame),
    _tcbuf(other._tcbuf),
    _tcoff(other._tcoff)
{}

TexCoords::TexCoords() :
    _tcsize(0),
    _numcoords(0),
    _tcbuf(NULL),
    _tcoff(0)
{}

TexCoords::~TexCoords() {}

TexCoords& TexCoords::operator=(const TexCoords &other) {
    _tc = std::unique_ptr<float>(other.copytc());
    _tcsize = other._tcsize;
    _numcoords = other._numcoords;
    frame = other.frame;
    _tcbuf = other._tcbuf;
    _tcoff = other._tcoff;
    return *this;
}

float* TexCoords::copytc() const {
    float *copy = new float[_tcsize];
    return (float*)memcpy((void*)copy, (void*)_tc.get(), _tcsize * sizeof(float));
}
unsigned TexCoords::tcsize() const { return _tcsize; }

unsigned TexCoords::tcoff() const { return _tcoff; }

void TexCoords::updatetc() {
    if (_tcbuf == NULL)
        return;
    
    float *data = frame.data();
    _tcbuf->subdata(_tcsize * sizeof(float), data, _tcoff);
    delete data;
}