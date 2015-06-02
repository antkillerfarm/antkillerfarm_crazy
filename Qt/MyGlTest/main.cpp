#include "window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    printf("send four parameter, first picture formal, second execture time, third pthread num(no beyond 20)\n");

    QApplication a(argc, argv);

    for(int i = 0; i < argc; i++)
    {
        printf("order is %s\n", argv[i]);
    }

    printf("========================================\n");
    Window w(argv[1], argv[2], argv[3]);
    w.show();

    return a.exec();
}
