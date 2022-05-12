#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server s;
    s.connectToDatabase();
    s.addChatHistoryTable();
    return a.exec();
}
