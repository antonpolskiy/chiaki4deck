#include "gamepadmessagebox.h"
#include "controllerevent.h"
#include "QAbstractButton"

#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

const QMap<QMessageBox::StandardButton, QString> GamepadMessageBox::c_button_text =
    {
        { QMessageBox::Ok, "Ok"},
        { QMessageBox::Open, "Open"},
        { QMessageBox::Save, "Save"},
        { QMessageBox::Cancel, "Cancel"},
        { QMessageBox::Close, "Close"},
        { QMessageBox::Discard, "Discard"},
        { QMessageBox::Apply, "Apply"},
        { QMessageBox::Reset, "Reset"},
        { QMessageBox::RestoreDefaults,"Restore"},
        { QMessageBox::Help, "Help"},
        { QMessageBox::SaveAll, "Save all"},
        { QMessageBox::Yes, "Yes"},
        { QMessageBox::YesToAll,"Yes to all"},
        { QMessageBox::No, "No"},
        { QMessageBox::NoToAll,"No to all"},
        { QMessageBox::Abort, "Abort"},
        { QMessageBox::Retry, "Retry"},
        { QMessageBox::Ignore, "Ignore"},
        { QMessageBox::NoButton,"No"}
};

QIcon GamepadMessageBox::c_information_icon = QIcon();
QIcon GamepadMessageBox::c_question_icon = QIcon();
QIcon GamepadMessageBox::c_warning_icon = QIcon();
QIcon GamepadMessageBox::c_critical_icon = QIcon();
QFont GamepadMessageBox::c_title_font = QFont();
QFont GamepadMessageBox::c_message_font = QFont();

GamepadMessageBox::GamepadMessageBox(QMessageBox::Icon icon, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QWidget *parent, int timeout, Qt::WindowFlags f)
    : QDialog(parent)
{
    initResources();
    if(parent)
        setGeometry(parent->rect());

    m_icon = icon;
    m_title = title;
    m_text = text;
    m_buttons.clear();
    m_focused = -1;
    m_selected_button = QMessageBox::NoButton;

    uint mask = QMessageBox::FirstButton;
    int i = 0;
    while (mask <= QMessageBox::LastButton) {
        uint sb = buttons & mask;
        mask <<= 1;
        if (!sb)
            continue;
        m_buttons.push_back((QMessageBox::StandardButton)sb);
        i++;
    }

    if(timeout > 0 && m_buttons.empty() )
    {
       QTimer::singleShot(timeout*1000, this, SLOT(reject()));
    }

    m_update_rects = true;
}


GamepadMessageBox::~GamepadMessageBox()
{
}

QMessageBox::StandardButton GamepadMessageBox::messageBox(QWidget* parent, QMessageBox::Icon icon, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
    GamepadMessageBox msgBox(icon, title, text, buttons, parent);
    for( int i = 0; i < msgBox.m_buttons.size(); ++i )
    {
        if( msgBox.m_buttons[i] == defaultButton )
        {
            msgBox.setFocused(i);
        }
        if( msgBox.m_focused == -1 && !msgBox.m_buttons.isEmpty() )
        {
            msgBox.setFocused(0);
        }
    }

    if (msgBox.exec() == 0)
        return QMessageBox::Cancel;
    return msgBox.selectedButton();
}

