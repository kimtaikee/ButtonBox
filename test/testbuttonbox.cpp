#include "testbuttonbox.h"
#include "ui_testbuttonbox.h"

#include <buttonbox.h>

#include <QToolButton>

void populateCategoryButtons(ButtonBox* box, const QString& category)
{
    for (int i = 0; i < 10; ++i) {
        QToolButton* btn = new QToolButton(box);
        btn->setIconSize(QSize(32, 32));

        box->addButton(category, btn);

        for (int j = 0; j < 5; ++j) {
            QToolButton* subBtn = new QToolButton(box);
            box->addSubButton(btn, subBtn);
            subBtn->setIconSize(QSize(32, 32));
        }
    }
}

TestButtonBox::TestButtonBox(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::TestButtonBox)
{
    m_ui->setupUi(this);
    ButtonBox* bb = new ButtonBox(this);
    m_ui->mainLayout->addWidget(bb);

    populateCategoryButtons(bb, "Layouts");
    populateCategoryButtons(bb, "Items");
    populateCategoryButtons(bb, "Algorithms");
    populateCategoryButtons(bb, "Filters");
    bb->expandAll();

    setMaximumWidth(300);
}

TestButtonBox::~TestButtonBox()
{
    delete m_ui;
}

QSize TestButtonBox::sizeHint() const
{
    return QSize(200, 500);
}
