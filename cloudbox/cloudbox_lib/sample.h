#ifndef SAMPLE_H_
#define SAMPLE_H_

#include <v8.h>



void BindSample(v8::Handle<v8::Context> context);



v8::Handle<v8::Context> CreateShellContext(v8::Isolate* isolate);
void RunShell(v8::Handle<v8::Context> context);
int RunMain(v8::Isolate* isolate, int argc, char* argv[]);
bool ExecuteString(v8::Isolate* isolate,
    v8::Handle<v8::String> source,
    v8::Handle<v8::Value> name,
    bool print_result,
    bool report_exceptions);
void Print(const v8::FunctionCallbackInfo<v8::Value>& args);
void Read(const v8::FunctionCallbackInfo<v8::Value>& args);
void Load(const v8::FunctionCallbackInfo<v8::Value>& args);
void Quit(const v8::FunctionCallbackInfo<v8::Value>& args);
void Version(const v8::FunctionCallbackInfo<v8::Value>& args);
v8::Handle<v8::String> ReadFile(v8::Isolate* isolate, const char* name);
void ReportException(v8::Isolate* isolate, v8::TryCatch* handler);

#endif  // SAMPLE_H_