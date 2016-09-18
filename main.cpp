#include "pointy.h"
#include "Execute.h"
#include "FCGI.h"
#include "Interfaces.h"

using namespace std;
using namespace v8;
using namespace pointy;

FCGX_Stream *g_out = nullptr;
FCGX_Stream *g_in = nullptr;
FCGX_Stream *g_err = nullptr;

void AddEnvironment(PointyContext *P, char **envp) {
    Locker L;
    HandleScope HS;

    for (auto env = envp; *env; env++) {
        std::string e(*env);

        size_t eq = e.find_first_of('=');
        if (eq != std::string::npos) {
            std::string name = e.substr(0, eq);
            std::string valu = e.substr(eq + 1);

            P->GlobalTemplate->Set(String::New(name.c_str()), String::New(valu.c_str()));
        }
    }
}

void printConUsage() {

}

int conMain(int argc, char **argv, char **envp) {
    if (argc < 3) {
        std::cout << "Invalid command." << std::endl;
        printConUsage();
        return 1;
    }

    char *SCRIPT_FILENAME = argv[2];

    if (!ifstream(SCRIPT_FILENAME).good()) {
        std::cout << "The script file [" << SCRIPT_FILENAME << "] does not exist." << std::endl;
        printConUsage();
        return 1;
    }

    Locker locker;
    HandleScope scope;

    // Initialize v8
    PointyContext P = pointy::PointyContext::Create();
    std::cout << "PointyContext::Create OK" << std::endl;

    //
    Interfaces::Register(&P);
    std::cout << "Interfaces::Register OK" << std::endl;

    //
    P.Initialize();
    std::cout << "PointyContext::Initialize OK" << std::endl;

    Execute ExecutionContext(P.GlobalTemplate, P.Context, [](Execute::eMessageType type, std::string s, void *user_data) {
        std::cout << s << std::endl;
    }, nullptr);

    std::cout << "SCRIPT FILENAME OK" << std::endl;
    auto request = ExecutionContext.Script(SCRIPT_FILENAME);
    std::cout << "Request Result [" << Execute::GetScriptResultString(request) << "]" << std::endl;

    return 0;
}

int main(int argc, char **argv, char **envp) {
    openlog("pointy", LOG_PID|LOG_NDELAY|LOG_PERROR, LOG_USER);

    if (argc > 1 && std::string(argv[1]).compare("-f") == 0) {
        int r = conMain(argc, argv, envp);
        closelog();
        return r;
    }

    Locker locker;
    HandleScope scope;

    syslog(LOG_INFO, "pointy request acknowledged.");

    // Initialize v8
    PointyContext P = PointyContext::Create();

    //
    Interfaces::Register(&P);

    //
    P.Initialize();
    syslog(LOG_INFO, "InitializeJSS OK");

    FCGI cgi;
    cgi.Initialize(&P);
    cgi.RegisterRequestCallback([](pointy::PointyContext *P, FCGX_Request *R) {
        const char *SCRIPT_FILENAME = FCGX_GetParam("SCRIPT_FILENAME", R->envp);
        syslog(LOG_INFO, "SCRIPT FILENAME TO EXECUTE [%s]", SCRIPT_FILENAME);

        if (SCRIPT_FILENAME && ifstream(SCRIPT_FILENAME).good()) {
            Execute ExecutionContext(P->GlobalTemplate, P->Context, [](Execute::eMessageType type, std::string s, void *user_data) {
                FCGX_PutS(s.c_str(), (FCGX_Stream *)user_data);
            }, R->out);

            syslog(LOG_INFO, "SCRIPT FILENAME OK");
            FCGX_PutS("Content-Type: text/html\r\n\r\n", R->out);
            auto request = ExecutionContext.Script(SCRIPT_FILENAME);
            syslog(LOG_INFO, "Request Result [%s]", Execute::GetScriptResultString(request));
        } else {
            FCGX_PutS("Content-Type: text/html\r\n\r\n404\n", R->out);
        }
    });
    cgi.Accept();
    cgi.Dispose();

    syslog(LOG_INFO, "EXITING.");

    closelog();

    //
    P.Context.Dispose();

    V8::Dispose();

    return 0;
}