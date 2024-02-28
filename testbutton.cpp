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
