#ifndef JS_HTTP_H_
#define JS_HTTP_H_

#include <v8.h>

namespace JS {
namespace Http {

void Bind(v8::Handle<v8::Context> context);

}  // namespace Http
}  // namespace JS

#endif  // JS_HTTP_H_