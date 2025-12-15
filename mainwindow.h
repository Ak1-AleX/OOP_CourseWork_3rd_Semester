#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>

// Предварительные объявления
class Simulation;
class DeviceWindow;
class Parameters;
class Events;
class Ball;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Слоты для кнопок параметров
    void on_accept_button_clicked();
    void on_reset_button_clicked();

    // Слоты для кнопок управления
    void on_start_button_clicked();
    void on_pause_button_clicked();
    void on_stop_button_clicked();

    // Слоты для обновления UI
    void updateUI();
    void onSimulationStateChanged(int state);
    void onParametersChanged();
    void onValidationError(const QString &message);

    // Слоты для спинбоксов
    void on_field_width_changed(int value);
    void on_field_height_changed(int value);
    void on_ball_radius_changed(int value);
    void on_ball_speed_changed(int value);
    void on_ball_angle_changed(int value);

private:
    Ui::MainWindow *ui;
    Simulation *m_simulation;
    DeviceWindow *m_deviceWindow;

    void setupUI();
    void setupConnections();
    void setupDeviceWindow();
    void updateParametersFromUI();
    void updateUIFromParameters();
    void updateControlsState();

    void showStatusMessage(const QString &message, bool isError = false);
    QString getStateString(int state);
};

#endif
