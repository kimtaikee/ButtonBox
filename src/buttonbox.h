#ifndef BUTTONBOX_H
#define BUTTONBOX_H

#include <QScrollArea>

class QToolButton;
class ButtonBoxPrivate;
class ButtonBox : public QScrollArea
{
    Q_OBJECT
public:
    explicit ButtonBox(QWidget* parent = nullptr);
    ~ButtonBox();

public slots:
    void addButton(const QString& category, QToolButton* button);
    void addSubButton(QToolButton* button, QToolButton* subButton);

    void expandAll();
    void collapseAll();

    void setOrientation(Qt::Orientation o);
    Qt::Orientation orientation() const;

    void setExclusive(bool exclusive);
    bool isExclusive() const;

protected:
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    void resizeEvent(QResizeEvent* e);
    void showEvent(QShowEvent* e);
    void contextMenuEvent(QContextMenuEvent* e);

private:
    ButtonBoxPrivate* d_ptr;
    Q_DISABLE_COPY(ButtonBox)
};

#endif // BUTTONBOX_H
