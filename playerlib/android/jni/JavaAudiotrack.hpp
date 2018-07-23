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
    };
    //
    //  一个java层的AudioTrack
    //
    class JavaAudioTrack : public JavaObject<JavaAudioTrackClass>{
        friend class JavaAudioTrackClass;
    protected:
        JavaAudioTrack(jobject javaAudioTrack);
        ~JavaAudioTrack();
    public:
        inline void setJniEnv(JNIEnv* env){
            mJNIEnv=env;
        }

        void play();
        void pause();
        void flush();
        void setVolume(float left,float right);
        void stop();
        void release();
        int write(uint8_t* data,int size);

    private:
        JNIEnv* mJNIEnv;
    };
}

#endif
