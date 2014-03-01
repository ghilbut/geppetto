#ifndef HTTP_H_
#define HTTP_H_

#include <v8.h>


namespace Http {


class Template {
public:
    static v8::Local<v8::FunctionTemplate> New(v8::Isolate* isolate);
    static v8::Local<v8::FunctionTemplate> Get(v8::Isolate* isolate);

private:
    static v8::Persistent<v8::FunctionTemplate> template_;
    Template(void) {}
};

const v8::PropertyAttribute kAttribute = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
v8::Local<v8::Object> NewInstance(v8::Isolate* isolate);


}  // namespace Http

#endif  // HTTP_H_