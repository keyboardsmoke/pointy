#include "pointy.h"

using namespace v8;
using namespace pointy;

pointy::PointyContext pointy::PointyContext::Create() {
    pointy::PointyContext R;

    V8::Initialize();

    const char *flags = "--debugger --expose_debug_as=v8debug";
    V8::SetFlagsFromString(flags, (int)strlen(flags));

    HandleScope scope();
    R.GlobalTemplate = ObjectTemplate::New();
    R.Context = Context::New(NULL, R.GlobalTemplate);

    //
    R.GlobalTemplate->Set(String::New("version"), Number::New(1.0), ReadOnly);

    //

    Context::Scope cs(R.Context);

    v8::Debug::SetMessageHandler2([](const v8::Debug::Message& message) {
        String::Utf8Value J(message.GetJSON());
        syslog(LOG_DEBUG, "%s", *J);
    });

    v8::Debug::SetDebugEventListener([](DebugEvent event, Handle<Object> exec_state, Handle<Object> event_data, Handle<Value> data) {
        syslog(LOG_DEBUG, "DEBUG EVENT [%d]", event);
    });

    return R;
}

void pointy::PointyContext::Initialize() {
    for (auto I : m_interfaces)
        I->Register(this);
}