#include "visabutton.h"

VisaButton::VisaButton(const QString & text, QWidget * parent) : SerialButton(text, parent) {
    this->is_visa = 1;
}

ViChar * VisaButton::getBuf(void) {
    return this->buf;
}

void VisaButton::write(ViString str) {
    strncpy(buf, str, buf_size);
    viWrite(vi, (ViBuf) buf, buf_size, &wcount);
}

void VisaButton::read() {
    viRead(vi, (ViBuf) buf, buf_size, &rcount);
}

void VisaButton::open(ViSession * rm, ViString str) {
    viOpen(*rm, ViString(str), VI_NULL,VI_NULL, &vi);
}


