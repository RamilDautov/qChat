#include "server.h"

Server::Server()
{
    if(this->listen(QHostAddress::Any, 2323))
    {
        qDebug() << "server started";
    }
    else
    {
        qDebug() << "error: server couldn't start";
    }
}

Server::~Server()
{
    m_msgHistoryDB.close();
}

void Server::connectToDatabase()
{
    m_msgHistoryDB = QSqlDatabase::addDatabase("QSQLITE");

    QString pathToDB = QString("D:\\Documents\\Chat\\server\\db\\chathistorydb.sqlite");
    m_msgHistoryDB.setDatabaseName(pathToDB);

    QFileInfo checkFile(pathToDB);
    if (checkFile.isFile())
    {
        if (m_msgHistoryDB.open())
        {
            qDebug() << "Connected to chat history database file";
        }
        else
        {
            qDebug() <<"Chat history database file does not exist";
        }
    }
}

void Server::addChatHistoryTable()
{
    QSqlQuery queryCreate(m_msgHistoryDB);
    queryCreate.exec("CREATE TABLE IF NOT EXISTS ChatHistory (Message text NOT NULL, Username text NOT NULL, Time text NOT NULL)");
}

void Server::addMsgToChatHistory(const QString& time, const QString& msg, const QString& userName)
{
    QSqlQuery query(m_msgHistoryDB);
    query.prepare("INSERT INTO ChatHistory (Message, Username, Time) VALUES (:Mes, :Username, :Time)");
    query.bindValue(":Mes", msg);
    query.bindValue(":Username", userName);
    query.bindValue(":Time", time);
    query.exec();
}

void Server::sendChatHistoryToUser(QTcpSocket *socket)
{
    QSqlQuery queryChat(m_msgHistoryDB);

    if(queryChat.exec("SELECT Message, Username, Time FROM ChatHistory"))
    {
        while (queryChat.next())
        {
            m_data.clear();
            QDataStream out(&m_data, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_6_0);
            out << quint16(0) << queryChat.value(2).toString() << queryChat.value(0).toString() << queryChat.value(1).toString();
            out.device()->seek(0);
            out << quint16(m_data.size() - sizeof (quint16));
            socket->write(m_data);
        }
    }
}

void Server::clearChatHistory()
{
    QSqlQuery query(m_msgHistoryDB);

    query.prepare("DELETE FROM ChatHistory");

    if(!query.exec())
        qDebug() << query.lastError();
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    m_socket = new QTcpSocket;
    m_socket->setSocketDescriptor(socketDescriptor);
    connect(m_socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &Server::onDisconnected);

    sendChatHistoryToUser(m_socket);

    m_sockets.insert(m_socket);
    qDebug() << "client connected: " << socketDescriptor;
}

void Server::onDisconnected()
{
    QTcpSocket* socketToDelete = qobject_cast<QTcpSocket*>(sender());
       if(socketToDelete)
       {
           m_sockets.remove(socketToDelete);
           socketToDelete->deleteLater();
       }
       else
       {
           // Handle error
           qDebug() << "error: socket to delete is empty";
       }
}

void Server::slotReadyRead()
{
    m_socket = (QTcpSocket*)sender();
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_6_0);

    if(in.status() == QDataStream::Ok)
    {
        qDebug() << "read...";
        for(;;)
        {
            if(m_nextBlockSize == 0)
            {
                qDebug() << "m_nextBlockSize == 0";
                if(m_socket->bytesAvailable() < 2)
                {
                    qDebug() << "data size < 2, break";
                    break;
                }
                in >> m_nextBlockSize;
                qDebug() << "m_nextBlockSize = " << m_nextBlockSize;
            }
            if(m_socket->bytesAvailable() < m_nextBlockSize)
            {
                qDebug() << "data is not full, break";
                break;
            }
            QString time, msg, userName;
            in >> time >> msg >> userName;
            qDebug() << time << " " << msg << " " << userName;
            m_nextBlockSize = 0;
            sendToClient(time, msg, userName);

            if (m_msgHistoryDB.open())
                addMsgToChatHistory(time, msg, userName);

            break;
        }
    }
    else
    {
        qDebug() << "Datastream read error";
    }
}

void Server::sendToClient(const QString& time, const QString& msg, const QString& userName)
{
    m_data.clear();
    QDataStream out(&m_data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << quint16(0) << time << msg << userName;
    out.device()->seek(0);
    out << quint16(m_data.size() - sizeof (quint16));

    for(const auto socket : m_sockets)
    {
        socket->write(m_data);
    }
}
