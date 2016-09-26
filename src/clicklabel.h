#ifndef CLICKLABEL_H
#define CLICKLABEL_H

#include <QLabel>

class ClickLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickLabel(QWidget* parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* e);

};

#endif // CLICKLABEL_H
