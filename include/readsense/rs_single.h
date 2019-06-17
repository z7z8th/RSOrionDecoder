#ifndef __RS_SINGLE_H__
#define __RS_SINGLE_H__

namespace readsense {

template<typename T>
class CSingle
{
private:
	CSingle(const CSingle&);
	CSingle& operator = (const CSingle&);
public:
	CSingle() {}
	virtual ~CSingle() {}

	static T& Instance() {
		static T instance;
		return instance;
	}
};

}

#endif	// __RS_SINGLE_H__