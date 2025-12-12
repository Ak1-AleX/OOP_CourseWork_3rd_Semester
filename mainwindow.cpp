#include "mainwindow.h"
#include "ui_mainwindow.h"

// Включаем наши классы
#include "simulation.h"
#include "devicewindow.h"
#include "parameters.h"
#include "events.h"
#include "ball.h"

#include <QMessageBox>
#include <QDebug>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_simulation(nullptr)
    , m_deviceWindow(nullptr)
{
    ui->setupUi(this);

    // 1. Создаем симуляцию
    m_simulation = new Simulation(this);

    // 2. Настраиваем DeviceWindow внутри контейнера
    setupDeviceWindow();

    // 3. Настраиваем остальной UI
    setupUI();
    setupConnections();

    // 4. Инициализация UI из параметров
    updateUIFromParameters();
    updateUI();

    qDebug() << "MainWindow initialized with DeviceWindow in container";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupDeviceWindow()
{
    // Создаем DeviceWindow
    m_deviceWindow = new DeviceWindow(this);

    // Находим контейнер из .ui файла
    QWidget *container = ui->deviceContainer;

    if (container) {
        // Создаем layout для контейнера
        QVBoxLayout *containerLayout = new QVBoxLayout(container);
        containerLayout->setContentsMargins(0, 0, 0, 0); // Убираем отступы
        containerLayout->setSpacing(0);

        // Добавляем DeviceWindow в контейнер
        containerLayout->addWidget(m_deviceWindow);

        // Настраиваем DeviceWindow
        m_deviceWindow->setSimulation(m_simulation);

        // DeviceWindow будет растягиваться внутри контейнера
        m_deviceWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        qDebug() << "DeviceWindow created and added to container";
    } else {
        qWarning() << "Container 'deviceContainer' not found in UI!";
    }
}

void MainWindow::setupUI()
{
    // Устанавливаем диапазоны для спинбоксов из Parameters
    ui->input_area_width->setRange(Parameters::MIN_FIELD_SIZE, Parameters::MAX_FIELD_SIZE);
    ui->input_area_height->setRange(Parameters::MIN_FIELD_SIZE, Parameters::MAX_FIELD_SIZE);
    ui->input_radius_size->setRange(Parameters::MIN_BALL_RADIUS, Parameters::MAX_BALL_RADIUS);
    ui->input_speed_size->setRange(Parameters::MIN_SPEED, Parameters::MAX_SPEED);
    ui->input_angle_size->setRange(Parameters::MIN_ANGLE, Parameters::MAX_ANGLE);

    // Устанавливаем значения по умолчанию
    ui->input_area_width->setValue(Parameters::DEFAULT_FIELD_WIDTH);
    ui->input_area_height->setValue(Parameters::DEFAULT_FIELD_HEIGHT);
    ui->input_radius_size->setValue(Parameters::DEFAULT_BALL_RADIUS);
    ui->input_speed_size->setValue(Parameters::DEFAULT_BALL_SPEED);
    ui->input_angle_size->setValue(Parameters::DEFAULT_BALL_ANGLE);

    // Настраиваем статусбар
    ui->statusbar->setStyleSheet("QStatusBar { color: #333; }");
}

void MainWindow::setupConnections()
{
    // Кнопки параметров
    connect(ui->accept_button, &QPushButton::clicked,
            this, &MainWindow::on_accept_button_clicked);
    connect(ui->reset_button, &QPushButton::clicked,
            this, &MainWindow::on_reset_button_clicked);

    // Кнопки управления
    connect(ui->start_button, &QPushButton::clicked,
            this, &MainWindow::on_start_button_clicked);
    connect(ui->pause_button, &QPushButton::clicked,
            this, &MainWindow::on_pause_button_clicked);
    connect(ui->stop_button, &QPushButton::clicked,
            this, &MainWindow::on_stop_button_clicked);

    // Спинбоксы - теперь два спинбокса для размеров!
    connect(ui->input_area_width, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::on_field_width_changed);
    connect(ui->input_area_height, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::on_field_height_changed);
    connect(ui->input_radius_size, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::on_ball_radius_changed);
    connect(ui->input_speed_size, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::on_ball_speed_changed);
    connect(ui->input_angle_size, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::on_ball_angle_changed);

    // Симуляция → UI
    if (m_simulation) {
        connect(m_simulation->events(), &Events::stateChanged,
                this, &MainWindow::onSimulationStateChanged);
        connect(m_simulation->parameters(), &Parameters::parametersChanged,
                this, &MainWindow::onParametersChanged);
        connect(m_simulation->parameters(), &Parameters::validationError,
                this, &MainWindow::onValidationError);
    }

    // Обновление DeviceWindow
    if (m_simulation && m_deviceWindow) {
        connect(m_simulation, &Simulation::simulationUpdated,
                m_deviceWindow, QOverload<>::of(&QWidget::update));
    }
}

void MainWindow::on_accept_button_clicked()
{
    if (!m_simulation->events()->canModifyParameters()) {
        QMessageBox::warning(this, "Ошибка",
                             "Изменение параметров возможно только когда симуляция остановлена!");
        return;
    }

    updateParametersFromUI();
    showStatusMessage("Параметры применены успешно", false);
}

void MainWindow::on_reset_button_clicked()
{
    if (!m_simulation->events()->canModifyParameters()) {
        QMessageBox::warning(this, "Ошибка",
                             "Сброс параметров возможен только когда симуляция остановлена!");
        return;
    }

    m_simulation->parameters()->resetToDefaults();
    updateUIFromParameters();
    showStatusMessage("Параметры сброшены к значениям по умолчанию", false);
}

void MainWindow::on_start_button_clicked()
{
    m_simulation->start();
}

void MainWindow::on_pause_button_clicked()
{
    m_simulation->pause();
}

void MainWindow::on_stop_button_clicked()
{
    m_simulation->stop();

    // Добавьте эти 2 строки:
    QPointF center(m_simulation->parameters()->fieldWidth() / 2.0,
                   m_simulation->parameters()->fieldHeight() / 2.0);
    m_simulation->ball()->resetPosition(center);

    // Обновляем UI
    updateUI();

    // Принудительно перерисовываем
    if (m_deviceWindow) {
        m_deviceWindow->update();
    }

    showStatusMessage("Симуляция остановлена, шарик в центре", false);
}

void MainWindow::on_field_width_changed(int value)
{
    if (m_simulation->events()->canModifyParameters()) {
        m_simulation->parameters()->setFieldWidth(value);
    } else {
        // Возвращаем старое значение
        ui->input_area_width->blockSignals(true);
        ui->input_area_width->setValue(m_simulation->parameters()->fieldWidth());
        ui->input_area_width->blockSignals(false);

        QMessageBox::warning(this, "Ошибка",
                             "Изменение параметров возможно только когда симуляция остановлена!");
    }
}

void MainWindow::on_field_height_changed(int value)
{
    if (m_simulation->events()->canModifyParameters()) {
        m_simulation->parameters()->setFieldHeight(value);
    } else {
        // Возвращаем старое значение
        ui->input_area_height->blockSignals(true);
        ui->input_area_height->setValue(m_simulation->parameters()->fieldHeight());
        ui->input_area_height->blockSignals(false);

        QMessageBox::warning(this, "Ошибка",
                             "Изменение параметров возможно только когда симуляция остановлена!");
    }
}

void MainWindow::on_ball_radius_changed(int value)
{
    if (m_simulation->events()->canModifyParameters()) {
        m_simulation->parameters()->setBallRadius(value);
    } else {
        ui->input_radius_size->blockSignals(true);
        ui->input_radius_size->setValue(m_simulation->parameters()->ballRadius());
        ui->input_radius_size->blockSignals(false);
    }
}

void MainWindow::on_ball_speed_changed(int value)
{
    if (m_simulation->events()->canModifyParameters()) {
        m_simulation->parameters()->setBallSpeed(value);
    } else {
        ui->input_speed_size->blockSignals(true);
        ui->input_speed_size->setValue(m_simulation->parameters()->ballSpeed());
        ui->input_speed_size->blockSignals(false);
    }
}

void MainWindow::on_ball_angle_changed(int value)
{
    if (m_simulation->events()->canModifyParameters()) {
        m_simulation->parameters()->setBallAngle(value);
    } else {
        ui->input_angle_size->blockSignals(true);
        ui->input_angle_size->setValue(m_simulation->parameters()->ballAngle());
        ui->input_angle_size->blockSignals(false);
    }
}

void MainWindow::updateParametersFromUI()
{
    // Получаем значения из UI
    int width = ui->input_area_width->value();
    int height = ui->input_area_height->value();
    int radius = ui->input_radius_size->value();
    int speed = ui->input_speed_size->value();
    int angle = ui->input_angle_size->value();

    // Применяем параметры
    if (!m_simulation->parameters()->applyParameters(width, height, radius, speed, angle)) {
        // Если applyParameters вернула false, значит была ошибка валидации
        // Сообщение об ошибке уже было отправлено через сигнал validationError
        updateUIFromParameters(); // Возвращаем старые значения в UI
    }
}

void MainWindow::updateUIFromParameters()
{
    Parameters *params = m_simulation->parameters();

    // Блокируем сигналы, чтобы избежать рекурсии
    ui->input_area_width->blockSignals(true);
    ui->input_area_height->blockSignals(true);
    ui->input_radius_size->blockSignals(true);
    ui->input_speed_size->blockSignals(true);
    ui->input_angle_size->blockSignals(true);

    // Устанавливаем значения из параметров
    ui->input_area_width->setValue(params->fieldWidth());
    ui->input_area_height->setValue(params->fieldHeight());
    ui->input_radius_size->setValue(params->ballRadius());
    ui->input_speed_size->setValue(params->ballSpeed());
    ui->input_angle_size->setValue(params->ballAngle());

    // Разблокируем сигналы
    ui->input_area_width->blockSignals(false);
    ui->input_area_height->blockSignals(false);
    ui->input_radius_size->blockSignals(false);
    ui->input_speed_size->blockSignals(false);
    ui->input_angle_size->blockSignals(false);
}

void MainWindow::updateUI()
{
    updateControlsState();

    if (m_simulation && m_simulation->ball()) {
        Ball *ball = m_simulation->ball();
        QPointF pos = ball->position();
        QPointF vel = ball->velocity();
        qreal speed = qSqrt(vel.x() * vel.x() + vel.y() * vel.y());

        // Используем метод getStateString с int
        int currentState = static_cast<int>(m_simulation->events()->state());
        QString stateStr = getStateString(currentState);

        QString info = QString("X: %1 Y: %2 | Скорость: %3 px/сек | %4")
                           .arg(pos.x(), 0, 'f', 1)
                           .arg(pos.y(), 0, 'f', 1)
                           .arg(speed, 0, 'f', 1)
                           .arg(stateStr);

        ui->statusbar->showMessage(info);
    }
}

// Реализация вспомогательного метода
QString MainWindow::getStateString(int state)
{
    // Преобразуем int в Events::State
    Events::State enumState = static_cast<Events::State>(state);

    switch (enumState) {
    case Events::Stopped:
        return "Остановлено";
    case Events::Running:
        return "Запущено";
    case Events::Paused:
        return "На паузе";
    default:
        return "Неизвестно";
    }
}

void MainWindow::onSimulationStateChanged(int state)
{
    Q_UNUSED(state);
    updateControlsState();
    updateUI();
}

void MainWindow::onParametersChanged()
{
    updateUIFromParameters();
    m_deviceWindow->update();

    // Показываем информацию о новых параметрах
    Parameters *params = m_simulation->parameters();
    QString msg = QString("Параметры обновлены: %1×%2, R=%3, V=%4, A=%5°")
                      .arg(params->fieldWidth())
                      .arg(params->fieldHeight())
                      .arg(params->ballRadius())
                      .arg(params->ballSpeed())
                      .arg(params->ballAngle());
    showStatusMessage(msg, false);
}

void MainWindow::onValidationError(const QString &message)
{
    showStatusMessage(message, true);
    QMessageBox::warning(this, "Ошибка валидации", message);
}

void MainWindow::updateControlsState()
{
    if (!m_simulation) return;

    Events::State state = m_simulation->events()->state();
    bool canModify = m_simulation->events()->canModifyParameters();

    // Управление кнопками симуляции
    ui->start_button->setEnabled(state != Events::Running);
    ui->pause_button->setEnabled(state == Events::Running);
    ui->stop_button->setEnabled(state != Events::Stopped);

    // Кнопки параметров
    ui->accept_button->setEnabled(canModify);
    ui->reset_button->setEnabled(canModify);

    // Спинбоксы
    ui->input_area_width->setEnabled(canModify);
    ui->input_area_height->setEnabled(canModify);
    ui->input_radius_size->setEnabled(canModify);
    ui->input_speed_size->setEnabled(canModify);
    ui->input_angle_size->setEnabled(canModify);
}

void MainWindow::showStatusMessage(const QString &message, bool isError)
{
    if (isError) {
        ui->statusbar->setStyleSheet("QStatusBar { color: red; }");
        ui->statusbar->showMessage("✗ " + message, 5000);
    } else {
        ui->statusbar->setStyleSheet("QStatusBar { color: black; }");
        ui->statusbar->showMessage("✓ " + message, 3000);
    }
}
