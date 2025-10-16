// защита от повторного включения файла
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// область пространства имен Qt
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow; // Предварительное объявление класса
}
QT_END_NAMESPACE

// класс главного окна (наследуется от QMainWindow)
class MainWindow : public QMainWindow
{
    Q_OBJECT // Макрос для сигналов/слотов

public:
    MainWindow(QWidget *parent = nullptr); // конструктор для дефолтного создания объекта класса MainWindow
    ~MainWindow(); // деструктор

private:
    Ui::MainWindow *ui; // Указатель на объект интерфейса
};
#endif // MAINWINDOW_H
