#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w; // объект главного окна
    w.show();
    return a.exec(); // обработка событий приложения
}
