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
    virtual ~SerialThread();
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
    void step_set(std::vector<QString> step_data);

signals:
    void serialComplete(Mode mode);
    void progressUpdate(int value);
    void tableUpdate(int row, int col, QString val);
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
    std::vector<QString> step_data;

    void mcu_configure(SerialButton * button, TestButton::Test test_choice);
    void psu_configure(SerialButton * button, TestButton::Test test_choice);
    void hipot_configure(SerialButton * button, TestButton::Test test_choice);
    void lcr_configure(SerialButton * button, TestButton::Test test_choice);
    void bv_test(TestButton *);
    void vth_test(TestButton *);
    void rdson_test(TestButton *);
    void bvstep_test(TestButton *);
    int max_row = 1;
    int max_col = 1;
};



#endif // SERIALTHREAD_H
