#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QtWidgets>

class TitleBar : public QWidget
{
    Q_OBJECT
public:
    TitleBar(QWidget *parent, bool withMaximize);

    void paintEvent(QPaintEvent *);

public slots:
    void showSmall();

    void showMaxRestore();
protected:
    void mousePressEvent(QMouseEvent *me);
    void mouseMoveEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    QToolButton *minimize;
    QToolButton *maximize;
    QToolButton *close;
    QPixmap restorePix, maxPix;
    bool maxNormal;
    QPoint startPos;
    QPoint clickPos;

    QPoint m_old_pos;
    bool m_mouse_down;
    bool left, right, top, bottom;
};

#endif // TITLEBAR_H
