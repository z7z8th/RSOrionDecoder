#ifndef SINGLETON_H
#define SINGLETON_H

#define DISABLE_COPY(Class) \
	private: \
		Class() {} \
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

#endif // SINGLETON_H
