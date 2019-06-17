#ifndef SINGLETON_H
#define SINGLETON_H

#define DISABLE_COPY(Class) \
    Class(const Class &) = delete; \
    Class &operator=(const Class &) = delete;

#define DCLR_SINGLETON(Class) \
    public: \
        static Class* instance(); \
        static void exitInstance(); \
    private: \
        DISABLE_COPY(Class) \
        static Class* s_pInstance;

#define IMPL_SINGLETON(Class) \
    Class* Class::s_pInstance = NULL; \
    Class* Class::instance(){ \
        if (s_pInstance == NULL){ \
            s_pInstance = new Class; \
        } \
        return s_pInstance; \
    } \
    void Class::exitInstance(){ \
        if (s_pInstance){  \
            delete s_pInstance; \
            s_pInstance = NULL; \
        }   \
    }

#include <iostream>

class Singleton {
public:
	static Singleton *GetInstance();
private:
	Singleton() {}
	~Singleton() {}
	static Singleton *m_pInstance;
};

Singleton* Singleton::m_pInstance = new Singleton();

Singleton* Singleton::GetInstance()
{
	return m_pInstance;
}

#endif // SINGLETON_H
