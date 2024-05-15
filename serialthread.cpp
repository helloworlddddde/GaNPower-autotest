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
    case TestButton::Test::VTHSTEP2:
        mcu_button->write("90409\n", timeout_msec);
        break;
    case TestButton::Test::VTHSTEP3:
        mcu_button->write("90409\n", timeout_msec);
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
    QString data_str;
    QByteArray data;
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
    case TestButton::Test::VTHSTEP:
        if (vthstep_data.empty()) {
            return;
        }
        qDebug() << vthstep_data.size() << vthstep_data[0].size();
        psu_button->write("SYST:REM\n", timeout_msec);
        data_str = "APPL CH3, STARTVOLTV,0.5A\n";
        data_str.replace("STARTVOLT", vthstep_data[0][0]);
        data = data_str.toUtf8();
        qDebug() << data_str;
        psu_button->write(data, timeout_msec);
        break;
    case TestButton::Test::VTHSTEP2:
        if (vthstep_data.empty()) {
            return;
        }
        qDebug() << vthstep_data.size() << vthstep_data[0].size();
        psu_button->write("SYST:REM\n", timeout_msec);
        data_str = "APPL CH3, STARTVOLTV,1A\n";
        data_str.replace("STARTVOLT", "0.1");
        data = data_str.toUtf8();
        qDebug() << data_str;
        psu_button->write(data, timeout_msec);
        break;
    case TestButton::Test::VTHSTEP3:
        if (vthstep_data.empty()) {
            return;
        }
        qDebug() << vthstep_data.size() << vthstep_data[0].size();
        psu_button->write("SYST:REM\n", timeout_msec);
        data_str = "APPL CH3, STARTVOLTV,1A\n";
        data_str.replace("STARTVOLT", "0.1");
        data = data_str.toUtf8();
        qDebug() << data_str;
        psu_button->write(data, timeout_msec);
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
    float progress_scale = ((float) 100)/(max_col * max_row);
    for(int c = 0; c < max_col; c++) {
        for(int r = 0; r < max_row; r++) {
            if (checkAbort()) {
                return;
            }
            QByteArray mcu_data = QString::number(c+1).append(QString::number(r+1)).append("000\n").toUtf8();
            mcu->write(mcu_data, 300);
            msleep(500);
            hipot->write("FUNC:STAR\n", 300);
            msleep(1500);
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
            emit progressUpdate(round((double) p * progress_scale));
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
    psu->write("APPL CH3,0.9V,0.1A\n", 300);
    psu->write("INST CH1\n", 300);
    psu->write("CHAN:OUTP ON\n", 300);
    int p = 0;
    float progress_scale = ((float)100)/(max_col * max_row);
    for(int c = 0; c < max_col; c++) {
        for(int r = 0; r < max_row; r++) {
            if (checkAbort()) {
                return;
            }
            QByteArray mcu_data = QString::number(c+1).append(QString::number(r+1)).append("000\n").toUtf8();
            qDebug() << mcu_data;
            mcu->write(mcu_data, 300);
            msleep(500);
            psu->write("INST CH3\n", 300);
            psu->write("VOLT 0.9V\n", 300);;
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
            msleep(500);
            i_low = i_low.trimmed();
            i_high = i_high.trimmed();
            psu->write("CHAN:OUTP OFF\n", 300);
            float vth_low = config_data[3].toFloat() / 1000;
            float vth_high = config_data[4].toFloat() / 1000;
            QString res_pf = (i_low.toFloat() < vth_low && i_high.toFloat() > vth_high) ? "Pass" : "Fail";
            p++;
            emit tableUpdate(p, 0, QDateTime::currentDateTime().toString().trimmed().remove(","));
            emit tableUpdate(p, 1, QString::number(c).trimmed());
            emit tableUpdate(p, 2, QString::number(r).trimmed());
            emit tableUpdate(p, 3, config_data[0].trimmed());
            emit tableUpdate(p, 7, QString::number(1000 * i_low.toFloat()));
            emit tableUpdate(p, 8, QString::number(1000 * i_high.toFloat()));
            emit tableUpdate(p, 9, res_pf);
            emit progressUpdate(round((double) p * progress_scale));
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
    float progress_scale = ((float)100)/(max_col * max_row);
    for(int c = 0; c < max_col; c++) {
        for(int r = 0; r < max_row; r++) {
            if (checkAbort()) {
                return;
            }
            QByteArray mcu_data = QString::number(c+1).append(QString::number(r+1)).append("000\n").toUtf8();
            qDebug() << mcu_data;
            mcu->write(mcu_data, 300);
            msleep(500);
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
            emit tableUpdate(p, 11, res_pf);
            emit progressUpdate(round((double) p * progress_scale));
        }
    }
    psu->write("INST CH1\n", 300);
    psu->write("CHAN:OUTP OFF\n", 300);
    psu->write("SYST:LOC\n", 300);
    mcu->write("000000", 300);
}

void SerialThread::bvstep_test(TestButton * selected) {
    SerialButton * hipot = *selected->findSerial(SerialButton::HIPOT);
    float progress_scale = ((float)100)/step_data.size();
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
        emit progressUpdate(round((double) p * progress_scale));
        emit tableUpdate(p, 0, QDateTime::currentDateTime().toString().trimmed());
        emit tableUpdate(p, 1, 0);
        emit tableUpdate(p, 2, 0);
        emit tableUpdate(p, 3, config_data[0].trimmed());
        if (QString(H).trimmed().split(",")[3] != "PASS") {
            this->abort = 1;
        }
        emit tableUpdate(p, 4, QString(H).trimmed().split(",")[1]);
        emit tableUpdate(p, 5, QString(H).trimmed().split(",")[2]);
        emit tableUpdate(p, 6, QString(H).trimmed().split(",")[3]);
    }
}

void SerialThread::vthstep_test(TestButton * selected) {
    SerialButton * psu = *selected->findSerial(SerialButton::PSU);
    VisaButton * dmm = (VisaButton *) *selected->findSerial(SerialButton::DMM);
    int size = vthstep_data.size();
    float progress_scale = ((float) 100)/size;
    int p = 0;
    psu->write("CHAN:OUTP ON\n", 300);
    for(int i = 0; i < size; i++) {
        if (checkAbort()) {
            return;
        }
        msleep(100);
        psu->write("INST CH3\n", 300);
        QString data_str = "VOLT USERVOLTV\n";
        data_str.replace("USERVOLT", vthstep_data[i][0]);
        QByteArray data = data_str.toUtf8();
        psu->write(data, 300);;
        msleep(100);
        dmm->write(ViString("MEAS:CURR:DC? 0.1, 0.00000007"));
        msleep(30);
        dmm->read();
        QString current = QString(dmm->getBuf()).split("\n")[0];
        p++;
        emit progressUpdate(round((double) p * progress_scale));
        emit tableUpdate(p, 0, QDateTime::currentDateTime().toString().trimmed().remove(","));
        emit tableUpdate(p, 3, config_data[0].trimmed());
        emit tableUpdate(p, 12, vthstep_data[i][0]);
        emit tableUpdate(p, 13, current);
    }
    psu->write("CHAN:OUTP OFF\n", 300);
    psu->write("SYST:LOC\n", 300);
}

void SerialThread::vthstep2_test(TestButton * selected) {
    SerialButton * psu = *selected->findSerial(SerialButton::PSU);
    VisaButton * dmm = (VisaButton *) *selected->findSerial(SerialButton::DMM);
    SerialButton * mcu = *selected->findSerial(SerialButton::MCU);


    psu->write("INST CH3\n", 300);
    QString data_str = "VOLT USERVOLTV\n";
    data_str.replace("USERVOLT", "0.1");
    QByteArray data = data_str.toUtf8();
    psu->write(data, 300);;
    msleep(500);
    psu->write("CHAN:OUTP ON\n", 300);
    msleep(500);

    int size = vthstep_data.size();
    float progress_scale = ((float) 100)/size;
    int p = 0;
    for(int i = 0; i < size; i++) {
        if (checkAbort()) {
            return;
        }

        QString step_string = vthstep_data[i][0];
        double val = step_string.toDouble();
        int dac = round((val / 3.3) * 4095);
        int digit_count = 0;
        if (dac < 0) {
            dac = 0;
        };
        if (dac > 4095) {
            dac = 4095;
        }

        if (dac < 10) {
            digit_count = 3;
        } else if (dac < 100) {
            digit_count = 2;
        } else if (dac < 1000) {
            digit_count = 1;
        } else {
            digit_count = 0;
        }
        QString dac_string = "9";
        for(int i = 0; i < digit_count; i++) {
            dac_string += "0";
        }
        dac_string += QString::number(dac) + "\n";
        qDebug() << dac_string.toUtf8();
        mcu->write(dac_string.toUtf8(), 500);
        msleep(30);
        dmm->write(ViString("MEAS:CURR:DC? 0.1, 0.00000007"));
        msleep(30); // 20
        dmm->read();
        QString current = QString(dmm->getBuf()).split("\n")[0];
        p++;
        emit progressUpdate(round((double) p * progress_scale));
        emit tableUpdate(p, 0, QDateTime::currentDateTime().toString().trimmed().remove(","));
        emit tableUpdate(p, 3, config_data[0].trimmed());
        emit tableUpdate(p, 12, vthstep_data[i][0]);
        emit tableUpdate(p, 13, current);
    }
    psu->write("CHAN:OUTP OFF\n", 300);
    psu->write("SYST:LOC\n", 300);
    msleep(300);
    msleep(300);

}


void SerialThread::vthstep3_test(TestButton * selected) {
    SerialButton * psu = *selected->findSerial(SerialButton::PSU);
    VisaButton * dmm = (VisaButton *) *selected->findSerial(SerialButton::DMM);
    SerialButton * mcu = *selected->findSerial(SerialButton::MCU);


    psu->write("INST CH3\n", 300);
    QString data_str = "VOLT USERVOLTV\n";
    data_str.replace("USERVOLT", "0.1");
    QByteArray data = data_str.toUtf8();

    psu->write(data, 300);;
    msleep(500);
    psu->write("CHAN:OUTP ON\n", 300);

    msleep(500);

    dmm->write(ViString("SAMP:COUN 10000"));
    msleep(500);
    dmm->write(ViString("SAMP:COUN:PRET 0"));
    msleep(500);
    dmm->write(ViString("TRIG:SOUR IMM"));
    msleep(500);
    dmm->write(ViString("SAMP:SOUR TIM"));
    msleep(500);
    dmm->write(ViString("SAMP:TIM 0.00002"));
    msleep(500);

    dmm->write(ViString("INIT"));


    mcu->write("99999\n", timeout_msec);


    emit progressUpdate(0);

    usleep(20 * 12500);

    emit progressUpdate(99);

    dmm->write(ViString("MMEM:STOR:DATA RDG_STORE, \"INT:\\newtest40_what\""));


    msleep(1 * 1000);




    psu->write("CHAN:OUTP OFF\n", 300);
    psu->write("SYST:LOC\n", 300);

    emit progressUpdate(100);

    msleep(300);
}



















































void SerialThread::setMaxRow(int max_row) {
    this->max_row = max_row;
}

void SerialThread::setMaxCol(int max_col) {
    this->max_col = max_col;
} // 1 us to 10 us. two things:

void SerialThread::configuration_set(std::vector<QString> config_data) {
    this->config_data = config_data;
}


void SerialThread::manual_connect(void) {
    if (manual_port->isVisa()) {
        qDebug() << "yo";
        VisaButton * temp = (VisaButton *) manual_port;
        temp->open(default_rm, temp->visa_port);
        qDebug() << temp->visa_port;
        this->thread()->msleep(200);
        if (temp->getLabel() == SerialButton::DMM) {
            temp->write(ViString("*IDN?"));
        } else {
            temp->write(ViString("*IDN?\n"));
        }


        this->thread()->msleep(200);
        temp->read();
        this->thread()->msleep(200);
        qDebug() << QString(temp->getBuf());
        if (QString(temp->getBuf()).contains(temp->getTarget())) {
            manual_port->connected = 1;
        } else {
            manual_port->connected = 0;
            viClose(temp->vi);
        }
        emit serialComplete(mode);
        return;
    }

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
    qDebug() << ID_str;
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
        if (temp_button->isVisa()) {
            VisaButton * temp = (VisaButton *) temp_button;
            viClose(temp->vi);

        }
        if (!temp_button->isVisa()) {
            if (temp_button->getPort() != nullptr) {
            temp_button->getPort()->close();
            delete(temp_button->getPort());
            temp_button->setPort(nullptr);
            }
        }
    }
    emit debug("showing all required idn's");
    for(size_t i = 0; i < selected->required.size(); i++) {
        emit debug((*selected->required[i])->getTarget());
        qDebug() << (*selected->required[i])->getTarget();
    }
    QList<QSerialPortInfo> serial_infos = QSerialPortInfo::availablePorts();
        emit debug("showing all available ports");
    qDebug() << serial_infos.size();
    for(int i = 0; i < serial_infos.size(); i++) {
        emit debug(serial_infos[i].description());
        qDebug() << serial_infos[i].description();
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
        msleep(300);
        temp_port->write(ID_comm);
        qDebug() << "sad";
        msleep(100);
        while(temp_port->waitForBytesWritten(timeout_msec));
        while(temp_port->waitForReadyRead(timeout_msec));
        QString ID_str = QString(temp_port->read(buffer_max));
        msleep(200);
        qDebug() << ID_str;
        int found = 0;
        for(size_t i = 0; i < selected->required.size(); i++) {
            SerialButton * temp_button = *(selected->required[i]);
            if (!temp_button->isVisa()) {
                if (ID_str.contains(temp_button->getTarget())) {
                    temp_button->setPort(temp_port);
                    emit debug("connected: " + temp_button->getTarget());
                    found = 1;
                }
            }

        }
        if (!found) {
            temp_port->close();
            delete(temp_port);
        }
    }


    for(size_t i = 0; i < selected->required.size(); i++) {
        SerialButton * temp_button = *(selected->required[i]);
        if (!temp_button->isVisa()) {
            if (temp_button->getPort() != nullptr) {
                temp_button->connected = 1;
            } else {
                temp_button->connected = 0;
            }
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
        int is_connected = ((*(selected->required[i]))->connected == 1);
        if (!is_connected) {
            emit debug("missing connections");
            emit serialComplete(mode);
            return;
        }
    }
    for(size_t i = 0; i < selected->required.size(); i++) {
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
        case SerialButton::Equipment::SIGGEN:
            siggen_configure((VisaButton *) serial_button, selected->getLabel());
            break;
        case SerialButton::Equipment::OSCO:
            osco_configure((VisaButton *) serial_button, selected->getLabel());
            break;
        case SerialButton::Equipment::DMM:
            dmm_configure((VisaButton *) serial_button, selected->getLabel());
            break;
        }
    }
    emit serialComplete(mode);
    return;
}

void SerialThread::dmm_configure(VisaButton * button, TestButton::Test label) {
    qDebug() << button->connected;
    QString res;
    switch(label) {
    case TestButton::Test::VTHSTEP: {
        qDebug() << "yo";
        button->write(ViString("SYST:COMM:ENAB ON, USB"));
        msleep(500);
        button->write(ViString("MEAS:CURR:DC? 0.1, 0.00000007"));
        msleep(500);
        button->read();
        msleep(500);
        res = QString(button->getBuf());
        qDebug() << res;
        if (!(res.contains("\n1") && (res.contains("E-") || res.contains("E+")))) {
            return;
        }
        break;
    }
    case TestButton::Test::VTHSTEP2:
        qDebug() << "yo";
        button->write(ViString("SYST:COMM:ENAB ON, USB"));
        msleep(500);
        button->write(ViString("MEAS:CURR:DC? 0.1, 0.00000007"));
        msleep(500);
        button->read();
        msleep(500);
        res = QString(button->getBuf());
        qDebug() << res;
        if (!(res.contains("\n1") && (res.contains("E-") || res.contains("E+")))) {
            return;
        }
        break;
    case TestButton::Test::VTHSTEP3:
        qDebug() << "yo";
        button->write(ViString("SYST:COMM:ENAB ON, USB"));
        msleep(500);
        // button->write(ViString("CONF:CURR:DC 0.1, 0.0000003"));

        button->write(ViString("CONF:CURR:DC 0.1, 0.000003"));
        msleep(500);
        button->write(ViString("SAMP:COUN 10"));
        msleep(500);
        button->write(ViString("SAMP:COUN:PRET 0"));
        msleep(500);
        button->write(ViString("TRIG:SOUR IMM"));
        msleep(500);
        button->write(ViString("SAMP:SOUR TIM"));
        msleep(500);
        button->write(ViString("SAMP:TIM 0.0001"));
        msleep(500);
        button->write(ViString("INIT"));
        msleep(3 * 1000);
        button->write(ViString("READ?"));
        msleep(500);
        button->read();
        msleep(500);
        res = QString(button->getBuf());
        qDebug() << res;
        if (!(res.contains("E-") || res.contains("E+"))) {
            return;
        }

        break;
    }
    button->configured = 1;
    emit debug("DMM configured");

}


void SerialThread::osco_configure(VisaButton * button, TestButton::Test label) {
    switch(label) {
        case TestButton::DPT:
        double duration = (2 * config_data[0].toDouble() + 3 * config_data[1].toDouble());

            // button->write(ViString("*RCL 1\n"));
            msleep(300);

            QString cmd = "TDIV USERDIVUS\n";
            cmd.replace("USERDIV", QString::number(duration / 10, 'f', 4));
            std::string cmd_std = cmd.toStdString();
            const char * chars_cmd = cmd_std.c_str();
            qDebug() << chars_cmd;
            button->write(ViString(chars_cmd));
            msleep(300);

            double delay = -1 * ((duration / 2) - 1*config_data[1].toDouble());
            QString delay_cmd = "TRDL USERDELUS\n";
            delay_cmd.replace("USERDEL", QString::number(delay));
            std::string delay_std = delay_cmd.toStdString();
            const char * delaychars_cmd = delay_std.c_str();
            qDebug() << delaychars_cmd;



            button->write(ViString(delaychars_cmd));
            msleep(300);

            button->write(ViString("C1:UNIT A\n"));
            msleep(300);

            button->write(ViString("C2:VDIV 2V\n"));
            msleep(300);

            button->write(ViString("C2:TRLV 300mv\n"));
            msleep(300);

            button->write(ViString("C2:OFST -3.2V\n"));
            msleep(300);

            button->write(ViString("C1:ATTN 1\n"));
            msleep(300);

            button->write(ViString("C1:VDIV 200mV\n"));
            msleep(300);


            button->write(ViString("TRSE EDGE, SR, C2, HT, TI, HV, 0US\n"));
            msleep(300);

            button->write(ViString("C3:ATTN 100\n"));
            msleep(300);


            button->write(ViString("C4:VDIV 200mV\n"));
            msleep(300);

            button->write(ViString("C4:OFST -990mV\n"));
            msleep(300);

            double v_value = (config_data[2].toDouble());


            QString v_cmd = "C3:VDIV V_VALV\n";
            v_cmd.replace("V_VAL", QString::number(v_value / 4));
            std::string v_cmd_std = v_cmd.toStdString();
            const char * v_chars_cmd = v_cmd_std.c_str();
            button->write(ViString(v_chars_cmd));
            msleep(300);

            v_cmd = "C3:OFST V_VALV\n";
            v_cmd.replace("V_VAL", QString::number(v_value * -1));
            v_cmd_std = v_cmd.toStdString();
            v_chars_cmd = v_cmd_std.c_str();
            button->write(ViString(v_chars_cmd));
            msleep(300);






            break;

        }
}
void SerialThread::siggen_configure(VisaButton * button, TestButton::Test label) {
    switch(label) {
        case TestButton::DPT:
            double period = config_data[0].toDouble() + config_data[1].toDouble();
            int frequency = round(pow(10, 6)/period);
            QString cmd = ":SOUR1:APPL:PULS USERFREQ,5,2.5,0\n";
            cmd.replace("USERFREQ", QString::number(frequency));
            std::string cmd_str = cmd.toStdString();
            const char * cmd_conv = cmd_str.c_str();
            button->write(ViString(cmd_conv));
            msleep(2000);



            QString cmd_temp = ":SOUR1:FUNC:PULS:PER USERP\n";
            cmd_temp.replace("USERP", QString::number(pow(10, -6) * period, 'f', 10));
            std::string cmdtemp_str = cmd_temp.toStdString();
            const char * cmdtemp_conv = cmdtemp_str.c_str();
            button->write(ViString(cmdtemp_conv));


            msleep(2000);


            QString width = QString::number(pow(10, -6) * config_data[0].toDouble(), 'f', 10);


            QString cmd_2 = ":SOUR1:FUNC:PULS:WIDT USERW\n";
            cmd_2.replace("USERW", width);

            std::string cmd2_str = cmd_2.toStdString();
            const char * cmd_2_conv = cmd2_str.c_str();
            button->write(ViString(cmd_2_conv));



            msleep(2000);

            break;
    }
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
        // int is_connected = ((*(selected->required[i]))->getPort() != nullptr);
        int is_connected = ((*(selected->required[i]))->connected == 1);
        if (!is_connected) {
            qDebug() << (*(selected->required[i]))->text();
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

    qDebug() << "halo";
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
        case TestButton::Test::VTHSTEP:
            vthstep_test(selected);
            break;
        case TestButton::Test::VTHSTEP2:
            vthstep2_test(selected);
            break;
        case TestButton::Test::VTHSTEP3:
            vthstep3_test(selected);
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
