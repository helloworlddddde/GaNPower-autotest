#ifndef STATICWINDOW_H
#define STATICWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QSerialPort>
#include "serialthread.h"
#include "serialbutton.h"
#include "testbutton.h"



class StaticWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit StaticWindow(QWidget *parent = nullptr);


signals:
    void signal_1(void);
public slots:
    void serialHandle(SerialThread::Mode mode);
    void indexChanged(int);
    void reset_table(void);
    void save_table(void);
protected:
    void closeEvent(QCloseEvent *event);

private:
    QString vectorToString(std::vector<QString>);

    void autoconnect(void);

    void configure(void);

    void update_buttons(void);




    QWidget * central_widget;
    QVBoxLayout * central_vbox;

    QHBoxLayout * hbox_0;
    QHBoxLayout * hbox_1;
    QHBoxLayout * hbox_2;
    QHBoxLayout * hbox_3;

    QPushButton * new_button;
    QPushButton * test_button;
    QPushButton * save_button;

    const int button_max = 4;
    std::vector<SerialButton *> serial_buttons;

    const int test_max = 4;
    std::vector<TestButton *> test_buttons;

    int max_gan = 3;
    int max_spec = 6;
    std::vector<std::vector<QString>> gan_data;

    QPushButton * autoconnect_button;
    QPushButton * configure_button;

    SerialThread * serial_thread;

    QString configuration_label;
    QTextEdit * configuration_text;
    QTableWidget * result_table;

    QComboBox * row_combox;
    QComboBox * col_combox;

    const int max_row = 5;
    const int max_col = 5;
    QComboBox * gan_combox;

    std::vector<QString> configuration_data;




};



#endif // STATICWINDOW_H