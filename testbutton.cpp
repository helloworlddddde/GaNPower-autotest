#include "testbutton.h"

TestButton::TestButton(const QString & text, QWidget * parent) : QPushButton(text, parent) {

}


void TestButton::setLabel(Test label) {
    this->label = label;
}

TestButton::Test TestButton::getLabel(void) {
    return label;
}
TestButton::~TestButton() {

}
