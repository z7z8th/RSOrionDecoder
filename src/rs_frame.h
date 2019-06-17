#ifndef __RS_FRAME_H__
#define __RS_FRAME_H__

#include <string>

#include "hw/hlog.h"

namespace readsense {

template<typename T>
class RSFrame {
public:
	RSFrame() { 
		//hlogi("RSFrame::RSFrame"); 
	}
	~RSFrame();
	RSFrame(const RSFrame &);
	RSFrame& operator=(const RSFrame &);
	//RSFrame(RSFrame &&);
	//RSFrame& operator=(RSFrame &&);

public:
	void init(int cap);
	void resize(int cap);
	int length() const { return len_; }
	T *data() const { return ptr_; }

public:
	int width_{ 0 };
	int height_{ 0 };
	int bpp_{ 0 };
	int type_{ 0 };
	uint64 ts_{ 0 };
	std::string src_;

private:
	T *ptr_{ nullptr };
	int len_{ 0 };
};

template<typename T>
RSFrame<T>::~RSFrame() {
	if (ptr_) {
		//hlogi("RSFrame::~RSFrame");
		delete[]ptr_;
		ptr_ = nullptr;
	}
}

template<typename T>
RSFrame<T>::RSFrame(const RSFrame<T> &rhs) {
	//hlogi("RSFrame::RSFrame(const RSFrame<T> &rhs)");
	int rlen = rhs.length();
	T *rdata = rhs.data();

	if (rlen > 0 && rdata) {
		init(rlen);
		memcpy(ptr_, rdata, rlen);
	}

	width_ = rhs.width_;
	height_ = rhs.height_;
	bpp_ = rhs.bpp_;
	type_ = rhs.type_;
	ts_ = rhs.ts_;
	src_ = rhs.src_;
}

template<typename T>
RSFrame<T> &RSFrame<T>::operator=(const RSFrame<T> &rhs) {
	//hlogi("RSFrame::operator=(const RSFrame<T> &rhs)");
	int rlen = rhs.length();
	T *rdata = rhs.data();

	if (rlen > 0 && rdata) {
		init(rlen);
		memcpy(ptr_, rdata, rlen);
	}

	width_ = rhs.width_;
	height_ = rhs.height_;
	bpp_ = rhs.bpp_;
	type_ = rhs.type_;
	ts_ = rhs.ts_;
	src_ = rhs.src_;

	return *this;
}

//template<typename T>
//RSFrame<T>::RSFrame(const Frame<T> &&rhs) {
//}
//
//template<typename T>
//RSFrame &RSFrame<T>::operator=(const RSFrame<T> &&rhs) {
//}

template<typename T>
void RSFrame<T>::init(int cap) {
	if (cap <= 0) return;
	if (len_ == cap) return;

	if (!ptr_) {
		ptr_ = new T[cap];
		len_ = cap;
		memset(ptr_, 0, len_);
	}
	else {
		T *tmp = new T[cap];
		int size = std::min(len_, cap);
		memcpy(tmp, ptr_, size);
		delete[]ptr_;

		ptr_ = tmp;
		len_ = cap;
		memset(ptr_, 0, len_);
	}
}

template<typename T>
void RSFrame<T>::resize(int cap) {
	init(cap);
}

}	// namespace readsense

#endif	// __RS_FRAME_H__
