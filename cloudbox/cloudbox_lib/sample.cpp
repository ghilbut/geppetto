#include "sample.h"

#include "dummy.h"
#include <assert.h>

#include <map>
#include <vector>
#include <string>



// [Save javascript function and callback]

class Person {
public:
    Person(void) { }
    ~Person(void) {
        //onload_.Dispose();
    }

    static Person* Unwrap(v8::Handle<v8::Object> object);
    static void GetOnLoad(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetOnLoad(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

    static void Load(const v8::FunctionCallbackInfo<v8::Value>& args);

    static v8::Handle<v8::ObjectTemplate> MakeObjectTemplate(v8::Handle<v8::Context> context);

    v8::Persistent<v8::Function> onload_;
};

Person* Person::Unwrap(v8::Handle<v8::Object> object) {
    v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    void* ptr = wrap->Value();
    return static_cast<Person*>(ptr);
}

void Person::GetOnLoad(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> self = info.Holder();
    Person* p = Unwrap(self);
    info.GetReturnValue().Set(p->onload_);
}

void Person::SetOnLoad(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::Local<v8::Object> self = info.Holder();
    Person* p = Unwrap(self);

    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        (p->onload_).Reset(info.GetIsolate(), func);
    }

    info.GetReturnValue().Set(value);
}

void Person::Load(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    v8::Local<v8::Object> self = args.Holder();
    Person* p = Unwrap(self);

    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate, p->onload_);

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


// [Constructor without C++ class]
/*
class Element {
public:
    explicit Element(const char* id) 
        : id_(id) {
    }

    std::string id_;
};
*/
v8::Persistent<v8::ObjectTemplate> element_t;

void ElementContructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    if (!args.IsConstructCall()) {
        v8::Handle<v8::Value> e = isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Cannot call constructor as function."));
        args.GetReturnValue().Set(e);
        return;
    }

    if (!args[0]->IsString()) {
        v8::Handle<v8::Value> e = isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Cannot call constructor with non-string value."));
        args.GetReturnValue().Set(e);
        return;
    }

    v8::HandleScope handle_scope(isolate);
    v8::Handle<v8::ObjectTemplate> object_t;
    if (element_t.IsEmpty()) {
        object_t = v8::ObjectTemplate::New(isolate);
    } else {
        object_t = v8::Local<v8::ObjectTemplate>::New(isolate, element_t);
    }
    v8::Handle<v8::Object> object = object_t->NewInstance();
    object->Set(v8::String::NewFromUtf8(isolate, "id"), args[0]->ToString());

    args.GetReturnValue().Set(object);
}


// [Constructor with C++ class]

class Resource {
public:
    explicit Resource(const char* id) 
        : id_(id) {
    }

    static Resource* Unwrap(v8::Handle<v8::Object> object);
    static void GetID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

    std::string id_;
};

Resource* Resource::Unwrap(v8::Handle<v8::Object> object) {
    v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    void* ptr = wrap->Value();
    return static_cast<Resource*>(ptr);
}

void Resource::GetID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> self = info.Holder();
    Resource* r = Unwrap(self);
    info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), (r->id_).c_str()));
}

void Resource::SetID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::Local<v8::Object> self = info.Holder();
    Resource* r = Unwrap(self);
    v8::String::Utf8Value utf8(value);
    r->id_ = *utf8;
}

v8::Persistent<v8::ObjectTemplate> resource_t;

void ResourceContructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    if (!args.IsConstructCall()) {
        //return ThrowException(v8::String::NewFromUtf8(isolate, "Cannot call constructor as function."));
        v8::Handle<v8::Value> e = isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Cannot call constructor as function."));
        args.GetReturnValue().Set(e);
        return;
    }

    if (!args[0]->IsString()) {
        //return ThrowException(v8::String::NewFromUtf8(isolate, "Cannot call constructor with non-string value."));
        v8::Handle<v8::Value> e = isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Cannot call constructor with non-string value."));
        args.GetReturnValue().Set(e);
        return;
    }

    v8::String::Utf8Value utf8(args[0]);
    Resource* r = new Resource(*utf8);

    v8::HandleScope handle_scope(isolate);
    v8::Handle<v8::ObjectTemplate> object_t;
    if (resource_t.IsEmpty()) {
        object_t = v8::ObjectTemplate::New(isolate);
        object_t->SetInternalFieldCount(1);
        object_t->SetAccessor(v8::String::NewFromUtf8(isolate, "id"), Resource::GetID, Resource::SetID);
        resource_t.Reset(args.GetIsolate(), object_t);
    } else {
        object_t = v8::Local<v8::ObjectTemplate>::New(isolate, resource_t);
    }
    v8::Handle<v8::Object> object = object_t->NewInstance();
    object->SetInternalField(0, v8::External::New(isolate, r));

    args.GetReturnValue().Set(object);
}


// [Interceptor]

typedef std::map<std::string, std::string> Dictionary;
typedef std::vector<std::string> Array;


Dictionary* UnwrapDictionary(v8::Handle<v8::Object> object) {
    v8::Handle<v8::External> field = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    void* ptr = field->Value();
    return static_cast<Dictionary*>(ptr);
}

Array* UnwrapArray(v8::Handle<v8::Object> object) {
    v8::Handle<v8::External> field = v8::Handle<v8::External>::Cast(object->GetInternalField(1));
    void* ptr = field->Value();
    return static_cast<Array*>(ptr);
}

void DictionaryGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();

    Dictionary* dic = UnwrapDictionary(info.Holder());

    v8::String::Utf8Value key(property);
    Dictionary::iterator itr = dic->find(*key);
    if (itr == dic->end()) {
        info.GetReturnValue().Set(v8::Undefined(isolate));
        return;
    }
    const std::string& value = itr->second;
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, value.c_str()));
}

void DictionarySetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();

    Dictionary* dic = UnwrapDictionary(info.Holder());

    if (!value->IsString()) {
        info.GetReturnValue().Set(v8::Undefined(isolate));
        return;
    }

    v8::String::Utf8Value key(property);
    v8::String::Utf8Value val(v8::Handle<v8::String>::Cast(value));
    (*dic)[*key] = *val;

    info.GetReturnValue().Set(value);
}

void ArrayGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();

    Array* array = UnwrapArray(info.Holder());

    if (index >= array->size()) {
        info.GetReturnValue().Set(v8::Undefined(isolate));
        return;
    }

    const std::string& str = (*array)[index];
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, str.c_str()));
}

void ArraySetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();

    Array* array = UnwrapArray(info.Holder());

    if (!value->IsString()) {
        info.GetReturnValue().Set(v8::Undefined(isolate));
        return;
    }

    v8::String::Utf8Value val(v8::Handle<v8::String>::Cast(value));
    const std::size_t size = array->size();
    if (index < size) {
        (*array)[index] = *val;
    } else if (index == size) {
        array->push_back(*val);
    } else {
        // ?
    }

    // TODO: on chrome browser console
    // array setter return array(itself)
    info.GetReturnValue().Set(v8::Undefined(isolate));
}



