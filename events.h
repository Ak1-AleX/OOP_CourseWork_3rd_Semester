#ifndef EVENTS_H
#define EVENTS_H

#include <QObject>

class Events : public QObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

public:
    enum State {
        Stopped,    // Симуляция остановлена, шарик в центре
        Running,    // Симуляция запущена
        Paused      // Симуляция на паузе
    };
    Q_ENUM(State)

    explicit Events(QObject *parent = nullptr);

    // Управление состоянием
    void start();
    void pause();
    void stop();
    void reset();

    // Проверки состояния
    State state() const { return m_state; }
    bool isRunning() const { return m_state == Running; }
    bool isStopped() const { return m_state == Stopped; }
    bool isPaused() const { return m_state == Paused; }

    // Блокировка изменения параметров
    bool canModifyParameters() const { return m_state == Stopped; }

signals:
    void started();
    void paused();
    void stopped();
    void resetRequested();
    void stateChanged(State state);
    void error(const QString &message);

private:
    State m_state;
};

#endif // EVENTS_H
