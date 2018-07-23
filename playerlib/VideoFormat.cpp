#include "VideoFormat.hpp"
#include "FFMpeg.hpp"

namespace player {
	struct FFMpegPixFormat2VideoFormatMap{
		int32_t mFFMpegFormat;
		int32_t mVideoFormat;
	};
	static FFMpegPixFormat2VideoFormatMap gFFMpegPixFormat2VideoFormatMap[]={
			{AV_PIX_FMT_YUV420P,VideoFormat::PIXEL_FORMAT_YUV420P},
            {AV_PIX_FMT_YUV420P,VideoFormat::PIXEL_FORMAT_YV12},
			{AV_PIX_FMT_RGBA,VideoFormat::PIXEL_FORMAT_RGBA_8888},
	};

	VideoFormat::VideoFormat(){
		mFormat=PIXEL_FORMAT_NONE;
		mWidht=-1;
		mHeight=-1;
	}
	VideoFormat::~VideoFormat(){

	}
	void VideoFormat::setFFMpegFormat(int32_t pixFormat){
		for(int32_t i=0;i<FFL_ARRAY_ELEMS(gFFMpegPixFormat2VideoFormatMap);i++){
			if(gFFMpegPixFormat2VideoFormatMap[i].mFFMpegFormat==pixFormat){
				mFormat=gFFMpegPixFormat2VideoFormatMap[i].mVideoFormat;
				break;
			}

		}
	}
	int32_t VideoFormat::getFFMpegFormat(){
		for(int32_t i=0;i<FFL_ARRAY_ELEMS(gFFMpegPixFormat2VideoFormatMap);i++){
			if(gFFMpegPixFormat2VideoFormatMap[i].mVideoFormat==mFormat){
				return gFFMpegPixFormat2VideoFormatMap[i].mFFMpegFormat;
			}
		}
		return AV_PIX_FMT_NONE;
	}
	VideoFormat& VideoFormat::operator=(const VideoFormat& r){
		mFormat=r.mFormat;
		mWidht=r.mWidht ;
		mHeight = r.mHeight;
		return  *this;
	}
	bool VideoFormat::operator==(const VideoFormat& r) const{
		return this->equal(r);
	}
	bool VideoFormat::equal(const VideoFormat& r) const{
		return  (mFormat==r.mFormat && mWidht==r.mWidht && mHeight == r.mHeight);
	}
}
