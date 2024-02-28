#ifndef TESTBUTTON_H
#define TESTBUTTON_H
#include <QPushButton>
#include "serialbutton.h"

class TestButton : public QPushButton
{
    Q_OBJECT
public:
    TestButton(const QString & text, QWidget * parent = 0);
    virtual ~TestButton();

    enum Test {
        NOTEST,
        BV,
        VTH,
        RDSON,
        BVSTEP
    };


    int selected = 0;
    void setLabel(Test label);
    TestButton::Test getLabel(void);
    std::vector<SerialButton * *> required;
    void setupToolTip(void);

private:
    Test label;

};

#endif // TESTBUTTON_H
