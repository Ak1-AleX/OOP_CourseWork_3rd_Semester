#ifndef BALL_H
#define BALL_H

#include <QObject>
#include <QPointF>
#include <QColor>
#include <QRectF>

class Ball : public QObject
{
    Q_OBJECT
public:
    explicit Ball(QObject *parent = nullptr);

    // Геттеры
    QPointF position() const { return m_position; }
    QPointF velocity() const { return m_velocity; }
    int radius() const { return m_radius; }
    QColor color() const { return m_color; }
    QRectF boundingRect() const;

    // Сеттеры
    void setPosition(const QPointF &position);
    void setVelocity(const QPointF &velocity);
    void setRadius(int radius);
    void setColor(const QColor &color);

    // Методы движения
    void move(qreal deltaTime);
    void resetPosition(const QPointF &center);

    // Столкновения
    bool checkWallCollision(const QRectF &boundary);
    void handleWallCollision(const QRectF &boundary);

signals:
    void positionChanged();
    void velocityChanged();

private:
    QPointF m_position;
    QPointF m_velocity;
    int m_radius;
    QColor m_color;
};

#endif // BALL_H
