#ifndef __RS_C1X_STD_H__
#define __RS_C1X_STD_H__

#include <memory>

namespace c1x_std {

template<typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts&&...params)
{
return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

}

#endif	// __RS_C1X_H__