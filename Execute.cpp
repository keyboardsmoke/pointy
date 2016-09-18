#include <string>
#include <fstream>
#include <sstream>
#include <v8.h>
#include <v8-debug.h>
#include <syslog.h>
#include "Execute.h"

using namespace v8;

Execute::eScriptResult Execute::ScriptString(Handle<String> source, Handle<Value> name, Handle<Value> *result) {
    Locker lock;
    HandleScope handle_scope;

    if( source->Length() == 0 )
        return SCRIPT_EMPTY;

    //Switch to the context we want to execute in
    Context::Scope context_scope( m_context );

    //Try to compile the script code
    TryCatch try_catch;
    v8::Handle<v8::Script> script = Script::Compile( source, name );

    //If the script is empty, there were compile errors.
    if (script.IsEmpty()) {
        DumpException(&try_catch);
        return SCRIPT_EMPTY;
    }


    if ( script.IsEmpty() ) {
        return SCRIPT_EMPTY;
    }
    else
    {
        Handle<Value> tmpResult = script->Run();

        if (result) {
            *result = tmpResult;
        }

        if (tmpResult.IsEmpty()) {
            DumpException(&try_catch);

            return SCRIPT_COMPILATION_FAILED;
        }
    }

    return SCRIPT_OK;
}

Execute::eScriptResult Execute::Script(std::string filename, Handle<Value> *result) {
    std::ifstream file(filename);
    if (!file.good())
        return SCRIPT_NOT_FOUND;

    std::string scriptdata((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
    if (scriptdata.empty())
        return SCRIPT_EMPTY;

    HandleScope scope;
    Handle<String> scriptString = String::New(scriptdata.c_str());
    if (scriptString.IsEmpty())
        return SCRIPT_EMPTY;

    return ScriptString(scriptString, String::New(filename.c_str()), result);
}

void Execute::DumpException(v8::TryCatch *TC) {
    syslog(LOG_INFO, "DumpException");

    if (!m_msg)
        return; // If they didn't register one, that's on them.

    Locker lock;
    HandleScope scope;

    std::stringstream ss;

    String::Utf8Value exception(TC->Exception());
    Handle<Message> Message = TC->Message();

    if (Message.IsEmpty()) {
        ss << "Exception Occured: " << *exception << std::endl;
        m_msg(MESSAGE_ERROR, ss.str(), m_userdata);
        return;
    }

    String::Utf8Value SourceLine(Message->GetSourceLine());
    String::Utf8Value Filename(Message->GetScriptResourceName());
    int LineNumber = Message->GetLineNumber();

    ss << *Filename << ":" << LineNumber << ": " << *exception << std::endl;
    ss << "> " << *SourceLine << std::endl;

    m_msg(MESSAGE_ERROR, ss.str(), m_userdata);
}