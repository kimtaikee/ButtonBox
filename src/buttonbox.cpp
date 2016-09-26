#include "buttonbox.h"
#include "flowlayout.h"
#include "clicklabel.h"

#include <QToolButton>
#include <QMap>
#include <QLabel>
#include <QSpacerItem>
#include <QDebug>
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QContextMenuEvent>
#include <QGraphicsDropShadowEffect>
#include <QWidgetAction>


typedef QList<QToolButton*> QToolButtonList;

class ToolButtonMenu : public QMenu
{
    Q_OBJECT
public:
    enum { Spacing = 2 };
    explicit ToolButtonMenu(QWidget* parent = nullptr);

    void addButton(QToolButton* button);

    QSize sizeHint() const;

private:
    QToolButtonList m_buttonList;
    QBoxLayout* m_layout;
};

ToolButtonMenu::ToolButtonMenu(QWidget *parent) : QMenu(parent)
{
    m_layout = new QHBoxLayout;
    m_layout->setContentsMargins(Spacing, Spacing, Spacing, Spacing);
    m_layout->setSpacing(Spacing);

    setLayout(m_layout);
}

void ToolButtonMenu::addButton(QToolButton *button)
{
    m_buttonList.append(button);
    m_layout->addWidget(button);
}

QSize ToolButtonMenu::sizeHint() const
{
    if (m_buttonList.isEmpty())
        return QSize(32, 32);

    return m_layout->sizeHint();
}

//////////////////////////////////////
/// The ButtonPopup class
//////////////////////////////////////
class ButtonPopup : public QFrame
{
    Q_OBJECT
public:
    explicit ButtonPopup(QWidget* parent = nullptr);

    void addButton(QToolButton* button);
    void addButtons(const QToolButtonList& buttonList);
    void clear();

    QSize sizeHint() const;

private:
    FlowLayout* m_layout;
};

ButtonPopup::ButtonPopup(QWidget *parent) : QFrame(parent, Qt::Popup)
{
    setFrameStyle(QFrame::StyledPanel);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    m_layout = new FlowLayout;
    setLayout(m_layout);
}

void ButtonPopup::addButton(QToolButton* button)
{
    m_layout->addWidget(button);
    button->show();
}

void ButtonPopup::addButtons(const QToolButtonList& buttonList)
{
    foreach (QToolButton* button, buttonList)
        addButton(button);
}

void ButtonPopup::clear()
{
    m_layout->clear();
}

QSize ButtonPopup::sizeHint() const
{
    return QSize(120, 80);
}

////////////////////////////////////////
/// The CategoryHeader class
////////////////////////////////////////
class CategoryHeader : public QFrame
{
    Q_OBJECT
public:
    explicit CategoryHeader(QWidget* parent = nullptr);

public slots:
    void setTextAlignment(Qt::Alignment align);
    Qt::Alignment textAlignment() const;

    void setTitle(const QString& title);
    QString title() const;

    void setChecked(bool check);
    bool checked() const;

signals:
    void expand(bool expand);

private slots:
    void onButtonToggled(bool toggle);
    void onLabelClicked();

private:
    Qt::Alignment m_textAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    QString m_title = tr("Title");
    QToolButton* m_expandButton;
    ClickLabel* m_titleLabel;
};

CategoryHeader::CategoryHeader(QWidget *parent) : QFrame(parent)
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(1, 1, 1, 1);
    layout->setSpacing(6);

    m_expandButton = new QToolButton(this);
    m_expandButton->setAutoRepeat(false);
    m_expandButton->setAutoRaise(true);
    m_expandButton->setCheckable(true);
    m_expandButton->setIconSize(QSize(24, 24));
    m_expandButton->setIcon(QIcon(":/images/arrow_down_24x24.png"));

    m_titleLabel = new ClickLabel(this);
    m_titleLabel->setAlignment(m_textAlignment);

    layout->addWidget(m_expandButton);
    layout->addWidget(m_titleLabel);
    setLayout(layout);

    setFixedHeight(30);

    connect(m_expandButton, SIGNAL(toggled(bool)), this, SLOT(onButtonToggled(bool)));
    connect(m_titleLabel, SIGNAL(clicked()), this, SLOT(onLabelClicked()));

    setFrameShape(QFrame::Panel);
    setFrameShadow(QFrame::Raised);

    setStyleSheet("background-color:rgb(100,158,223);");
}

void CategoryHeader::setTextAlignment(Qt::Alignment align)
{
    m_titleLabel->setAlignment(align);
}

Qt::Alignment CategoryHeader::textAlignment() const
{
    return m_titleLabel->alignment();
}

void CategoryHeader::setTitle(const QString& title)
{
    m_titleLabel->setText(title);
}

