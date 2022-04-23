#pragma once

#ifndef PHYSICS_HPP_
#define PHYSICS_HPP_

#include "glutil.hpp"

using namespace glm;

class Physics {
protected:
    vec2 _dim;
    vec2 _pos;
    vec2 _vel;
    float _spd_max;
    float _accel;
    float _deccel;
    vec2 _dir;

    virtual void _collision(int collider_id);

    void _move(vec2 dir);
public:
    Physics();
    ~Physics();

    void physics_init(vec2 dimensions, vec2 position, vec2 velocity, float spd_max, float acceleration, float decceleration, vec2 direction);
    void collision(int collider_id);

    static bool detect_collision(Physics *a, Physics *b);
};

#endif