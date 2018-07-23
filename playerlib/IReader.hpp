#pragma once

class IReader{
public:
	
    virtual ~IReader(){}

public:
	//
	//  打开
	//
	virtual void play(const char* url)=0;
	//
	//  暂停读，但是不关闭
	//
	virtual void pause()=0;
	//
	//  恢复读
	//
	virtual void resume()=0;
	//
	// 设置开始读位置
	//
	virtual void seek(int64_t pos)=0;
	//
	// 关闭
	//
	virtual void close()=0;

public:
	//
	// 获取播放时长
	//
	virtual int64_t getDurationUs()=0;
	//
	// 获取当前的播放位置
	//
	virtual int64_t getCurrentUs() = 0;
};
