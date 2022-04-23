#pragma once

#ifndef CORE_HPP_
#define CORE_HPP_

#include "entity.hpp"
#include "util.hpp"
#include <unordered_map>
#include <boost/function.hpp>
#include <boost/bind/bind.hpp>

class Core {
    //stored type
    struct EntityType {
        Model model;
        boost::function<Behavior*(void)> allocator;
        std::string name;
    };

    unsigned _capacity;
public:
    GLBuffer _model_buf;
    GLBuffer _element_buf;
    GLBuffer _transform_buf;
    GLBuffer _texcoords_buf;
    GLBuffer _draw_buf;

    GLBuffer _vertex_buf;

    GLStage _compute_stage;
    GLStage _render_stage;

    glm::mat4 _view;
    glm::mat4 _proj;
    std::unordered_map<std::string, int> _istorage;
    std::unordered_map<std::string, float> _fstorage;
    std::unordered_map<std::string, glm::vec2> _2fvstorage;

    Partitioner _ids;

    Entity **_ents;
    std::unordered_map<std::string, EntityType> _entitytypes;
    std::vector<int> _execqueue;
    std::vector<int> _erasequeue;
    //highest id pushed (NOT current maximum id)
    int _maxid;

    GLTexture2DArray _texarray;

    bool _clear;
    void _erase(int id);
public:
    Core();
    ~Core();
    Core(const Core &other) = delete;
    Core operator=(const Core &other) = delete;

    void setviewport(GLint x, GLint y, GLint width, GLint height);
    void setview(glm::mat4 view);
    void setproj(glm::mat4 proj);

    //write into storage maps
    void writei(std::string name, int value);
    void writef(std::string name, float value);
    void write2fv(std::string name, glm::vec2 value);
    //read storage maps
    int readi(std::string name);
    float readf(std::string name);
    glm::vec2 read2fv(std::string name);

    void add(Model model, boost::function<Behavior*()> allocator, const char *name);
    //push entity and put into execution queue
    int push(const Model model, Behavior *behavior);
    int push(std::string name);
    //put entity into erase queue
    void erase(int id);

    //get pointer to entity
    Entity* const get(int id);
    //get pointer to behavior
    Behavior* const getbhv(int id);
    //get pointer to transform mat
    Matrix* const gettransf(int id);

    void settexarray(GLuint width, GLuint height, GLuint depth);
    void settexture(Image img, GLuint xoffset, GLuint yoffset, GLuint zoffset);

    //queue all entities for execution
    void queueall();
    //execute all queued entity behaviors
    void exec();
    //erase all entities queued for erasure
    void exec_erasequeue();
    void draw();

    void setclear(bool clear);
    void clearcolor(float r, float g, float b, float a);

    int maxid();

    static void init();
};

#endif