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
#include <FFL.h>
#include "SDL_render_gles2.hpp"
extern "C" {
#include "SDL_shaders_gles2.h"
}

#include "VideoTexture.hpp"
#include "SDL_renderTexture.cpp"

void GLES2_checkError(const char* op) {
	for (GLint error = glGetError(); error; error = glGetError()) {
		FFL_LOG_ERROR("[GLES2] after %s() glError (0x%x)\n", op, error);
	}
}

void GLES2_printString(const char *name, GLenum s) {
	const char *v = (const char *)glGetString(s);
	FFL_LOG_INFO("[GLES2] %s = %s\n", name, v);
}

GLES2Renderer::GLES2Renderer(){
	mProgram=0;
	mVertexShader=0;
	mFragmentShader=0;

	mTexture[0]=0;
	mTexture[1]=0;
	mTexture[2]=0;

    for( int32_t i=0;i<GLES2_ATTRIBUTE_COUNT;i++) {
        mVertexAttribLocations[i]=-1;
    }
    for( int32_t i=0;i<GLES_UNIFORM_COUNT;i++) {
        mUniformLocations[i]=-1;
    }
}
GLES2Renderer::~GLES2Renderer(){

}

static GLES2_ShaderType VideoFormat2GLES2_ShaderType(const player::VideoFormat* fmt){
	switch (fmt->mFormat){
		case player::VideoFormat::PIXEL_FORMAT_YUV420P:
			return  GLES2_SHADER_FRAGMENT_TEXTURE_YUV_SRC;
		case player::VideoFormat::PIXEL_FORMAT_RGBA_8888:
			return  GLES2_SHADER_FRAGMENT_TEXTURE_ARGB_SRC;
		case player::VideoFormat::PIXEL_FORMAT_NV12:
			return  GLES2_SHADER_FRAGMENT_TEXTURE_NV12_SRC;
		case player::VideoFormat::PIXEL_FORMAT_NV21:
			return  GLES2_SHADER_FRAGMENT_TEXTURE_NV21_SRC;
	}
    return  GLES2_SHADER_NONE;
}
bool GLES2Renderer::install(const player::VideoFormat* fmt,int32_t viewWidht,int32_t viewHeight){
	if(!fmt){
		return false;
	}

	GLES2_ShaderType type= VideoFormat2GLES2_ShaderType(fmt);
	if(!createFragmentShader(type) ||
			!createVertexShader()){
		uninstall();
		return  false;
	}

	if(!createProgram()){
		uninstall();
		return  false;
	}

	//
	//设置默认的值
	//
	glUseProgram(mProgram);

	glGenTextures(3,mTexture);
	glUniform1i(mUniformLocations[GLES2_UNIFORM_TEXTURE_V], 2);
	glUniform1i(mUniformLocations[GLES2_UNIFORM_TEXTURE_U], 1);
	glUniform1i(mUniformLocations[GLES2_UNIFORM_TEXTURE], 0);

	updateProjectionMat(viewWidht,viewHeight);
	glUniformMatrix4fv(mUniformLocations[GLES2_UNIFORM_PROJECTION], 1, GL_FALSE, (GLfloat *)mProjection);

	glUniform4f(mUniformLocations[GLES2_UNIFORM_MODULATION], 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4f(mUniformLocations[GLES2_UNIFORM_COLOR], 1.0f, 1.0f, 1.0f, 1.0f);

	return true;
}
void GLES2Renderer::uninstall() {
	if(mProgram) {
		glDeleteProgram(mProgram);
		mProgram=0;
	}

	if(mVertexShader) {
		glDeleteShader(mVertexShader);
		mVertexShader=0;
	}

	if(mFragmentShader) {
		glDeleteShader(mFragmentShader);
		mFragmentShader=0;
	}

	if(mTexture[0]) {
		glDeleteTextures(3, mTexture);
		mTexture[0]=0;
	}
}
//
// 使用当前render
//
bool GLES2Renderer::useProgram(){
	FFL_LOG_INFO("useProgram\n");

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glUseProgram(mProgram);

	for(int32_t i=0;i<3;i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, mTexture[i]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	return true;
}
//
//  清空缓冲区
//
void GLES2Renderer::clearColor(GLfloat r,GLfloat g,GLfloat b,GLfloat a){
	glClearColor(r,g,b,a);
	glDisable(GL_SCISSOR_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	//glEnable(GL_SCISSOR_TEST);
}
//
//  更新viewport
//
void GLES2Renderer::updateViewport(int widht,int height) {
	if(mViewport.w!=widht ||mViewport.h!=height) {
		mViewport.w=widht;
		mViewport.h=height;
		glViewport(0, 0, widht, height);
	}

	if(updateProjectionMat(widht,height)){
		uploadOrthographicProjection();
	}
}

bool GLES2Renderer::uploadTexture(player::VideoTexture* tex, const SDL_Rect *rect){
    if(!isValid()){
        FFL_LOG_WARNING("Faild to GLES2Renderer::uploadTexture programe not create");
        return false;
    }

	switch (tex->getVideoFormat()->mFormat){
		case player::VideoFormat::PIXEL_FORMAT_YUV420P:
			return  uploadTexture_yuv(tex,rect,mTexture);
            //return  yuv420p_uploadTexture(tex,mTexture);
		case player::VideoFormat::PIXEL_FORMAT_RGBA_8888:
			return  uploadTexture_rgba(tex,rect,mTexture);
		case player::VideoFormat::PIXEL_FORMAT_NV12:

		case player::VideoFormat::PIXEL_FORMAT_NV21:
			break;
	}
	return false;
}
//
//  更新顶点相关信息
//
static void updateVertexBuffer(GLES2_Attribute attr,
                               GLuint index,
						const void *vertexData,
						size_t dataSizeInBytes){
	glVertexAttribPointer(index, attr == GLES2_ATTRIBUTE_ANGLE ? 1 : 2, GL_FLOAT, GL_FALSE, 0, vertexData);
}
//
//  开始绘制
//
int GLES2Renderer::render(player::VideoTexture* tex,
		   const SDL_Rect *srcrect,
		   const SDL_FRect *dstrect,
		   const double angle,
		   const SDL_FPoint *center,
		   const SDL_RendererFlip flip){
	GLfloat vertices[8];
	GLfloat texCoords[8];
	GLfloat translate[8];
	GLfloat fAngle[4];
	GLfloat tmp;

    if(!isValid()){
        FFL_LOG_WARNING("Faild to GLES2Renderer::render programe not create");
        return -1;
    }
	glEnableVertexAttribArray(mVertexAttribLocations[GLES2_ATTRIBUTE_CENTER]);
	glEnableVertexAttribArray(mVertexAttribLocations[GLES2_ATTRIBUTE_ANGLE]);
	fAngle[0] = fAngle[1] = fAngle[2] = fAngle[3] = (GLfloat)(360.0f - angle);
	/* Calculate the center of rotation */
	translate[0] = translate[2] = translate[4] = translate[6] = (center->x + dstrect->x);
	translate[1] = translate[3] = translate[5] = translate[7] = (center->y + dstrect->y);

	/* Emit the textured quad */
	vertices[0] = dstrect->x;
	vertices[1] = dstrect->y;
	vertices[2] = (dstrect->x + dstrect->w);
	vertices[3] = dstrect->y;
	vertices[4] = dstrect->x;
	vertices[5] = (dstrect->y + dstrect->h);
	vertices[6] = (dstrect->x + dstrect->w);
	vertices[7] = (dstrect->y + dstrect->h);
	if (flip & SDL_FLIP_HORIZONTAL) {
		tmp = vertices[0];
		vertices[0] = vertices[4] = vertices[2];
		vertices[2] = vertices[6] = tmp;
	}
	if (flip & SDL_FLIP_VERTICAL) {
		tmp = vertices[1];
		vertices[1] = vertices[3] = vertices[5];
		vertices[5] = vertices[7] = tmp;
	}

	updateVertexBuffer(GLES2_ATTRIBUTE_ANGLE,
                       mVertexAttribLocations[GLES2_ATTRIBUTE_ANGLE], fAngle, 4 * sizeof(GLfloat));
	updateVertexBuffer(GLES2_ATTRIBUTE_CENTER,
                       mVertexAttribLocations[GLES2_ATTRIBUTE_CENTER], translate, 8 * sizeof(GLfloat));

    glEnableVertexAttribArray(mVertexAttribLocations[GLES2_ATTRIBUTE_POSITION]);
	updateVertexBuffer(GLES2_ATTRIBUTE_POSITION,
                       mVertexAttribLocations[GLES2_ATTRIBUTE_POSITION],vertices, 8 * sizeof(GLfloat));

	texCoords[0] = srcrect->x / (GLfloat)tex->mWidth;
	texCoords[1] = srcrect->y / (GLfloat)tex->mHeight;
	texCoords[2] = (srcrect->x + srcrect->w) / (GLfloat)tex->mWidth;
	texCoords[3] = srcrect->y / (GLfloat)tex->mHeight;
	texCoords[4] = srcrect->x / (GLfloat)tex->mWidth;
	texCoords[5] = (srcrect->y + srcrect->h) / (GLfloat)tex->mHeight;
	texCoords[6] = (srcrect->x + srcrect->w) / (GLfloat)tex->mWidth;
	texCoords[7] = (srcrect->y + srcrect->h) / (GLfloat)tex->mHeight;
	/*glVertexAttribPointer(GLES2_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, texCoords);*/
    glEnableVertexAttribArray(mVertexAttribLocations[GLES2_ATTRIBUTE_TEXCOORD]);
	updateVertexBuffer(GLES2_ATTRIBUTE_TEXCOORD,
                       mVertexAttribLocations[GLES2_ATTRIBUTE_TEXCOORD], texCoords, 8 * sizeof(GLfloat));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableVertexAttribArray(mVertexAttribLocations[GLES2_ATTRIBUTE_CENTER]);
	glDisableVertexAttribArray(mVertexAttribLocations[GLES2_ATTRIBUTE_ANGLE]);
	return 1;
}
bool GLES2Renderer::isValid() const{
    return mProgram!=0;

}
void GLES2Renderer::uploadOrthographicProjection(){
	const GLuint locProjection = mUniformLocations[GLES2_UNIFORM_PROJECTION];
	glUniformMatrix4fv(locProjection, 1, GL_FALSE, (GLfloat *)mProjection);
}
//
// 创建顶点着色器
//
bool GLES2Renderer::createVertexShader(){
	const GLES2_Shader *shader = GLES2_GetShader(GLES2_SHADER_VERTEX_DEFAULT);
	const GLES2_ShaderInstance *instance = shader->instances[0];
	GLuint shaderId = glCreateShader(instance->type);
	if(shaderId<=0){
		FFL_LOG_WARNING("Failed to GLES2Renderer::createVertexShader");
		return false;
	}

	GLES2_checkError("glCreateShader");
	GLint compileSuccessful=GL_FALSE;
	if (instance->format == (GLenum) -1) {
		glShaderSource(shaderId, 1, (const char **) (char *) &instance->data, NULL);
		glCompileShader(shaderId);
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileSuccessful);
	} else {
		glShaderBinary(1, &shaderId, instance->format, instance->data, instance->length);
		compileSuccessful = GL_TRUE;
	}

	mVertexShader=shaderId;

	if(!compileSuccessful ){
		GLES2_checkError("glCreateShader");
		return  false;
	}
	return true;
}
bool GLES2Renderer::createFragmentShader(GLES2_ShaderType type){
	if(type==GLES2_SHADER_NONE){
		FFL_LOG_ERROR("Failed to createFragmentShader type=%d",type);
		return false;
	}

	const GLES2_Shader *shader = GLES2_GetShader(type);
	GLint compileSuccessful=GL_FALSE;
	const GLES2_ShaderInstance *instance = shader->instances[0];
	GLuint shaderId = glCreateShader(instance->type);
	if(shaderId<=0){
		FFL_LOG_ERROR("Failed to glCreateShader shaderId=%d",shaderId);
		return false;
	}
	GLES2_checkError("glCreateShader");
	if (instance->format == (GLenum) -1) {
		glShaderSource(shaderId, 1, (const char **) (char *) &instance->data, NULL);
		glCompileShader(shaderId);
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileSuccessful);
	} else {
		glShaderBinary(1, &shaderId, instance->format, instance->data, instance->length);
		compileSuccessful = GL_TRUE;
	}
	mFragmentShader=shaderId;
	if(!compileSuccessful ){
		return  false;
	}
	return true;
}
//
// 创建gpuprogram
//
bool GLES2Renderer::createProgram(){
	if(mVertexShader<=0 || mFragmentShader<=0){
		return false;
	}

	mProgram = glCreateProgram();
	GLES2_checkError("glCreateProgram");
	if (mProgram<=0) {
		return false;
	}

	glAttachShader(mProgram, mVertexShader);
	GLES2_checkError("glAttachShader(vertex)");
	glAttachShader(mProgram, mFragmentShader);
	GLES2_checkError("glAttachShader(fragment)");

//	glBindAttribLocation(mProgram, GLES2_ATTRIBUTE_POSITION, "a_position");
//	glBindAttribLocation(mProgram, GLES2_ATTRIBUTE_TEXCOORD, "a_texCoord");
//	glBindAttribLocation(mProgram, GLES2_ATTRIBUTE_ANGLE, "a_angle");
//	glBindAttribLocation(mProgram, GLES2_ATTRIBUTE_CENTER, "a_center");

	glLinkProgram(mProgram);
	GLES2_checkError("glLinkProgram");
	GLint link_status = GL_FALSE;
	glGetProgramiv(mProgram, GL_LINK_STATUS, &link_status);
	if (!link_status) {
		return false;
	}

	//
	//  获取顶点属性在gpu中的索引
	//
	mVertexAttribLocations[GLES2_ATTRIBUTE_POSITION]=glGetAttribLocation(mProgram,"a_position");
	mVertexAttribLocations[GLES2_ATTRIBUTE_TEXCOORD]=glGetAttribLocation(mProgram,"a_texCoord");
	mVertexAttribLocations[GLES2_ATTRIBUTE_ANGLE]=glGetAttribLocation(mProgram,"a_angle");
	mVertexAttribLocations[GLES2_ATTRIBUTE_CENTER]=glGetAttribLocation(mProgram,"a_center");

	//
	//  获取uniform的索引
	//
	mUniformLocations[GLES2_UNIFORM_PROJECTION]=glGetUniformLocation(mProgram, "u_projection");
	mUniformLocations[GLES2_UNIFORM_TEXTURE_V]=glGetUniformLocation(mProgram, "u_texture_v");
	mUniformLocations[GLES2_UNIFORM_TEXTURE_U] =glGetUniformLocation(mProgram, "u_texture_u");
	mUniformLocations[GLES2_UNIFORM_TEXTURE]=glGetUniformLocation(mProgram, "u_texture");
	mUniformLocations[GLES2_UNIFORM_MODULATION]=glGetUniformLocation(mProgram, "u_modulation");
	mUniformLocations[GLES2_UNIFORM_COLOR]=glGetUniformLocation(mProgram, "u_color");

	return true;
}
//
// 更新一下投影矩阵,返回是否更新了
//
bool GLES2Renderer::updateProjectionMat(int32_t width,int32_t height){
	GLfloat projection[4][4];

	projection[0][0] = 2.0f / width;
	projection[0][1] = 0.0f;
	projection[0][2] = 0.0f;
	projection[0][3] = 0.0f;
	projection[1][0] = 0.0f;
	//if (renderer->target) {
	//	projection[1][1] = 2.0f / renderer->viewport.h;
	//} else {
	projection[1][1] = -2.0f / height;
	//}
	projection[1][2] = 0.0f;
	projection[1][3] = 0.0f;
	projection[2][0] = 0.0f;
	projection[2][1] = 0.0f;
	projection[2][2] = 0.0f;
	projection[2][3] = 0.0f;
	projection[3][0] = -1.0f;
	//if (renderer->target) {
	//	projection[3][1] = -1.0f;
	//} else {
	projection[3][1] = 1.0f;
	//}
	projection[3][2] = 0.0f;
	projection[3][3] = 1.0f;

	if(memcmp(mProjection, projection, sizeof (projection)) != 0) {
	   memcpy(mProjection, projection, sizeof (projection));
	   return true;
	}

	return  false;
}