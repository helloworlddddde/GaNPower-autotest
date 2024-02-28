#include "testbutton.h"

TestButton::TestButton(const QString & text, QWidget * parent) : QPushButton(text, parent) {

}


void TestButton::setLabel(Test label) {
    this->label = label;
}

TestButton::Test TestButton::getLabel(void) {
    return label;
}

void TestButton::setupToolTip(void) {
    QString str = "";
    for(size_t i = 0; i < required.size(); i++) {
        if (i == 0) {
            str += (*(required[i]))->text();
        } else {
            str += ", " + (*(required[i]))->text();
        }
    }
    this->setToolTip(str);
}

int TestButton::handshake(int buffer_size, int msec) {
    for(size_t i = 0; i < required.size(); i++) {
        SerialButton * serial_button = *required[i];
        QByteArray ID_comm = "*IDN?\n";
        serial_button->write(ID_comm, msec);
        QString id_str = QString(serial_button->read(buffer_size, msec));
        if(!id_str.contains(serial_button->getTarget())) {
            return 0;
        }
    }
    return 1;
}
SerialButton * * TestButton::findSerial(SerialButton::Equipment label) {
    for(size_t i = 0; i < required.size(); i++) {
        if ((*required[i])->getLabel() == label) {
            return required[i];
        }
    }
    return nullptr;
}
TestButton::~TestButton() {

}
