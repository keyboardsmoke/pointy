#ifndef POINTY_GLOBALINTERFACE_H
#define POINTY_GLOBALINTERFACE_H

class GlobalInterface : public pointy::IScriptInterface {
public:
    virtual void Register(pointy::PointyContext *P);
};


#endif //POINTY_GLOBALINTERFACE_H
