#ifndef DISPLAYSERVERBUTTON_H
#define DISPLAYSERVERBUTTON_H

#include <QAbstractButton>
class SettingsButton : public QAbstractButton
{
    Q_OBJECT

    QIcon m_icon;
    bool m_inside;
public:
    explicit SettingsButton(QWidget *parent = nullptr);
    ~SettingsButton();

protected:
    virtual void paintEvent(QPaintEvent *) override;
    void paintActive( QPainter* painter, const QRect& rect );
    void paintNormal( QPainter* painter, const QRect& rect );
    virtual void mousePressEvent(QMouseEvent* e) override;
    virtual void mouseReleaseEvent(QMouseEvent* e) override;
    virtual void mouseMoveEvent(QMouseEvent* e) override;
};

class DisplayServerButton : public QAbstractButton
{
    Q_OBJECT
public:
    enum EState
    {
        State_None,
        State_Unavailable,
        State_Standby,
        State_Ready,
        State_Manual
    };

private:
    QIcon m_icon;
    QString m_name;
    bool m_inside;
    EState m_state;

public:
    explicit DisplayServerButton(const QString& name, const QString& icon, QWidget *parent = nullptr);
    ~DisplayServerButton();
    void setName( const QString& name );
    void setState(EState state);
protected:
    virtual void paintEvent(QPaintEvent *) override;
    void paintActive( QPainter* painter, const QRect& rect );
    void paintNormal( QPainter* painter, const QRect& rect );
    void paintState(QPainter* painter, const QRect& rect, QIcon::Mode mode );
    virtual void mousePressEvent(QMouseEvent* e) override;
    virtual void mouseReleaseEvent(QMouseEvent* e) override;
    virtual void mouseMoveEvent(QMouseEvent* e) override;
};

#endif // DISPLAYSERVERBUTTON_H
