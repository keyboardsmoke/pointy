#ifndef POINTY_FCGI_H
#define POINTY_FCGI_H

#include <fcgi_config.h>
#include <fastcgi.h>
#include <fcgiapp.h>
#include <fcgio.h>
#include <fcgi_stdio.h>
#include <vector>
#include "pointy.h"

class FCGI {
public:
    typedef void (*RequestCB)(pointy::PointyContext *P, FCGX_Request *R);

    void Initialize(pointy::PointyContext *P);
    void RegisterRequestCallback(RequestCB CB);
    void Accept();
    void Dispose();

private:
    std::streambuf *m_originalCoutBuffer;
    std::streambuf *m_originalCinBuffer;
    std::streambuf *m_originalErrsBuffer;

    std::streambuf *m_newCoutBuffer;


    pointy::PointyContext *m_context;

    FCGX_Request m_request;
    std::vector<RequestCB> m_requestCb;
};


#endif //POINTY_FCGI_H
