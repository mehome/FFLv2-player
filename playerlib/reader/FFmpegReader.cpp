#include "FFmpegReader.hpp"
#include "Stream.hpp"
#include "ReaderStreamManager.hpp"
#include "../MessageFFMpegPacket.hpp"
#include "../PlayerEvent.hpp"
#include "../FFMpegStream.hpp"

namespace reader {
	FFMPegReader::FFMPegReader() :mAVFormatContext(NULL){
		setName("reader");
		mRestart = false;
		mSeekSerialNumber = 1;		
		mEOFFlag = 0;		
		mMessageCache = new FFL::PipelineMessageCache(MSG_FFMPEG_AVPACKET);
	}
	FFMPegReader::~FFMPegReader() {
		mMessageCache->clear();
	}
	//
	// 获取播放时长us
	//
	int64_t FFMPegReader::getDuration() {
		if (mAVFormatContext) {
			return mAVFormatContext->duration;
		}
		return 0;
	}
	//
	// 获取当前的播放位置 us
	//
	int64_t FFMPegReader::getCurrentPosition() {
		return 0;
	}

	//
	//  读取一帧数据
	//
	void FFMPegReader::onReadFrame(int64_t trackId) {
		if (mAVFormatContext == NULL) {			
			FFL_LOG_WARNING(" FFMPegReader::onReadFrame context is null.");
			FFL_sleep(10);
			return;
		}
		if (mEOFFlag) {
			//
			//  重定位到文件头部进行播放
			//
			FFL_sleep(100);
			if (onSeek(0)) {
				mEOFFlag = false;
			} else {
				FFL_LOG_WARNING(" FFMPegReader::onReadFrame eof");				
			}
			return;
		}

		message::FFMpegPacket* packet = NULL;
		FFL::sp<FFL::PipelineMessage> msg = message::createMessageFromCache(
			mMessageCache, &packet, MSG_FFMPEG_AVPACKET);
		msg->trackIdReset(trackId);
		FFL_ASSERT_LOG(packet, "FFMpegPacket is NULL.\n");
		
		//
		//  打印处理时间
		//		
		msg->trackStat("ffmpegReader:%" lld64, FFL_getNowUs());

		int err = av_read_frame(mAVFormatContext, packet->mPacket);
		if (0 != err) {
			if (err == AVERROR_EOF) {
				msg->consume(this);
				handleEof();
				return;
			}
			FFL_LOG_WARNING("Failed to read frame.");
		}
		else {

		}

		int streamIndex = packet->mPacket->stream_index;
		StreamEntry& stream = mStreamVector[streamIndex];
		if (!stream.isValid()) {
			msg->consume(this);
			return;
		}

		packet->mSerialNumber = mSeekSerialNumber;
		packet->mPacketType = stream.mStream->getStreamType();
		switch (packet->mPacketType)
		{
		case STREAM_TYPE_VIDEO:
			if (packet->mPacket->flags &AV_PKT_FLAG_KEY) {
				packet->mIFrame = 1;
			}
			else {
				packet->mIFrame = 0;
			}
			break;
		case STREAM_TYPE_AUDIO:
			packet->mIFrame = 0;
			break;
		default:
			packet->mIFrame = 0;
			break;
		}
	

		if (FFL_OK != postMessage(stream.mStream->getSource().mId, msg))
		{
			msg->consume(this);
		}
		else {
		}
	}
	//
	// open函数，具体实现
	//
	void FFMPegReader::onOpen(const char* url) {		
		if (mAVFormatContext) {
			avformat_close_input(&mAVFormatContext);
			mAVFormatContext = NULL;
		}

		//AVERROR
		int err = avformat_open_input(&mAVFormatContext, url, 0, 0);
		if (0 == err) {			
			avformat_find_stream_info(mAVFormatContext, NULL);
			openStream(mAVFormatContext->streams, mAVFormatContext->nb_streams);
		}		
	}

	//
	// close函数，具体实现
	//
	void FFMPegReader::onClose() {
		if (mAVFormatContext) {
			avformat_close_input(&mAVFormatContext);
			mAVFormatContext = NULL;
		}
	}

	//
	// pause函数，resume 具体实现
	//
	void FFMPegReader::onPause() {

	}
	void FFMPegReader::onResume() {

	}

