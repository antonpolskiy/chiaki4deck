// SPDX-License-Identifier: LicenseRef-AGPL-3.0-only-OpenSSL

#include "controllerevent.h"
#include <loginpindialog.h>
#include <QPainter>
#include <QPaintEvent>

#define PIN_LENGTH 4

LoginPINDialog::LoginPINDialog(bool incorrect, QWidget *parent)
    : QDialog(parent)
    , m_entered_count(0)
    , pin("    ")
    , m_button_map(":/icons/button_map.png")
    , m_entry_empty(":/icons/login_entry_empty.svg")
    , m_entry_filled(":/icons/login_entry_filled.svg")
{
    setWindowTitle(tr("Console Login PIN"));
    QSize size(636,467);
    setMaximumSize(size);
    setMinimumSize(size);
    m_axises.resize(6);
}

QString LoginPINDialog::GetPIN()	{ return pin; }

bool LoginPINDialog::enterChar(QChar c)
{
    if( m_entered_count < PIN_LENGTH )
    {
        pin[m_entered_count++] = c;
        return true;
    }
    return false;
}

bool LoginPINDialog::removeChar()
{
    if( m_entered_count > 0 )
    {
        pin[--m_entered_count] = ' ';
        return true;
    }
    return false;
}

void LoginPINDialog::keyPressEvent(QKeyEvent *event)
{
    switch( event->key() )
    {
    case Qt::Key_1 : enterChar('1'); break;
    case Qt::Key_2 : enterChar('2'); break;
    case Qt::Key_3 : enterChar('3'); break;
    case Qt::Key_4 : enterChar('4'); break;
    case Qt::Key_5 : enterChar('5'); break;
    case Qt::Key_6 : enterChar('6'); break;
    case Qt::Key_7 : enterChar('7'); break;
    case Qt::Key_8 : enterChar('8'); break;
    case Qt::Key_9 : enterChar('9'); break;
    case Qt::Key_0 : enterChar('0'); break;
    case Qt::Key_Return : if(m_entered_count == PIN_LENGTH) accept(); break;
    case Qt::Key_Backspace : removeChar();   break;
    case Qt::Key_Escape : reject(); break;
    }
    update();
}

bool LoginPINDialog::event(QEvent *event)
{
    if( event->type() != ControllerEvent::type )
        return QDialog::event(event);

    ControllerEvent* cevent = dynamic_cast<ControllerEvent*>(event);
    ControllerEvent::EControllerEventType etype = cevent->eventType();
    if(etype == ControllerEvent::Event_Button )
    {
        ControllerButtonEvent* cbevent = dynamic_cast<ControllerButtonEvent*>(cevent);
        if( cbevent->pressed() )
        {
            switch( cbevent->button() )
            {
            case CHIAKI_CONTROLLER_BUTTON_DPAD_UP : enterChar('2'); break;
            case CHIAKI_CONTROLLER_BUTTON_DPAD_DOWN : enterChar('4'); break;
            case CHIAKI_CONTROLLER_BUTTON_DPAD_LEFT : enterChar('1'); break;
            case CHIAKI_CONTROLLER_BUTTON_DPAD_RIGHT : enterChar('3'); break;

            case CHIAKI_CONTROLLER_BUTTON_L1 : enterChar('7'); break;
            case CHIAKI_CONTROLLER_BUTTON_R1 : enterChar('5'); break;

            case CHIAKI_CONTROLLER_BUTTON_PYRAMID : enterChar('9'); break;
            case CHIAKI_CONTROLLER_BUTTON_BOX : enterChar('0'); break;

            case CHIAKI_CONTROLLER_BUTTON_CROSS : if(m_entered_count == PIN_LENGTH) { std::cout << pin.toStdString() << std::endl; accept(); } break;
            case CHIAKI_CONTROLLER_BUTTON_MOON : if( !removeChar() ) reject(); break;
            }
        }
    }
    else if(etype == ControllerEvent::Event_Axis)
    {
        static const int16_t dead_zone = INT16_MAX * 0.5;
        int press;
        ControllerAxisEvent* caevent = dynamic_cast<ControllerAxisEvent*>(cevent);
        int16_t prev = m_axises[caevent->axis()];

        if( prev == caevent->value() )
        {
            press = 0;
            return true;
        }
        else if( prev <= dead_zone && caevent->value() > dead_zone )
        {
            press = 1;
            m_axises[caevent->axis()] = caevent->value();
        }
        else if( prev >= -dead_zone && caevent->value() < -dead_zone)
        {
            press = -1;
            m_axises[caevent->axis()] = caevent->value();
        }
        else
        {
            m_axises[caevent->axis()] = caevent->value();
            return true;
        }

        switch( caevent->axis() )
        {
        case ControllerAxisEvent::Axis_LeftTrigger: if(press) enterChar('8'); break;
        case ControllerAxisEvent::Axis_RightTrigger: if(press) enterChar('6'); break;
        case ControllerAxisEvent::Axis_LeftX:
            if( press == 1 ) enterChar('3');
            else if( press == -1 ) enterChar('1'); break;
        case ControllerAxisEvent::Axis_LeftY:
            if( press == 1 ) enterChar('4');
            else if( press == -1 ) enterChar('2'); break;
        }
    }
    update();
    return true;
}

void LoginPINDialog::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QRect wnd_rect = event->rect();
    QRect entry_rect(155,40,322,52);

    paintEntry(p,entry_rect);

    QPixmap button_map(":/icons/button_map.png");
    QRect button_map_rect(156,170,324,248);
    p.drawPixmap(button_map_rect, button_map);
}

void LoginPINDialog::paintEntry(QPainter &p, QRect r)
{
    for( int i = 0; i < PIN_LENGTH; ++i )
    {
        QRect rect(r.x() + (i*86), r.y(), 64, 52);

        if( i < m_entered_count )
            m_entry_filled.paint(&p, rect, Qt::AlignCenter, QIcon::Normal);
        else
            m_entry_empty.paint(&p, rect, Qt::AlignCenter, QIcon::Normal);
    }
}
