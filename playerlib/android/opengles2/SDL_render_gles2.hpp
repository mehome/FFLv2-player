/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  SDL_render_gles2.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/06 
*  https://github.com/zhenfei2016/FFL-v2.git
*  移植修改的SDL2中的opengles相关部分
*
*/
#ifndef _SDL_RENDER_GLES_HPP_
#define _SDL_RENDER_GLES_HPP_


#include <GLES2/gl2.h>

extern "C" {
#include "SDL_shaders_gles2.h"
};

typedef struct SDL_Point
{
	int x;
	int y;
} SDL_Point;

typedef struct SDL_Rect
{
	int x, y;
	int w, h;
} SDL_Rect;

typedef struct
{
	float x;
	float y;
	float w;
	float h;
} SDL_FRect;

typedef struct
{
	float x;
	float y;
} SDL_FPoint;

typedef enum
{
	SDL_FLIP_NONE = 0x00000000,     /**< Do not flip */
	SDL_FLIP_HORIZONTAL = 0x00000001,    /**< flip horizontally */
	SDL_FLIP_VERTICAL = 0x00000002     /**< flip vertically */
} SDL_RendererFlip;

typedef unsigned char Uint8;

//
//  顶点属性在sharder中的索引位置(句柄值)
//  定点位置，纹理坐标，旋转角度，旋转中心位置
//
typedef enum
{
	GLES2_ATTRIBUTE_POSITION = 0,
	GLES2_ATTRIBUTE_TEXCOORD = 1,
	GLES2_ATTRIBUTE_ANGLE = 2,
	GLES2_ATTRIBUTE_CENTER = 3,
    GLES2_ATTRIBUTE_COUNT
} GLES2_Attribute;

//
//  sharder中的uniform存储于类实例mUniformLocations数组中的索引位置
//
typedef enum
{
	GLES2_UNIFORM_PROJECTION,
	GLES2_UNIFORM_MODULATION,
	GLES2_UNIFORM_COLOR,
	GLES2_UNIFORM_TEXTURE,
	GLES2_UNIFORM_TEXTURE_U,
	GLES2_UNIFORM_TEXTURE_V,
	GLES_UNIFORM_COUNT,
} GLES2_Uniform;

namespace player{
	class VideoTexture;
	class VideoFormat;
}
class GLES2Renderer {
public:
	GLES2Renderer();
	~GLES2Renderer();
	//
	//  安装，使用，卸载 opengl program
	//
	bool install(const  player::VideoFormat* fmt,int32_t viewWidht,int32_t viewHeight);
	void uninstall();
	//
	// 使用当前render
	//
	bool useProgram();
	//
	//  清空缓冲区
	//
	void clearColor(GLfloat r,GLfloat g,GLfloat b,GLfloat a);
	//
	//  更新viewport
	//
	void updateViewport(int widht,int height);
	//
	//  把原图片的子区域更新到目标纹理上
    //  tex ：原图片
    // rect:原图片的子区域
	//
	bool uploadTexture(player::VideoTexture* tex,const SDL_Rect *rect);
	//
	//  开始绘制
	//
	int render(player::VideoTexture* tex,
			   	     const SDL_Rect *srcrect,
					 const SDL_FRect *dstrect,
					 const double angle,
					 const SDL_FPoint *center,
					 const SDL_RendererFlip flip);

private:
    bool isValid() const;
	//
	// 上传投影矩阵
	//
	void uploadOrthographicProjection();
	//
	// 创建顶点着色器
	//
	bool createVertexShader();
	//
	// 创建片段着色器
	//
	bool createFragmentShader(GLES2_ShaderType type);
    //
	// 创建gpuprogram
	//
	bool createProgram();
	//
	// 更新一下投影矩阵,返回是否更新了
	//
	bool updateProjectionMat(int32_t width,int32_t height);
private:
	//
	//  顶点，片段着色
	//
	GLuint mVertexShader;
	GLuint mFragmentShader;
	GLuint mProgram;
    GLuint mVertexAttribLocations[GLES2_ATTRIBUTE_COUNT];
	GLuint mUniformLocations[GLES_UNIFORM_COUNT];
	//
	//  yuv纹理或者 0:指向rgb
	//
	GLuint mTexture[3];
	//
	//  视口，投影矩阵
	//
	SDL_Rect mViewport;
	GLfloat mProjection[4][4];
};

#endif
