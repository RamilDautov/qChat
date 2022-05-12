#include "frame.h"

Frame::Frame()
{
    m_mouse_down = false;
    setFrameShape(Panel);

    // Make this a borderless window which can't
    // be resized or moved via the window system
    setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking(true);

    m_titleBar = new TitleBar(this);
    m_titleBar->setObjectName("titleBar");

    m_content = new QWidget(this);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->addWidget(m_titleBar);
    //vbox->setMargin(0);
    vbox->setContentsMargins(0,0,0,0);
    vbox->setSpacing(0);

    QVBoxLayout *l = new QVBoxLayout(this);
    l->addWidget(m_content);
    l->setContentsMargins(0,0,0,0);
    l->setSpacing(0);
    vbox->addLayout(l);

    m_mainWindow = new MainWindow;

    m_connectWindow = new ConnectWindow(m_content);

    // set callback
    m_connectWindow->connectFunc( [=](const QString &userName){
        m_mainWindow->setUserName(userName);
        bool connected = m_mainWindow->connectToServer();
        return connected;
    });

    m_layout = new QVBoxLayout(m_content);
    m_layout->setContentsMargins(0,0,0,0);
    m_layout->addWidget(m_connectWindow);

    connect(m_mainWindow->socket(), &QTcpSocket::connected, this, &Frame::onClientConnected);
}

void Frame::onClientConnected()
{
    m_layout->replaceWidget(m_connectWindow, m_mainWindow);
    this->resize(640, 480);
}

void Frame::mousePressEvent(QMouseEvent *e)
{
    m_old_pos = e->pos();
    m_mouse_down = e->button() == Qt::LeftButton;
}

void Frame::mouseMoveEvent(QMouseEvent *e)
{
    int x = e->x();
    int y = e->y();

    if (m_mouse_down)
    {
        int dx = x - m_old_pos.x();
        int dy = y - m_old_pos.y();

        QRect g = geometry();

        if (left)
            g.setLeft(g.left() + dx);
        if (right)
            g.setRight(g.right() + dx);
        if(top)
            g.setTop(g.top() + dy);
        if (bottom)
            g.setBottom(g.bottom() + dy);

        setGeometry(g);

        m_old_pos = QPoint(!left ? e->x() : m_old_pos.x(), e->y());
    }
    else
    {
        QRect r = rect();
        left = qAbs(x - r.left()) <= 5;
        right = qAbs(x - r.right()) <= 5;
        top = qAbs(y - r.top()) <= 5;
        bottom = qAbs(y - r.bottom()) <= 5;
        bool hor = left | right;

        if (hor && bottom)
        {
            if (left)
                setCursor(Qt::SizeBDiagCursor);
            else
                setCursor(Qt::SizeFDiagCursor);
        }
        else if (top && bottom)
        {
            if (left)
                setCursor(Qt::SizeFDiagCursor);
            else
                setCursor(Qt::SizeBDiagCursor);
        }
        else if (hor)
        {
            setCursor(Qt::SizeHorCursor);
        }
        else if (bottom || top)
        {
            setCursor(Qt::SizeVerCursor);
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
    }
}

void Frame::mouseReleaseEvent(QMouseEvent *e)
{
    m_mouse_down = false;
}
