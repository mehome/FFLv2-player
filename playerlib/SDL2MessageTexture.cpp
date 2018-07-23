/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  SDL2MessageTexture.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/16
*
*  sdl2的texture
*
*/

#include "SDL2MessageTexture.hpp"


namespace message {
	SDL2Texture::SDL2Texture(){
		mTexture = NULL;
        setType(MSG_SDL2_TEXTURE);
	}
	SDL2Texture::~SDL2Texture(){}

	SDL2Samples::SDL2Samples() {
		setType(MSG_SDL2_SAMPLES);
	}
	SDL2Samples::~SDL2Samples() {}
	
}
