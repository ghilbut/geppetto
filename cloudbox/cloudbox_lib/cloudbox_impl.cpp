#include "cloudbox.h"
#include "cloudbox_impl.h"
#include "storage.h"
#include "sample.h"
#include "http.h"



CloudBox::CloudBox(const Storage& storage)
    : pimpl_(new CloudBox::Impl(storage)) {
}

CloudBox::~CloudBox(void) {
}

void CloudBox::RunShell(void) {
    pimpl_->RunShell();
}

CloudBox::Impl::Impl(const Storage& storage)
    : storage_(storage)
    , work_(new boost::asio::io_service::work(io_service_))
    , thread_(boost::bind(&boost::asio::io_service::run, &io_service_))
    , isolate_(v8::Isolate::GetCurrent())
    , handle_scope_(isolate_) {

    
    isolate_->SetData(0, &io_service_);
    
    v8::Isolate* isolate = isolate_;
    context_ = CreateShellContext(isolate);
    context_->Enter();





    BindSample(context_);
    Http::Bind(context_);





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

CloudBox::Impl::~Impl(void) {

    context_->Exit();
    v8::V8::Dispose();



    delete work_;
    io_service_.stop();
    thread_.join();
}

void CloudBox::Impl::RunShell(void) {
    ::RunShell(context_);
}