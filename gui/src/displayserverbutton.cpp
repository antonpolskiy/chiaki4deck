#include "displayserverbutton.h"
#include "QPainter"
#include "QPaintEvent"
#include "chiaki/log.h"
#include <iostream>

DisplayServerButton::DisplayServerButton(const QString &name, const QString &icon, QWidget *parent) :
    QAbstractButton(parent), m_icon(icon), m_name(name), m_inside(false)
{
    QSize size(356,356);
    setMaximumSize(size);
    setMinimumSize(size);
    setCheckable(true);
}

DisplayServerButton::~DisplayServerButton()
{
}

void DisplayServerButton::setName(const QString &name)
{
    m_name = name;
    update();
}

void DisplayServerButton::paintEvent(QPaintEvent *e)
{
    ChiakiLog log;
    chiaki_log_init(&log,CHIAKI_LOG_ALL,chiaki_log_cb_print,nullptr);
    QPainter p(this);
    if( isChecked() )
    {
        paintActive(&p,e->rect());
    }
    else
    {
        paintNormal(&p,e->rect());
    }
}


void DisplayServerButton::paintActive( QPainter* painter, const QRect& rect )
{
    QPixmap bg(":/icons/focused_btn_bg.png");
    painter->drawPixmap(rect, bg);
    QRect ps(0,0,150,150);
    ps.moveLeft((rect.width()-ps.width())/2);
    ps.moveTop((rect.height()-ps.height())/2);
    m_icon.paint(painter, ps, Qt::AlignCenter, QIcon::Normal);

    QFont font = painter->font();
    font.setPixelSize(32);
    painter->setFont(font);
    QPen pen(palette().color(QPalette::Active, QPalette::ButtonText));


    QRect rectangle = rect;
    rectangle.setBottom(150);
    QRect boundingRect;
    painter->setPen(pen);
    painter->drawText(rectangle, Qt::AlignCenter, m_name, &boundingRect);
}

void DisplayServerButton::paintNormal(QPainter *painter, const QRect &rect)
{
    QRect ps(0,0,150,150);
    ps.moveLeft((rect.width()-ps.width())/2);
    ps.moveTop((rect.height()-ps.height())/2);
    m_icon.paint(painter, ps, Qt::AlignCenter, QIcon::Disabled);

    QFont font = painter->font();
    font.setPixelSize(32);
    painter->setFont(font);
    QPen pen(palette().color(QPalette::Disabled, QPalette::ButtonText));

    QRect rectangle = rect;
    rectangle.setBottom(150);
    QRect boundingRect;
    painter->setPen(pen);
    painter->drawText(rectangle, Qt::AlignCenter, m_name, &boundingRect);
}

void DisplayServerButton::mousePressEvent(QMouseEvent *e)
{
    m_inside = rect().contains( e->pos() );
}

void DisplayServerButton::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_inside && rect().contains(e->pos()) )
    {
        if( isChecked() )
            emit clicked();
        else
            setChecked(true);
    }
}

void DisplayServerButton::mouseMoveEvent(QMouseEvent *e)
{
    if( !rect().contains(e->pos()) )
        m_inside = false;
}


SettingsButton::SettingsButton(QWidget *parent) :
    QAbstractButton(parent), m_icon(":/icons/Gear.png"), m_inside(false)
{
    QSize size(218,110);
    setMaximumSize(size);
    setMinimumSize(size);
    setCheckable(true);
}

SettingsButton::~SettingsButton()
{
}

void SettingsButton::paintEvent(QPaintEvent *e)
{
    ChiakiLog log;
    chiaki_log_init(&log,CHIAKI_LOG_ALL,chiaki_log_cb_print,nullptr);
    QPainter p(this);
    if( isChecked() )
    {
        paintActive(&p,e->rect());
    }
    else
    {
        paintNormal(&p,e->rect());
    }
}


void SettingsButton::paintActive( QPainter* painter, const QRect& rect )
{
    QPixmap bg(":/icons/focused_settings_btn.png");
    painter->drawPixmap(rect, bg);
    QRect gear(48,(rect.height()-34)/2,34,34);
    m_icon.paint(painter, gear, Qt::AlignCenter, QIcon::Normal);

    QFont font = painter->font();
    font.setPixelSize(20);
    painter->setFont(font);
    QPen pen(palette().color(QPalette::Active, QPalette::ButtonText));


    QRect rectangle = rect;
    rectangle.setLeft(34);
    QRect boundingRect;
    painter->setPen(pen);
    painter->drawText(rectangle, Qt::AlignCenter, "Settings", &boundingRect);
}

void SettingsButton::paintNormal(QPainter *painter, const QRect &rect)
{
    QRect gear(48,(rect.height()-34)/2,34,34);
    m_icon.paint(painter, gear, Qt::AlignCenter, QIcon::Disabled);

    QFont font = painter->font();
    font.setPixelSize(20);
    painter->setFont(font);
    QPen pen(palette().color(QPalette::Disabled, QPalette::ButtonText));

    QRect rectangle = rect;
    rectangle.setLeft(34);
    QRect boundingRect;
    painter->setPen(pen);
    painter->drawText(rectangle, Qt::AlignCenter, "Settings", &boundingRect);
}

void SettingsButton::mousePressEvent(QMouseEvent *e)
{
    m_inside = rect().contains( e->pos() );
}

void SettingsButton::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_inside && rect().contains(e->pos()) )
    {
        if( isChecked() )
            emit clicked();
        else
            setChecked(true);
    }
}

void SettingsButton::mouseMoveEvent(QMouseEvent *e)
{
    if( !rect().contains(e->pos()) )
        m_inside = false;
}
