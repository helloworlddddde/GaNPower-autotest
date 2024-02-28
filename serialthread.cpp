#include "serialthread.h"

SerialThread::SerialThread(QObject *parent)
    : QThread{parent} {
}

void SerialThread::bv_test(void) {

}

void SerialThread::vth_test(void) {

}

void SerialThread::rdson_test(void) {

}

void SerialThread::bvstep_test(void) {

}

void SerialThread::setMaxRow(int max_row) {
    this->max_row = max_row;
}

void SerialThread::setMaxCol(int max_col) {
    this->max_col = max_col;
}

void SerialThread::configuration_set(std::vector<QString> config_data) {
    this->config_data = config_data;
}

void SerialThread::manual_connect(void) {
    if (manual_port->getPort() != nullptr) {
        manual_port->getPort()->close();
        delete(manual_port->getPort());
        manual_port->setPort(nullptr);
    }
    QSerialPort * temp_port = new QSerialPort();
    temp_port->setPort(manual_info);
    if (!temp_port->open(QIODevice::ReadWrite)) {
        qDebug() << temp_port->errorString();
        emit serialComplete(mode);
        return;
    }
    const char * ID_comm = "*IDN?\n";
    temp_port->write(ID_comm);
    while(temp_port->waitForBytesWritten(timeout_msec));
    while(temp_port->waitForReadyRead(timeout_msec));
    char ID_bytes[buffer_max];
    temp_port->readLine(ID_bytes, buffer_max);
    QString ID_str = QString(ID_bytes);
    if (ID_str == manual_port->getTarget()) {
        manual_port->setPort(temp_port);
        manual_port->connected = 1;
    } else {
        manual_port->connected = 0;
        temp_port->close();
        delete(temp_port);
    }
    emit serialComplete(mode);
    return;
}

void SerialThread::autoconnect(void) {
    TestButton * selected = nullptr;
    for(size_t i = 0; i < test_buttons.size(); i++) {
        if (test_buttons[i]->selected == 1) {
            selected = test_buttons[i];
        }
    }
    if (selected == nullptr) {
        emit debug("no test selected");
        emit serialComplete(mode);
        return;
    }
    for(size_t i = 0; i < selected->required.size(); i++) {
        SerialButton * temp_button = *(selected->required[i]);
        if (temp_button->getPort() != nullptr) {
            temp_button->getPort()->close();
            delete(temp_button->getPort());
            temp_button->setPort(nullptr);
        }
    }
    emit debug("showing all needed port idn's");
    for(size_t i = 0; i < selected->required.size(); i++) {
        emit debug((*selected->required[i])->getTarget());
    }
    QList<QSerialPortInfo> serial_infos = QSerialPortInfo::availablePorts();
        emit debug("showing all available ports");
    for(int i = 0; i < serial_infos.size(); i++) {
        emit debug(serial_infos[i].description());
    }
    qDebug() << "showing all successful connections";
    for(int i = 0; i < serial_infos.size(); i++) {
        QSerialPortInfo temp_info = serial_infos[i];
        QSerialPort * temp_port = new QSerialPort();
        temp_port->setPort(temp_info);
        if (!temp_port->open(QIODevice::ReadWrite)) {
            continue;
        }
        const char * ID_comm = "*IDN?\n";
        temp_port->write(ID_comm);
        while(temp_port->waitForBytesWritten(timeout_msec));
        while(temp_port->waitForReadyRead(timeout_msec));
        char ID_bytes[buffer_max];
        temp_port->readLine(ID_bytes, buffer_max);
        QString ID_str = QString(ID_bytes);

        int found = 0;
        for(size_t i = 0; i < selected->required.size(); i++) {
            SerialButton * temp_button = *(selected->required[i]);
            if (temp_button->getTarget() == ID_str) {
                temp_button->setPort(temp_port);
                qDebug() << "connected:" << temp_button->getTarget();
                found = 1;
            }
        }
        if (!found) {
            temp_port->close();
            delete(temp_port);
        }
    }
    for(size_t i = 0; i < selected->required.size(); i++) {
        SerialButton * temp_button = *(selected->required[i]);
        if (temp_button->getPort() != nullptr) {
            temp_button->connected = 1;
        } else {
            temp_button->connected = 0;
        }
    }
    emit serialComplete(mode);
    return;
}

void SerialThread::test_set(std::vector<TestButton *> test_buttons) {
    this->test_buttons = test_buttons;
}

