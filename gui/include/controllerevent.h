#ifndef CONTROLLEREVENT_H
#define CONTROLLEREVENT_H

#include <QEvent>
#include "chiaki/controller.h"
#include  <iostream>

class ControllerEvent : public QEvent
{
public:
    enum EControllerEventType
    {
        Event_Button,
        Event_Axis
    };

private:
    EControllerEventType m_event_type;
public:
    static int type;

    explicit ControllerEvent(EControllerEventType event_type) : QEvent((QEvent::Type)type), m_event_type(event_type) {}
    EControllerEventType eventType() { return m_event_type; }
};

class ControllerButtonEvent : public ControllerEvent
{
private:
    ChiakiControllerButton m_btn;
    bool m_pressed;
public:
    ControllerButtonEvent(ChiakiControllerButton btn, bool pressed) : ControllerEvent(Event_Button), m_btn(btn), m_pressed(pressed) {}
    bool pressed() {return m_pressed;}
    ChiakiControllerButton button() {return m_btn;}
};

class ControllerAxisEvent : public ControllerEvent
{
public:
    enum ControllerAxis
    {
        Axis_LeftTrigger,
        Axis_RightTrigger,
        Axis_LeftX,
        Axis_LeftY,
        Axis_RightX,
        Axis_RightY,
    };
private:
    ControllerAxis m_axis;
    int16_t m_value;
public:
    ControllerAxisEvent(ControllerAxis axis, int16_t value) : ControllerEvent(Event_Axis), m_axis(axis), m_value(value) {}
    int16_t value() {return m_value;}
    ControllerAxis axis() {return m_axis;}
};

#endif // CONTROLLEREVENT_H
