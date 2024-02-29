#include "serialthread.h"
#include "qdatetime.h"

SerialThread::SerialThread(QObject *parent)
    : QThread{parent} {
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

void SerialThread::setAbort() {
    this->abort = 1;
}
int SerialThread::checkAbort(void) {
    return this->abort;
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
    QString BV_UPPCDATA;
    QString BV_TTIMDATA;
    QString BV_RTIMDATA;
    QString BV_FTIMDATA;
    QString data_str;
    QByteArray data;
    switch(test_choice) {
    case TestButton::Test::NOTEST:
        break;
    case TestButton::Test::BV:
        BV_VDATA = config_data[1];
        BV_UPPCDATA = config_data[2];
        data_str = "FUNC:SOUR:STEP 1:DC:VOLT BV_VDATA;UPPC BV_UPPCDATA;LOWC 0;TTIM 0.1;RTIM 0.1;FTIM 0.1;ARC 0;WTIM 0;RAMP ON\n";
        data_str.replace("BV_VDATA", BV_VDATA);
        data_str.replace("BV_UPPCDATA", BV_UPPCDATA);
        data = data_str.toUtf8();
        hipot_button->write(data, timeout_msec);
        break;
    case TestButton::Test::VTH:
        break;
    case TestButton::Test::RDSON:
        break;
    case TestButton::Test::BVSTEP:
        if (step_data.empty()) {
            return;
        }
        BV_VDATA = step_data[0][0];
        BV_UPPCDATA = step_data[0][1];
        BV_TTIMDATA = step_data[0][2];
        BV_RTIMDATA = step_data[0][3];
        BV_FTIMDATA = step_data[0][4];
        data_str = "FUNC:SOUR:STEP 1:DC:VOLT BV_VDATA;UPPC BV_UPPCDATA;LOWC 0;TTIM BV_TTIMDATA;RTIM BV_RTIMDATA;FTIM BV_FTIMDATA;ARC 0;WTIM 0;RAMP ON\n";
        data_str.replace("BV_VDATA", BV_VDATA);
        data_str.replace("BV_UPPCDATA", BV_UPPCDATA);
        data_str.replace("BV_TTIMDATA", BV_TTIMDATA);
        data_str.replace("BV_RTIMDATA", BV_RTIMDATA);
        data_str.replace("BV_FTIMDATA", BV_FTIMDATA);
        data = data_str.toUtf8();
        hipot_button->write(data, timeout_msec);
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

void SerialThread::bv_test(TestButton * selected) {
    SerialButton * mcu = *selected->findSerial(SerialButton::MCU);
    SerialButton * psu = *selected->findSerial(SerialButton::PSU);
    SerialButton * hipot = *selected->findSerial(SerialButton::HIPOT);
    psu->write("SYST:REM\n", 300);
    psu->write("APPL CH1,5V,1A\n", 300);
    psu->write("INST CH1\n", 300);
    psu->write("CHAN:OUTP ON\n", 300);
    int p = 0;
    float progress_scale = 100/(max_col * max_row);
    for(int c = 0; c < max_col; c++) {
        for(int r = 0; r < max_row; r++) {
            if (checkAbort()) {
                return;
            }
            QByteArray mcu_data = QString::number(c).append(QString::number(r)).append("000\n").toUtf8();
            mcu->write(mcu_data, 300);
            hipot->write("FUNC:STAR\n", 300);
            msleep(500);
            QByteArray H = hipot->read(200, 300);
            msleep(300);
            hipot->write("FUNC:STOP\n", 300);
            msleep(300);
            p++;
            emit tableUpdate(p, 0, QDateTime::currentDateTime().toString().trimmed());
            emit tableUpdate(p, 1, QString::number(c).trimmed());
            emit tableUpdate(p, 2, QString::number(r).trimmed());
            emit tableUpdate(p, 3, config_data[0].trimmed());
            emit tableUpdate(p, 4, QString(H).trimmed().split(",")[1]);
            emit tableUpdate(p, 5, QString(H).trimmed().split(",")[2]);
            emit tableUpdate(p, 6, QString(H).trimmed().split(",")[3]);
            emit progressUpdate(p * progress_scale);
        }
    }
    psu->write("INST CH1\n", 300);
    psu->write("CHAN:OUTP OFF\n", 300);
    psu->write("SYST:LOC\n", 300);
    mcu->write("000000", 300);
}

void SerialThread::vth_test(TestButton * selected) {
    SerialButton * mcu = *selected->findSerial(SerialButton::MCU);
    SerialButton * psu = *selected->findSerial(SerialButton::PSU);
    psu->write("SYST:REM\n", 300);
    psu->write("APPL CH1,5V,1A\n", 300);
    psu->write("APPL CH3,6V,0.5A\n", 300);
    psu->write("INST CH1\n", 300);
    psu->write("CHAN:OUTP ON\n", 300);
    int p = 0;
    float progress_scale = 100/(max_col * max_row);
    for(int c = 0; c < max_col; c++) {
        for(int r = 0; r < max_row; r++) {
            if (checkAbort()) {
                return;
            }
            QByteArray mcu_data = QString::number(c).append(QString::number(r)).append("000\n").toUtf8();
            mcu->write(mcu_data, 300);
            psu->write("INST CH3\n", 300);
            psu->write("VOLT 0.9V\n", 300);
            msleep(500);
            psu->write("CHAN:OUTP ON\n", 300);
            msleep(500);
            psu->write("MEAS:CURR?\n", 300);
            msleep(800);
            QByteArray i_low = psu->read(200, 300);
            psu->write("VOLT 2V\n", 500);
            msleep(500);
            psu->write("MEAS:CURR?\n", 300);
            msleep(500);
            QByteArray i_high = psu->read(200, 300);
            i_low = i_low.trimmed();
            i_high = i_high.trimmed();
            psu->write("CHAN:OUTP OFF\n", 300);
            float vth_low = config_data[2].toFloat() / 1000;
            float vth_high = config_data[3].toFloat() / 1000;
            QString res_pf = (i_low.toFloat() < vth_low && i_high.toFloat() > vth_high) ? "Pass" : "Fail";
            p++;
            emit tableUpdate(p, 0, QDateTime::currentDateTime().toString().trimmed().remove(","));
            emit tableUpdate(p, 1, QString::number(c).trimmed());
            emit tableUpdate(p, 2, QString::number(r).trimmed());
            emit tableUpdate(p, 3, config_data[0].trimmed());
            emit tableUpdate(p, 7, QString::number(1000 * i_high.toFloat()));
            emit tableUpdate(p, 8, QString::number(1000 * i_high.toFloat()));
            emit tableUpdate(p, 9, res_pf);
            emit progressUpdate(p * progress_scale);
        }
    }
    psu->write("INST CH1\n", 300);
    psu->write("CHAN:OUTP OFF\n", 300);
    psu->write("SYST:LOC\n", 300);
    mcu->write("000000", 300);
}

void SerialThread::rdson_test(TestButton * selected) {
    SerialButton * mcu = *selected->findSerial(SerialButton::MCU);
    SerialButton * psu = *selected->findSerial(SerialButton::PSU);
    SerialButton * lcr = *selected->findSerial(SerialButton::LCR);
    psu->write("SYST:REM\n", 300);
    psu->write("APPL CH1,5V,1A\n", 300);
    psu->write("APPL CH3,6V,0.5A\n", 300);
    psu->write("INST CH1\n", 300);
    psu->write("CHAN:OUTP ON\n", 300);
    int p = 0;
    float progress_scale = 100/(max_col * max_row);
    for(int c = 0; c < max_col; c++) {
        for(int r = 0; r < max_row; r++) {
            if (checkAbort()) {
                return;
            }
            QByteArray mcu_data = QString::number(c).append(QString::number(r)).append("000\n").toUtf8();
            mcu->write(mcu_data, 300);
            psu->write("INST CH3\n", 300);
            msleep(500);
            psu->write("CHAN:OUTP ON\n", 300);
            msleep(500);
            lcr->write("FETC?\n", 300);
            msleep(800);
            QByteArray R = lcr->read(200, 300);
            psu->write("CHAN:OUTP OFF\n", 300);
            // qDebug() << i_low << i_high;
            R = R.trimmed();
            float R_limit = config_data[5].trimmed().toFloat() / 1000;
            QString res_pf = (R.toFloat() < R_limit) ? "Pass" : "Fail";
            p++;
            emit tableUpdate(p, 0, QDateTime::currentDateTime().toString().trimmed());
            emit tableUpdate(p, 1, QString::number(c).trimmed());
            emit tableUpdate(p, 2, QString::number(r).trimmed());
            emit tableUpdate(p, 3, config_data[0].trimmed());
            emit tableUpdate(p, 10, QString::number(1000 * R.toFloat()));
            emit tableUpdate(p, 11, "TBA");
            emit progressUpdate(p * progress_scale);
        }
    }
    psu->write("INST CH1\n", 300);
    psu->write("CHAN:OUTP OFF\n", 300);
    psu->write("SYST:LOC\n", 300);
    mcu->write("000000", 300);
}

void SerialThread::bvstep_test(TestButton * selected) {
    SerialButton * hipot = *selected->findSerial(SerialButton::HIPOT);
    float progress_scale = 100/step_data.size();
    int p = 0;
    for(size_t i = 0; i < step_data.size(); i++) {
        if (checkAbort()) {
            return;
        }
        QString BV_VDATA = step_data[i][0];
        QString BV_UPPCDATA = step_data[i][1];
        QString BV_TTIMDATA = step_data[i][2];
        QString BV_RTIMDATA = step_data[i][3];
        QString BV_FTIMDATA = step_data[i][4];
        QString data_str = "FUNC:SOUR:STEP 1:DC:VOLT BV_VDATA;UPPC BV_UPPCDATA;LOWC 0;TTIM BV_TTIMDATA;RTIM BV_RTIMDATA;FTIM BV_FTIMDATA;ARC 0;WTIM 0;RAMP ON\n";
        data_str.replace("BV_VDATA", BV_VDATA);
        data_str.replace("BV_UPPCDATA", BV_UPPCDATA);
        data_str.replace("BV_TTIMDATA", BV_TTIMDATA);
        data_str.replace("BV_RTIMDATA", BV_RTIMDATA);
        data_str.replace("BV_FTIMDATA", BV_FTIMDATA);
        QByteArray data = data_str.toUtf8();
        msleep(300);
        hipot->write(data, 300);
        msleep(300);
        hipot->write("FUNC:STAR\n", 300);
        msleep(500);
        QByteArray H = hipot->read(200, 300);
        msleep(300);
        hipot->write("FUNC:STOP\n", 300);
        msleep(300);
        p++;
        emit progressUpdate(p * progress_scale);
        emit tableUpdate(p, 0, QDateTime::currentDateTime().toString().trimmed());
        emit tableUpdate(p, 1, 0);
        emit tableUpdate(p, 2, 0);
        emit tableUpdate(p, 3, config_data[0].trimmed());
        emit tableUpdate(p, 4, QString(H).trimmed().split(",")[1]);
        emit tableUpdate(p, 5, QString(H).trimmed().split(",")[2]);
        emit tableUpdate(p, 6, QString(H).trimmed().split(",")[3]);
    }
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
    QByteArray ID_comm = "*IDN?\n";
    temp_port->write(ID_comm);
    while(temp_port->waitForBytesWritten(timeout_msec));
    while(temp_port->waitForReadyRead(timeout_msec));
    QString ID_str = QString(temp_port->read(buffer_max));
    if (ID_str.contains(manual_port->getTarget())) {
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
        QByteArray ID_comm = "*IDN?\n";
        temp_port->write(ID_comm);
        while(temp_port->waitForBytesWritten(timeout_msec));
        while(temp_port->waitForReadyRead(timeout_msec));
        QString ID_str = QString(temp_port->read(buffer_max));
        int found = 0;
        for(size_t i = 0; i < selected->required.size(); i++) {
            SerialButton * temp_button = *(selected->required[i]);
            if (ID_str.contains(temp_button->getTarget())) {
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

void SerialThread::step_set(std::vector<std::vector<QString>> step_data) {
    this->step_data = step_data;
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

    // for(size_t i = 0; i < selected->required.size(); i++) {
    //     SerialButton * serial_button = *selected->required[i];
    //     QByteArray ID_comm = "*IDN?\n";
    //     serial_button->write(ID_comm, timeout_msec);
    //     QString id_str = QString(serial_button->read(buffer_max, timeout_msec));
    //     qDebug() << id_str;
    //     if(!id_str.contains(serial_button->getTarget())) {
    //         emit debug("faulty serial connnections");
    //         emit serialComplete(mode);
    //         return;
    //     }
    // }

    if (!(selected->handshake(buffer_max, timeout_msec))) {
        for(size_t i = 0; i < selected->required.size(); i++) {
            (*selected->required[i])->connected = 0;
        }
        emit debug("faulty serial connnections");
        emit serialComplete(mode);
        return;
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













SerialThread::~SerialThread() {
}
