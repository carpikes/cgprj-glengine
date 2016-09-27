#ifndef GLENGINE_SINGLETON_H
#define GLENGINE_SINGLETON_H

namespace GLEngine {

template<typename T>
class Singleton {
public:
    // FIXME: completely NOT thread safe
    static T& instance() {
        static T theInstance;
        return theInstance;
    }
protected:
    Singleton() {};

private:
    Singleton(const Singleton&);
    Singleton(const Singleton&&);
    Singleton& operator=(const Singleton &);
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_SINGLETON_H */
