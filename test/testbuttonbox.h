#ifndef TESTBUTTONBOX_H
#define TESTBUTTONBOX_H

#include <QWidget>

namespace Ui {
class TestButtonBox;
}

class TestButtonBox : public QWidget
{
    Q_OBJECT

public:
    explicit TestButtonBox(QWidget *parent = 0);
    ~TestButtonBox();

    QSize sizeHint() const;

private:
    Ui::TestButtonBox *m_ui;
};

#endif // TESTBUTTONBOX_H
