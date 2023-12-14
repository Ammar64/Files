#include <QApplication>
#include "mainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Window mainWindow(nullptr);
    return a.exec();
}
 