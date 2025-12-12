#include "simulation.h"
#include <QDebug>

Simulation::Simulation(QObject *parent)
    : QObject(parent)
    , m_parameters(new Parameters(this))
    , m_events(new Events(this))
{
    setupConnections();

    // Настройка таймера (60 FPS)
    m_timer.setInterval(16); // ~16ms = 60 FPS
    connect(&m_timer, &QTimer::timeout, this, &Simulation::update);

    // Инициализация шарика
    applyParametersToBall();
    resetBallToCenter();

    qDebug() << "Simulation created";
}

Simulation::~Simulation()
{
    m_timer.stop();
}

void Simulation::setupConnections()
{
    // Параметры → симуляция
    connect(m_parameters, &Parameters::parametersChanged,
            this, &Simulation::onParametersChanged);

    // События → симуляция
    connect(m_events, &Events::stateChanged,
            this, &Simulation::onEventsStateChanged);
    connect(m_events, &Events::resetRequested,
            this, &Simulation::onResetRequested);

    // Шарик → обновление отрисовки
    connect(&m_ball, &Ball::positionChanged,
            this, &Simulation::simulationUpdated);
    connect(&m_ball, &Ball::velocityChanged,
            this, &Simulation::simulationUpdated);
}

void Simulation::start()
{
    m_events->start();
}

void Simulation::pause()
{
    m_events->pause();
}

void Simulation::stop()
{
    m_events->stop();
}

void Simulation::reset()
{
    m_events->reset();
}

QRectF Simulation::fieldBoundary() const
{
    return QRectF(0, 0, m_parameters->fieldWidth(), m_parameters->fieldHeight());
}

void Simulation::update()
{
    if (!m_events->isRunning()) {
        return;
    }

    // Двигаем шарик
    m_ball.move(0.016); // dt для 60 FPS

    // Проверяем столкновения со стенками
    if (m_ball.checkWallCollision(fieldBoundary())) {
        m_ball.handleWallCollision(fieldBoundary());
    }

    emit simulationUpdated();
}

void Simulation::onParametersChanged()
{
    if (m_events->canModifyParameters()) {
        applyParametersToBall();
        resetBallToCenter();
    }
}

void Simulation::onEventsStateChanged(Events::State state)
{
    switch (state) {
    case Events::Running:
        m_timer.start();
        emit simulationStarted();
        break;
    case Events::Paused:
    case Events::Stopped:
        m_timer.stop();
        if (state == Events::Stopped) {
            emit simulationStopped();
        }
        break;
    }
}

void Simulation::onResetRequested()
{
    resetBallToCenter();
    emit simulationReset();
}

void Simulation::applyParametersToBall()
{
    m_ball.setRadius(m_parameters->ballRadius());
    updateBallVelocity();
}

void Simulation::resetBallToCenter()
{
    QPointF center(m_parameters->fieldWidth() / 2.0,
                   m_parameters->fieldHeight() / 2.0);
    m_ball.resetPosition(center);
}

void Simulation::updateBallVelocity()
{
    m_ball.setVelocity(m_parameters->calculateVelocity());
}
