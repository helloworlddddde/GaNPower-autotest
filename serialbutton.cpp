#include "serialbutton.h"

SerialButton::SerialButton(const QString & text, QWidget * parent) : QPushButton(text, parent) {

}


void SerialButton::setPort(QSerialPort * port) {
    this->port = port;
}

void SerialButton::setLabel(SerialButton::Equipment label) {
    this->label = label;
}

void SerialButton::setTarget(const QString & target) {
    this->target = target;
}

QSerialPort * SerialButton::getPort(void) {
    return port;
}

void SerialButton::write(QByteArray data, int msec) {
    this->getPort()->write(data);
    while(this->getPort()->waitForBytesWritten(msec));
}
SerialButton::Equipment SerialButton::getLabel(void) {
    return label;
}

QString SerialButton::getTarget(void) {
    return target;
}

QByteArray SerialButton::read(int max_size, int msec) {
    while(this->getPort()->waitForReadyRead(msec));
    return this->getPort()->readLine(max_size);
}

SerialButton::~SerialButton() {
}
