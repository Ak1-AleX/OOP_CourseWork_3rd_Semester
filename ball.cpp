#include "ball.h"
#include <QDebug>
#include <QtMath>

Ball::Ball(QObject *parent)
    : QObject(parent)
    , m_position(0, 0)
    , m_velocity(0, 0)
    , m_radius(25)
    , m_color(Qt::red)
{
}

void Ball::setPosition(const QPointF &position)
{
    if (m_position != position) {
        m_position = position;
        emit positionChanged();
    }
}

void Ball::setVelocity(const QPointF &velocity)
{
    if (m_velocity != velocity) {
        m_velocity = velocity;
        emit velocityChanged();
    }
}

void Ball::setRadius(int radius)
{
    if (m_radius != radius && radius > 0) {
        m_radius = radius;
        emit positionChanged(); // Изменение рад
    }
}

void Ball::setColor(const QColor &color)
{
    if (m_color != color) {
        m_color = color;
    }
}

QRectF Ball::boundingRect() const
{
    return QRectF(m_position.x() - m_radius,
                  m_position.y() - m_radius,
                  m_radius * 2,
                  m_radius * 2);
}

void Ball::move(qreal deltaTime)
{
    m_position += m_velocity * deltaTime;
    emit positionChanged();
}

void Ball::resetPosition(const QPointF &center)
{
    setPosition(center);
    emit positionChanged();
}

bool Ball::checkWallCollision(const QRectF &boundary)
{
    QRectF ballRect = boundingRect();
    return (ballRect.left() <= boundary.left() ||
            ballRect.right() >= boundary.right() ||
            ballRect.top() <= boundary.top() ||
            ballRect.bottom() >= boundary.bottom());
}

void Ball::handleWallCollision(const QRectF &boundary)
{
    QRectF ballRect = boundingRect();

    // Проверка столкновения с стенк l/r
    if (ballRect.left() <= boundary.left() ||
        ballRect.right() >= boundary.right()) {
        m_velocity.setX(-m_velocity.x()); // горизонт скорость
        // Корректм поз, чтобы шарик не застрял в стене
        if (ballRect.left() <= boundary.left()) {
            m_position.setX(boundary.left() + m_radius);
        } else {
            m_position.setX(boundary.right() - m_radius);
        }
    }

    // Проверка столкновения с u/d стенкой
    if (ballRect.top() <= boundary.top() ||
        ballRect.bottom() >= boundary.bottom()) {
        m_velocity.setY(-m_velocity.y()); // Инвертируем вертикальную скорость
        // Корректируем позицию
        if (ballRect.top() <= boundary.top()) {
            m_position.setY(boundary.top() + m_radius);
        } else {
            m_position.setY(boundary.bottom() - m_radius);
        }
    }

    emit velocityChanged();
    emit positionChanged();
}
