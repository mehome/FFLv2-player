#ifndef _VIDEO_FORAMT_HPP_
#define _VIDEO_FORAMT_HPP_

#include <FFL.h>
namespace player {
	class VideoFormat{
	public:
		enum {
			PIXEL_FORMAT_NONE = 0,
			PIXEL_FORMAT_YUV420P = 1,

            //
            //  YYYYYYYYYYYYYYYY
            //  uuvvv
            //
			PIXEL_FORMAT_YV12 = 2,
            //
            //  uv交错
            //  YYYYYYYYYYYYYYYY
            //  uvuv
            //
            PIXEL_FORMAT_NV12 = 2,
			PIXEL_FORMAT_NV21 ,
			PIXEL_FORMAT_RGBA_8888,
		};

	public:
		VideoFormat();
		~VideoFormat();

		void setFFMpegFormat(int32_t pixFormat);
        int32_t getFFMpegFormat();

        int32_t getFormat() const {
            return mFormat;
        }

		VideoFormat& operator=(const VideoFormat& r);
		bool operator==(const VideoFormat& r) const;
		bool equal(const VideoFormat& r) const;
	public:
		//
		//  格式，宽度，高度
		//
		int32_t mFormat;
		int32_t mWidht;
		int32_t mHeight;
	};
}
#endif
