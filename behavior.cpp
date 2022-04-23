#include "behavior.hpp"
#include "core.hpp"

Behavior::Behavior(unsigned type) : _killed(false), _type(type) {}

void Behavior::setMatrix(Matrix *matrix) {
    _t = matrix;
}
void Behavior::setTexFrame(TexFrame *textureframe) {
    _f = textureframe;
}
void Behavior::setCore(Core *core) {
    _c = core;
}

void Behavior::_init() {}
void Behavior::_base() {}
void Behavior::_kill() {}

void Behavior::init() {
    _init();
}
void Behavior::base() {
    _base();
}

void Behavior::kill() {
    if (!_killed) {
        _c->erase(_id);
        _kill();
        _killed = true;
    }
}

void Behavior::setpos(glm::vec3 pos) {
    _t->pos = pos;
}

int Behavior::id() { return _id; }
unsigned Behavior::type() { return _type; }