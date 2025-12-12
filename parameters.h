#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QObject>
#include <QSize>

class Parameters : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int fieldWidth READ fieldWidth WRITE setFieldWidth NOTIFY fieldWidthChanged)
    Q_PROPERTY(int fieldHeight READ fieldHeight WRITE setFieldHeight NOTIFY fieldHeightChanged)
    Q_PROPERTY(int ballRadius READ ballRadius WRITE setBallRadius NOTIFY ballRadiusChanged)
    Q_PROPERTY(int ballSpeed READ ballSpeed WRITE setBallSpeed NOTIFY ballSpeedChanged)
    Q_PROPERTY(int ballAngle READ ballAngle WRITE setBallAngle NOTIFY ballAngleChanged)

public:
    // Константы по умолчанию (из задания)
    static constexpr int DEFAULT_FIELD_WIDTH = 800;
    static constexpr int DEFAULT_FIELD_HEIGHT = 600;
    static constexpr int DEFAULT_BALL_RADIUS = 25;
    static constexpr int DEFAULT_BALL_SPEED = 20;
    static constexpr int DEFAULT_BALL_ANGLE = 45;

    // Ограничения
    static constexpr int MIN_FIELD_SIZE = 100;
    static constexpr int MAX_FIELD_SIZE = 2000;
    static constexpr int MIN_BALL_RADIUS = 5;
    static constexpr int MAX_BALL_RADIUS = 100;
    static constexpr int MIN_SPEED = 1;
    static constexpr int MAX_SPEED = 800;
    static constexpr int MIN_ANGLE = 0;
    static constexpr int MAX_ANGLE = 360;

    explicit Parameters(QObject *parent = nullptr);

    // Геттеры
    int fieldWidth() const { return m_fieldWidth; }
    int fieldHeight() const { return m_fieldHeight; }
    int ballRadius() const { return m_ballRadius; }
    int ballSpeed() const { return m_ballSpeed; }
    int ballAngle() const { return m_ballAngle; }
    QSize fieldSize() const { return QSize(m_fieldWidth, m_fieldHeight); }

    // Сеттеры с валидацией
    void setFieldWidth(int width);
    void setFieldHeight(int height);
    void setBallRadius(int radius);
    void setBallSpeed(int speed);
    void setBallAngle(int angle);

    // Пакетная установка параметров
    bool applyParameters(int width, int height, int radius, int speed, int angle);

    // Сброс к значениям по умолчанию
    void resetToDefaults();

    // Валидация
    bool isValid() const;
    QString validate() const;

    // Расчет скорости по углу (для симуляции)
    QPointF calculateVelocity() const;

signals:
    void fieldWidthChanged(int width);
    void fieldHeightChanged(int height);
    void ballRadiusChanged(int radius);
    void ballSpeedChanged(int speed);
    void ballAngleChanged(int angle);
    void parametersChanged();
    void validationError(const QString &message);

private:
    int m_fieldWidth;
    int m_fieldHeight;
    int m_ballRadius;
    int m_ballSpeed;
    int m_ballAngle;

    bool validateFieldSize(int width, int height) const;
    bool validateBallRadius(int radius) const;
    bool validateBallSpeed(int speed) const;
    bool validateBallAngle(int angle) const;
};

#endif // PARAMETERS_H