QString CategoryHeader::title() const
{
    return m_titleLabel->text();
}

void CategoryHeader::setChecked(bool check)
{
    m_expandButton->setChecked(check);
}

bool CategoryHeader::checked() const
{
    return m_expandButton->isChecked();
}

void CategoryHeader::onButtonToggled(bool toggle)
{
    m_expandButton->setIcon(toggle ? QIcon(":/images/arrow_up_24x24.png") : QIcon(":/images/arrow_down_24x24.png"));
    emit expand(!toggle);
}

void CategoryHeader::onLabelClicked()
{
    m_expandButton->toggle();
}

////////////////////////////////////////
/// The CategoryContainer class
////////////////////////////////////////
class CategoryContainer : public QWidget
{
    Q_OBJECT
public:
    explicit CategoryContainer(QWidget* parent = nullptr);

    void addButton(QToolButton* button);

    int height() const { return m_layout->heightForWidth(this->width()); }

private:
    FlowLayout* m_layout;
};

CategoryContainer::CategoryContainer(QWidget *parent) : QWidget(parent)
{
    m_layout = new FlowLayout;
    setLayout(m_layout);
}

void CategoryContainer::addButton(QToolButton *button)
{
    m_layout->addWidget(button);
}

////////////////////////////////////////
/// The CategoryWidget class
////////////////////////////////////////
class CategoryWidget : public QFrame
{
    Q_OBJECT
public:
    explicit CategoryWidget(QWidget* parent = nullptr);

    void setTitle(const QString& title);
    QString title() const;

    void setTitleAlignment(Qt::Alignment align);
    Qt::Alignment titleAlignment() const;

    void addButton(QToolButton* button);

    QSize sizeHint() const;

    void updateGeo();

    void setOrientation(Qt::Orientation o);
    Qt::Orientation orientation() const;

signals:
    void expanded(bool expand);

public slots:
    void expand(bool expand);

protected:
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);

private slots:
    void onAnimationFinished();

private:
    bool m_expand = false;
    Qt::Orientation m_orientation = Qt::Vertical;
    CategoryHeader* m_header = nullptr;
    CategoryContainer* m_container = nullptr;
    QBoxLayout* m_layout = nullptr;
};

CategoryWidget::CategoryWidget(QWidget *parent) : QFrame(parent)
{
    m_header = new CategoryHeader(this);
    m_container = new CategoryContainer(this);

    m_layout = new QVBoxLayout;
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_header);
    m_layout->addWidget(m_container);
    setLayout(m_layout);

    connect(m_header, SIGNAL(expand(bool)), this, SLOT(expand(bool)));

    setFrameShape(QFrame::Panel);
    setFrameShadow(QFrame::Sunken);
}

void CategoryWidget::setTitle(const QString &title)
{
    m_header->setTitle(title);
}

QString CategoryWidget::title() const
{
    return m_header->title();
}

void CategoryWidget::setTitleAlignment(Qt::Alignment align)
{
    m_header->setTextAlignment(align | Qt::AlignVCenter);
}

Qt::Alignment CategoryWidget::titleAlignment() const
{
    return m_header->textAlignment();
}

void CategoryWidget::addButton(QToolButton* button)
{
    m_container->addButton(button);
}

QSize CategoryWidget::sizeHint() const
{
    return QSize(m_header->width(), m_header->height() + m_container->height());
}

void CategoryWidget::updateGeo()
{
    qDebug() << "container height: " << m_container->height();

    setFixedHeight(m_container->isVisible() ? (m_header->height() + m_container->height()) : m_header->height());
}

void CategoryWidget::setOrientation(Qt::Orientation o)
{
    return;

    if (m_orientation != o) {
        m_orientation = o;

        delete m_layout;

        if (m_orientation == Qt::Horizontal)
            m_layout = new QHBoxLayout;
        else
            m_layout = new QVBoxLayout;

        m_layout->setSpacing(0);
        m_layout->setContentsMargins(0, 0, 0, 0);

        m_layout->addWidget(m_header);
        m_layout->addWidget(m_container);
        setLayout(m_layout);
    }
}

Qt::Orientation CategoryWidget::orientation() const
{
    return m_orientation;
}

void CategoryWidget::expand(bool expand)
{
    m_expand = expand;
#if 0
    QPropertyAnimation *animation = new QPropertyAnimation(m_container, "maximumHeight");
    animation->setDuration(500);

    //Now animation to provide a fly-by effect
    if (expand) {
        animation->setStartValue(0);
        animation->setEndValue(m_container->heightForWidth(this->width()));
    } else {
        animation->setStartValue(m_container->heightForWidth(this->width()));
        animation->setEndValue(0);
    }

    connect(animation, SIGNAL(finished()), this, SLOT(onAnimationFinished()));

    animation->start();

#else
    onAnimationFinished();
#endif
}

