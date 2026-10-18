#include <QApplication>
#include "taskmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TaskManager w;
    w.show();

    return app.exec();
}
