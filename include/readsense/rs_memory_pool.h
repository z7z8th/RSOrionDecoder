﻿#ifndef __RS_MMEMARY_H__
#define __RS_MMEMARY_H__

#include <new>
#include <functional>
#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <assert.h>

static const int __align = 8;
static const int __max_bytes = 128;
static const int __number_of_free_lists = __max_bytes / __align;
static const int __number_add_nodes = 20;

namespace readsense {

class CMemoryPool {
public:
	CMemoryPool();
	//bulk memory size. everytime add nodes num
	CMemoryPool(const int large_sz, const int add_num);
	~CMemoryPool();

	//for object. invocation of constructors and destructors
	template<typename T, typename... Args >
	T* PoolNew(Args&&... args);
	template<typename T>
	void PoolDelete(T* &c);

	//for continuous memory
	template<typename T>
	T* PoolMalloc(int size);
	template<typename T>
	void PoolFree(T* &m, int len);

	//for bulk memory. 
	//return one bulk memory node
	template<typename T>
	T* PoolLargeMalloc();
	template<typename T>
	void PoolLargeFree(T* &m);
	template<typename T>
	T* PoolLargeMalloc(int size, int& res);
	template<typename T>
	void PoolLargeFree(T* &m, int size);

	std::thread::id GetCreateThreadId();

	//return bulk memory size
	int GetLargeSize() const;

private:
	int RoundUp(int size, int align = __align) {
		return ((size + align - 1) & ~(align - 1));
	}

	int FreeListIndex(int size, int align = __align) {
		return (size + align - 1) / align - 1;
	}

	void* ReFill(int size, int num = __number_add_nodes, bool is_large = false);
	void* ChunkAlloc(int size, int& nums, bool is_large = false);

private:
	union MemNode {
		MemNode*	_next;
		char		_data[1];
	};

	MemNode*	_free_list[__number_of_free_lists];
	std::mutex	_mutex;
	char*		_pool_start;
	char*		_pool_end;
	std::thread::id		_create_thread_id;
	std::vector<char*>  _malloc_vec;