void BindSample(v8::Handle<v8::Context> context) {
    v8::Isolate* isolate = context->GetIsolate();

    // [Save javascript function and callback]
    {
        Person* p = new Person();

        v8::Handle<v8::ObjectTemplate> object_t = v8::ObjectTemplate::New(isolate);
        object_t->SetInternalFieldCount(1);
        const char* name_onload = "onload";
        const char* name_load = "load";
        object_t->SetAccessor(v8::String::NewFromOneByte(isolate, (const uint8_t*)name_onload), Person::GetOnLoad, Person::SetOnLoad);
        object_t->Set(v8::String::NewFromOneByte(isolate, (const uint8_t*)name_load), v8::FunctionTemplate::New(isolate, Person::Load));

        v8::Handle<v8::Object> object = object_t->NewInstance();
        object->SetInternalField(0, v8::External::New(isolate, p));
        const char* name_person = "Person";
        context->Global()->Set(v8::String::NewFromOneByte(isolate, (const uint8_t*)name_person), object);
    }

    // [Constructor without C++ class]
    {
        v8::HandleScope handle_scope(isolate);
        v8::Handle<v8::FunctionTemplate> constructor_t = v8::FunctionTemplate::New(isolate, ElementContructor);
        v8::Handle<v8::Function> constructor_f = constructor_t->GetFunction();
        context->Global()->Set(v8::String::NewFromUtf8(isolate, "Element"), constructor_f);
    }

    // [Constructor with C++ class]
    {
        v8::HandleScope handle_scope(isolate);
        v8::Handle<v8::FunctionTemplate> constructor_t = v8::FunctionTemplate::New(isolate, ResourceContructor);
        v8::Handle<v8::Function> constructor_f = constructor_t->GetFunction();
        context->Global()->Set(v8::String::NewFromUtf8(isolate, "Resource"), constructor_f);
    }

    // [Interceptor]
    {
        v8::HandleScope handle_scope(isolate);
        v8::Handle<v8::ObjectTemplate> object_t = v8::ObjectTemplate::New(isolate);
        object_t->SetNamedPropertyHandler(DictionaryGetter, DictionarySetter);
        object_t->SetIndexedPropertyHandler(ArrayGetter, ArraySetter);
        object_t->SetInternalFieldCount(2);

        v8::Handle<v8::Object> object = object_t->NewInstance();

        Dictionary* dic = new Dictionary();
        (*dic)["Kim"] = "Haejung Kim";
        v8::Handle<v8::External> ex_dic = v8::External::New(isolate, dic);
        object->SetInternalField(0, ex_dic);

        Array* array = new Array();
        array->push_back("Incross!");
        v8::Handle<v8::External> ex_array = v8::External::New(isolate, array);
        object->SetInternalField(1, ex_array);

        context->Global()->Set(v8::String::NewFromUtf8(isolate, "interceptor"), object);
    }
}










// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}


// Creates a new execution environment containing the built-in
// functions.
v8::Handle<v8::Context> CreateShellContext(v8::Isolate* isolate) {
    // Create a template for the global object.
    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    // Bind the global 'print' function to the C++ Print callback.
    global->Set(v8::String::NewFromUtf8(isolate, "print"),
        v8::FunctionTemplate::New(isolate, Print));
    // Bind the global 'read' function to the C++ Read callback.
    global->Set(v8::String::NewFromUtf8(isolate, "read"),
        v8::FunctionTemplate::New(isolate, Read));
    // Bind the global 'load' function to the C++ Load callback.
    global->Set(v8::String::NewFromUtf8(isolate, "load"),
        v8::FunctionTemplate::New(isolate, Load));
    // Bind the 'quit' function
    global->Set(v8::String::NewFromUtf8(isolate, "quit"),
        v8::FunctionTemplate::New(isolate, Quit));
    // Bind the 'version' function
    global->Set(v8::String::NewFromUtf8(isolate, "version"),
        v8::FunctionTemplate::New(isolate, Version));

    return v8::Context::New(isolate, NULL, global);
}


// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called.  Prints its arguments on stdout separated by
// spaces and ending with a newline.
void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
    bool first = true;
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope(args.GetIsolate());
        if (first) {
            first = false;
        } else {
            printf(" ");
        }
        v8::String::Utf8Value str(args[i]);
        const char* cstr = ToCString(str);
        printf("%s", cstr);
    }
    printf("\n");
    fflush(stdout);
}


