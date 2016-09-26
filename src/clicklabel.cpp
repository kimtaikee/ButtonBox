#include "clicklabel.h"

#include <QMouseEvent>

ClickLabel::ClickLabel(QWidget* parent) : QLabel(parent)
{

}

void ClickLabel::mousePressEvent(QMouseEvent *e)
{
    QLabel::mousePressEvent(e);

    if (e->buttons() & Qt::LeftButton)
        emit clicked();
}
