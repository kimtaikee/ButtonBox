#include "testbuttonbox.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    TestButtonBox win;
    win.show();
    return app.exec();
}