// The callback that is invoked by v8 whenever the JavaScript 'read'
// function is called.  This function loads the content of the file named in
// the argument into a JavaScript string.
void Read(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (args.Length() != 1) {
        args.GetIsolate()->ThrowException(
            v8::String::NewFromUtf8(args.GetIsolate(), "Bad parameters"));
        return;
    }
    v8::String::Utf8Value file(args[0]);
    if (*file == NULL) {
        args.GetIsolate()->ThrowException(
            v8::String::NewFromUtf8(args.GetIsolate(), "Error loading file"));
        return;
    }
    v8::Handle<v8::String> source = ReadFile(args.GetIsolate(), *file);
    if (source.IsEmpty()) {
        args.GetIsolate()->ThrowException(
            v8::String::NewFromUtf8(args.GetIsolate(), "Error loading file"));
        return;
    }
    args.GetReturnValue().Set(source);
}


// The callback that is invoked by v8 whenever the JavaScript 'load'
// function is called.  Loads, compiles and executes its argument
// JavaScript file.
void Load(const v8::FunctionCallbackInfo<v8::Value>& args) {
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope(args.GetIsolate());
        v8::String::Utf8Value file(args[i]);
        if (*file == NULL) {
            args.GetIsolate()->ThrowException(
                v8::String::NewFromUtf8(args.GetIsolate(), "Error loading file"));
            return;
        }
        v8::Handle<v8::String> source = ReadFile(args.GetIsolate(), *file);
        if (source.IsEmpty()) {
            args.GetIsolate()->ThrowException(
                v8::String::NewFromUtf8(args.GetIsolate(), "Error loading file"));
            return;
        }
        if (!ExecuteString(args.GetIsolate(),
            source,
            v8::String::NewFromUtf8(args.GetIsolate(), *file),
            false,
            false)) {
                args.GetIsolate()->ThrowException(
                    v8::String::NewFromUtf8(args.GetIsolate(), "Error executing file"));
                return;
        }
    }
}


// The callback that is invoked by v8 whenever the JavaScript 'quit'
// function is called.  Quits.
void Quit(const v8::FunctionCallbackInfo<v8::Value>& args) {
    // If not arguments are given args[0] will yield undefined which
    // converts to the integer value 0.
    int exit_code = args[0]->Int32Value();
    fflush(stdout);
    fflush(stderr);
    exit(exit_code);
}


void Version(const v8::FunctionCallbackInfo<v8::Value>& args) {
    args.GetReturnValue().Set(
        v8::String::NewFromUtf8(args.GetIsolate(), v8::V8::GetVersion()));
}


// Reads a file into a v8 string.
v8::Handle<v8::String> ReadFile(v8::Isolate* isolate, const char* name) {
    FILE* file = fopen(name, "rb");
    if (file == NULL) return v8::Handle<v8::String>();

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);

    char* chars = new char[size + 1];
    chars[size] = '\0';
    for (int i = 0; i < size;) {
        int read = static_cast<int>(fread(&chars[i], 1, size - i, file));
        i += read;
    }
    fclose(file);
    v8::Handle<v8::String> result =
        v8::String::NewFromUtf8(isolate, chars, v8::String::kNormalString, size);
    delete[] chars;
    return result;
}


// Process remaining command line arguments and execute files
int RunMain(v8::Isolate* isolate, int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        const char* str = argv[i];
        if (strcmp(str, "--shell") == 0) {
            //run_shell = true;
        } else if (strcmp(str, "-f") == 0) {
            // Ignore any -f flags for compatibility with the other stand-
            // alone JavaScript CloudBoxs.
            continue;
        } else if (strncmp(str, "--", 2) == 0) {
            fprintf(stderr,
                "Warning: unknown flag %s.\nTry --help for options\n", str);
        } else if (strcmp(str, "-e") == 0 && i + 1 < argc) {
            // Execute argument given to -e option directly.
            v8::Handle<v8::String> file_name =
                v8::String::NewFromUtf8(isolate, "unnamed");
            v8::Handle<v8::String> source =
                v8::String::NewFromUtf8(isolate, argv[++i]);
            if (!ExecuteString(isolate, source, file_name, false, true)) return 1;
        } else {
            // Use all other arguments as names of files to load and run.
            v8::Handle<v8::String> file_name = v8::String::NewFromUtf8(isolate, str);
            v8::Handle<v8::String> source = ReadFile(isolate, str);
            if (source.IsEmpty()) {
                fprintf(stderr, "Error reading '%s'\n", str);
                continue;
            }
            if (!ExecuteString(isolate, source, file_name, false, true)) return 1;
        }
    }
    return 0;
}


