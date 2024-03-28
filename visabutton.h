#ifndef VISABUTTON_H
#define VISABUTTON_H

#include "serialbutton.h"
#include "visa.h"

class VisaButton : public SerialButton
{
    Q_OBJECT
public:
    VisaButton(const QString & text, QWidget * parent = 0);
    void write(ViString);
    void read(void);
    void open(ViSession *, ViString);
    ViChar * getBuf(void);
    ViString visa_port;
    ViSession vi = VI_NULL;


private:

    ViUInt32 wcount;
    ViUInt32 rcount;
    int buf_size = 256;
    ViChar buf[256];

};

#endif // VISABUTTON_H
