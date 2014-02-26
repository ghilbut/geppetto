#include "http_server.h"
#include "dummy.h"
#include <mongoose.h>


namespace Http {


void Server::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Server::Wrapper::Constructor(args);
}

void Server::DoListen(const std::string& document_root, uint16_t port) {
    io_service_.post(boost::bind(&Server::handle_listen, this, document_root, port));
}

void Server::DoClose(void) {
    io_service_.post(boost::bind(&Server::handle_close, this));
}

void Server::FireRequest(const v8::FunctionCallbackInfo<v8::Value>& args) {
    io_service_.post(boost::bind(&Server::handle_request, this, args));
}

void Server::FireMessage(const v8::FunctionCallbackInfo<v8::Value>& args) {
    io_service_.post(boost::bind(&Server::handle_message, this, args));
}

void Server::FireError(const v8::FunctionCallbackInfo<v8::Value>& args) {
    io_service_.post(boost::bind(&Server::handle_error, this, args));
}



Server::Server(boost::asio::io_service& io_service)
    : base_(*this), io_service_(io_service) {
        // nothing
}

Server::~Server(void) {
    //onload_.Dispose();
}

void Server::handle_listen(const std::string& document_root, uint16_t port) {
    base_.Listen(document_root, port);
}

void Server::handle_close() {
    base_.Close();
}

void Server::handle_request(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    Server* s = Wrapper::Unwrap(args);

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

void Server::handle_message(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    Server* s = Wrapper::Unwrap(args);

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

void Server::handle_error(const v8::FunctionCallbackInfo<v8::Value>& args) {

}



Server::Base::Base(Server& server)
    : server_(server)
    , is_alive_(false)
    , is_stop_(false) {
    // nothing
}

Server::Base::~Base() {
    Close();
}

bool Server::Base::Listen(const std::string& document_root, uint16_t port) {
    if (is_alive_) {
        return false;
    }

    is_alive_ = true;
    is_stop_ = false;
    document_root_ = document_root;
    port_ = port;

    thread_.swap(boost::thread(boost::bind(&Server::Base::thread_main, this)));
    return true;
}

void Server::Base::Close(void) {
    if (!is_alive_) {
        return;
    }
    is_stop_ = true;
    thread_.join();
}

int Server::Base::request_handler(struct mg_connection *conn) {
    mg_printf_data(conn, "Hello! Requested URI is [%s]", conn->uri);

    /*Server* s = static_cast<Server*>(conn->server_param);

    if (conn->is_websocket) {
        s->FireOnMessage(conn);
    } else {
        s->FireOnRequest(conn);
    }*/


    return MG_REQUEST_PROCESSED;
}

void Server::Base::thread_main(void) {

    mg_server* server = mg_create_server(this);
    if (server == 0) {
        goto ERROR0;
    }

    const char* error_msg = 0;
    //error_msg = mg_set_option(server, "document_root", document_root_.c_str());
    if (error_msg) {
        // TODO(ghilbut): delegate error
        goto ERROR1;
    }

    char sport[10];  
    sprintf(sport, "%d", port_);
    error_msg = mg_set_option(server, "listening_port", sport);
    if (error_msg) {
        // TODO(ghilbut): delegate error
        goto ERROR1;
    }

    mg_set_request_handler(server, &Server::Base::request_handler);
    while(is_stop_ == false) {
        if (mg_poll_server(server, 1000) == 0) {
            // TODO(ghilbut): delegate error
            is_stop_ = true;
            goto ERROR1;
        }
    }

ERROR1:
    mg_destroy_server(&server);
ERROR0:
    is_alive_ = false;
}


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

    Server* server = new Server(*io_service);
    v8::Handle<v8::Object> object = object_t->NewInstance();
    object->SetInternalField(0, v8::External::New(isolate, server));

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
    Server* s = Unwrap(info);

    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        (s->on_request_).Reset(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }

    info.GetReturnValue().Set(value);
}

void Server::Wrapper::GetEventMessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Server* s = Unwrap(info);
    info.GetReturnValue().Set(s->on_message_);
}

void Server::Wrapper::SetEventMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    Server* s = Unwrap(info);

    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        (s->on_message_).Reset(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }

    info.GetReturnValue().Set(value);
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
    //bool result = s.Listen(document_root, static_cast<uint16_t>(port));
    //args.GetReturnValue().Set(v8::Boolean::New(isolate, result));
    s->DoListen(document_root, static_cast<uint16_t>(port));
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