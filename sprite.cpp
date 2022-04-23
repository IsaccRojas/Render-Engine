#include "sprite.hpp"

Sprite::Sprite() : _currentanimstate(0), _animcounter(0), _tex(0) {}
Sprite::~Sprite() {}

void Sprite::_addanimstate(std::initializer_list<ivec2> framepositions, int frametime, AnimType type) {
    //add new animation state
    _animstates.push_back({
        framepositions,
        type,
        frametime
    });
}

void Sprite::_setanimstate(int animstate) {
    //set new state and reset counter
    _currentanimstate = animstate;
    _animcounter = 0;
}

void Sprite::_inc() {
    if (_animstates.size() > 0) {
        AnimState &animstate = _animstates[_currentanimstate];
        //increment counter if animation type is not static
        if (animstate.type != SPR_STATIC) {
            _animcounter++;
        
            //wrap counter to 0 if animation type is loop
            if ((animstate.type == SPR_LOOP) && (_animcounter >= animstate.frametime * int(animstate.framepositions.size())))
                _animcounter = 0;
        }
    }
}

void Sprite::_setframe(TexFrame *f) {
    if (_animstates.size() > 0) {
        //set texframe based on current animation state and counter value
        AnimState &animstate = _animstates[_currentanimstate];
        ivec2 &pos = animstate.framepositions[_animcounter / animstate.frametime];
        f->set(pos.x, pos.y, _tex, _spritedim.x - 1, _spritedim.y - 1);
    }
}

void Sprite::_anim(TexFrame *f) {
	_setframe(f);
	_inc();
}

void Sprite::sprite_init(int texture, ivec2 dimensions) {
    _tex = texture;
    _spritedim = dimensions;
}