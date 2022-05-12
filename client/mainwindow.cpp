#include "mainwindow.h"
#include "ui_mainwindow.h"
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    // set style sheet
    QString css = readTextFile("://styles/mainwindow.css");
    this->setStyleSheet(css);

    ui->setupUi(this);

    // change color of placeholder
    connect(ui->msgLineEdit, &QLineEdit::textChanged, [=]{ style()->polish(ui->msgLineEdit); });

    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);
}

MainWindow::~MainWindow()
{
    m_socket->disconnectFromHost();
    delete ui;
}

void MainWindow::onDisconnected()
{
    m_socket->deleteLater();

    ui->msgLineEdit->setDisabled(true);
    ui->sendButton->setDisabled(true);
    statusBar()->showMessage(tr("Connection lost! Please check your connection and restart the app"));
}

void MainWindow::setUserName(const QString &name)
{
    m_userName = name;
}

bool MainWindow::connectToServer()
{
    m_socket->connectToHost("127.0.0.1", 2323);
    m_socket->waitForConnected(3000);
    return (m_socket->state() == QAbstractSocket::ConnectedState);
}

void MainWindow::sendToServer(const QString &msg)
{
    if(!m_socket->isOpen())
        return;

    m_data.clear();
    QDataStream out(&m_data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);

    out << quint16(0) << QTime::currentTime().toString() << msg << m_userName;
    out.device()->seek(0);
    out << quint16(m_data.size() - sizeof (quint16));
    m_socket->write(m_data);

    ui->msgLineEdit->clear();
}

void MainWindow::slotReadyRead()
{
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_6_0);

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

            ui->textBrowser->append("<p style=\"background-color:#2a4f6a;margin-top:2px;font-size:10pt;\"><b>&nbsp;" + userName + "</b><br/>&nbsp;" + msg + "<br/><i style=\"font-size:8pt;\">&nbsp;" + time + "</i></p>");

            // move slider down to show the last message
            ui->textBrowser->verticalScrollBar()->setSliderPosition(ui->textBrowser->verticalScrollBar()->maximum());
        }
    }
    else
    {
        ui->textBrowser->append("read error");
    }
}

void MainWindow::on_sendButton_clicked()
{
    QString msg = ui->msgLineEdit->text();

    if(msg.size() > 0)
        sendToServer(msg);
}

void MainWindow::on_msgLineEdit_returnPressed()
{
    QString msg = ui->msgLineEdit->text();

    if(msg.size() > 0)
        sendToServer(msg);
}
