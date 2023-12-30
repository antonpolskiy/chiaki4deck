// SPDX-License-Identifier: LicenseRef-AGPL-3.0-only-OpenSSL

#include "controllerevent.h"
#include <loginpindialog.h>
#include <QPainter>
#include <QPaintEvent>

#define PIN_LENGTH 4

static const int c_btn_map_width = 324;
static const int c_btn_map_height = 248;
static const int c_entry_width = 64;
static const int c_entry_height = 54;
static const int c_entry_spacing = 22;
static const int c_y_offset = 74;
static const int c_y_min_margin = 54;

LoginPINDialog::LoginPINDialog(bool incorrect, QWidget *parent)
    : QDialog(parent)
    , m_entered_count(0)
    , pin("    ")
    , m_button_map(":/icons/button_map.svg")
    , m_entry_empty(":/icons/login_entry_empty.svg")
    , m_entry_filled(":/icons/login_entry_filled.svg")
{
    setWindowTitle(tr("Console Login PIN"));
    if(parent)
    {
        setGeometry(parent->geometry());
    }
    else
    {
        QSize size(636,467);
        setMaximumSize(size);
        setMinimumSize(size);
    }
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

    p.fillRect(wnd_rect, QBrush(QColor("#272D28")));
    int height = c_y_min_margin*2 + c_entry_height + c_y_offset + c_btn_map_height;

    QRect entry_rect = wnd_rect.marginsRemoved(QMargins(0,(wnd_rect.height()-height)/2 ,0,(wnd_rect.height()-height)/2 + c_y_offset + c_btn_map_height + c_y_min_margin ));
    QRect button_map_rect = wnd_rect.marginsRemoved(QMargins((wnd_rect.width()-c_btn_map_width)/2,(wnd_rect.height()-height)/2 + c_y_offset + c_entry_height + c_y_min_margin ,(wnd_rect.width()-c_btn_map_width)/2,(wnd_rect.height()-height)/2));

    paintEntry(p,entry_rect);
    paintButtonMap(p,button_map_rect);
}

void LoginPINDialog::paintButtonMap(QPainter &p, QRect r)
{
    m_button_map.paint(&p, r );
}

void LoginPINDialog::paintEntry(QPainter &p, QRect r)
{
    int x = r.x() + ( r.width() - ((PIN_LENGTH * c_entry_width) + (PIN_LENGTH-1)*c_entry_spacing)) / 2;
    int y = r.y() +  ( r.height() - c_entry_height ) / 2;
    for( int i = 0; i < PIN_LENGTH; ++i )
    {
        QRect rect(x + (i*(c_entry_spacing+c_entry_width)), y, c_entry_width, c_entry_height);

        if( i < m_entered_count )
            m_entry_filled.paint(&p, rect, Qt::AlignCenter, QIcon::Normal);
        else
            m_entry_empty.paint(&p, rect, Qt::AlignCenter, QIcon::Normal);
    }
}
