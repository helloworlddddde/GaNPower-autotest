#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QApplication>
#include "serialbutton.h"
#include "testbutton.h"




class SerialThread : public QThread
{
    Q_OBJECT
    void run() override {
        switch(mode) {
            case NONE:
                break;
            case AUTOCONNECT:
                SerialThread::autoconnect();
                break;
            case CONFIGURE:
                SerialThread::configure();
                break;
            case TEST:
                SerialThread::test();
                break;
        }
    }

public:
    explicit SerialThread(QObject *parent = nullptr);
    enum Mode {
        NONE,
        AUTOCONNECT,
        CONFIGURE,
        TEST
    };
    Mode mode = Mode::NONE;

    int timeout_msec = 200;
    qint64 buffer_max = 128;


    void setMaxRow(int max_row);
    void setMaxCol(int max_col);

    void test_set(std::vector<TestButton *> test_buttons);

    void serial_set(std::vector<SerialButton *> serial_buttons);

    void configuration_set(std::vector<QString> config_data);

signals:
    void serialComplete(Mode mode);

private:
    std::vector<SerialButton *> serial_buttons;
    std::vector<TestButton *> test_buttons;
    void autoconnect(void);
    void configure(void);
    void test(void);

    std::vector<QString> config_data;

    void mcu_configure(SerialButton * button, TestButton::Test test_choice);
    void psu_configure(SerialButton * button, TestButton::Test test_choice);
    void hipot_configure(SerialButton * button, TestButton::Test test_choice);
    void lcr_configure(SerialButton * button, TestButton::Test test_choice);
    int max_row = 1;
    int max_col = 1;
};



#endif // SERIALTHREAD_H
