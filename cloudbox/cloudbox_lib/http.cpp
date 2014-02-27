#include "http.h"
#include "http_server.h"
#include "http_request.h"


namespace Http {

class Object {
public:
    static void Bind(v8::Handle<v8::Context> context) {
        v8::Isolate* isolate = context->GetIsolate();

        v8::HandleScope handle_scope(isolate);
        v8::Handle<v8::ObjectTemplate> object_t = v8::ObjectTemplate::New(isolate);
        object_t->SetInternalFieldCount(1);
        v8::Handle<v8::Object> object = object_t->NewInstance();

        {
            v8::Handle<v8::FunctionTemplate> ft = v8::FunctionTemplate::New(isolate, Server::Constructor);
            object->Set(v8::String::NewFromUtf8(isolate, "Server"), ft->GetFunction());
        }

        {
            v8::Handle<v8::FunctionTemplate> ft = v8::FunctionTemplate::New(isolate, Request::Constructor);
            object->Set(v8::String::NewFromUtf8(isolate, "Request"), ft->GetFunction());
        }

        Object* http = new Object();
        object->SetInternalField(0, v8::External::New(isolate, http));
        context->Global()->Set(v8::String::NewFromUtf8(isolate, "http"), object);
    }

private:
    Object(void) {};
    ~Object(void) {};
};

void Bind(v8::Handle<v8::Context> context) {
    Object::Bind(context);
}

}  // namespace Http