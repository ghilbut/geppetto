#include "http_server.h"
#include "http_server_template.h"


namespace Http {

v8::Persistent<v8::FunctionTemplate> ServerTemplate::template_;

v8::Local<v8::FunctionTemplate> ServerTemplate::Get(v8::Isolate* isolate) {
    v8::Local<v8::FunctionTemplate> ft;
    if (template_.IsEmpty()) {
        ft = v8::FunctionTemplate::New(isolate, Constructor);
        ft->Set(isolate, "listen", v8::FunctionTemplate::New(isolate, ServerTemplate::Listen));
        ft->Set(isolate, "close", v8::FunctionTemplate::New(isolate, ServerTemplate::Close));

        v8::Local<v8::ObjectTemplate> ot = ft->InstanceTemplate();
        ot->Set(v8::String::NewFromUtf8(isolate, "listen"), v8::FunctionTemplate::New(isolate, ServerTemplate::Listen));
        ot->Set(v8::String::NewFromUtf8(isolate, "close"), v8::FunctionTemplate::New(isolate, ServerTemplate::Close));
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onrequest"), ServerTemplate::GetEventRequest, ServerTemplate::SetEventRequest);
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onmessage"), ServerTemplate::GetEventMessage, ServerTemplate::SetEventMessage);
        ot->SetInternalFieldCount(1);

        template_.Reset(isolate, ft);
    } else {
        ft = v8::Local<v8::FunctionTemplate>::New(isolate, template_);
    }
    return ft;
}

void ServerTemplate::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    if (!args.IsConstructCall()) {
        const char* msg = "Failed to construct 'http.Server': Please use the 'new' operator, this DOM object constructor cannot be called as a function.";
        v8::Local<v8::Value> e = v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, msg));
        args.GetReturnValue().Set(isolate->ThrowException(e));
        return;
    }

    boost::asio::io_service* io_service = static_cast<boost::asio::io_service*>(isolate->GetData(0));
    Server* server = new Server(isolate, *io_service);
   
    v8::Local<v8::Object> object = args.Holder();
    server->MakeWeak(isolate, object);
    args.GetReturnValue().Set(object);
}

template<typename T>
Server* ServerTemplate::Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    //v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    //void* ptr = wrap->Value();
    //return static_cast<Server*>(ptr);
    return static_cast<Server*>(object->GetAlignedPointerFromInternalField(0));
}

void ServerTemplate::GetEventRequest(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Server* s = Unwrap(info);
    info.GetReturnValue().Set(s->request_trigger(info.GetIsolate()));
}

void ServerTemplate::SetEventRequest(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        Unwrap(info)->set_request_trigger(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }
}

void ServerTemplate::GetEventMessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Server* s = Unwrap(info);
    info.GetReturnValue().Set(s->message_trigger(info.GetIsolate()));
}

void ServerTemplate::SetEventMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        Unwrap(info)->set_message_trigger(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }
}

void ServerTemplate::GetEventError(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Server* s = Unwrap(info);
    info.GetReturnValue().Set(s->error_trigger(info.GetIsolate()));
}

void ServerTemplate::SetEventError(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        Unwrap(info)->set_error_trigger(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }
}

void ServerTemplate::Listen(const v8::FunctionCallbackInfo<v8::Value>& args) {
    static const char* err_msg_prefix = "Failed to execute 'listen' on 'http.Server': ";
    char error_msg[1024];

    v8::Isolate* isolate = args.GetIsolate();

    if (args.Length() < 1) {
        sprintf(error_msg, "%s 1 arguments required, but only %d present.", err_msg_prefix, args.Length());
        v8::Local<v8::Value> e = v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, error_msg));
        args.GetReturnValue().Set(isolate->ThrowException(e));
        return;
    }

    v8::Handle<v8::Value> param0 = args[0];
    if (!param0->IsNumber() || !param0->IsUint32()) {
        sprintf(error_msg, "%s '%d' is not a valid port number.", err_msg_prefix, param0->ToString());
        v8::Local<v8::Value> e = v8::Exception::SyntaxError(v8::String::NewFromUtf8(isolate, error_msg));
        args.GetReturnValue().Set(isolate->ThrowException(e));
        return;
    }
    const uint32_t port = param0->Uint32Value();
    if (port > 65535) {
        sprintf(error_msg, "%s '%u' is not a valid port number.", err_msg_prefix, port);
        v8::Local<v8::Value> e = v8::Exception::SyntaxError(v8::String::NewFromUtf8(isolate, error_msg));
        args.GetReturnValue().Set(isolate->ThrowException(e));
        return;
    }

    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    Server* s = Unwrap(args);
    bool result = s->DoListen(static_cast<uint16_t>(port));
    args.GetReturnValue().Set(v8::Boolean::New(isolate, result));
}

void ServerTemplate::Close(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    Server* s = Unwrap(args);
    s->DoClose();
}

}  // namespace Http