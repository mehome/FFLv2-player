/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  MessageFFMpegPacket.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/16
*
*  sdl2的texture
*
*/

#pragma once
#include "MessageBase.hpp"
#include "SDL.h"

namespace message {
	//
	//   纹理数据
	//
	class SDL2Texture : public MessageBase
	{
	public:
		SDL2Texture();
		~SDL2Texture();
	public:	
		SDL_Texture* mTexture;
		//
		//私有的数据 
		//
		void* mOpaque;
			
	};

	class SDL2Samples : public MessageBase
	{
	public:
		SDL2Samples();
		~SDL2Samples();
	public:
		
		//
		//私有的数据 
		//
		void* mOpaque;

	};

}
