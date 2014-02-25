#include "js_http_server.h"
#include "ext/dummy.h"


namespace JS {
namespace Http {

 
static Server* Unwrap(v8::Handle<v8::Object> object) {
    v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    void* ptr = wrap->Value();
    return static_cast<Server*>(ptr);
}

template<typename T>
Server* GetServer(T _t) {
    v8::Local<v8::Object> self = _t.Holder();
    return Unwrap(self);
}



Server::Server(void) {
    server_.BindDelegate(this);
}

Server::~Server(void) {
    server_.UnbindDelegate();
    //onload_.Dispose();
}


v8::Persistent<v8::ObjectTemplate> Server::template_;

void Server::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    if (!args.IsConstructCall()) {
        v8::Handle<v8::Value> e = isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Cannot call constructor as function."));
        args.GetReturnValue().Set(e);
        return;
    }

    if (args.Length() > 0) {
        v8::Handle<v8::Value> e = isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Cannot call constructor with any parameter(s)."));
        args.GetReturnValue().Set(e);
        return;
    }

    v8::HandleScope handle_scope(isolate);
    v8::Handle<v8::ObjectTemplate> object_t;
    if (template_.IsEmpty()) {
        object_t = v8::ObjectTemplate::New(isolate);
        object_t->SetInternalFieldCount(1);
        object_t->Set(v8::String::NewFromUtf8(isolate, "listen"), v8::FunctionTemplate::New(isolate, Server::Listen));
        object_t->Set(v8::String::NewFromUtf8(isolate, "close"), v8::FunctionTemplate::New(isolate, Server::Close));
        object_t->SetAccessor(v8::String::NewFromUtf8(isolate, "onrequest"), Server::GetEventRequest, Server::SetEventRequest);
        object_t->SetAccessor(v8::String::NewFromUtf8(isolate, "onmessage"), Server::GetEventMessage, Server::SetEventMessage);
        //object_t->Set(v8::String::NewFromUtf8(isolate, ""), v8::FunctionTemplate::New(isolate, Person::Load));
        template_.Reset(isolate, object_t);
    } else {
        object_t = v8::Local<v8::ObjectTemplate>::New(isolate, template_);
    }

    Server* server = new Server();
    v8::Handle<v8::Object> object = object_t->NewInstance();
    object->SetInternalField(0, v8::External::New(isolate, server));

    args.GetReturnValue().Set(object);
}

Ext::Http::Server Server::server(void) {
    return server_;
}

void Server::Listen(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    if (args.Length() < 2) {
        const char* text = "Invalid usgae.";
        args.GetReturnValue().Set(v8::Exception::SyntaxError(v8::String::NewFromUtf8(isolate, text)));
        return;
    }

    v8::Handle<v8::Value> param0 = args[0];
    if (!param0->IsString()) {
        const char* text = "Document root should be string.";
        args.GetReturnValue().Set(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, text)));
        return;
    }
    const v8::String::Utf8Value utf8(param0); 
    const std::string document_root = (utf8.length() > 0 ? *utf8 : "");
    // TODO(ghilbut): check path is exists

    v8::Handle<v8::Value> param1 = args[1];
    if (!param1->IsNumber() || !param1->IsUint32()) {
        const char* text = "Port should be a positive number.";
        args.GetReturnValue().Set(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, text)));
        return;
    }
    const uint32_t port = param1->Uint32Value();
    if (port > 65535) {
        const char* text = "Max port number is 65535.";
        args.GetReturnValue().Set(v8::Exception::RangeError(v8::String::NewFromUtf8(isolate, text)));
        return;
    }

    Ext::Http::Server& s = GetServer(args)->server();
    bool result = s.Listen(document_root, static_cast<uint16_t>(port));
    args.GetReturnValue().Set(v8::Boolean::New(isolate, result));
}

void Server::Close(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    Ext::Http::Server& s = GetServer(args)->server();
    s.Close();
}

void Server::GetEventRequest(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Ext::Http::Server& s = GetServer(info)->server();
}

void Server::SetEventRequest(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    Server* s = GetServer(info);

    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        (s->on_request_).Reset(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }

    info.GetReturnValue().Set(value);
}

void Server::GetEventMessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Server* s = GetServer(info);
    info.GetReturnValue().Set(s->on_message_);
}

void Server::SetEventMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    Server* s = GetServer(info);

    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        (s->on_message_).Reset(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }

    info.GetReturnValue().Set(value);
}

void Server::FireRequest(const v8::FunctionCallbackInfo<v8::Value>& args) {

    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    Server* s = GetServer(args);

    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate, s->on_request_);

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

void Server::FireMessage(const v8::FunctionCallbackInfo<v8::Value>& args) {

    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    Server* s = GetServer(args);

    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate, s->on_message_);

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

Ext::Http::Response& Server::OnRequest(const Ext::Http::Request& request) {
    Ext::Http::Response res;
    return res;
}

void Server::OnMessage(const Ext::Http::Websocket& sock, const Ext::Http::Message& msg) {

}

void Server::OnError(const Ext::Http::Error& error) {

}

}  // namespace Http
}  // namespace JS