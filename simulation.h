#ifndef SIMULATION_H
#define SIMULATION_H

#include <QObject>
#include <QTimer>
#include <QRectF>
#include "ball.h"
#include "parameters.h"
#include "events.h"

class Simulation : public QObject
{
    Q_OBJECT
public:
    explicit Simulation(QObject *parent = nullptr);
    ~Simulation();

    // Управление симуляцией
    void start();
    void pause();
    void stop();
    void reset();

    // Получение компонентов
    Ball* ball() { return &m_ball; }
    Parameters* parameters() { return m_parameters; }
    Events* events() { return m_events; }

    // Границы площадки
    QRectF fieldBoundary() const;

signals:
    void simulationUpdated();
    void simulationStarted();
    void simulationStopped();
    void simulationReset();

private slots:
    void update();
    void onParametersChanged();
    void onEventsStateChanged(Events::State state);
    void onResetRequested();

private:
    QTimer m_timer;
    Ball m_ball;
    Parameters* m_parameters;
    Events* m_events;

    void setupConnections();
    void applyParametersToBall();
    void resetBallToCenter();
    void updateBallVelocity();
};

#endif // SIMULATION_H
