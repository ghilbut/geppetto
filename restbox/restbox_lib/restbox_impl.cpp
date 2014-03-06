#include "restbox.h"
#include "restbox_impl.h"
#include "storage.h"
#include "sample.h"
#include "http.h"



RestBox::RestBox(const Storage& storage)
    : pimpl_(new RestBox::Impl(storage)) {
}

RestBox::~RestBox(void) {
}

void RestBox::RunShell(void) {
    pimpl_->RunShell();
}

RestBox::Impl::Impl(const Storage& storage)
    : storage_(storage)
    , work_(new boost::asio::io_service::work(io_service_))
    , thread_(boost::bind(&boost::asio::io_service::run, &io_service_))
    , isolate_(v8::Isolate::GetCurrent())
    , handle_scope_(isolate_) {



    v8::Isolate* isolate = isolate_;
    isolate->SetData(0, &io_service_);



    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(v8::String::NewFromUtf8(isolate, "print"), v8::FunctionTemplate::New(isolate, Print));
    global->Set(v8::String::NewFromUtf8(isolate, "read"), v8::FunctionTemplate::New(isolate, Read));
    global->Set(v8::String::NewFromUtf8(isolate, "load"), v8::FunctionTemplate::New(isolate, Load));
    global->Set(v8::String::NewFromUtf8(isolate, "quit"), v8::FunctionTemplate::New(isolate, Quit));
    global->Set(v8::String::NewFromUtf8(isolate, "version"), v8::FunctionTemplate::New(isolate, Version));



    global->Set(
        v8::String::NewFromUtf8(isolate, "Http")
        , Http::Template::New(isolate)
        , Http::kAttribute);



    context_ = v8::Context::New(isolate, NULL, global);
    //context_ = CreateShellContext(isolate);
    context_->Enter();



    BindSample(context_);
    {
        v8::HandleScope handle_scope(context_->GetIsolate());
        context_->Global()->Set(
            v8::String::NewFromUtf8(isolate, "http")
            , Http::NewInstance(isolate)
            , Http::kAttribute);
    }



    v8::Handle<v8::String> source = ReadFile(isolate, storage_.settings());
    if (source.IsEmpty()) {
        //return -1;
    }
    if (!ExecuteString(isolate,
        source,
        v8::String::NewFromUtf8(isolate, storage_.settings()),
        false,
        false)) {
            //return -1;
    }
}

RestBox::Impl::~Impl(void) {

    context_->Exit();
    v8::V8::Dispose();



    delete work_;
    io_service_.stop();
    thread_.join();
}

void RestBox::Impl::RunShell(void) {
    ::RunShell(context_);
}