/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  JavaFFLPlayer.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/06/28
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  java层FFLPlayer和Native层FFLplayer的一些相关导出，导入封装
*/
#include "JavaFFLPlayer.hpp"
#include "NativeFFLPlayer.hpp"
#include "NativeFFLPlayer.hpp"

namespace android{
    static  JavaFFLPlayerClass* gJavaFFLPlayerClass=NULL;

    JavaFFLPlayerClass* importJavaFFLPlayerClass( JNIEnv& env){
        if(!gJavaFFLPlayerClass) {
            gJavaFFLPlayerClass = new JavaFFLPlayerClass(env);
            gJavaFFLPlayerClass->init(env);
        }
        return gJavaFFLPlayerClass;
    }
    JavaFFLPlayerClass* getJJavaFFLPlayerClass(){
        return  gJavaFFLPlayerClass;
    }

    JavaFFLPlayerClass::JavaFFLPlayerClass(JNIEnv &env):
            JavaClass(env,"ffl/player/FFLPlayer"){
    }
    //
    //  进行初始化，反初始化具体逻辑
    //
    bool JavaFFLPlayerClass::onInit(JNIEnv& env){
        const char *sign = NULL;
        mField_mNativePlayer=getFieldId(env,"mNativePlayer", "J");

        sign = "(Ljava/lang/Object;IIILjava/lang/Object;)V";
        mMethod_postEventFromNative=getStaticMethodId(env,"postEventFromNative", sign);

        int methodNum=0;
        JNINativeMethod* methodList=NativeFFLPlayer::getExportMethods(&methodNum);
        exportNativeMethod(env,methodList,methodNum);
        return  true;
    }
    void JavaFFLPlayerClass::onUninit(JNIEnv& env){
    }

    //
    //  保存native层player到java对象中
    //
    void JavaFFLPlayerClass::saveNativePlayer(JNIEnv& env,jobject thiz,NativeFFLPlayer* player){
        jlong val=(jlong)player;
        env.SetLongField(thiz,
                         gJavaFFLPlayerClass->mField_mNativePlayer,
                         val);
    }
    //
    //  java对象中获取native层的player
    //
    NativeFFLPlayer* JavaFFLPlayerClass::getNativePlayer(JNIEnv& env,jobject thiz){
        jlong player=env.GetLongField(thiz,gJavaFFLPlayerClass->mField_mNativePlayer);
        return  (NativeFFLPlayer*)player;
    }
    JavaFFLPlayer::JavaFFLPlayer(jobject javaPlayer, bool autodel):
            JavaObject<JavaFFLPlayerClass>(*gJavaFFLPlayerClass,javaPlayer,autodel){

    }
    JavaFFLPlayer::~JavaFFLPlayer(){

    }

    void JavaFFLPlayer::postMessage(){

    }
}