void CategoryWidget::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);
    updateGeo();
}

void CategoryWidget::showEvent(QShowEvent *event)
{
    QFrame::showEvent(event);
    updateGeo();
}

void CategoryWidget::onAnimationFinished()
{
    m_container->setVisible(m_expand);

    m_header->blockSignals(true);
    m_header->setChecked(!m_expand);
    m_header->blockSignals(false);

    updateGeo();
    emit expanded(m_expand);
}

//////////////////////////////////////
/// The ButtonBoxPrivate class
//////////////////////////////////////
class ButtonBoxPrivate : public QWidget
{
    Q_OBJECT
public:
    ButtonBoxPrivate(QScrollArea* q);

    void updateGeo();
    void setOrientation(Qt::Orientation o);
    Qt::Orientation orientation() const;

    Qt::Orientation orient = Qt::Vertical; // default to vertical
    QScrollArea* q_ptr;
    QMap<QString, QWidget*> categoryWidgetMap;
    QBoxLayout* layout = nullptr;
    QList<QToolButton*> rootButtons;
    QMap<QToolButton*, QToolButtonList> button2SubButtonsMap;
    QMap<QToolButton*, ToolButtonMenu*> button2MenuMap;
    ButtonPopup* buttonPopup = nullptr;

    QMenu* contextMenu = nullptr;

private slots:
    void onExpand(bool expand);
    void onButtonToggled(bool toggled);
};

ButtonBoxPrivate::ButtonBoxPrivate(QScrollArea *q) : q_ptr(q)
{
    layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);
}

void ButtonBoxPrivate::updateGeo()
{
    int hei = 0;
    auto iter = categoryWidgetMap.begin();
    while (iter != categoryWidgetMap.end()) {
        QWidget* widget = iter.value();
        hei += widget->height();
        ++iter;
    }

    this->setFixedHeight(hei);
}

void ButtonBoxPrivate::setOrientation(Qt::Orientation o)
{
    if (orient != o) {
        orient = o;

        delete this->layout;

        if (orient == Qt::Horizontal)
            this->layout = new QHBoxLayout;
        else
            this->layout = new QVBoxLayout;
        this->layout->setSpacing(0);
        this->layout->setContentsMargins(0, 0, 0, 0);

        QWidgetList widgets = this->categoryWidgetMap.values();
        foreach (QWidget* widget, widgets) {
            CategoryWidget* cw = qobject_cast<CategoryWidget*>(widget);
            cw->setOrientation(o);

            this->layout->addWidget(widget);
        }

        this->setLayout(this->layout);
    }
}

Qt::Orientation ButtonBoxPrivate::orientation() const
{
    return this->orient;
}

void ButtonBoxPrivate::onExpand(bool expand)
{
    Q_UNUSED(expand)
    updateGeo();
}

void ButtonBoxPrivate::onButtonToggled(bool toggled)
{
    if (!toggled)
        return;

    if (!buttonPopup)
        buttonPopup = new ButtonPopup;

    QToolButton* button = qobject_cast<QToolButton*>(sender());

    buttonPopup->clear();
    QToolButtonList buttonList = this->button2SubButtonsMap.value(button);
    buttonPopup->addButtons(buttonList);

    // layout
    const QRect desktop = QApplication::desktop()->geometry();
    // Make sure the popup is inside the desktop.

    QPoint pos = button->mapToGlobal(button->pos());
    qDebug() << "gpos: " << pos;
    if (pos.x() < desktop.left())
        pos.setX(desktop.left());
    if (pos.y() < desktop.top())
        pos.setY(desktop.top());

    if ((pos.x() + buttonPopup->sizeHint().width()) > desktop.width())
        pos.setX(desktop.width() - buttonPopup->sizeHint().width());
    if ((pos.y() + buttonPopup->sizeHint().height()) > desktop.bottom())
        pos.setY(desktop.bottom() - buttonPopup->sizeHint().height());
    buttonPopup->move(pos);

    // Remove focus from this widget, preventing the focus rect
    // from showing when the popup is shown. Order an update to
    // make sure the focus rect is cleared.
    clearFocus();
    update();

    // Allow keyboard navigation as soon as the popup shows.
    buttonPopup->setFocus();

    // Execute the popup. The popup will enter the event loop.
    buttonPopup->show();
}

