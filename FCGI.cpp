#include "FCGI.h"

// Maximum number of bytes allowed to be read from stdin
static const unsigned long STDIN_MAX = 1000000;

static uintptr_t gstdin(FCGX_Request * request, char ** content)
{
    char * clenstr = FCGX_GetParam("CONTENT_LENGTH", request->envp);
    uintptr_t clen = STDIN_MAX;

    if (clenstr)
    {
        clen = (uintptr_t)strtol(clenstr, &clenstr, 10);
        if (*clenstr)
        {
            std::cerr << "can't parse \"CONTENT_LENGTH="
            << FCGX_GetParam("CONTENT_LENGTH", request->envp)
            << "\"\n";
            clen = STDIN_MAX;
        }

        // *always* put a cap on the amount of data that will be read
        if (clen > STDIN_MAX) clen = STDIN_MAX;

        *content = new char[clen];

        std::cin.read(*content, clen);
        clen = (uintptr_t)std::cin.gcount();
    }
    else
    {
        // *never* read stdin when CONTENT_LENGTH is missing or unparsable
        *content = 0;
        clen = 0;
    }

    // Chew up any remaining stdin - this shouldn't be necessary
    // but is because mod_fastcgi doesn't handle it correctly.

    // ignore() doesn't set the eof bit in some versions of glibc++
    // so use gcount() instead of eof()...
    do std::cin.ignore(1024); while (std::cin.gcount() == 1024);

    return clen;
}

void FCGI::Initialize(pointy::PointyContext *P) {
    // Store stdin/stdout
    m_originalCoutBuffer = std::cout.rdbuf();
    m_originalCinBuffer = std::cin.rdbuf();
    m_originalErrsBuffer = std::cerr.rdbuf();

    m_context = P;

    FCGX_Init();
    FCGX_InitRequest(&m_request, 0, 0);
}

void FCGI::Accept() {
    while (FCGX_Accept_r(&m_request) == 0) {
        // Override stdout/stdin while we are processing
        fcgi_streambuf cinf(m_request.in);
        fcgi_streambuf coutf(m_request.out);
        fcgi_streambuf cerrf(m_request.err);

#if HAVE_IOSTREAM_WITHASSIGN_STREAMBUF
        std::cin  = &cinf;
        std::cout = &coutf;
        std::cerr = &cerrf;
#else
        std::cin.rdbuf(&cinf);
        std::cout.rdbuf(&coutf);
        std::cerr.rdbuf(&cerrf);
#endif

        char *content;
        uintptr_t clen = gstdin(&m_request, &content);

        for (auto CB : m_requestCb) {
            CB(m_context, &m_request);
        }

        FCGX_Finish_r(&m_request);
    }
}

void FCGI::RegisterRequestCallback(RequestCB CB) {
    m_requestCb.push_back(CB);
}

void FCGI::Dispose() {
    // Restore stdin/stdout
    std::cout.rdbuf(m_originalCoutBuffer);
    std::cin.rdbuf(m_originalCinBuffer);
    std::cerr.rdbuf(m_originalErrsBuffer);
}