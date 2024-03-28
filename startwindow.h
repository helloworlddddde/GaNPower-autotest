#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include <staticwindow.h>
#include <dynamicwindow.h>


class StartWindow : public QMainWindow
{
    Q_OBJECT
public:
    StartWindow(QWidget *parent = nullptr);
    void left_fn(void);
    void right_fn(void);
public slots:
    void slot_1(void);
    void slot_2(void);

private:
    QWidget * central_widget;
    QHBoxLayout * central_hbox;
    QVBoxLayout * left_vbox;
    QVBoxLayout * right_vbox;

    QPushButton * left_button;
    QPushButton * right_button;

    StaticWindow * static_window;
    DynamicWindow * dynamic_window;

    int static_on;

    int dynamic_on;
};
#endif // STARTWINDOW_H