	std::mutex	_large_mutex;
	int			_number_large_add_nodes;			//everytime add nodes num
	int			_large_size;						//bulk memory size
	std::map<int, MemNode*>	_free_large;			//bulk memory list
};

template<typename T, typename... Args>
T* CMemoryPool::PoolNew(Args&&... args) {
	int sz = sizeof(T);
	if (sz > __max_bytes) {
		void* bytes = malloc(sz);
		T* res = new(bytes) T(std::forward<Args>(args)...);
		return res;
	}

	std::unique_lock<std::mutex> lock(_mutex);
	MemNode** my_free = &(_free_list[FreeListIndex(sz)]);
	MemNode* result = *my_free;
	if (result == nullptr) {
		void* bytes = ReFill(RoundUp(sz));
		T* res = new(bytes) T(std::forward<Args>(args)...);
		return res;
	}

	*my_free = result->_next;
	T* res = new(result) T(std::forward<Args>(args)...);
	return res;
}

template<typename T>
void CMemoryPool::PoolDelete(T* &c) {
	if (!c) {
		return;
	}

	int sz = sizeof(T);
	if (sz > __max_bytes) {
		free(c);
		return;
	}

	MemNode* node = (MemNode*)c;
	MemNode** my_free = &(_free_list[FreeListIndex(sz)]);

	std::unique_lock<std::mutex> lock(_mutex);
	c->~T();
	node->_next = *my_free;
	*my_free = node;
	c = nullptr;
}

template<typename T>
T* CMemoryPool::PoolMalloc(int sz) {
	if (sz > __max_bytes) {
		void* bytes = malloc(sz);
		memset(bytes, 0, sz);
		return (T*)bytes;
	}

	std::unique_lock<std::mutex> lock(_mutex);
	MemNode** my_free = &(_free_list[FreeListIndex(sz)]);
	MemNode* result = *my_free;
	if (result == nullptr) {
		void* bytes = ReFill(RoundUp(sz));
		memset(bytes, 0, sz);
		return (T*)bytes;
	}

	*my_free = result->_next;
	memset(result, 0, sz);
	return (T*)result;
}

template<typename T>
void CMemoryPool::PoolFree(T* &m, int len) {
	if (!m) {
		return;
	}

	if (len > __max_bytes) {
		free(m);
		m = nullptr;
		return;
	}

	MemNode* node = (MemNode*)m;
	MemNode** my_free = &(_free_list[FreeListIndex(len)]);

	std::unique_lock<std::mutex> lock(_mutex);
	node->_next = *my_free;
	*my_free = node;
	m = nullptr;
}

template<typename T>
T* CMemoryPool::PoolLargeMalloc() {
	if (_number_large_add_nodes == 0 || _large_size == 0) {
		throw std::exception("Large block of memory is not set!");
		return nullptr;
	}
	std::unique_lock<std::mutex> lock(_large_mutex);

	if (_free_large.find(_large_size) == _free_large.end()) {
		_free_large[_large_size] = nullptr;
	}

	MemNode** my_free = &_free_large[_large_size];
	MemNode* result = _free_large[_large_size];
	if (result == nullptr) {
		void* bytes = ReFill(_large_size, _number_large_add_nodes, true);
		memset(bytes, 0, _large_size);
		return (T*)bytes;
	}

	*my_free = result->_next;
	memset(result, 0, _large_size);
	return (T*)result;
}

template<typename T>
void CMemoryPool::PoolLargeFree(T* &m) {
	if (!m) {
		return;
	}

	if (_free_large.find(_large_size) == _free_large.end()) {
		throw std::exception("free_large map error!");
		return;
	}

	MemNode* node = (MemNode*)m;

	std::unique_lock<std::mutex> lock(_large_mutex);
	MemNode** my_free = &_free_large[_large_size];
	node->_next = *my_free;
	*my_free = node;
	m = nullptr;
}

template<typename T>
T* CMemoryPool::PoolLargeMalloc(int size, int& res) {
	if (_number_large_add_nodes == 0 || _large_size == 0) {
		throw std::exception("Large block of memory is not set!");
		return nullptr;
	}
	int large_size = RoundUp(size, _large_size);
	res = large_size;

	std::unique_lock<std::mutex> lock(_large_mutex);
	if (_free_large.find(large_size) == _free_large.end()) {
		_free_large[large_size] = nullptr;
	}
	MemNode** my_free = &_free_large[large_size];
	MemNode* result = _free_large[large_size];

	if (result == nullptr) {
		void* bytes = ReFill(large_size, _number_large_add_nodes, true);
		memset(bytes, 0, large_size);
		return (T*)bytes;
	}

	*my_free = result->_next;
	memset(result, 0, large_size);
	return (T*)result;
}

template<typename T>
void CMemoryPool::PoolLargeFree(T* &m, int size) {
	if (!m) {
		return;
	}

	int large_size = RoundUp(size, _large_size);
	if (_free_large.find(large_size) == _free_large.end()) {
		throw std::exception("free_large map error!");
		return;
	}

	MemNode* node = (MemNode*)m;

	std::unique_lock<std::mutex> lock(_large_mutex);
	MemNode** my_free = &_free_large[large_size];
	node->_next = *my_free;
	*my_free = node;
	m = nullptr;
}

CMemoryPool::CMemoryPool() {
	memset(_free_list, 0, sizeof(_free_list[__number_of_free_lists]));
	_create_thread_id = std::this_thread::get_id();
}

CMemoryPool::CMemoryPool(const int large_sz, const int add_num) : _large_size(RoundUp(large_sz)), _number_large_add_nodes(add_num) {
	memset(_free_list, 0, sizeof(_free_list[__number_of_free_lists]));
	_create_thread_id = std::this_thread::get_id();
}

CMemoryPool::~CMemoryPool() {
	assert(_create_thread_id == std::this_thread::get_id());
	for (auto iter = _malloc_vec.begin(); iter != _malloc_vec.end(); ++iter) {
		if (*iter) {
			free(*iter);
		}
	}
}

std::thread::id CMemoryPool::GetCreateThreadId() {
	return _create_thread_id;
}

int CMemoryPool::GetLargeSize() const {
	return _large_size;
}

void* CMemoryPool::ReFill(int size, int num, bool is_large) {
	int nums = num;

	char* chunk = (char*)ChunkAlloc(size, nums);
	MemNode** my_free;
	MemNode* res, *current, *next;
	if (1 == nums) {
		return chunk;
	}

	res = (MemNode*)chunk;

	if (is_large) {
		if (_free_large.find(size) == _free_large.end()) {
			_free_large[size] = nullptr;
		}
		my_free = &_free_large[size];

		*my_free = next = (MemNode*)(chunk + size);
		for (int i = 1;; i++) {
			current = next;
			next = (MemNode*)((char*)next + size);
			if (nums - 1 == i) {
				current->_next = nullptr;
				break;

			}
			else {
				current->_next = next;
			}
		}

	}
	else {
		my_free = &(_free_list[FreeListIndex(size)]);

		*my_free = next = (MemNode*)(chunk + size);
		for (int i = 1;; i++) {
			current = next;
			next = (MemNode*)((char*)next + size);
			if (nums - 1 == i) {
				current->_next = nullptr;
				break;

			}
			else {
				current->_next = next;
			}
		}
	}
	return res;
}

void* CMemoryPool::ChunkAlloc(int size, int& nums, bool is_large) {
	char* res;
	int need_bytes = size * nums;
	int left_bytes = _pool_end - _pool_start;

	//ÄÚ´æ³Ø¹»ÓÃ
	if (left_bytes >= need_bytes) {
		res = _pool_start;
		_pool_start += need_bytes;
		return res;

	}
	else if (left_bytes >= size) {
		nums = left_bytes / size;
		need_bytes = size * nums;
		res = _pool_start;
		_pool_start += need_bytes;
		return res;

	}
	int bytes_to_get = size * nums;

	if (!is_large) {
		if (left_bytes > 0) {
			MemNode* my_free = _free_list[FreeListIndex(left_bytes)];
			((MemNode*)_pool_start)->_next = my_free;
			_free_list[FreeListIndex(size)] = (MemNode*)_pool_start;
		}

	}
	else {
		free(_pool_start);
	}


	_pool_start = (char*)malloc(bytes_to_get);

	if (0 == _pool_start) {
		throw std::exception("There memary is not enough!");
	}

	_malloc_vec.push_back(_pool_start);
	_pool_end = _pool_start + bytes_to_get;
	return ChunkAlloc(size, nums, is_large);
}

}	// namespace readsense

#endif	// __RS_MMEMARY_H__