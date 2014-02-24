#include "js_http.h"
#include <boost/atomic.hpp>


namespace JS {

void Http::Bind(v8::Handle<v8::Context> context) {

    v8::Isolate* isolate = context->GetIsolate();
    v8::Handle<v8::ObjectTemplate> object_t = MakeObjectTemplate(context);

    Http* p = new Http();
    v8::Handle<v8::Object> object = object_t->NewInstance();
    object->SetInternalField(0, v8::External::New(isolate, p));
    const char* name_person = "Person";
    context->Global()->Set(v8::String::NewFromOneByte(isolate, (const uint8_t*)name_person), object);
}

v8::Handle<v8::ObjectTemplate> Http::MakeObjectTemplate(v8::Handle<v8::Context> context) {

    v8::Isolate* isolate = context->GetIsolate();
    v8::Handle<v8::ObjectTemplate> object_t = v8::ObjectTemplate::New(isolate);
    object_t->SetInternalFieldCount(1);
    object_t->SetAccessor(v8::String::NewFromUtf8(isolate, "onrequest"), Http::GetEventRequest, Http::SetEventRequest);
    object_t->SetAccessor(v8::String::NewFromUtf8(isolate, "onmessage"), Http::GetEventMessage, Http::SetEventMessage);
    //object_t->Set(v8::String::NewFromUtf8(isolate, ""), v8::FunctionTemplate::New(isolate, Person::Load));
    return object_t;
}

Http* Http::Unwrap(v8::Handle<v8::Object> object) {
    v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    void* ptr = wrap->Value();
    return static_cast<Http*>(ptr);
}

void Http::GetEventRequest(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> self = info.Holder();
    Http* p = Unwrap(self);
    info.GetReturnValue().Set(p->on_request_);
}

void Http::SetEventRequest(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::Local<v8::Object> self = info.Holder();
    Http* p = Unwrap(self);

    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        (p->on_request_).Reset(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }

    info.GetReturnValue().Set(value);
}

void Http::GetEventMessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> self = info.Holder();
    Http* p = Unwrap(self);
    info.GetReturnValue().Set(p->on_message_);
}

void Http::SetEventMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::Local<v8::Object> self = info.Holder();
    Http* p = Unwrap(self);

    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        (p->on_message_).Reset(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }

    info.GetReturnValue().Set(value);
}

void Http::FireRequest(const v8::FunctionCallbackInfo<v8::Value>& args) {

    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    v8::Local<v8::Object> self = args.Holder();
    Http* p = Unwrap(self);

    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate, p->on_request_);

    if (func.IsEmpty()) {
        return;
    }

    v8::Handle<v8::Value> params[2];
    params[0] = v8::Integer::New(isolate, 1);
    params[1] = v8::Integer::New(isolate, 2);

    v8::Local<v8::Object> global = context->Global();
    v8::Handle<v8::Value> js_result = func->Call(global, 2, params);

    if (js_result->IsInt32()) {
        int32_t result = (js_result->ToInt32())->Value();
        // do something with the result
        args.GetReturnValue().Set(v8::Integer::New(isolate, result));
    }
}

void Http::FireMessage(const v8::FunctionCallbackInfo<v8::Value>& args) {

    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    v8::Local<v8::Object> self = args.Holder();
    Http* p = Unwrap(self);

    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate, p->on_message_);

    if (func.IsEmpty()) {
        return;
    }

    v8::Handle<v8::Value> params[2];
    params[0] = v8::Integer::New(isolate, 1);
    params[1] = v8::Integer::New(isolate, 2);

    v8::Local<v8::Object> global = context->Global();
    v8::Handle<v8::Value> js_result = func->Call(global, 2, params);

    if (js_result->IsInt32()) {
        int32_t result = (js_result->ToInt32())->Value();
        // do something with the result
        args.GetReturnValue().Set(v8::Integer::New(isolate, result));
    }
}

}  // namespace JS