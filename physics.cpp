#include "physics.hpp"

Physics::Physics() : _spd_max(0.0f), _accel(0.0f), _deccel(0.0f) {}
Physics::~Physics() {}

void Physics::_collision(int collider_id) {}

void Physics::_move(glm::vec2 dir) {
    glm::vec2 vel_i = _vel;
    float spd_i = glm::length(_vel);
    float dec_factor;

    //get decceleration based on current speed and apply
    if (spd_i != 0.0f)
        dec_factor = _deccel / glm::length(_vel);
    else
        dec_factor = 0.0f;
    _vel -= _vel * dec_factor;

    //determine if deccelerated completely ("passed" 0)
    if ((vel_i.x > 0 && _vel.x < 0) || (vel_i.x < 0 && _vel.x > 0))
        _vel.x = 0.0f;
    if ((vel_i.y > 0 && _vel.y < 0) || (vel_i.y < 0 && _vel.y > 0))
        _vel.y = 0.0f;

    //accelerate based on input
    _vel += dir * _accel;

    //reduce velocity to max if speed exceeds max
    if (glm::length(_vel) > _spd_max)
        _vel = glm::normalize(_vel) * _spd_max;

    //apply
    _pos = _pos += _vel;
}

void Physics::physics_init(vec2 dimensions, vec2 position, vec2 velocity, float spd_max, float acceleration, float decceleration, vec2 direction) {
    _dim = dimensions;
    _pos = position;
    _vel = velocity;
    _spd_max = spd_max;
    _accel = acceleration;
    _deccel = decceleration;
    _dir = direction;
}
void Physics::collision(int collider_id) {
    _collision(collider_id);
}

bool Physics::detect_collision(Physics *a, Physics *b) {
    return 
        (glm::abs(a->_pos.x - b->_pos.x) * 2 < (a->_dim.x + b->_dim.x)) 
        && (glm::abs(a->_pos.y - b->_pos.y) * 2 < (a->_dim.y + b->_dim.y))
    ;
}