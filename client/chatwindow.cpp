#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "titlebar.h"
#include <QFile>


static QString readTextFile(const QString &path)
{
    QFile file(path);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        return in.readAll();
    }

    return "";
}

ChatWindow::ChatWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatWindow)
{
    // set style sheet
    QString css = readTextFile("://styles/chatwindow.css");
    this->setStyleSheet(css);

    m_mouse_down = false;

    // Make this a borderless window which can't
    // be resized or moved via the window system
    setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking(true);

    TitleBar *titleBar = new TitleBar(this, true);
    titleBar->setObjectName("titleBar");
    titleBar->setMouseTracking(true);

    ui->setupUi(this);

    ui->verticalLayout->setContentsMargins(0,0,0,3);
    ui->horizontalLayout->setContentsMargins(10, 0, 10, 0);

    ui->verticalLayout->insertWidget(0, titleBar);

    m_statusBar = new QStatusBar;
    ui->verticalLayout->addWidget(m_statusBar);

    m_socket = new QTcpSocket(this);

    // change color of placeholder
    connect(ui->msgLineEdit, &QLineEdit::textChanged, [=]{ style()->polish(ui->msgLineEdit); });

    connect(m_socket, &QTcpSocket::connected, this, [&]{ show(); });
    connect(m_socket, &QTcpSocket::readyRead, this, &ChatWindow::slotReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &ChatWindow::onDisconnected);
}

ChatWindow::~ChatWindow()
{
    m_socket->disconnectFromHost();
    delete ui;
}

void ChatWindow::onDisconnected()
{
    m_socket->deleteLater();

    ui->msgLineEdit->setDisabled(true);
    ui->sendButton->setDisabled(true);
    m_statusBar->showMessage(tr("Connection lost! Please check your connection and restart the app"));
}

void ChatWindow::setUserName(const QString &name)
{
    m_userName = name;
}

bool ChatWindow::connectToServer()
{
    m_socket->connectToHost("127.0.0.1", 2323);
    m_socket->waitForConnected(3000);
    return (m_socket->state() == QAbstractSocket::ConnectedState);
}

void ChatWindow::sendToServer(const QString &msg)
{
    if(!m_socket->isOpen())
        return;

    m_data.clear();
    QDataStream out(&m_data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);

    out << quint16(0) << QTime::currentTime().toString() << msg << m_userName;
    out.device()->seek(0);
    out << quint16(m_data.size() - sizeof (quint16));
    m_socket->write(m_data);

    ui->msgLineEdit->clear();
}

void ChatWindow::slotReadyRead()
{
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_5_9);

    if(in.status() == QDataStream::Ok)
    {
        for(;;)
        {
            if(m_nextBlockSize == 0)
            {
                if(m_socket->bytesAvailable() < 2)
                {
                    break;
                }
                in >> m_nextBlockSize;
            }
            if(m_socket->bytesAvailable() < m_nextBlockSize)
            {
                break;
            }
            QString time, msg, userName;
            in >> time >> msg >> userName;

            m_nextBlockSize = 0;

            ui->textBrowser->append("<p style=\"background-color:#2a4f6a;margin-top:2px;font-size:10pt;\"><b>&nbsp;"
                                    + userName + "</b><br/>&nbsp;" + msg + "<br/><i style=\"font-size:8pt;\">&nbsp;"
                                    + time + "</i></p>");

            // move slider down to show the last message
            ui->textBrowser->verticalScrollBar()->setSliderPosition(ui->textBrowser->verticalScrollBar()->maximum());
        }
    }
    else
    {
        ui->textBrowser->append("Error: read error");
    }
}

void ChatWindow::on_sendButton_clicked()
{
    QString msg = ui->msgLineEdit->text();

    if(msg.size() > 0)
        sendToServer(msg);
}

void ChatWindow::on_msgLineEdit_returnPressed()
{
    QString msg = ui->msgLineEdit->text();

    if(msg.size() > 0)
        sendToServer(msg);
}


void ChatWindow::mousePressEvent(QMouseEvent *e)
{
    m_old_pos = e->pos();
    m_mouse_down = e->button() == Qt::LeftButton;
}

void ChatWindow::mouseMoveEvent(QMouseEvent *e)
{
    int x = e->x();
    int y = e->y();

    if (m_mouse_down)
    {
        qDebug() << "here";
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
        else if (hor && top)
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

void ChatWindow::mouseReleaseEvent(QMouseEvent *e)
{
    m_mouse_down = false;
}
