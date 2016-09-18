#ifndef POINTY_EXECUTE_H
#define POINTY_EXECUTE_H


#include <fcgiapp.h>

class Execute {
public:
    enum eScriptResult {
        SCRIPT_OK = 0,
        SCRIPT_NOT_FOUND,
        SCRIPT_EMPTY,
        SCRIPT_COMPILATION_FAILED,
        SCRIPT_ERROR_COUNT
    };

    enum eMessageType {
        MESSAGE_INFO,
        MESSAGE_WARNING,
        MESSAGE_ERROR,
        MESSAGE_COUNT
    };

    typedef void (*MessageCB)(eMessageType type, std::string ss, void *user_data);

    static const char *GetScriptResultString(eScriptResult r) {
        static const char *rs[] = {
                "SCRIPT_OK",
                "SCRIPT_NOT_FOUND",
                "SCRIPT_EMPTY",
                "SCRIPT_COMPILATION_FAILED",
                "SCRIPT_ERROR_COUNT"
        };

        return rs[r];
    }

    Execute(
            v8::Handle<v8::ObjectTemplate> GlobalTemplate,
            v8::Handle<v8::Context> Context,
            MessageCB msg,
            void *user_data
    ) : m_object(GlobalTemplate), m_context(Context), m_msg(msg), m_userdata(user_data) {
    }

    eScriptResult ScriptString(v8::Handle<v8::String> source, v8::Handle<v8::Value> name, v8::Handle<v8::Value> *result = nullptr);
    eScriptResult Script(std::string filename, v8::Handle<v8::Value> *result = nullptr);

private:
    void DumpException(v8::TryCatch *TC);

    MessageCB m_msg;
    void *m_userdata;

    v8::Handle<v8::ObjectTemplate> m_object;
    v8::Handle<v8::Context> m_context;
};


#endif //POINTY_EXECUTE_H