QMessageBox::StandardButton GamepadMessageBox::critical(QWidget* parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
    return messageBox( parent, QMessageBox::Critical, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton GamepadMessageBox::information(QWidget* parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
    return messageBox( parent, QMessageBox::Information, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton GamepadMessageBox::question(QWidget* parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
    return messageBox( parent, QMessageBox::Question, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton GamepadMessageBox::warning(QWidget* parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
    return messageBox( parent, QMessageBox::Warning, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton GamepadMessageBox::selectedButton()
{
    return m_selected_button;
}

GamepadMessageBox* GamepadMessageBox::splash(QWidget* parent, const QString &title, const QString &text, int timeout)
{
    GamepadMessageBox* msgBox = new GamepadMessageBox(QMessageBox::Information, title, text, QMessageBox::NoButton, parent, timeout);
    msgBox->show();
    msgBox->setAttribute(Qt::WA_DeleteOnClose);

    return msgBox;
}

void GamepadMessageBox::setFocused( int button )
{
    std::cout <<__PRETTY_FUNCTION__ << " " << button << std::endl;
    if( m_buttons.isEmpty() )
    {
        m_focused = -1;
        m_selected_button = QMessageBox::Cancel;
    }
    else if( button >= 0 && button < m_buttons.size() )
    {
        m_focused = button;
        m_selected_button = m_buttons[m_focused];
    }
    update();
}

void GamepadMessageBox::focusNextButton()
{
    setFocused( m_focused+1 );
}

void GamepadMessageBox::focusPrevButton()
{
    setFocused(m_focused-1);
}

void GamepadMessageBox::keyPressEvent(QKeyEvent *event) {
    std::string key;
    bool accepted = true;
    switch( event->key() )
    {
    case Qt::Key_Left : key = "Left"; focusPrevButton(); break;
    case Qt::Key_Right : key = "Right"; focusNextButton(); break;
    case Qt::Key_Return : key = "Enter"; accept(); break;
    case Qt::Key_Escape : key = "Back";  reject(); break;
    default: accepted = false;
    }
    std::cout <<"MSGBOX: " << "key: " << key << " " << this << std::endl;
    if(accepted)
        event->accept();
    else
        event->ignore();

    QWidget::keyPressEvent(event);
}

bool GamepadMessageBox::event(QEvent *event)
{
    if( event->type() != ControllerEvent::type )
        return QWidget::event(event);

    std::cout <<"GAMEPAD " <<__func__ << " " << std::endl;
    ControllerEvent* cevent = dynamic_cast<ControllerEvent*>(event);
    if(cevent->eventType() != ControllerEvent::Event_Button)
        return true;
    ControllerButtonEvent* cbevent = dynamic_cast<ControllerButtonEvent*>(event);
    std::string key;

    if( cbevent->pressed() )
    {
        switch( cbevent->button() )
        {
        case CHIAKI_CONTROLLER_BUTTON_DPAD_LEFT : key = "Left"; focusPrevButton(); break;
        case CHIAKI_CONTROLLER_BUTTON_DPAD_RIGHT : key = "Right"; focusNextButton();  break;
        case CHIAKI_CONTROLLER_BUTTON_CROSS : key = "Enter"; accept(); break;
        case CHIAKI_CONTROLLER_BUTTON_MOON : key = "Back"; reject(); break;
        }
    }
    std::cout <<"MSGBOX: " <<  "key: " << key << std::endl;

    return true;
}

static const int c_icon_width = 161;
static const int c_icon_height = 110;
static const int c_spacing = 12;
static const int c_button_spacing = 46;

void GamepadMessageBox::getRects(const QRect& wnd_rect)
{
    if( m_wnd_rect == wnd_rect && !m_update_rects )
    {
        return;
    }
    m_wnd_rect = wnd_rect;
    m_update_rects = false;
    QPainter p;
    QFontMetrics ft(c_title_font);
    QFontMetrics fm(c_message_font);
    QSize is(c_icon_width,c_icon_height);
    QSize ts = ft.boundingRect(m_title_rect,Qt::AlignCenter,m_title).size();
    QSize ms = fm.boundingRect(m_message_rect,Qt::AlignCenter,m_text).size();
    QSize bss(218*m_buttons.size() + (m_buttons.size()-1)*c_button_spacing,110);
    int content_height = is.height() + c_spacing + ts.height() +
                         c_spacing + ms.height() + c_spacing*3 + bss.height();


    m_icon_rect = wnd_rect;
    m_icon_rect.setSize(is);
    m_icon_rect.moveTop((wnd_rect.height() - content_height)/2);
    m_icon_rect.moveLeft((wnd_rect.width() - is.width())/2);

    m_title_rect = wnd_rect;
    m_title_rect.setSize(ts);
    m_title_rect.moveTop(m_icon_rect.bottom() + c_spacing);
    m_title_rect.moveLeft((wnd_rect.width() - ts.width())/2);

    m_message_rect = wnd_rect;
    m_message_rect.setSize(ms);
    m_message_rect.moveTop(m_title_rect.bottom() + c_spacing);
    m_message_rect.moveLeft((wnd_rect.width() - ms.width())/2);

    QRect buttons_rect = wnd_rect;
    buttons_rect.setSize(bss);
    buttons_rect.moveTop(m_message_rect.bottom() + c_spacing*3);
    buttons_rect.moveLeft((wnd_rect.width() - bss.width())/2);

    m_buttons_rect.clear();
    m_buttons_rect.resize(m_buttons.size());
    for( int i = 0; i < m_buttons.size(); ++i )
    {
        m_buttons_rect[i] = buttons_rect;
        QSize bs(218,110);
        m_buttons_rect[i].setSize(bs);
        m_buttons_rect[i].moveLeft(buttons_rect.left() + i*(bs.width()+c_button_spacing));
    }
    //std::cout << title_rect.x() << "X" << title_rect.y() << " " << title_rect.width() <<"x"<< title_rect.height() << std::endl;
}

void GamepadMessageBox::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QRect wnd_rect = event->rect();

    p.fillRect(wnd_rect, QBrush(QColor("#272D28")));

    getRects(wnd_rect);

    drawIcon(&p);
    drawTitle(&p);
    drawMessage(&p);
    drawButtons(&p);
}

void GamepadMessageBox::drawTitle(QPainter* p )
{
    p->setFont(c_title_font);
    p->setPen(QColor(255,255,255,191));
    p->drawText(m_title_rect, Qt::AlignCenter ,m_title);
}

void GamepadMessageBox::drawMessage(QPainter* p )
{
    p->setFont(c_message_font);
    p->setPen(QColor(255,255,255,107));
    p->drawText(m_message_rect, Qt::AlignCenter ,m_text);
}

void GamepadMessageBox::drawButtons(QPainter* p)
{
    QFont font = QFont("Inter");
    font.setPixelSize(20);
    font.setWeight(QFont::Medium);
    p->setFont(font);

    for( int i = 0; i < m_buttons.size(); ++i )
    {
        p->setPen(QColor(255,255,255,128));
        if( m_focused == i )
        {
            QPixmap bg(":/icons/focused_settings_btn.png");
            p->drawPixmap(m_buttons_rect[i], bg);
            p->setPen(QColor(255,255,255,255));
        }
        p->drawText(m_buttons_rect[i], Qt::AlignCenter , c_button_text[m_buttons[i]]);
    }
}

void GamepadMessageBox::drawIcon(QPainter* p)
{
    const QIcon* icon_ptr = nullptr;
    switch(m_icon)
    {
    case QMessageBox::Information: icon_ptr = &c_information_icon; break;
    case QMessageBox::Question: icon_ptr = &c_question_icon; break;
    case QMessageBox::Warning: icon_ptr = &c_warning_icon;  break;
    case QMessageBox::Critical: icon_ptr = &c_critical_icon; break;
    default:return;
    }
    icon_ptr->paint(p,m_icon_rect);
}

void GamepadMessageBox::initResources()
{
    if( c_information_icon.isNull() )
    {
        c_information_icon = QIcon(":/icons/information.svg");
    }

    if( c_question_icon.isNull() )
    {
        c_question_icon = QIcon(":/icons/question.svg");
    }

    if( c_warning_icon.isNull() )
    {
        c_warning_icon = QIcon(":/icons/warning.svg");
    }

    if( c_critical_icon.isNull() )
    {
        c_critical_icon = QIcon(":/icons/critical.svg");
    }

    if( c_title_font.family().isEmpty() )
    {
        c_title_font = QFont("Inter");
        c_title_font.setPixelSize(48);
        c_title_font.setWeight(QFont::Normal);
    }

    if( c_message_font.family().isEmpty() )
    {
        c_message_font = QFont("Inter");
        c_message_font.setPixelSize(32);
        c_message_font.setWeight(QFont::Normal);
    }
}

void GamepadMessageBox::focusInEvent(QFocusEvent *event)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void GamepadMessageBox::focusOutEvent(QFocusEvent *event)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void GamepadMessageBox::mousePressEvent(QMouseEvent *e)
{
    for( int i = 0; i < m_buttons_rect.size(); ++i )
    {
        if( m_buttons_rect[i].contains(e->pos()))
        {
            m_inside = i;
            break;
        }
    }
}

void GamepadMessageBox::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_inside >= 0 && m_inside < m_buttons_rect.size() && m_buttons_rect[m_inside].contains(e->pos()) )
    {
        if(m_focused == m_inside)
            emit accept();
        else
        {
            setFocused(m_inside);
        }
    }
}

void GamepadMessageBox::mouseMoveEvent(QMouseEvent *e)
{
    if(m_inside >= 0 && m_inside < m_buttons_rect.size() && !m_buttons_rect[m_inside].contains(e->pos()) )
    {
        m_inside = -1;
    }
}