/////////////////////////////////////
/// The ButtonBox class
/////////////////////////////////////
ButtonBox::ButtonBox(QWidget* parent) : QScrollArea(parent)
{
    d_ptr = new ButtonBoxPrivate(this);
    this->setContentsMargins(0, 0, 0, 0);
//    setMaximumWidth(300);
    setMinimumWidth(120);

    setWidget(d_ptr);
    setWidgetResizable(true);
}

ButtonBox::~ButtonBox()
{
    delete d_ptr;
}

QMenu* createMenu()
{
    QMenu* menu = new QMenu();
    QAction* act1 = new QAction();
    act1->setText("Action 1");
    menu->addAction(act1);

    QAction* act2 = new QAction();
    act2->setText("Action 2");
    menu->addAction(act2);

    QAction* act3 = new QAction();
    act3->setText("Action 3");
    menu->addAction(act3);

    return menu;
}

void ButtonBox::addButton(const QString& category, QToolButton* button)
{
    if (!d_ptr->categoryWidgetMap.contains(category)) {
        CategoryWidget* cw = new CategoryWidget(this);
        cw->setTitle(category);
        d_ptr->categoryWidgetMap.insert(category, cw);
        cw->addButton(button);
        d_ptr->layout->addWidget(cw);
        connect(cw, SIGNAL(expanded(bool)), d_ptr, SLOT(onExpand(bool)));
    } else {
        CategoryWidget* cw = qobject_cast<CategoryWidget*>(d_ptr->categoryWidgetMap.value(category));
        cw->addButton(button);
    }

    disconnect(button, SIGNAL(toggled(bool)), d_ptr, SLOT(onButtonToggled(bool)));
    connect(button, SIGNAL(toggled(bool)), d_ptr, SLOT(onButtonToggled(bool)));

    d_ptr->rootButtons.append(button);
}

void ButtonBox::addSubButton(QToolButton* button, QToolButton* subButton)
{

    if (!button || !subButton)
        return;

    Q_ASSERT_X(d_ptr->rootButtons.contains(button), "addSubButton" , "trying to add button to non-root button");

    if (!d_ptr->button2MenuMap.contains(button)) {
        button->setPopupMode(QToolButton::InstantPopup);

        ToolButtonMenu* btm = new ToolButtonMenu(this);
        btm->addButton(subButton);
        button->setMenu(btm);
        d_ptr->button2MenuMap.insert(button, btm);
    } else {
        ToolButtonMenu* menu = d_ptr->button2MenuMap[button];
        menu->addButton(subButton);
    }
}

void ButtonBox::expandAll()
{
    QWidgetList categoryWidgets = d_ptr->categoryWidgetMap.values();
    foreach (QWidget* widget, categoryWidgets) {
        CategoryWidget* cw = qobject_cast<CategoryWidget*>(widget);
        cw->expand(true);
    }
}

void ButtonBox::collapseAll()
{
    QWidgetList categoryWidgets = d_ptr->categoryWidgetMap.values();
    foreach (QWidget* widget, categoryWidgets) {
        CategoryWidget* cw = qobject_cast<CategoryWidget*>(widget);
        cw->expand(false);
    }
}

void ButtonBox::setOrientation(Qt::Orientation o)
{
    if (o == Qt::Horizontal) {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }

    d_ptr->setOrientation(o);
}

Qt::Orientation ButtonBox::orientation() const
{
    return d_ptr->orientation();
}

void ButtonBox::setExclusive(bool exclusive)
{
    // TODO
}

bool ButtonBox::isExclusive() const
{
    // TODO
    return false;
}

QSize ButtonBox::sizeHint() const
{
    return QSize(220, 80);
}

QSize ButtonBox::minimumSizeHint() const
{
    return QSize(80, 50);
}

void ButtonBox::resizeEvent(QResizeEvent *e)
{
    QScrollArea::resizeEvent(e);
    d_ptr->updateGeo();
}

void ButtonBox::showEvent(QShowEvent *e)
{
    QScrollArea::showEvent(e);
    d_ptr->updateGeo();
}

void ButtonBox::contextMenuEvent(QContextMenuEvent* e)
{
    if (!d_ptr->contextMenu) {
        d_ptr->contextMenu = new QMenu(this);

        QAction* expandAllAct = new QAction(this);
        expandAllAct->setText(tr("Expand All"));
        connect(expandAllAct, SIGNAL(triggered()), this, SLOT(expandAll()));
        d_ptr->contextMenu->addAction(expandAllAct);

        QAction* collapseAllAct = new QAction(this);
        collapseAllAct->setText(tr("Collapse All"));
        connect(collapseAllAct, SIGNAL(triggered()), this, SLOT(collapseAll()));
        d_ptr->contextMenu->addAction(collapseAllAct);
    }

    d_ptr->contextMenu->exec(e->globalPos());
}

#include "buttonbox.moc"
