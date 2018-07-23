
#include "JavaThread.hpp"
#include <FFL.h>

extern JavaVM* getJavaVM();

namespace android {
    JavaThread::JavaThread():mJniEnv(0){

    }
    void JavaThread::threadLoopStart(){
        getJavaVM()->AttachCurrentThread(&mJniEnv,0);
    }
    void JavaThread::threadLoopExit(const FFL::Thread* t){
        getJavaVM()->DetachCurrentThread();
    }
}

