#ifndef SERIALBUTTON_H
#define SERIALBUTTON_H

#include <QPushButton>
#include <QSerialPort>

class SerialButton : public QPushButton
{
    Q_OBJECT
public:
    SerialButton(const QString & text, QWidget * parent = 0);
    virtual ~SerialButton();

    enum Equipment {
        MCU,
        PSU,
        HIPOT,
        LCR,
        DMM,
        SIGGEN,
        OSCO,
        POW
    };

    void setPort(QSerialPort * port);
    void setLabel(Equipment label);
    void setTarget(const QString& target);
    int isVisa(void);

    QSerialPort * getPort(void);
    Equipment getLabel(void);
    QString getTarget(void);

    int configured = 0;
    int connected = 0;

    void write(QByteArray, int);
    QByteArray read(int, int);



protected:
    Equipment label;
    QSerialPort * port;
    QString target;
    int is_visa = 0;

};




#endif // SERIALBUTTON_H
