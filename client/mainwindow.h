#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTime>
#include "titlebar.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setUserName(const QString &name);
    bool connectToServer();
    QTcpSocket *socket() const { return m_socket; };

private:
    Ui::MainWindow *ui;
    QTcpSocket *m_socket;
    QByteArray m_data;
    quint16 m_nextBlockSize{0};
    QString m_userName{};

    void sendToServer(const QString &msg);

    void onDisconnected();

private slots:
    void on_sendButton_clicked();

    void on_msgLineEdit_returnPressed();

public slots:
    void slotReadyRead();
};
#endif // MAINWINDOW_H
