/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  JavaAudiotrack.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/28 
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  java层的AudioTrack用于播放音频
*/
#ifndef _JAVA_AUDIOTRACK_HPP_
#define _JAVA_AUDIOTRACK_HPP_

#include <FFL.h>
#include "jnihelper/JavaClass.hpp"
#include "jnihelper/JavaObject.hpp"

namespace android{
    class JavaAudioTrackClass;
    class JavaAudioTrack;
    //
    //  引用进来audiotrack
    //
    JavaAudioTrackClass* importJavaAudioTrack(JNIEnv& env);
    JavaAudioTrackClass* getJavaAudioTrackClass();

    enum {
        //
        //  播放音乐
        //
        AUDIO_TRACK_STREAM_MUSIC = 3,
        //
        //  双声道
        //
        AUDIOTRACK_CHANNEL_OUT_STEREO = 12,
        //
        //  位深
        //
        AUDIOTRACK_ENCODING_PCM_16BIT = 2,
        AUDIOTRACK_ENCODING_PCM_8BIT = 3,
        AUDIOTRACK_ENCODING_PCM_FLOAT = 4,
        //
        //  模式，静态塞进去，动态拉取
        //
        AUDIOTRACK_MODE_STATIC = 0,
        AUDIOTRACK_MODE_STREAM = 1,
    };
    //
    //  AudioTrack的class
    //
	class JavaAudioTrackClass : public  JavaClass{
        friend  JavaAudioTrackClass* importJavaAudioTrack(JNIEnv& env);
    private:
        JavaAudioTrackClass(JNIEnv &env);
    protected:
        //
        //  进行初始化，反初始化具体逻辑
        //
        virtual bool onInit(JNIEnv& env);
        virtual void onUninit(JNIEnv& env);
    public:
        //
        //   创建，删除一个Audiotrack
        //
        JavaAudioTrack* allocAudioTrack(JNIEnv& env,
                                        jint streamType,
                                        jint sampleRateInHz,
                                        jint channelConfig,
                                        jint audioFormat,
                                        jint bufferSizeInBytes,
                                        jint mode);
        void freeAudioTrack(JavaAudioTrack* audiotrack);

    public:
        int getMinBufferSize(JNIEnv* env,int sampleRateInHz, int channelConfig, int audioFormat);
    protected:
        friend class JavaAudioTrack;
        jmethodID  mMethod_constructor;
        jmethodID  mMethod_play;
        jmethodID  mMethod_pause;
        jmethodID  mMethod_stop;
        jmethodID  mMethod_flush;
        jmethodID  mMethod_setVolume;
        jmethodID  mMethod_release;
        jmethodID  mMethodr_write;

        jmethodID  mStaticMethod_getMinBufferSize;
    };
    //
    //  一个java层的AudioTrack
    //
    class JavaAudioTrack : public JavaObject<JavaAudioTrackClass>{
        friend class JavaAudioTrackClass;
    protected:
        JavaAudioTrack(JNIEnv* env,jobject javaAudioTrack);
        ~JavaAudioTrack();
    public:

        void play();
        void pause();
        void flush();
        void setVolume(float left,float right);
        void stop();
        void release();
        int write(uint8_t* data,int size);

    private:


        jbyteArray mJByteArray;
        int32_t mJByteArrayCapcity;
    };
}

#endif
