/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2017 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  SDL_shaders_gles2.h   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/06 
*  https://github.com/zhenfei2016/FFL-v2.git
*  基于sdl 修改的opengles
*
*/

#ifndef SDL_shaders_gles2_h_
#define SDL_shaders_gles2_h_

#if defined(ANDROID)
#include <GLES2/gl2platform.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif defined(APPLE)

#endif

typedef struct GLES2_ShaderInstance
{
    GLenum type;
    GLenum format;
    int length;
    const void *data;
} GLES2_ShaderInstance;

typedef struct GLES2_Shader
{
    int instance_count;
    const GLES2_ShaderInstance *instances[4];
} GLES2_Shader;

typedef enum
{
    GLES2_SHADER_NONE,
    GLES2_SHADER_VERTEX_DEFAULT,
    GLES2_SHADER_FRAGMENT_SOLID_SRC,
    GLES2_SHADER_FRAGMENT_TEXTURE_ABGR_SRC,
    GLES2_SHADER_FRAGMENT_TEXTURE_ARGB_SRC,
    GLES2_SHADER_FRAGMENT_TEXTURE_BGR_SRC,
    GLES2_SHADER_FRAGMENT_TEXTURE_RGB_SRC,
    GLES2_SHADER_FRAGMENT_TEXTURE_YUV_SRC,
    GLES2_SHADER_FRAGMENT_TEXTURE_NV12_SRC,
    GLES2_SHADER_FRAGMENT_TEXTURE_NV21_SRC
} GLES2_ShaderType;

#define GLES2_SOURCE_SHADER (GLenum)-1

const GLES2_Shader *GLES2_GetShader(GLES2_ShaderType type);

#endif 
