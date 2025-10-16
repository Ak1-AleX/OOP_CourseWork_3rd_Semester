#include "mainwindow.h"
#include "./ui_mainwindow.h"

// реализация конструктора для дефолтного создания объекта класса MainWindow
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this); // для настройки интерфейса
}

// реализация деструктора
MainWindow::~MainWindow()
{
    delete ui;
}
