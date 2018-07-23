/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  MemcopyRender.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/06/28
*  https://github.com/zhenfei2016/FFL-v2.git
*  android ANativewindow数据拷贝显示方式
*
*/
#include <android/native_window.h>
#include "MemcopyRender.hpp"

#include "VideoTexture.hpp"
#include "VideoSurface.hpp"
#include "FFMpeg.hpp"
namespace android{
    status_t ANativeWindowDisplay(ANativeWindow *window, player::VideoTexture* tex);

    MemcopyRender::MemcopyRender(){}
    MemcopyRender::~MemcopyRender(){}
    //
    //  创建这个render ，arg自定义的参数
    //
    status_t MemcopyRender::create(player::VideoSurface* surface,const player::VideoFormat*  arg){

        return  FFL_OK;
    }
    //
    //  绘制图片tex到 surface上面
    //
    status_t MemcopyRender::draw(player::VideoSurface* surface,player::VideoTexture* tex){
        return  ANativeWindowDisplay(surface->getHandle(),tex);
    }
    //
    //  释放这个render，释放后这个render就不可用了
    //
    void MemcopyRender::destroy(){

    }

    static int renderRgb2Rgb(ANativeWindow_Buffer *out_buffer, const player::VideoTexture *tex, int bpp) {
        assert(tex->mPlanesCount == 1);

        int min_height = FFL_MIN(out_buffer->height, tex->mHeight);
        int dst_stride = out_buffer->stride;
        int src_line_size = tex->mPitches[0];
        int dst_line_size = dst_stride * bpp / 8;

        uint8_t *dst_pixels =(uint8_t*) out_buffer->bits;
        uint8_t *src_pixels = tex->mPixels[0];


//        for(int i=0;i<255;i++){
//            for(int j=0;j<src_line_size;j++)
//               *(src_pixels+i*src_line_size + j) =0xFF;
//        }
        if (dst_line_size == src_line_size) {
            int plane_size = src_line_size * min_height;
            memcpy(dst_pixels, src_pixels, plane_size);
        } else {
            int bytewidth = FFL_MIN(dst_line_size, src_line_size);
            av_image_copy_plane(dst_pixels, dst_line_size, src_pixels, src_line_size, bytewidth, min_height);
        }
        return 0;
    }

    status_t ANativeWindowDisplay(ANativeWindow *window, player::VideoTexture* tex){
        if (NULL==window || NULL==tex) {
            FFL_LOG_ERROR("ANativeWindow is Null or tex is NULL.");
            return FFL_FAILED;
        }

        if (tex->mWidth <= 0 || tex->mHeight <= 0){
            return FFL_FAILED;
        }

        int32_t ret;
        int curWidth = ANativeWindow_getWidth(window);
        int curHeight = ANativeWindow_getHeight(window);
        int currForamt = ANativeWindow_getFormat(window);
        int imageWidth = FFL_ALIGN(tex->mWidth, 2);
        int imageHeight = FFL_ALIGN(tex->mHeight, 2);

        ANativeWindow_Buffer framebuffer;
        ret = ANativeWindow_lock(window, &framebuffer, NULL);
        if (ret < 0)
        {
            FFL_LOG_ERROR("Faild to ANativeWindow_lock ret %d", ret);
            return ret;
        }

        if (framebuffer.width != imageWidth || framebuffer.height != imageHeight){
            ANativeWindow_unlockAndPost(window);
            //ANativeWindow_setBuffersGeometry(window, imageWidth, imageHeight, ARBt);
            return -1;
        }

        renderRgb2Rgb(&framebuffer,tex,32);

        ret = ANativeWindow_unlockAndPost(window);
        if (ret < 0)
        {
            FFL_LOG_ERROR("Failed to ANativeWindowDisplay %d", ret);
            return ret;
        }
        return FFL_OK;
    }

    //  获取支持的格式
    //  wanted: 如果为nUll则返回所有支持的格式
    //           非null 返回跟他匹配的
    //  fmtList: 返回支持的格式list
    //
    void MemcopyRender::getSupportFormat(player::VideoSurface* surface,const player::VideoFormat* wanted,FFL::List<player::VideoFormat>& fmtList){
        player::VideoFormat targetFormat;
        surface->getFormat(targetFormat);
        fmtList.push_back(targetFormat);
    }
    bool MemcopyRender::getOptimalFormat(player::VideoSurface* surface,const player::VideoFormat* wanted,player::VideoFormat* optinal){
        player::VideoFormat targetFormat;
        surface->getFormat(targetFormat);
        if(optinal){
            *optinal=targetFormat;
        }
        return true;
    }
    bool MemcopyRender::isSupportFormat(player::VideoSurface* surface,const player::VideoFormat* wanted){
        player::VideoFormat targetFormat;
        surface->getFormat(targetFormat);
        return  targetFormat.equal(*wanted);
    }

}
