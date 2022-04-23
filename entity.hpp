#pragma once

#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include "behavior.hpp"

//unit of object in scene
/*
NOTE: on copy construction/assignment, the constructed/assigned Entity will have a
reference to the original Entity's Behavior ptr
*/

class Core;

class Entity {
    friend class Core;

    bool _initialize;
public:
    Model m;
    Transform t;
    TexCoords tc;
    std::shared_ptr<Behavior> bptr;

    Entity(Model model, Transform transform, TexCoords texcoords, std::shared_ptr<Behavior> behavior);
    Entity(const Entity &other);
    Entity();
    Entity& operator=(const Entity &other);

    void update();
};

#endif