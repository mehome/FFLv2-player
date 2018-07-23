/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  JNImain.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/06/28
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  jni入口函数
*/
#include <jni.h>
#include <string>
#include "android/JavaAudiotrack.hpp"
#include "android/JavaFFLPlayer.hpp"
#include "FFMpeg.hpp"


namespace android {
    //
    //  获取，设置虚拟机
    //
    extern JavaVM *getJavaVM();

    extern void setJavaVM(JavaVM *jvm);

    //
    //  获取当前线程的env
    //
    extern void getJNIEnv(JNIEnv **env);
}

extern "C" {
#include <android/log.h>
}

extern "C" int fflv2PrintLog(int level,const char* tag,const char *format, va_list vaList){
    __android_log_print(ANDROID_LOG_ERROR,tag?tag:"FFLv2",format,vaList);
    return 1;
}

extern "C"  void ffmpegPrintLog(void *ptr, int level, const char *fmt, va_list vl)
{
    static int print_prefix = 1;
    static int count;
    static char prev[1024];
    char line[1024];
    static int is_atty;

    av_log_format_line(ptr, level, fmt, vl, line, sizeof(line), &print_prefix);

    strcpy(prev, line);
    //sanitize((uint8_t *)line);

    if (level <= AV_LOG_WARNING)
    {
        __android_log_print(ANDROID_LOG_ERROR,"ffmpeg","%s",line);;
    }
    else
    {
        __android_log_print(ANDROID_LOG_DEBUG,"ffmpeg","%s",line);;
    }
}

void Loginit(){
    FFL_LogSetLevel(FFL_LOG_LEVEL_ALL);
    FFL_LogHook(fflv2PrintLog);
    av_log_set_level(AV_LOG_DEBUG);
    av_log_set_callback(ffmpegPrintLog);
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    android::setJavaVM(vm);
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    Loginit();

    android::importJavaAudioTrack(*env);
    android::importJavaFFLPlayerClass(*env);

    return JNI_VERSION_1_4;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {

}
