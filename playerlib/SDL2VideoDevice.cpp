#include "SDL2VideoDevice.hpp"
#include "SDL2MessageTexture.hpp"
#include "VideoRender.hpp"
#include "VideoSurface.hpp"

namespace player {
	class SDL2VideoSurface : public VideoSurface{
	public:
		//
		//  设置窗口的宽度，高度
		//
		void setWindowSize(int32_t widht, int32_t height) {
			SDL_SetWindowSize(mWindow,widht,height);
		}
		
		SDL_Window* mWindow;
	};
	SDL2VideoDevice::SDL2VideoDevice():
		mWindow(NULL),
	    mSDLRenderer(NULL)
	{
		mMessageCache = new FFL::PipelineMessageCache(MSG_SDL2_TEXTURE);
	}
	SDL2VideoDevice::~SDL2VideoDevice() {
		mMessageCache->clear();
	}
	//
	//  获取支持的格式
	//  wanted: 如果为nUll则返回所有支持的格式
	//           非null 返回跟他匹配的
	//  fmtList: 返回支持的格式list
	//
	void SDL2VideoDevice::getSupportFormat(const player::VideoFormat* wanted, FFL::List<player::VideoFormat>& fmtList) {

	}
	bool SDL2VideoDevice::getOptimalFormat(const player::VideoFormat* wanted, player::VideoFormat* optinal) {
		optinal->mWidht = -1;
		optinal->mHeight = -1;
		optinal->mFormat = VideoFormat::PIXEL_FORMAT_YUV420P;
		return true;
	}
	bool SDL2VideoDevice::isSupportFormat(const player::VideoFormat* wanted) {
		if (wanted->getFormat() == VideoFormat::PIXEL_FORMAT_YUV420P) {
			return true;
	    }
		return false;
	}

	FFL::sp<VideoSurface> SDL2VideoDevice::getSurface() {
		return mSurface;
	}
	//
	// 更新绘制的目标窗口
	//
	void SDL2VideoDevice::setSurface(SurfaceHandle surface) {
		FFL_ASSERT("SDL2VideoDevice::setSurface");
	}

	//
	//  打开关闭视频设备
	//
	bool SDL2VideoDevice::onOpen(SurfaceHandle surface, int32_t widht, int32_t height) {
		SDL_Window * hWnd=NULL;
		if (surface==NULL) {
			hWnd = SDL_CreateWindow("PlayerCore", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				widht, height, SDL_WINDOW_OPENGL );
		}else {
			hWnd = SDL_CreateWindowFrom(surface);
		}

		if (hWnd == NULL) {
			FFL_LOG_ERROR("SDL2VideoDevice::open wnd=null.");
			return false;
		}
		mWindow = hWnd;
		mMessageCache->clear();
		SDL2VideoSurface* sdl2Surface = new SDL2VideoSurface();
		sdl2Surface->setHandle(surface);
		sdl2Surface->mWindow = mWindow;
		mSurface=sdl2Surface;
		return mWindow!=NULL;
	}
	void SDL2VideoDevice::onClose() {
		mSurface = NULL;
		SDL_DestroyRenderer(mSDLRenderer);
		mSDLRenderer = NULL;

		SDL_DestroyWindow(mWindow);		
		mWindow = NULL;		
	}
	//
	//  清空cache的数据
	//
	void SDL2VideoDevice::clearCache() {
		FFL_LOG_DEBUG("SDL2VideoDevice::clearCache");
	}
	bool SDL2VideoDevice::showTexture(VideoTexture* texture) {
		FFL::sp<FFL::PipelineMessage> msg = createSDL2Texture(texture);
		return showSDL2Texture(msg);
	}
	//
	// 通过纹理创建一个sdl2模块可以处理的消息
	//
	FFL::sp<FFL::PipelineMessage> SDL2VideoDevice::createSDL2Texture(VideoTexture* texture){
		if (mSDLRenderer == NULL) {
			mSDLRenderer = SDL_CreateRenderer(mWindow, 0, 0);
		}

		if (mSDLRenderer == NULL) {
			return NULL;
		}

		message::SDL2Texture* sdlTexture = 0;
		FFL::sp<FFL::PipelineMessage> msg = message::createMessageFromCache(mMessageCache, &sdlTexture, MSG_SDL2_TEXTURE);

		uint32_t pixformat = 0;
		pixformat = SDL_PIXELFORMAT_IYUV;
		SDL_Texture* pic = sdlTexture->mTexture;
		if (pic == NULL) {
			pic = SDL_CreateTexture(mSDLRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, texture->mWidth, texture->mHeight);
			sdlTexture->mTexture = pic;
		}

	

		SDL_UpdateYUVTexture(sdlTexture->mTexture, NULL,
			texture->mPixels[0], texture->mPitches[0],
			texture->mPixels[1], texture->mPitches[1],
			texture->mPixels[2], texture->mPitches[2]);
		
		return msg;
	}


	bool SDL2VideoDevice::showSDL2Texture(FFL::sp<FFL::PipelineMessage> msg) {
		if (msg.isEmpty()) {
			return false;
		}

		if (msg->getType() == MSG_SDL2_TEXTURE )
		{
			SDL_Texture * texture = ((message::SDL2Texture*) msg->getPayload())->mTexture;

			//
			//清空渲染器
			//复制数据纹理给渲染器
			//显示
			//
			SDL_RenderClear(mSDLRenderer);
			SDL_RenderCopy(mSDLRenderer, texture, NULL, NULL);
			//SDL_SetRenderDrawColor(mSDLRenderer, 255,255, 0, 255);
			//SDLTest_DrawString(mSDLRenderer, 0, 0, "Hi");//
			SDL_RenderPresent(mSDLRenderer);
		}

		msg->consume(this);
		return true;
	}
	void SDL2VideoDevice::resetRender() {
		VideoDevice::resetRender();

		SDL_DestroyRenderer(mSDLRenderer);
		mSDLRenderer = NULL;
		mMessageCache->clear();
	}
}