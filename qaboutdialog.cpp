#include "qaboutdialog.h"
#include <QGridLayout>
#include <QLayout>

QAboutDialog::QAboutDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	QGridLayout *layout = new QGridLayout; 
	layout->setSizeConstraint(QLayout::SetFixedSize);
	QWidget::setWindowFlags(Qt::Drawer);
	layout->setColumnMinimumWidth(0, 500);
	layout->addWidget(ui.label, 0,0);
	layout->addWidget(ui.label_2, 1,0);
	layout->addWidget(ui.label_3, 2,0);
	layout->addWidget(ui.pushOk,3,0);
	setLayout(layout);
	connect(ui.pushOk, SIGNAL(clicked()), this, SLOT(accept()));

}

QAboutDialog::~QAboutDialog()
{

}
