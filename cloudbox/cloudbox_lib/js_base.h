#ifndef JS_BASE_H_
#define JS_BASE_H_

#include <v8.h>


namespace JS {

template<typename T>
T* Unwrap(v8::Handle<v8::Object> object) {
    v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    void* ptr = wrap->Value();
    return static_cast<T*>(ptr);
}

}  // namespace JS

#endif  // JS_BASE_H_