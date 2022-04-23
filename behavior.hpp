#pragma once

#ifndef BEHAVIOR_HPP_
#define BEHAVIOR_HPP_

#include "glutil.hpp"

#include <memory>
#include <iostream>

using namespace glm;

class Core;

//entity type data
class Behavior {
    bool _killed;
protected:
    friend class Core;
    int _id;
    unsigned _type;
    Matrix *_t;
    TexFrame *_f;
    Core *_c;

    virtual void _init();
    virtual void _base();
    virtual void _kill();
public:
    Behavior(unsigned type = 0);
    virtual ~Behavior() = default;

    void setMatrix(Matrix *matrix);
    void setTexFrame(TexFrame *textureframe);
    void setCore(Core *core);
    
    void init();
    void base();
    void kill();

    void setpos(glm::vec3 pos);

    int id();
    unsigned type();
};

#endif