#ifndef POINTY_POINTY_H
#define POINTY_POINTY_H

#include <sstream>
#include <fstream>
#include <vector>

#include <v8.h>
#include <v8-debug.h>
#include <fastcgi.h>
#include <fcgio.h>

#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <cstring>

namespace pointy {
    struct PointyContext;

    class IScriptInterface
    {
    public:
        virtual void Register(PointyContext *P) = 0;
    };

    struct PointyContext {
        v8::Handle<v8::ObjectTemplate> GlobalTemplate;
        v8::Persistent<v8::Context> Context;

        // Create a new context
        static pointy::PointyContext Create();

        // Add interface registration
        template<typename T> void RegisterInterface() {
            m_interfaces.push_back(new T);
        }

        // Invoke all existing registrations
        void Initialize();

    private:
        std::vector<IScriptInterface *> m_interfaces;
    };


}

#endif //POINTY_POINTY_H
