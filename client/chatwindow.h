#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QStatusBar>
#include <QTcpSocket>

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();

    void setUserName(const QString &name);
    bool connectToServer();
    QTcpSocket *socket() const { return m_socket; };

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    Ui::ChatWindow *ui;
    QStatusBar *m_statusBar;
    QTcpSocket *m_socket;
    QByteArray m_data;
    quint16 m_nextBlockSize{0};
    QString m_userName{};

    QPoint m_old_pos;
    bool m_mouse_down;
    bool left, right, top, bottom;

    void sendToServer(const QString &msg);

    void onDisconnected();

private slots:
    void on_sendButton_clicked();

    void on_msgLineEdit_returnPressed();

public slots:
    void slotReadyRead();
};

#endif // CHATWINDOW_H
