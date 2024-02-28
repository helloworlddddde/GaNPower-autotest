#include "startwindow.h"

void StartWindow::left_fn(void) {
    if (static_on) {
        return;
    }
    static_on = 1;
    static_window = new StaticWindow(central_widget);
    static_window->show();
    connect(static_window, &StaticWindow::signal_1, this, &StartWindow::slot_1);
}

StartWindow::StartWindow(QWidget *parent)
    : QMainWindow(parent)
{
    static_on = 0;

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setFixedSize(200, 100);
    this->setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "/Logo.ico"));
    this->setWindowTitle("Start Menu");
    central_widget = new QWidget(this);
    this->setCentralWidget(central_widget);

    central_hbox = new QHBoxLayout();
    central_widget->setLayout(central_hbox);

    left_vbox = new QVBoxLayout();
    central_hbox->addLayout(left_vbox);

    right_vbox = new QVBoxLayout();
    central_hbox->addLayout(right_vbox);

    left_button = new QPushButton("static");
    left_vbox->addWidget(left_button);
    connect(left_button, &QPushButton::released, this, &StartWindow::left_fn);

    right_button = new QPushButton("dynamic");
    right_vbox->addWidget(right_button);

    central_hbox->setAlignment(Qt::AlignTop);
    left_vbox->setAlignment(Qt::AlignTop);
    right_vbox->setAlignment(Qt::AlignTop);
    left_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void StartWindow::slot_1(void) {
    static_on = 0;
}
