#ifndef __RS_BUFFER_H__
#define __RS_BUFFER_H__

template <typename T>
class CBuffer {
public:
	CBuffer();
	CBuffer(const CBuffer &);
	CBuffer &operator=(const CBuffer &);
	CBuffer(CBuffer &&);
	CBuffer &operator=(CBuffer &&);
	~CBuffer();

public:
	void Resize();
	bool Empty() { return size_ == 0 || ptr_ == nullptr; }
private:
	int size_{ 0 };
	T *ptr_{ nullptr };
};

template <typename T>
CBuffer<T>::CBuffer() = default;

template <typename T>
CBuffer<T>::~CBuffer()
{
	delete[]ptr_;
	ptr_ = nullptr;
	size_ = 0;
}

template <typename T>
CBuffer<T>::CBuffer(const CBuffer<T> &buffer)
{
	if (!buffer.Empty()) {
		size_ = buffer.size;
		ptr_ = new T[size_];

		if (ptr_ != nullptr) {
			memcpy(this->ptr_, buffer.ptr_, size_ * sizeof(T));
		}
	}
}

template <typename T>
CBuffer<T>& CBuffer<T>::operator=(const CBuffer<T> &buffer)
{
	if (ptr_ != nullptr) {
		delete[]ptr_;
		ptr_ = nullptr;
		size_ = 0;
	}

	if (!buffer.Empty()) {
		size_ = buffer.size;
		ptr_ = new T[size_];

		if (ptr_ != nullptr) {
			memcpy(ptr_, buffer.ptr_, size_ * sizeof(T));
		}
	}

	return *this;
}

#endif