#include "entity.hpp"

Entity::Entity(Model model, Transform transform, TexCoords texcoords, std::shared_ptr<Behavior> behavior) : 
    _initialize(true), 
    m(model), 
    t(transform), 
    tc(texcoords), 
    bptr(behavior) 
{
    //set bptr with new matrix and texframe BUT same node and same core
    bptr->setMatrix(&(t.mat));
    bptr->setTexFrame(&(tc.frame));
}

Entity::Entity(const Entity &other) : 
    _initialize(true),
    m(other.m), 
    t(other.t), 
    tc(other.tc), 
    bptr(other.bptr)
{
    //set bptr with new matrix and texframe BUT same node and same core
    bptr->setMatrix(&(t.mat));
    bptr->setTexFrame(&(tc.frame));
}

Entity::Entity() {}

Entity& Entity::operator=(const Entity &other) {
    m = other.m;
    t = other.t;
    tc = other.tc;
    bptr = other.bptr;
    //set bptr with new matrix and texframe BUT same node and same core
    bptr->setMatrix(&t.mat);
    bptr->setTexFrame(&tc.frame);
    return *this;
}


void Entity::update() {
    m.updatemod();
    m.updateelem();
    tc.updatetc();
    t.updatet();
}