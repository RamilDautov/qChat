#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QSet>
#include <QTime>
#include <QtSql>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server();
    ~Server();

    void connectToDatabase();
    void addChatHistoryTable();
    void clearChatHistory();

private:
    QTcpSocket *m_socket;
    QSet<QTcpSocket*> m_sockets;
    QByteArray m_data;
    quint16 m_nextBlockSize{0};

    QSqlDatabase m_msgHistoryDB;

    void addMsgToChatHistory(const QString& time, const QString& msg, const QString& userName);
    void sendChatHistoryToUser(QTcpSocket *socket);

    void sendToClient(const QString& time, const QString& msg, const QString& userName);

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void onDisconnected();
    void slotReadyRead();
};

#endif // SERVER_H
