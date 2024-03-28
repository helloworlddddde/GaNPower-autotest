#ifndef DYNAMICWINDOW_H
#define DYNAMICWINDOW_H
#include <QMainWindow>
#include <QtWidgets>
#include <QSerialPort>
#include "serialthread.h"
#include "serialbutton.h"
#include "testbutton.h"
#include "visabutton.h"

class DynamicWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DynamicWindow(QWidget *parent = nullptr);
public slots:
    void serialHandle(SerialThread::Mode mode);
    void onCompute(QString);
protected:
    void closeEvent(QCloseEvent *event);
signals:
    void signal_1(void);
private:
    void update_buttons(void);
    std::vector<SerialButton *> serial_buttons;
    std::vector<TestButton *> test_buttons;
    SerialThread * serial_thread;

    std::vector<QString> configuration_data;
    QLineEdit * a_line;
    QLineEdit * b_line;
    QLineEdit * v_line;
    float f;
};

#endif // DYNAMICWINDOW_H
