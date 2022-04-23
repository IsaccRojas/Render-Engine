#include "core.hpp"

//gl error handler
void GLAPIENTRY gl_err_handler(
    GLenum src,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei len,
    const GLchar* msg,
    const void* param
) 
{
    std::cout << "ERR: gl_err_handler(): " << msg << std::endl;
    exit(0);
}

Core::Core() :
    _capacity(4096),

    _model_buf(GL_STATIC_DRAW, 64 * _capacity),
    _element_buf(GL_STATIC_DRAW, 24 * _capacity),
    _transform_buf(GL_DYNAMIC_DRAW, 64 * _capacity),
    _texcoords_buf(GL_DYNAMIC_DRAW, 64 * _capacity),
    _draw_buf(GL_STATIC_DRAW, 4 * _capacity),

    _vertex_buf(GL_DYNAMIC_DRAW, 128 * _capacity),

    _compute_stage(GLP_COMPUTE),
    _render_stage(GLP_RENDER)
{
    //get vertex shader and fragment shader source as string
    char *vert_shader_src = new char[2048];
    char *frag_shader_src = new char[2048];
    char *comp_shader_src = new char[2048];
    get_file_str("vert_shader.glsl", vert_shader_src, 2048);
    get_file_str("frag_shader.glsl", frag_shader_src, 2048);
    get_file_str("comp_shader.glsl", comp_shader_src, 2048);

    //create compute stage
    char *compute_src[] = {comp_shader_src};
    GLenum compute_type[] = {GL_COMPUTE_SHADER};
    _compute_stage.program(compute_src, compute_type, 1);
    _compute_stage.storage(_model_buf.handle(), 0);
    _compute_stage.storage(_transform_buf.handle(), 1);
    _compute_stage.storage(_texcoords_buf.handle(), 2);
    _compute_stage.storage(_draw_buf.handle(), 3);
    _compute_stage.storage(_vertex_buf.handle(), 4);

    //create shader stage
    char *shader_srcs[] = {vert_shader_src, frag_shader_src};
    GLenum shader_types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
    GLStage renderstage(GLP_RENDER);
    _render_stage.program(shader_srcs, shader_types, 2);
    _render_stage.attrib(_vertex_buf.handle(), 1, 4, GL_FLOAT, sizeof(float) * 8, (void*)(sizeof(float) * 0));
    _render_stage.attrib(_vertex_buf.handle(), 2, 3, GL_FLOAT, sizeof(float) * 8, (void*)(sizeof(float) * 4));
    _render_stage.element(_element_buf.handle());
    

    delete[] vert_shader_src;
    delete[] frag_shader_src;
    delete[] comp_shader_src;

    setview(glm::mat4(1.0f));
    setproj(glm::mat4(1.0f));

    _ents = new Entity*[_capacity];
    
    _clear = true;
    _maxid = -1;
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    glEnable(GL_DEPTH_TEST);
}

Core::~Core() {
    delete[] _ents;
}

void Core::setviewport(GLint x, GLint y, GLint width, GLint height) {
    glViewport(x, y, width, height);
}

void Core::setview(glm::mat4 view) {
    _view = view;
    glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(_view));
}

void Core::setproj(glm::mat4 proj) {
    _proj = proj;
    glUniformMatrix4fv(5, 1, GL_FALSE, glm::value_ptr(_proj));
}

//write into storage maps
void Core::writei(std::string name, int value) { _istorage[name] = value; }
void Core::writef(std::string name, float value) { _fstorage[name] = value; }
void Core::write2fv(std::string name, glm::vec2 value) { _2fvstorage[name] = value; }
//read storage maps
int Core::readi(std::string name) { return _istorage[name]; }
float Core::readf(std::string name) { return _fstorage[name]; }
glm::vec2 Core::read2fv(std::string name) { return _2fvstorage[name]; }

//associate model and allocator with name
void Core::add(Model model, boost::function<Behavior*()> allocator, const char *name) {
    _entitytypes[name] = EntityType{model, allocator, name};
}

