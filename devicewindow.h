#ifndef DEVICEWINDOW_H
#define DEVICEWINDOW_H

#include <QWidget>
#include <QSize>

class Simulation;

class DeviceWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceWindow(QWidget *parent = nullptr);

    void setSimulation(Simulation *simulation);

    // Переопределяем sizeHint для лучшего управления размерами
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Simulation *m_simulation;
};

#endif
