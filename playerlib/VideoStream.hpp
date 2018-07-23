#ifndef _VIDEO_STREAM_HPP_
#define _VIDEO_STREAM_HPP_

#include "reader/Stream.hpp"
namespace player {
	class VideoStream : public reader::Stream {
	public:
		VideoStream();
		virtual ~VideoStream();
		//
		// 获取宽度，高度
		//
		virtual void getSize(uint32_t& width, uint32_t& height)=0;
	};
}
#endif