//push new ID, and allocate new entity at ID position
int Core::push(const Model model, Behavior *behavior) {
    //push new ID and set draw flag
    int id = _ids.push();
    float draw = 1.0f;
    _draw_buf.subdata(sizeof(float), &draw, id * sizeof(float));
    if (id > _maxid)
        _maxid = id;

    //allocate new entity and assign its components' buffer offsets based on ID
    _ents[id] = new Entity(
        Model(
            model, id,
            &_model_buf, id * (model.modsize() * sizeof(float)),
            &_element_buf, id * (model.elemsize() * sizeof(unsigned))
        ), 
        Transform(
            &_transform_buf, id * (16 * sizeof(float))
        ), 
        TexCoords(
            model.numpts(),
            &_texcoords_buf, id * (16 * sizeof(float))
        ),
        std::shared_ptr<Behavior>(behavior)
    );
    //give a reference to this core and node to behavior
    _ents[id]->bptr->setCore(this);
    _ents[id]->bptr->_id = id;
    _ents[id]->update();

    //put into execution queue
    _execqueue.push_back(id);

    return id;
}

//push new ID, and allocate new named entity type at ID position
int Core::push(std::string name) {
    return push(_entitytypes[name].model, _entitytypes[name].allocator());
}

//erase ID, and deallocate entity at ID position
void Core::_erase(int id) {
    //erase ID and unset draw flag
    _ids.erase_at(id);
    float draw = 0.0f;
    _draw_buf.subdata(sizeof(float), &draw, id * sizeof(float));

    //deallocate entity
    delete _ents[id];
}

//put entity into erase queue
void Core::erase(int id) {
    _erasequeue.push_back(id);
}

//get pointer to entity
Entity* const Core::get(int id) {
    if (_ids.at(id))
        return _ents[id];
    else
        return nullptr;
}
//get pointer to behavior
Behavior* const Core::getbhv(int id) {
    if (_ids.at(id))
        return _ents[id]->bptr.get();
    else
        return nullptr;
}
//get pointer to transform mat
Matrix* const Core::gettransf(int id) {
    if (_ids.at(id))
        return &(_ents[id]->t.mat);
    else
        return nullptr;
}

void Core::settexarray(GLuint width, GLuint height, GLuint depth) {
    _texarray.alloc(1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, width, height, depth);
}

void Core::settexture(Image img, GLuint xoffset, GLuint yoffset, GLuint zoffset) {
    _texarray.subimage(0, xoffset, yoffset, zoffset, img.width(), img.height(), img.copydata());
}

void Core::queueall() {
    //queue all entities for execution
    _execqueue.clear();
    for (unsigned i = 0; i < _ids.size(); i++)
        if (_ids.at(i))
            _execqueue.push_back(i);
}

void Core::exec() {
    unsigned id;
    for (unsigned i = 0; i < _execqueue.size(); i++) {
        //check if ID is active
        id = _execqueue[i];
        if (_ids.at(id)) {
            //check if entity needs to be initialized
            if (_ents[id]->_initialize) {
                _ents[id]->bptr->init();
                _ents[id]->_initialize = false;
            }

            _ents[id]->bptr->base();
            _ents[id]->t.updatet();
            _ents[id]->tc.updatetc();
        }

        //erase any queued entities
        exec_erasequeue();
    }

    _execqueue.clear();
}

void Core::exec_erasequeue() {
    for (unsigned i = 0; i < _erasequeue.size(); i++)
        _erase(_erasequeue[i]);
    
    _erasequeue.clear();
}

void Core::draw() {
    if (_clear)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _compute_stage.compute((_maxid + 1) * 4, 1, 1);
    _render_stage.render(_ids.size() * 6);
}

void Core::setclear(bool clear) {
    _clear = clear;
}
void Core::clearcolor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

int Core::maxid() { return _maxid; }

void Core::init() {
    //initialize glew
    glewInit();

    //enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_err_handler, NULL);
}