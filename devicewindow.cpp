#include "devicewindow.h"
#include "simulation.h"
#include "ball.h"
#include "parameters.h"
#include <QPainter>
#include <QDebug>
#include <QtMath>

DeviceWindow::DeviceWindow(QWidget *parent)
    : QWidget(parent)
    , m_simulation(nullptr)
{
    // Устанавливаем Expanding размерную политику
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Устанавливаем белый фон
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

void DeviceWindow::setSimulation(Simulation *simulation)
{
    m_simulation = simulation;

    if (m_simulation) {
        // Подключаем сигналы для обновления отрисовки
        connect(m_simulation, &Simulation::simulationUpdated,
                this, QOverload<>::of(&QWidget::update));
        connect(m_simulation->parameters(), &Parameters::parametersChanged,
                this, QOverload<>::of(&QWidget::update));
    }

    update();
}

void DeviceWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Очищаем фон
    painter.fillRect(rect(), QColor(240, 240, 240));

    if (!m_simulation) {
        // Если симуляция не установлена
        painter.setPen(Qt::darkGray);
        painter.setFont(QFont("Arial", 12));
        painter.drawText(rect(), Qt::AlignCenter, "DeviceWindow\n(Ожидание инициализации симуляции)");
        return;
    }

    // Получаем параметры
    Parameters *params = m_simulation->parameters();
    Ball *ball = m_simulation->ball();

    if (!params || !ball) {
        painter.setPen(Qt::darkGray);
        painter.drawText(rect(), Qt::AlignCenter, "Нет данных для отрисовки");
        return;
    }

    // Получаем реальные размеры поля
    int fieldWidth = params->fieldWidth();
    int fieldHeight = params->fieldHeight();

    if (fieldWidth <= 0 || fieldHeight <= 0) {
        painter.setPen(Qt::red);
        painter.drawText(rect(), Qt::AlignCenter, "Неверные размеры поля");
        return;
    }

    // Вычисляем масштаб с сохранением пропорций
    qreal scaleX = width() / static_cast<qreal>(fieldWidth);
    qreal scaleY = height() / static_cast<qreal>(fieldHeight);
    qreal scale = qMin(scaleX, scaleY);

    // Если поле слишком большое для отображения, уменьшаем масштаб
    if (scale > 1.5) scale = 1.5; // Максимальный zoom

    // Центрируем поле
    qreal scaledWidth = fieldWidth * scale;
    qreal scaledHeight = fieldHeight * scale;
    qreal offsetX = (width() - scaledWidth) / 2.0;
    qreal offsetY = (height() - scaledHeight) / 2.0;

    // Рисуем фон поля
    QRectF fieldRect(offsetX, offsetY, scaledWidth, scaledHeight);
    painter.setBrush(QBrush(Qt::white));
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(fieldRect);

    // Рисуем сетку (опционально, для наглядности)
    painter.setPen(QPen(QColor(220, 220, 220), 1));

    // Вертикальные линии
    int gridLines = 5;
    for (int i = 1; i < gridLines; i++) {
        qreal x = offsetX + (scaledWidth / gridLines) * i;
        painter.drawLine(QPointF(x, offsetY), QPointF(x, offsetY + scaledHeight));
    }

    // Горизонтальные линии
    for (int i = 1; i < gridLines; i++) {
        qreal y = offsetY + (scaledHeight / gridLines) * i;
        painter.drawLine(QPointF(offsetX, y), QPointF(offsetX + scaledWidth, y));
    }

    // Рисуем центр поля
    painter.setPen(QPen(Qt::blue, 3));
    QPointF center(offsetX + scaledWidth / 2, offsetY + scaledHeight / 2);
    painter.drawEllipse(center, 3, 3);

    // Рисуем шарик
    QPointF ballPos = ball->position();
    qreal ballRadius = ball->radius();

    // Проверяем, что позиция шарика валидна
    if (ballPos.x() >= 0 && ballPos.x() <= fieldWidth &&
        ballPos.y() >= 0 && ballPos.y() <= fieldHeight) {

        // Масштабируем позицию шарика
        QPointF scaledPos(
            offsetX + ballPos.x() * scale,
            offsetY + ballPos.y() * scale
            );

        qreal scaledBallRadius = ballRadius * scale;

        // Ограничиваем минимальный размер шарика для видимости
        if (scaledBallRadius < 3) scaledBallRadius = 3;

        // Рисуем шарик с градиентом
        QRadialGradient gradient(scaledPos, scaledBallRadius);
        gradient.setColorAt(0, Qt::yellow);
        gradient.setColorAt(0.7, Qt::red);
        gradient.setColorAt(1, Qt::darkRed);

        painter.setBrush(QBrush(gradient));
        painter.setPen(QPen(Qt::darkRed, 2));
        painter.drawEllipse(scaledPos, scaledBallRadius, scaledBallRadius);

        // Рисуем центр шарика
        painter.setPen(QPen(Qt::white, 2));
        painter.drawPoint(scaledPos);

        // Рисуем направление движения (если симуляция запущена)
        if (m_simulation->events()->isRunning()) {
            QPointF velocity = ball->velocity();
            qreal speed = qSqrt(velocity.x() * velocity.x() + velocity.y() * velocity.y());

            if (speed > 0.1) {
                // Нормализуем вектор скорости
                QPointF direction(velocity.x() / speed, velocity.y() / speed);
                direction *= scaledBallRadius * 1.5;

                painter.setPen(QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap));
                painter.drawLine(scaledPos, scaledPos + direction);

                // Рисуем стрелочку
                painter.setBrush(Qt::green);
                qreal angle = qAtan2(direction.y(), direction.x());
                QPointF arrowP1 = scaledPos + direction;
                QPointF arrowP2 = scaledPos + direction -
                                  QPointF(qCos(angle - M_PI/6) * 8,
                                          qSin(angle - M_PI/6) * 8);
                QPointF arrowP3 = scaledPos + direction -
                                  QPointF(qCos(angle + M_PI/6) * 8,
                                          qSin(angle + M_PI/6) * 8);

                QPolygonF arrow;
                arrow << arrowP1 << arrowP2 << arrowP3;
                painter.drawPolygon(arrow);
            }
        }

        // Отображаем координаты
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 9));
        QString posText = QString("(%1, %2)")
                              .arg(ballPos.x(), 0, 'f', 1)
                              .arg(ballPos.y(), 0, 'f', 1);
        painter.drawText(scaledPos + QPointF(scaledBallRadius + 5, 0), posText);
    }

    // Отображаем информацию о масштабе
    painter.setPen(Qt::darkGray);
    painter.setFont(QFont("Arial", 8));
    QString scaleText = QString("Масштаб: 1:%1 | Поле: %2×%3")
                            .arg(1.0/scale, 0, 'f', 2)
                            .arg(fieldWidth)
                            .arg(fieldHeight);
    painter.drawText(5, 15, scaleText);
}

// Переопределяем sizeHint для задания предпочтительного размера
QSize DeviceWindow::sizeHint() const
{
    return QSize(500, 400); // Предпочтительный размер
}

QSize DeviceWindow::minimumSizeHint() const
{
    return QSize(200, 150); // Минимальный размер
}
