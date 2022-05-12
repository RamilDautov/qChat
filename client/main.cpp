#include "mainwindow.h"
#include "connectwindow.h"
#include "frame.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Frame box;
    box.resize(480, 320);

    box.show();
    return a.exec();
}