void SerialThread::configure(void) {
    TestButton * selected = nullptr;
    for(size_t i = 0; i < test_buttons.size(); i++) {
        if (test_buttons[i]->selected == 1) {
            selected = test_buttons[i];
        }
    }
    if (selected == nullptr) {
        qDebug() << "no test selected";
        emit serialComplete(mode);
        return;
    }
    for(size_t i = 0; i < selected->required.size(); i++) {
        int is_connected = ((*(selected->required[i]))->getPort() != nullptr);
        if (!is_connected) {
            qDebug() << "missing connections";
            emit serialComplete(mode);
            return;
        }
        SerialButton * serial_button = *(selected->required[i]);
        switch(serial_button->getLabel()) {
            case SerialButton::Equipment::MCU:
                mcu_configure(serial_button, selected->getLabel());
                break;
            case SerialButton::Equipment::PSU:
                psu_configure(serial_button, selected->getLabel());
                break;
            case SerialButton::Equipment::HIPOT:
                hipot_configure(serial_button, selected->getLabel());
                break;
            case SerialButton::Equipment::LCR:
                lcr_configure(serial_button, selected->getLabel());
                break;
        }
    }
    emit serialComplete(mode);
    return;
}

void SerialThread::test(void) {
    TestButton * selected = nullptr;
    for(size_t i = 0; i < test_buttons.size(); i++) {
        if (test_buttons[i]->selected == 1) {
            selected = test_buttons[i];
        }
    }
    if (selected == nullptr) {
        qDebug() << "no test selected";
        emit serialComplete(mode);
        return;
    }
    for(size_t i = 0; i < selected->required.size(); i++) {
        int is_connected = ((*(selected->required[i]))->getPort() != nullptr);
        if (!is_connected) {
            qDebug() << "missing connections";
            emit serialComplete(mode);
            return;
        }
        int is_configured = ((*(selected->required[i]))->configured == 1);
        if (!is_configured) {
            qDebug() << "missing configurations";
            emit serialComplete(mode);
            return;
        }
    }
    emit serialComplete(mode);
    return;
}

void SerialThread::setManualPort(SerialButton * manual_port, QSerialPortInfo manual_info) {
    this->manual_port = manual_port;
    this->manual_info = manual_info;
}











void SerialThread::mcu_configure(SerialButton * mcu_button, TestButton::Test test_choice) {
    switch(test_choice) {
        case TestButton::Test::NOTEST:
            break;
        case TestButton::Test::BV:
            break;
        case TestButton::Test::VTH:
            break;
        case TestButton::Test::RDSON:
            break;
        case TestButton::Test::BVSTEP:
            break;
    }
    mcu_button->configured = 1;
    qDebug() << "MCU configured";
    return;
}

void SerialThread::psu_configure(SerialButton * psu_button, TestButton::Test test_choice) {
    qDebug() << config_data;
    switch(test_choice) {
        case TestButton::Test::NOTEST:
            break;
        case TestButton::Test::BV:
            break;
        case TestButton::Test::VTH:
            psu_button->getPort()->write("SYST:REM\n");
            while(psu_button->getPort()->waitForBytesWritten(timeout_msec));
            psu_button->getPort()->write("APPL CH1,5V,1A\n");
            while(psu_button->getPort()->waitForBytesWritten(timeout_msec));
            psu_button->getPort()->write("APPL CH3,0.9V,0.1A\n");
            while(psu_button->getPort()->waitForBytesWritten(timeout_msec));
            break;
        case TestButton::Test::RDSON:
            psu_button->getPort()->write("SYST:REM\n");
            while(psu_button->getPort()->waitForBytesWritten(timeout_msec));
            psu_button->getPort()->write("APPL CH1,5V,1A\n");
            while(psu_button->getPort()->waitForBytesWritten(timeout_msec));
            psu_button->getPort()->write("APPL CH3,6V,0.5A\n");
            while(psu_button->getPort()->waitForBytesWritten(timeout_msec));
            break;
        case TestButton::Test::BVSTEP:
            break;
    }
    psu_button->configured = 1;
    qDebug() << "PSU configured";
    return;
}

void SerialThread::hipot_configure(SerialButton * hipot_button, TestButton::Test test_choice) {
    switch(test_choice) {
    case TestButton::Test::NOTEST:
        break;
    case TestButton::Test::BV:
        break;
    case TestButton::Test::VTH:
        break;
    case TestButton::Test::RDSON:
        break;
    case TestButton::Test::BVSTEP:
        break;
    }
    hipot_button->configured = 1;
    qDebug() << "HIPOT configured";
    return;
}

void SerialThread::lcr_configure(SerialButton * lcr_button, TestButton::Test test_choice) {
    switch(test_choice) {
    case TestButton::Test::NOTEST:
        break;
    case TestButton::Test::BV:
        break;
    case TestButton::Test::VTH:
        break;
    case TestButton::Test::RDSON:
        lcr_button->getPort()->write("FUNC:IMP DCR\n");
        while(lcr_button->getPort()->waitForBytesWritten(timeout_msec));
        break;
    case TestButton::Test::BVSTEP:
        break;
    }
    lcr_button->configured = 1;
    qDebug() << "LCR configured";
    return;
}
