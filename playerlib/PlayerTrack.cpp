#include "PlayerTrack.hpp"
#include <utils/FFL_String.hpp>

namespace player {

	FFLPlayerTrack::FFLPlayerTrack() {
		mTrackFile = new FFL::File();

		char processPath[1024] = {};		
		FFL_getCurrentProcessPath(processPath, 1023, 0);

		FFL::String trackFilePath;
		trackFilePath = processPath;		
		trackFilePath.append("track.json");
		mTrackFile->create(trackFilePath.c_str());
	}

	FFLPlayerTrack::~FFLPlayerTrack() {
		mTrackFile->close();
		FFL_SafeFree(mTrackFile);
	}

	void FFLPlayerTrack::write(const char* data) {
		if (data) {
			size_t writed = 0;

			FFL::CMutex::Autolock l(mLock);			
			mTrackFile->write((void*)data, strlen(data),&writed);
			mTrackFile->write((void*)"\r",1,&writed);
		}
	}

}