	//
	// seek函数，具体实现
	//	
	bool FFMPegReader::onSeek(int64_t pos) {
		bool ret = seekPos(pos);
		//
		//  通知后面的节点
		//
		FFL::sp<FFL::PipelineMessage> msgControl = new FFL::PipelineMessage(MSG_CONTROL_READER_SEEK);
		mSeekSerialNumber++;
		msgControl->setParams(mSeekSerialNumber, mRestart?0:1);

		for (int i = 0; i < SUPORT_STREAM_NUM; i++) {
			StreamEntry& entry = mStreamVector[i];
			if (!entry.isValid()) {
				continue;
			}

			if (!mRestart) {
				clearMessage(entry.getOutputId());
			}

			if (FFL_OK != postMessageDelay(entry.getOutputId(), msgControl, -1)) {
			}
		}	
		mRestart = false;

		//
		//  通知ui层
		//
		event::postPlayerEvent(getOwner(), event::EVENT_SEEK_END,
			getCurrentPosition(),
			getDuration());
		event::postPlayerEvent(this, event::EVENT_BUFFERING_START);		

		return ret;
	}
	bool FFMPegReader::seekPos(int64_t pos) {
		int64_t ts = FFMPegUsToSeekfileTimestamp(pos);
		if (mAVFormatContext->start_time != -1) {
			ts += mAVFormatContext->start_time;
		}

		int err = avformat_seek_file(mAVFormatContext, -1, INT64_MIN, ts, INT64_MAX, 0);
		if (err < 0) {
			FFL_LOG_WARNING("Failed to seek %" lld64, ts);
			return false;
		}

		return true;
	}
	void FFMPegReader::openStream(AVStream** streams, uint32_t count) {
		FFL::sp<FFL::Pipeline> pipeline = getPipeline();
		AVStream* stream = 0;

		int streamIndexVec[3] = { -1 };
		streamIndexVec[0] = av_find_best_stream(mAVFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
		streamIndexVec[1] = av_find_best_stream(mAVFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);		
		streamIndexVec[2] = av_find_best_stream(mAVFormatContext, AVMEDIA_TYPE_SUBTITLE,-1, -1, NULL, 0);
		streamIndexVec[2] = -1;

		for (int i = 0; i < FFL_ARRAY_ELEMS(streamIndexVec); i++) {
			int32_t index = streamIndexVec[i];
			if (index< 0) continue;
			stream = streams[index];

			FFL::String name;
			StreamPtr streamInfo;
			switch (avcodec_get_type(stream->codecpar->codec_id)) {
			case AVMEDIA_TYPE_VIDEO:
			{
				name = "video";
				streamInfo = new player::FFMpegVideoStream(stream);				
				fillMetaData(streamInfo, stream);
			}
			break;
			case AVMEDIA_TYPE_AUDIO:
			{
				name = "audio";
				streamInfo = new player::FFMpegAudioStream(stream);
			}
			break;
			default:
				break;
			}
			if (streamInfo.isEmpty()) {
				FFL_LOG_WARNING("stream %d not support ", stream->index);
				continue;
			}

			player::OutputInterface output = createOutputInterface();			
			FFL::formatString(output.mName, "%s-decoder-%s-%d", getName(),name.c_str(), index);
			streamInfo->setSource(output);	

			StreamEntry& entry = mStreamVector[index];
			entry.mStream = streamInfo;			
			getStreamManager()->addStream(this,streamInfo);
		}

		getStreamManager()->addStream(this, NULL);
	}
	void FFMPegReader::fillMetaData(StreamPtr stream, AVStream* avstream) {
		FFL::Dictionary* dic = stream->getDictionary();
        dic->setKey("duration", "0");
	}
	void FFMPegReader::handleEof() {
		mEOFFlag = 1;
		if (mLoopCount < 0 || --mLoopCount >= 0 ) {
			//
			//  从头开始播放
			//
			mRestart = true;
		}else {
			mRestart = false;
		}

		FFL::sp<FFL::PipelineMessage> msgEof = new FFL::PipelineMessage(MSG_CONTROL_READER_EOF);
		msgEof->setParams(0, mRestart?0 : 1);
		for (int i = 0; i < SUPORT_STREAM_NUM; i++) {
			StreamEntry& entry = mStreamVector[i];
			if (!entry.isValid()) {
				continue;
			}
			if (FFL_OK != postMessage(entry.getOutputId(), msgEof)) {
			}
		}

		
	
		if (!mRestart) {
			//
			//  关闭当前的输入
			//
			mRestart = false;
			pauseLooper();
		}
	}
}
