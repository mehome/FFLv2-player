/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  JavaFFLPlayer.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/28 
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  java层FFLPlayer和Native层FFLplayer的一些相关导出，导入封装
*/
#ifndef _JAVA_FFLPLAYER_HPP_
#define _JAVA_FFLPLAYER_HPP_

#include <FFL.h>
#include "jnihelper/JavaClass.hpp"
#include "jnihelper/JavaObject.hpp"
namespace android{
    class JavaFFLPlayerClass;
    class JavaFFLPlayer;
    class NativeFFLPlayer;
    //
    //  引用进来java层的FFLPlayer
    //
    JavaFFLPlayerClass* importJavaFFLPlayerClass( JNIEnv& env);
    JavaFFLPlayerClass* getJavaFFLPlayerClass();

      //
    //  FFLplayer的class
    //
    class JavaFFLPlayerClass : public  JavaClass{
        friend  JavaFFLPlayerClass* importJavaFFLPlayerClass(JNIEnv& env);
    private:
        JavaFFLPlayerClass(JNIEnv &env);
    protected:
        //
        //  进行初始化，反初始化具体逻辑
        //
        virtual bool onInit(JNIEnv& env);
        virtual void onUninit(JNIEnv& env);

    public:
        //
        //  保存native层player到java对象中
        //
        static void saveNativePlayer(JNIEnv& env,jobject thiz,NativeFFLPlayer* player);
        //
        //  java对象中获取native层的player
        //
        static NativeFFLPlayer* getNativePlayer(JNIEnv& env,jobject thiz);
    public:
        friend class JavaFFLPlayer;
        //
        //  java对象中，保存的native层变量
        //
        jfieldID mField_mNativePlayer;
        //
        //  java对象中的函数
        //
        jmethodID mMethod_postEventFromNative;
    };
    //
    //  一个java层的FFLPlayer
    //
    class JavaFFLPlayer : public JavaObject<JavaFFLPlayerClass>{
    public:
        //
        //  javaObj:java层对象，golbal对象
        //   autoDel: 是否当这个JavaFFLPlayer对象删除的时候，需要自动删除java层对象
        //
        JavaFFLPlayer(jobject javaObj,bool autoDel);
        ~JavaFFLPlayer();
    public:
        inline void setJniEnv(JNIEnv* env){
            mJNIEnv=env;
        }

        void postMessage();
    private:
        JNIEnv* mJNIEnv;
    };
}
#endif
