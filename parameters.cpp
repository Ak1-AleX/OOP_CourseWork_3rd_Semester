#include "parameters.h"
#include <QtMath>
#include <QDebug>
#include <QPointF>

Parameters::Parameters(QObject *parent)
    : QObject(parent)
    , m_fieldWidth(DEFAULT_FIELD_WIDTH)
    , m_fieldHeight(DEFAULT_FIELD_HEIGHT)
    , m_ballRadius(DEFAULT_BALL_RADIUS)
    , m_ballSpeed(DEFAULT_BALL_SPEED)
    , m_ballAngle(DEFAULT_BALL_ANGLE)
{
}

void Parameters::setFieldWidth(int width)
{
    if (!validateFieldSize(width, m_fieldHeight)) {
        emit validationError(QString("Ширина должна быть от %1 до %2 пикселей")
                                 .arg(MIN_FIELD_SIZE).arg(MAX_FIELD_SIZE));
        return;
    }

    if (m_fieldWidth != width) {
        m_fieldWidth = width;
        emit fieldWidthChanged(width);
        emit parametersChanged();
    }
}

void Parameters::setFieldHeight(int height)
{
    if (!validateFieldSize(m_fieldWidth, height)) {
        emit validationError(QString("Высота должна быть от %1 до %2 пикселей")
                                 .arg(MIN_FIELD_SIZE).arg(MAX_FIELD_SIZE));
        return;
    }

    if (m_fieldHeight != height) {
        m_fieldHeight = height;
        emit fieldHeightChanged(height);
        emit parametersChanged();
    }
}

void Parameters::setBallRadius(int radius)
{
    if (!validateBallRadius(radius)) {
        emit validationError(QString("Радиус должен быть от %1 до %2 пикселей")
                                 .arg(MIN_BALL_RADIUS).arg(MAX_BALL_RADIUS));
        return;
    }

    // Дополнительная проверка: шарик должен помещаться в площадку
    if (radius * 2 > m_fieldWidth || radius * 2 > m_fieldHeight) {
        emit validationError("Шарик слишком большой для указанных размеров площадки");
        return;
    }

    if (m_ballRadius != radius) {
        m_ballRadius = radius;
        emit ballRadiusChanged(radius);
        emit parametersChanged();
    }
}

void Parameters::setBallSpeed(int speed)
{
    if (!validateBallSpeed(speed)) {
        emit validationError(QString("Скорость должна быть от %1 до %2 пикс/сек")
                                 .arg(MIN_SPEED).arg(MAX_SPEED));
        return;
    }

    if (m_ballSpeed != speed) {
        m_ballSpeed = speed;
        emit ballSpeedChanged(speed);
        emit parametersChanged();
    }
}

void Parameters::setBallAngle(int angle)
{
    if (!validateBallAngle(angle)) {
        emit validationError(QString("Угол должен быть от %1 до %2 градусов")
                                 .arg(MIN_ANGLE).arg(MAX_ANGLE));
        return;
    }

    if (m_ballAngle != angle) {
        m_ballAngle = angle;
        emit ballAngleChanged(angle);
        emit parametersChanged();
    }
}

bool Parameters::applyParameters(int width, int height, int radius, int speed, int angle)
{
    // Проверка всех параметров перед установкой
    if (!validateFieldSize(width, height) ||
        !validateBallRadius(radius) ||
        !validateBallSpeed(speed) ||
        !validateBallAngle(angle)) {
        return false;
    }

    // Дополнительная проверка на размер шарика
    if (radius * 2 > width || radius * 2 > height) {
        emit validationError("Шарик слишком большой для указанных размеров площадки");
        return false;
    }

    bool changed = false;

    if (m_fieldWidth != width) {
        m_fieldWidth = width;
        emit fieldWidthChanged(width);
        changed = true;
    }

    if (m_fieldHeight != height) {
        m_fieldHeight = height;
        emit fieldHeightChanged(height);
        changed = true;
    }

    if (m_ballRadius != radius) {
        m_ballRadius = radius;
        emit ballRadiusChanged(radius);
        changed = true;
    }

    if (m_ballSpeed != speed) {
        m_ballSpeed = speed;
        emit ballSpeedChanged(speed);
        changed = true;
    }

    if (m_ballAngle != angle) {
        m_ballAngle = angle;
        emit ballAngleChanged(angle);
        changed = true;
    }

    if (changed) {
        emit parametersChanged();
    }

    return true;
}

void Parameters::resetToDefaults()
{
    applyParameters(DEFAULT_FIELD_WIDTH, DEFAULT_FIELD_HEIGHT,
                    DEFAULT_BALL_RADIUS, DEFAULT_BALL_SPEED, DEFAULT_BALL_ANGLE);
}

bool Parameters::isValid() const
{
    return validateFieldSize(m_fieldWidth, m_fieldHeight) &&
           validateBallRadius(m_ballRadius) &&
           validateBallSpeed(m_ballSpeed) &&
           validateBallAngle(m_ballAngle) &&
           (m_ballRadius * 2 <= m_fieldWidth) &&
           (m_ballRadius * 2 <= m_fieldHeight);
}

QString Parameters::validate() const
{
    if (!validateFieldSize(m_fieldWidth, m_fieldHeight)) {
        return QString("Размеры площадки должны быть от %1x%1 до %2x%2 пикселей")
            .arg(MIN_FIELD_SIZE).arg(MAX_FIELD_SIZE);
    }

    if (!validateBallRadius(m_ballRadius)) {
        return QString("Радиус шарика должен быть от %1 до %2 пикселей")
            .arg(MIN_BALL_RADIUS).arg(MAX_BALL_RADIUS);
    }

    if (m_ballRadius * 2 > m_fieldWidth || m_ballRadius * 2 > m_fieldHeight) {
        return "Шарик слишком большой для указанных размеров площадки";
    }

    if (!validateBallSpeed(m_ballSpeed)) {
        return QString("Скорость должна быть от %1 до %2 пикс/сек")
            .arg(MIN_SPEED).arg(MAX_SPEED);
    }

    if (!validateBallAngle(m_ballAngle)) {
        return QString("Угол должен быть от %1 до %2 градусов")
            .arg(MIN_ANGLE).arg(MAX_ANGLE);
    }

    return QString(); // Нет ошибок
}

QPointF Parameters::calculateVelocity() const
{
    // Конвертируем угол в радианы
    qreal radianAngle = qDegreesToRadians(static_cast<qreal>(m_ballAngle));

    // Рассчитываем компоненты скорости
    qreal vx = m_ballSpeed * qCos(radianAngle);
    qreal vy = m_ballSpeed * qSin(radianAngle);

    return QPointF(vx, vy);
}

bool Parameters::validateFieldSize(int width, int height) const
{
    return (width >= MIN_FIELD_SIZE && width <= MAX_FIELD_SIZE &&
            height >= MIN_FIELD_SIZE && height <= MAX_FIELD_SIZE);
}

bool Parameters::validateBallRadius(int radius) const
{
    return (radius >= MIN_BALL_RADIUS && radius <= MAX_BALL_RADIUS);
}

bool Parameters::validateBallSpeed(int speed) const
{
    return (speed >= MIN_SPEED && speed <= MAX_SPEED);
}

bool Parameters::validateBallAngle(int angle) const
{
    return (angle >= MIN_ANGLE && angle <= MAX_ANGLE);
}
