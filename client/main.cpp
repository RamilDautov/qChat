#include "chatwindow.h"
#include "connectwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ConnectWindow connectWindow;
    ChatWindow chatWindow;

    connectWindow.show();
    connectWindow.resize(480, 320);

    connectWindow.connectFunc( [&](const QString &userName){
        chatWindow.setUserName(userName);

        bool connected = chatWindow.connectToServer();

        // if there are some click() events in event loop
        // process them
        QApplication::processEvents();

        return connected;
    });

    return a.exec();
}
