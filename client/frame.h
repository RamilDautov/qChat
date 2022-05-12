#ifndef FRAME_H
#define FRAME_H

#include <QVBoxLayout>
#include "titlebar.h"
#include "connectwindow.h"
#include "mainwindow.h"

class Frame : public QFrame
{
public:

    Frame();

    // Allows you to access the content area of the frame
    // where widgets and layouts can be added
    QWidget *contentWidget() const { return m_content; }

    TitleBar *titleBar() const { return m_titleBar; }

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    TitleBar *m_titleBar;
    QWidget *m_content;
    QVBoxLayout *m_layout;

    MainWindow *m_mainWindow;
    ConnectWindow *m_connectWindow;

    QPoint m_old_pos;
    bool m_mouse_down;
    bool left, right, top, bottom;

    void onClientConnected();
};

#endif // FRAME_H
