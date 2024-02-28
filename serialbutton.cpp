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

SerialButton::Equipment SerialButton::getLabel(void) {
    return label;
}

QString SerialButton::getTarget(void) {
    return target;
}

SerialButton::~SerialButton() {
}
