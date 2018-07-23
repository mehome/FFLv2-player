#ifndef _JNIJAVA_THREAD_HPP_
#define _JNIJAVA_THREAD_HPP_

#include <jni.h>
#include <thread/FFL_Thread.hpp>


namespace android {
	class JavaThread : public FFL::Thread {
	public:
		JavaThread();

	protected:
		virtual void threadLoopStart();

		virtual void threadLoopExit(const FFL::Thread *t);

		JNIEnv *getJniEnv() const { return mJniEnv; }

	private:
		JNIEnv *mJniEnv;
	};
}
#endif
