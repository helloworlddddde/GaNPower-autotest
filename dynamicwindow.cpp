#include "dynamicwindow.h"



ViSession default_rm;


QList<QSerialPortInfo> dialog_info_dynamic;
SerialButton * dialog_button_dynamic = nullptr;


void DynamicWindow::update_buttons(void) {
    for(size_t i = 0; i < serial_buttons.size(); i++) {
        SerialButton * temp_button = serial_buttons[i];
        if (temp_button->connected && temp_button->configured) {
            temp_button->setStyleSheet("background-color: rgba(0, 0, 255, 0.4);");
        } else if (temp_button->connected) {
            temp_button->setStyleSheet("background-color: rgba(46, 204, 113, 0.4);");
        } else {
            temp_button->setStyleSheet("");
        }
    }
    for(size_t i = 0; i < test_buttons.size(); i++) {
        TestButton * temp_button = test_buttons[i];
        if (temp_button->selected) {
            temp_button->setStyleSheet("background-color: rgba(46, 204, 113, 0.4);");
        } else {
            temp_button->setStyleSheet("");
        }
    }
}


void DynamicWindow::onCompute(QString choice) {
    int idx = choice.split(":").at(0).toInt();
    serial_thread->setManualPort(dialog_button_dynamic, dialog_info_dynamic[idx]);
    serial_thread->mode = SerialThread::Mode::MANUALCONNECT;
    serial_thread->start();
}



void DynamicWindow::serialHandle(SerialThread::Mode mode) {
    switch(mode) {
        case SerialThread::NONE:
            break;
        case SerialThread::MANUALCONNECT:
            dialog_info_dynamic.clear();
            dialog_button_dynamic = nullptr;
            update_buttons();
            break;
        case SerialThread::AUTOCONNECT:
            break;
        case SerialThread::CONFIGURE:
            break;
        case SerialThread::TEST:
            break;
    }
}


