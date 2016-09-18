#include "../pointy.h"
#include "GlobalInterface.h"

using namespace v8;

static Handle<Value> Print(const Arguments &args) {
    syslog(LOG_INFO, "Print");

    /*
    Locker locker;
    HandleScope scope;

    if( args.Length() ) {
        String::Utf8Value message( args[0]->ToString() );
        if( message.length() ) {
            //std::cout << *message;

            return scope.Close(Boolean::New(true));
        }
    }*/

    return Undefined();
}

void GlobalInterface::Register(pointy::PointyContext *P) {
    P->Context->Global()->Set(String::New("print"), FunctionTemplate::New(Print)->GetFunction());
}