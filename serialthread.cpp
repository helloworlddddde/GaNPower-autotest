#include "serialthread.h"

SerialThread::SerialThread(QObject *parent)
    : QThread{parent} {
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

void SerialThread::autoconnect(void) {
    for(size_t i = 0; i < serial_buttons.size(); i++) {
        SerialButton * temp_button = serial_buttons[i];
        if (temp_button->getPort() != nullptr) {
            temp_button->getPort()->close();
            delete(temp_button->getPort());
            temp_button->setPort(nullptr);
        }
    }
    qDebug() << "showing all successful connections";
    QList<QSerialPortInfo> serial_infos = QSerialPortInfo::availablePorts();
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
        for(size_t i = 0; i < serial_buttons.size(); i++) {
            SerialButton * temp_button = serial_buttons[i];
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
    for(size_t i = 0; i < serial_buttons.size(); i++) {
        SerialButton * temp_button = serial_buttons[i];
        if (temp_button->getPort() != nullptr) {
            temp_button->connected = 1;
        } else {
            temp_button->connected = 0;
        }
    }
    emit serialComplete(mode);
    return;
}

void SerialThread::serial_set(std::vector<SerialButton *> serial_buttons) {
    this->serial_buttons = serial_buttons;
}


void SerialThread::test_set(std::vector<TestButton *> test_buttons) {
    this->test_buttons = test_buttons;
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

void SerialThread::configure(void) {
    TestButton * selected = nullptr;
    for(size_t i = 0; i < test_buttons.size(); i++) {
        if (test_buttons[i]->selected == 1) {
            selected = test_buttons[i];
        }
    }
    if (selected == nullptr) {
        emit serialComplete(mode);
        qDebug() << "no test selected";
        return;
    }
    for(size_t i = 0; i < serial_buttons.size(); i++) {
        int is_required = std::find(selected->required.begin(), selected->required.end(), serial_buttons[i]->getLabel()) != selected->required.end();
        int is_connected = (serial_buttons[i]->getPort() != nullptr);
        if (!is_connected && is_required) {
            qDebug() << "missing connections";
            emit serialComplete(mode);
            return;
        } else if (is_connected && is_required) {
            SerialButton * serial_button = serial_buttons[i];
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
    }
    emit serialComplete(mode);
    return;
}


void SerialThread::test(void) {

}
