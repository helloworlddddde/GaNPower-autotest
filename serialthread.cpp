#include "serialthread.h"

SerialThread::SerialThread(QObject *parent)
    : QThread{parent} {
}

void SerialThread::bv_test(TestButton * selected) {

}

void SerialThread::vth_test(TestButton * selected) {
    SerialButton * mcu = *selected->findSerial(SerialButton::MCU);
    SerialButton * psu = *selected->findSerial(SerialButton::PSU);
    psu->write("INST CH1\n", 300);
    psu->write("CHAN:OUTP ON\n", 300);
    psu->write("INST CH3\n", 300);
    psu->write("VOLT 1V\n", 300);
    psu->write("CHAN:OUTP ON\n", 300);
    psu->write("MEAS:CURR?\n", 300);
    QByteArray i_low = psu->read(200, 300);
    psu->write("VOLT 2V\n", 500);
    psu->write("MEAS:CURR?\n", 300);
    QByteArray i_high = psu->read(200, 300);
    psu->write("CHAN:OUTP OFF\n", 300);
    psu->write("INST CH1\n", 300);
    psu->write("CHAN:OUTP OFF\n", 300);
    psu->write("SYST:LOC\n", 300);
}

void SerialThread::rdson_test(TestButton * selected) {

}

void SerialThread::bvstep_test(TestButton * selected) {

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
        emit debug(temp_port->errorString());
        emit serialComplete(mode);
        return;
    }
    const char * ID_comm = "*IDN?\n";
    while(temp_port->waitForReadyRead(timeout_msec));
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
    emit debug("showing all required idn's");
    for(size_t i = 0; i < selected->required.size(); i++) {
        emit debug((*selected->required[i])->getTarget());
    }
    QList<QSerialPortInfo> serial_infos = QSerialPortInfo::availablePorts();
        emit debug("showing all available ports");
    for(int i = 0; i < serial_infos.size(); i++) {
        emit debug(serial_infos[i].description());
    }
    emit debug("showing all successful connections");
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
                emit debug("connected: " + temp_button->getTarget());
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
        emit debug("no test selected");
        emit serialComplete(mode);
        return;
    }
    for(size_t i = 0; i < selected->required.size(); i++) {
        int is_connected = ((*(selected->required[i]))->getPort() != nullptr);
        if (!is_connected) {
            emit debug("missing connections");
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
        emit debug("no test selected");
        emit serialComplete(mode);
        return;
    }
    for(size_t i = 0; i < selected->required.size(); i++) {
        int is_connected = ((*(selected->required[i]))->getPort() != nullptr);
        if (!is_connected) {
            emit debug("missing connections");
            emit serialComplete(mode);
            return;
        }
        int is_configured = ((*(selected->required[i]))->configured == 1);
        if (!is_configured) {
            emit debug("missing configurations");
            emit serialComplete(mode);
            return;
        }
    }

    switch(selected->getLabel()) {
        case TestButton::Test::NOTEST:
            break;
        case TestButton::Test::BV:
            bv_test(selected);
            break;
        case TestButton::Test::VTH:
            vth_test(selected);
            break;
        case TestButton::Test::RDSON:
            rdson_test(selected);
            break;
        case TestButton::Test::BVSTEP:
            bvstep_test(selected);
            break;
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
    emit debug("MCU configured");
    return;
}

void SerialThread::psu_configure(SerialButton * psu_button, TestButton::Test test_choice) {
    switch(test_choice) {
        case TestButton::Test::NOTEST:
            break;
        case TestButton::Test::BV:
            psu_button->write("SYST:REM\n", timeout_msec);
            psu_button->write("APPL CH1,5V,1A\n", timeout_msec);
            break;
        case TestButton::Test::VTH:
            psu_button->write("SYST:REM\n", timeout_msec);
            psu_button->write("APPL CH1,5V,1A\n", timeout_msec);
            psu_button->write("APPL CH3,0.9V,0.1A\n", timeout_msec);
            break;
        case TestButton::Test::RDSON:
            psu_button->write("SYST:REM\n", timeout_msec);
            psu_button->write("APPL CH1,5V,1A\n", timeout_msec);
            psu_button->write("APPL CH3,6V,0.5A\n", timeout_msec);
            break;
        case TestButton::Test::BVSTEP:
            psu_button->write("SYST:REM\n", timeout_msec);
            psu_button->write("APPL CH1,5V,1A\n", timeout_msec);
            break;
    }
    psu_button->configured = 1;
    emit debug("PSU configured");
    return;
}

void SerialThread::hipot_configure(SerialButton * hipot_button, TestButton::Test test_choice) {
    QString BV_VDATA;
    QString BV_UPCDATA;
    QString data_str;
    QByteArray data;
    switch(test_choice) {
    case TestButton::Test::NOTEST:
        break;
    case TestButton::Test::BV:
        qDebug() << config_data[1] << config_data[2];
        BV_VDATA = config_data[1];
        BV_UPCDATA = config_data[2];
        data_str = "FUNC:SOUR:STEP 1:DC:VOLT BV_VDATA;UPPC BV_UPCDATA;LOWC 0;TTIM 0.1;RTIM 0.1;FTIM 0.1;ARC 0;WTIM 0;RAMP ON\n";
        data_str.replace("BV_VDATA", BV_VDATA);
        data_str.replace("BV_UPCDATA", BV_UPCDATA);
        data = QByteArray(data_str.toUtf8());
        hipot_button->write(data, timeout_msec);
        break;
    case TestButton::Test::VTH:
        break;
    case TestButton::Test::RDSON:
        break;
    case TestButton::Test::BVSTEP:
        break;
    }
    hipot_button->configured = 1;
    emit debug("HIPOT configured");
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
        lcr_button->write("FUNC:IMP DCR\n", timeout_msec);
        break;
    case TestButton::Test::BVSTEP:
        break;
    }
    lcr_button->configured = 1;
    emit debug("LCR configured");
    return;
}

SerialThread::~SerialThread() {
}
