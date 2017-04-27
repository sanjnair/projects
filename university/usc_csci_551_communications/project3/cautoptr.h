#ifndef CAUTOPTR_H
#define CAUTOPTR_H

#include "cexception.h"

template <typename T>
class CAutoPtr
{
public:
    explicit CAutoPtr();
    explicit CAutoPtr(T *p);
    CAutoPtr(const CAutoPtr<T> &rhs);
    ~CAutoPtr();

    CAutoPtr<T> &operator =(const CAutoPtr<T> &rhs);
    CAutoPtr<T> &operator = (T *rhs);
    operator T *() const;
    T &operator *() const; 
    T *operator ->() const; 
    
    bool isNull() const;
    const T *get() const;
    T *get();
    T *release();
    void reset(T *p = 0);
private:
    void errorIfInvalidPtr() const;
    T *_ptr;
};

/*!
    Constructs the Autopointer with Null pointer
*/
template <typename T>
CAutoPtr<T>::CAutoPtr() : _ptr(0) {}

/*!
    Constructs the Autopointer with specified pointer
*/
template <typename T>
CAutoPtr<T>::CAutoPtr(T *p) : _ptr(p) {}

/*!
    Copy Constructor
*/
template <typename T>
CAutoPtr<T>::CAutoPtr(const CAutoPtr<T> &rhs)
{
    CAutoPtr<T> *r = const_cast<CAutoPtr<T>* >(&rhs);
    reset(r->release());
}

/*!
    Destroys auto pointer
*/
template <typename T>
CAutoPtr<T>::~CAutoPtr()
{
    reset();
}

/*!
    Assignment operator
*/
template <typename T>
CAutoPtr<T>& CAutoPtr<T>::operator =(const CAutoPtr<T> &rhs)
{
    if (this != &rhs) {
        CAutoPtr<T> *r = const_cast<CAutoPtr<T>* >(&rhs);
        reset(r->release());
    }
    return *this;
}

/*!
    Assignment operator
*/
template <typename T>
CAutoPtr<T>& CAutoPtr<T>::operator =(T *rhs)
{
    if (_ptr != rhs) {
        reset(rhs);
    }
    return *this;
}

/*!
    \override
*/
template <typename T>
CAutoPtr<T>::operator T *() const
{
    errorIfInvalidPtr();
    return static_cast<T*>(const_cast<T*>(_ptr));
}

/*!
    Assignment operator
*/
template <typename T>
T& CAutoPtr<T>::operator *() const
{
    errorIfInvalidPtr();
    return *static_cast<T*>(const_cast<T*>(_ptr));
}

/*!
    Assignment operator
*/
template <typename T>
T* CAutoPtr<T>::operator ->() const
{
    errorIfInvalidPtr();
    return static_cast<T*>(const_cast<T*>(_ptr));
}

/*!
    Returns true if the pointer is null
*/
template <typename T>
bool CAutoPtr<T>::isNull() const
{
    return (0 == _ptr) ? true : false;
}

/*!
    Returns the pointer
*/
template <typename T>
T* CAutoPtr<T>::get()
{
    return _ptr;
}

/*!
    Returns the pointer
*/
template <typename T>
const T* CAutoPtr<T>::get() const
{
    return _ptr;
}


/*!
    Releases the pointer and returns the pointer.
    \Attention It is the user's responsiblity to delete this pointer.
    Otherwise it will result in memory leak.
*/
template <typename T>
T* CAutoPtr<T>::release()
{
    T *t = _ptr;
    _ptr = 0;
    return t;
}

template <typename T>
void CAutoPtr<T>::reset(T *p)
{
    if (_ptr != p) {
        if (_ptr) {
            delete _ptr;
        }
        _ptr = p;
    }
}

/*!
    Throws exception if the pointer is null
*/
template <typename T>
void CAutoPtr<T>::errorIfInvalidPtr() const
{
    if (! _ptr) {
        C_THROW("Invalid (NULL) pointer");
    }
}

#endif //CAUTOPTR_H
