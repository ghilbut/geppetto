#include "http_server.h"
#include "http_server_wrapper.h"


namespace Http {

v8::Persistent<v8::ObjectTemplate> Server::Wrapper::template_;

void Server::Wrapper::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
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
        object_t->Set(v8::String::NewFromUtf8(isolate, "listen"), v8::FunctionTemplate::New(isolate, Server::Wrapper::Listen));
        object_t->Set(v8::String::NewFromUtf8(isolate, "close"), v8::FunctionTemplate::New(isolate, Server::Wrapper::Close));
        object_t->SetAccessor(v8::String::NewFromUtf8(isolate, "onrequest"), Server::Wrapper::GetEventRequest, Server::Wrapper::SetEventRequest);
        object_t->SetAccessor(v8::String::NewFromUtf8(isolate, "onmessage"), Server::Wrapper::GetEventMessage, Server::Wrapper::SetEventMessage);
        template_.Reset(isolate, object_t);
    } else {
        object_t = v8::Local<v8::ObjectTemplate>::New(isolate, template_);
    }

    boost::asio::io_service* io_service = static_cast<boost::asio::io_service*>(isolate->GetData(0));

    Server* server = new Server(isolate, *io_service);
    v8::Handle<v8::Object> object = object_t->NewInstance();
    object->SetInternalField(0, v8::External::New(isolate, server));
    server->object_.Reset(isolate, object);

    args.GetReturnValue().Set(object);
}

template<typename T>
Server* Server::Wrapper::Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    void* ptr = wrap->Value();
    return static_cast<Server*>(ptr);
}

void Server::Wrapper::GetEventRequest(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Server* s = Unwrap(info);
    info.GetReturnValue().Set(s->on_request_);
}

void Server::Wrapper::SetEventRequest(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        (Unwrap(info)->on_request_).Reset(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }
}

void Server::Wrapper::GetEventMessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Server* s = Unwrap(info);
    info.GetReturnValue().Set(s->on_message_);
}

void Server::Wrapper::SetEventMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        (Unwrap(info)->on_message_).Reset(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }
}

void Server::Wrapper::GetEventError(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Server* s = Unwrap(info);
    info.GetReturnValue().Set(s->on_error_);
}

void Server::Wrapper::SetEventError(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        (Unwrap(info)->on_error_).Reset(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }
}

void Server::Wrapper::Listen(const v8::FunctionCallbackInfo<v8::Value>& args) {
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

    Server* s = Unwrap(args);
    bool result = s->DoListen(document_root, static_cast<uint16_t>(port));
    args.GetReturnValue().Set(v8::Boolean::New(isolate, result));
}

void Server::Wrapper::Close(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    //Ext::Http::Server& s = Unwrap(args)->server();
    //s.Close();
    Server* s = Unwrap(args);
    s->DoClose();
}


}  // namespace Http