// The read-eval-execute loop of the shell.
void RunShell(v8::Handle<v8::Context> context) {
    fprintf(stderr, "V8 version %s [sample shell]\n", v8::V8::GetVersion());
    static const int kBufferSize = 256;
    // Enter the execution environment before evaluating any code.

    v8::Local<v8::String> name(v8::String::NewFromUtf8(context->GetIsolate(), "(shell)"));
    while (true) {
        char buffer[kBufferSize];
        fprintf(stderr, "> ");
        char* str = fgets(buffer, kBufferSize, stdin);
        if (str == NULL) break;

        v8::Context::Scope context_scope(context);
        v8::HandleScope handle_scope(context->GetIsolate());
        ExecuteString(context->GetIsolate(),
            v8::String::NewFromUtf8(context->GetIsolate(), str),
            name,
            true,
            true);
    }
    fprintf(stderr, "\n");
}


// Executes a string within the current v8 context.
bool ExecuteString(v8::Isolate* isolate,
    v8::Handle<v8::String> source,
    v8::Handle<v8::Value> name,
    bool print_result,
    bool report_exceptions) {
        v8::HandleScope handle_scope(isolate);
        v8::TryCatch try_catch;
        v8::Handle<v8::Script> script = v8::Script::Compile(source, name);
        if (script.IsEmpty()) {
            // Print errors that happened during compilation.
            if (report_exceptions)
                ReportException(isolate, &try_catch);
            return false;
        } else {
            v8::Handle<v8::Value> result = script->Run();
            if (result.IsEmpty()) {
                assert(try_catch.HasCaught());
                // Print errors that happened during execution.
                if (report_exceptions)
                    ReportException(isolate, &try_catch);
                return false;
            } else {
                assert(!try_catch.HasCaught());
                if (print_result && !result->IsUndefined()) {
                    // If all went well and the result wasn't undefined then print
                    // the returned value.
                    v8::String::Utf8Value str(result);
                    const char* cstr = ToCString(str);
                    printf("%s\n", cstr);
                }
                return true;
            }
        }
}


void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch) {
    v8::HandleScope handle_scope(isolate);
    v8::String::Utf8Value exception(try_catch->Exception());
    const char* exception_string = ToCString(exception);
    v8::Handle<v8::Message> message = try_catch->Message();
    if (message.IsEmpty()) {
        // V8 didn't provide any extra information about this error; just
        // print the exception.
        fprintf(stderr, "%s\n", exception_string);
    } else {
        // Print (filename):(line number): (message).
        v8::String::Utf8Value filename(message->GetScriptResourceName());
        const char* filename_string = ToCString(filename);
        int linenum = message->GetLineNumber();
        fprintf(stderr, "%s:%i: %s\n", filename_string, linenum, exception_string);
        // Print line of source code.
        v8::String::Utf8Value sourceline(message->GetSourceLine());
        const char* sourceline_string = ToCString(sourceline);
        fprintf(stderr, "%s\n", sourceline_string);
        // Print wavy underline (GetUnderline is deprecated).
        int start = message->GetStartColumn();
        for (int i = 0; i < start; i++) {
            fprintf(stderr, " ");
        }
        int end = message->GetEndColumn();
        for (int i = start; i < end; i++) {
            fprintf(stderr, "^");
        }
        fprintf(stderr, "\n");
        v8::String::Utf8Value stack_trace(try_catch->StackTrace());
        if (stack_trace.length() > 0) {
            const char* stack_trace_string = ToCString(stack_trace);
            fprintf(stderr, "%s\n", stack_trace_string);
        }
    }
}
