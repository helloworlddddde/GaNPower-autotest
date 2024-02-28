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
            case MANUALCONNECT:
                SerialThread::manual_connect();
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
        MANUALCONNECT,
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

    void setManualPort(SerialButton *, QSerialPortInfo);
    void configuration_set(std::vector<QString> config_data);

signals:
    void serialComplete(Mode mode);
    void progressUpdate(int value);
    void debug(QString str);

private:
    std::vector<TestButton *> test_buttons;
    void manual_connect(void);
    void autoconnect(void);
    void configure(void);
    void test(void);


    SerialButton * manual_port = nullptr;
    QSerialPortInfo manual_info;

    std::vector<QString> config_data;


    void mcu_configure(SerialButton * button, TestButton::Test test_choice);
    void psu_configure(SerialButton * button, TestButton::Test test_choice);
    void hipot_configure(SerialButton * button, TestButton::Test test_choice);
    void lcr_configure(SerialButton * button, TestButton::Test test_choice);
    void bv_test(void);
    void vth_test(void);
    void rdson_test(void);
    void bvstep_test(void);
    int max_row = 1;
    int max_col = 1;
};



#endif // SERIALTHREAD_H
