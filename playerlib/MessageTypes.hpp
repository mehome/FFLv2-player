#ifndef _PLAYER_MESSAGE_TYPE_ID_
#define _PLAYER_MESSAGE_TYPE_ID_

//
//  pipeline间传递的所有的消息类型
//
enum
{

    //
    // FFMpeg格式编码的数据包，音视频
    //
    MSG_FFMPEG_AVPACKET=2,

    //
    //  FFMPEG解码后的Avframe
    //
    MSG_FFMPEG_VIDEO_FRAME=4,
	//
	//  进行一次转码后的
	//
	MSG_FFMPEG_VIDEO_FRAME_SCALE = 5,
    //
    //  音频采样包
    //
	MSG_FFMPEG_AUDIO_FRAME ,
	//
	//  纹理数据
	//
	
	MSG_SDL2_TEXTURE,
	MSG_SDL2_SAMPLES,

	//
	// 读到结尾了
	//
	MSG_CONTROL_READER_EOF,	
	//
	// 一次seek操作   param1: 新的seekid
	//                param2: 标志是否丢弃前面的一些帧
	//	
	MSG_CONTROL_READER_SEEK,
    //
    // 对其音视频同步指令
    //
    MSG_CONTROL_AV_SYNC,
    

};

typedef struct MsgCodeEntry{
    int32_t code;
    const char* name;
}MsgCodeEntry;

#define REGISTER_MSG_CODE(msg) {msg,FFL_TOSTRING(msg)}

//
// 获取对应消息的名称
//
inline const char* getMsgName(int32_t msg){
    static MsgCodeEntry entry[]={
        REGISTER_MSG_CODE(MSG_FFMPEG_AVPACKET),
        REGISTER_MSG_CODE(MSG_FFMPEG_VIDEO_FRAME),
        REGISTER_MSG_CODE(MSG_FFMPEG_AUDIO_FRAME),
        REGISTER_MSG_CODE(MSG_SDL2_TEXTURE),
        REGISTER_MSG_CODE(MSG_SDL2_SAMPLES),
        REGISTER_MSG_CODE(MSG_CONTROL_READER_EOF),
		REGISTER_MSG_CODE(MSG_CONTROL_READER_SEEK),
        REGISTER_MSG_CODE(MSG_CONTROL_AV_SYNC),
    };
    
    for(int i=0;i<sizeof(entry)/sizeof(entry[0]);i++){
    
        if(entry[i].code==msg){
            return entry[i].name;
            
        }
    }
    
    FFL_ASSERT_LOG(0, "unregister msg");
    return "unknow";
    
}
//FFL_TOSTRING(MSG_DECODER_CONTROL)



#endif
