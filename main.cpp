#include "dgipydrone.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DGIpydrOne w;
    w.show();

    return a.exec();
}
