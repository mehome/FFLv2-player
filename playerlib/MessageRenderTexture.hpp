#pragma once
#include "MessageBase.hpp"
#include "FFL_Texture.hpp"

namespace message {
	//
	//   纹理数据, NodeDecoder出来的数据，NodeRender输入的数据
	//
	class Texture : public MessageBase
	{
	public:
		Texture();
		~Texture();
	public:	
		FFLTexture* mTexture;
		//
		//私有的数据 
		//
		void* mOpaque;
			
	};

}
