#ifndef CONTROLLEREVENT_H
#define CONTROLLEREVENT_H

#include <QEvent>
#include "chiaki/controller.h"
#include  <iostream>

class ControllerEvent : public QEvent
{
public:
    static int type;
    explicit ControllerEvent(ChiakiControllerButton _btn, bool _pressed) : QEvent((QEvent::Type)type), btn(_btn), pressed(_pressed) {}
    ChiakiControllerButton btn;
    bool pressed;
};

#endif // CONTROLLEREVENT_H
