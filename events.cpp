#include "events.h"
#include <QDebug>

Events::Events(QObject *parent)
    : QObject(parent)
    , m_state(Stopped)
{
}

void Events::start()
{
    if (m_state == Running) {
        emit error("Симуляция уже запущена");
        return;
    }

    m_state = Running;
    emit stateChanged(m_state);
    emit started();

    qDebug() << "Simulation started";
}

void Events::pause()
{
    if (m_state != Running) {
        emit error("Невозможно поставить на паузу: симуляция не запущена");
        return;
    }

    m_state = Paused;
    emit stateChanged(m_state);
    emit paused();

    qDebug() << "Simulation paused";
}

void Events::stop()
{
    if (m_state == Stopped) {
        return;
    }

    m_state = Stopped;
    emit stateChanged(m_state);
    emit stopped();

    qDebug() << "Simulation stopped";
}

void Events::reset()
{
    m_state = Stopped;
    emit stateChanged(m_state);
    emit resetRequested();
    emit stopped();

    qDebug() << "Simulation reset";
}
