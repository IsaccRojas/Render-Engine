#pragma once

#ifndef SPRITE_HPP_
#define SPRITE_HPP_

#include "glutil.hpp"
#include <vector>

using namespace glm;

enum AnimType { SPR_STATIC, SPR_LINEAR, SPR_LOOP };

class Sprite {
protected:
    struct AnimState {
		std::vector<ivec2> framepositions;
		AnimType type;
		int frametime;
	};
	
	int _currentanimstate;
	int _animcounter;
	int _tex;
	ivec2 _spritedim;
	std::vector<AnimState> _animstates;

	void _addanimstate(std::initializer_list<ivec2> framepositions, int frametime, AnimType type);

	void _setanimstate(int animstate);
	void _inc();
	void _setframe(TexFrame *f);

    void _anim(TexFrame *f);
public:
    Sprite();
    ~Sprite();

    void sprite_init(int texture, ivec2 dimensions);
};

#endif