DynamicWindow::DynamicWindow(QWidget *parent)
    : QMainWindow{parent} {


    viOpenDefaultRM (&default_rm);


    this->setWindowTitle("Dynamic Testing");
    this->setFixedSize(QSize(1250, 350));
    this->setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "/Logo.ico"));
    QWidget * central_widget = new QWidget(this);
    setCentralWidget(central_widget);
    QHBoxLayout * L = new QHBoxLayout();

    serial_thread = new SerialThread(this);
    connect(serial_thread, &SerialThread::serialComplete, this, &DynamicWindow::serialHandle);
    serial_thread->mode = SerialThread::Mode::NONE;
    serial_thread->timeout_msec = 200;
    serial_thread->buffer_max = 128;
    serial_thread->default_rm = &default_rm;

    central_widget->setLayout(L);

    serial_buttons.push_back(new VisaButton("SIGGEN"));
    serial_buttons[0]->setTarget("DG2102");
    serial_buttons[0]->setLabel(SerialButton::SIGGEN);
    VisaButton * temp0 = (VisaButton *) serial_buttons[0];
    temp0->visa_port = ViString("USB0::0x1AB1::0x0644::DG2P233700946::INSTR");

    serial_buttons.push_back(new VisaButton("OSCO"));
    serial_buttons[1]->setTarget("SDS2");
    serial_buttons[1]->setLabel(SerialButton::OSCO);
    VisaButton * temp1 = (VisaButton *) serial_buttons[1];
    temp1->visa_port = ViString("USB0::0xF4EC::0x1011::SDS2PDDX6R1608::INSTR");

    serial_buttons.push_back(new SerialButton("POWER"));
    serial_buttons[2]->setTarget("ITECH");
    serial_buttons[2]->setLabel(SerialButton::POW);

    for(size_t i = 0; i < serial_buttons.size(); i++) {
        serial_buttons[i]->setPort(nullptr);
        L->addWidget(serial_buttons[i]);
        if (serial_buttons[i]->isVisa()) {
            connect(serial_buttons[i], &QPushButton::released, this, [this, i](void) {
            if (serial_thread->isRunning()) {
                return;
            }
            serial_thread->setManualPort(serial_buttons[i], (QSerialPortInfo) NULL);
            serial_thread->mode = SerialThread::Mode::MANUALCONNECT;
            serial_thread->start();
            });
            continue;
        }
        connect(serial_buttons[i], &QPushButton::released, this, [this, i](void) {
            if (serial_thread->isRunning()) {
                return;
            }
            QList<QSerialPortInfo> serial_infos = QSerialPortInfo::availablePorts();
            QInputDialog serial_dialog;
            serial_dialog.setLabelText("select port");
            serial_dialog.setWindowTitle("Manual Connect");
            QStringList connections;
            for(int i = 0; i < serial_infos.size(); i++) {
                connections << QString::number(i) + ":" + QString(serial_infos[i].portName() + " " + serial_infos[i].description());
            }
            if (connections.size() == 0) {
                qDebug() << "No port found";
                return;
            }
            serial_dialog.setComboBoxItems(connections);
            serial_dialog.setOptions(QInputDialog::UseListViewForComboBoxItems);
            dialog_info_dynamic = serial_infos;
            dialog_button_dynamic = serial_buttons[i];
            connect(&serial_dialog, SIGNAL(textValueSelected(QString)), this, SLOT(onCompute(QString)));
            serial_dialog.exec();
        });
    }

    test_buttons.push_back(new TestButton("DPT"));
    test_buttons[0]->setLabel(TestButton::Test::DPT);
    test_buttons[0]->required.insert(test_buttons[0]->required.end(), {&serial_buttons[0], &serial_buttons[1]});
    test_buttons[0]->setupToolTip();

    for(size_t i = 0; i < test_buttons.size(); i++) {
        L->addWidget(test_buttons[i]);
        connect(test_buttons[i], &QPushButton::released, this, [this, i](void) {
            if (serial_thread->isRunning()) {
                return;
            }
            for (size_t j = 0; j < test_buttons.size(); j++) {
                test_buttons[j]->selected = 0;
            }
            for (size_t k = 0; k < serial_buttons.size(); k++) {
                serial_buttons[k]->configured = 0;
            }
            test_buttons[i]->selected = 1;
            update_buttons();
        });
    }


    configuration_data = std::vector<QString>(3);
    a_line = new QLineEdit(this);
    a_line->setValidator(new QDoubleValidator(1, 10, 4, this));
    a_line->setPlaceholderText("a (us)");

    b_line = new QLineEdit(this);
    b_line->setValidator(new QDoubleValidator(1, 10, 4, this));
    b_line->setPlaceholderText("b (us)");

    v_line = new QLineEdit(this);
    v_line->setValidator(new QDoubleValidator(100, 1200, 0, this));
    v_line->setPlaceholderText("V (V)");


    L->addWidget(a_line);
    L->addWidget(b_line);
    L->addWidget(v_line);

    QPushButton * configure_button = new QPushButton("configure");
    connect(configure_button, &QPushButton::released, this, [this](void) {
        if (serial_thread->isRunning()) {
            return;
        }
        configuration_data[0] = a_line->text();
        QDoubleValidator * a_val = (QDoubleValidator * ) a_line->validator();
        if (configuration_data[0].toDouble() < a_val->bottom()) {
            configuration_data[0] = QString::number(a_val->bottom());
            a_line->setText(QString::number(a_val->bottom()));
        }
        if (configuration_data[0].toDouble() > a_val->top()) {
            configuration_data[0] = QString::number(a_val->top());
            a_line->setText(QString::number(a_val->top()));
        }
        configuration_data[1] = b_line->text();
        QDoubleValidator * b_val = (QDoubleValidator * ) b_line->validator();
        if (configuration_data[1].toDouble() < b_val->bottom()) {
            configuration_data[1] = QString::number(b_val->bottom());
            b_line->setText(QString::number(b_val->bottom()));
        }
        if (configuration_data[1].toDouble() > b_val->top()) {
            configuration_data[1] = QString::number(b_val->top());
            b_line->setText(QString::number(b_val->top()));
        }
        configuration_data[2] = v_line->text();
        QDoubleValidator * v_val = (QDoubleValidator * ) v_line->validator();
        if (configuration_data[2].toDouble() < v_val->bottom()) {
            configuration_data[2] = QString::number(v_val->bottom());
            v_line->setText(QString::number(v_val->bottom()));
        }
        if (configuration_data[2].toDouble() > v_val->top()) {
            configuration_data[2] = QString::number(v_val->top());
            v_line->setText(QString::number(v_val->top()));
        }
        serial_thread->test_set(test_buttons);
        serial_thread->configuration_set(configuration_data);
        serial_thread->mode = SerialThread::Mode::CONFIGURE;
        serial_thread->start();
    });
    L->addWidget(configure_button);

    ViFindList   fList;
    ViChar       desc[VI_FIND_BUFLEN];
    ViUInt32     numInstrs;
    viFindRsrc(default_rm, ViString("?*INSTR"), &fList, &numInstrs, desc);

    while (numInstrs--) {
        qDebug() << desc;
        viFindNext(fList, desc);
    }































}

void DynamicWindow::closeEvent(QCloseEvent * event) {
    emit signal_1();
}




