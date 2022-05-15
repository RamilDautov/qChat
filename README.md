# qChat <img src="client/images/icon.png" alt="icon" width="30"/>

Simple client/server chat app with messages history saved in SQLite database.

Assets for Windows and Linux can be downloaded from [here](https://github.com/RamilDautov/qChat/releases/tag/v1.0.0)

## How to use

Database with chat history should be placed near server executable file. Or you can change path to the database in `server/server.cpp`:

```c++
...

void Server::connectToDatabase()
{
    m_msgHistoryDB = QSqlDatabase::addDatabase("QSQLITE");
    QString pathToDB = QString("YOUR\\PATH\\TO\\DB\\chathistorydb.sqlite");

    ...
}

...
```

By default, server address is `127.0.0.1:2323`. You can change it in `client/chatwindow.cpp`:

```c++
...

bool ChatWindow::connectToServer()
{
    m_socket->connectToHost("127.0.0.1", 2323);
    m_socket->waitForConnected(3000);
    return (m_socket->state() == QAbstractSocket::ConnectedState);
}

...
```
