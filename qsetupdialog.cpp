#include "qsetupdialog.h"
#include <QHBoxLayout>
#include <QFontDialog>
#include <QStyleFactory>
#include <QColorDialog>
#include <QColor>

QSetupDialog::QSetupDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QGridLayout *layout = new QGridLayout; 
	layout->setSizeConstraint(QLayout::SetFixedSize);
	layout->addWidget(ui.label, 0,0);
	layout->addWidget(ui.lineEdit, 0,1);
	layout->addWidget(ui.label_2,1,0);
	layout->addWidget(ui.lineEdit_2,1,1);
	layout->addWidget(ui.label_3,2,0);
	layout->addWidget(ui.lineEdit_3,2,1);

	QHBoxLayout *line4 = new QHBoxLayout;
	line4->addWidget(ui.pushSetterminalFont);  
	layout->addLayout(line4,3,0);   

//	layout->addWidget(ui.pushSetterminalFont,3,0);
	layout->addWidget(ui.labelStyle, 4,0);
	layout->addWidget(ui.comboStyle, 4,1);
	layout->addWidget(ui.pushButton, 5,0);
	layout->addWidget(ui.pushMainColor, 5,1);
	layout->addWidget(ui.pushSelectErrorColor, 6,0);
	layout->addWidget(ui.pushSelectCallColor, 6,1);
	
	layout->addWidget(ui.pushOk, 7,0);
	layout->addWidget(ui.pushCancel, 7,1);
	//layout->addLayout(ui.gridLayout1);
	setLayout(layout);
	ui.comboStyle->addItems(QStyleFactory::keys());

	connect(ui.pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui.pushOk, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.comboStyle, SIGNAL(activated(const QString &)),	this, SLOT(changeStyle(const QString &)));

}

QSetupDialog::~QSetupDialog()
{

}


void QSetupDialog::on_pushSetterminalFont_clicked()
{
	bool ok;

	QFont m_selFont = QFontDialog::getFont(&ok, QFont(fontName, fontSize, fontWidth, fontItalic));

	if (ok)
	{
		// Забираем параметры шрифта
		fontName = m_selFont.family();
		fontSize = m_selFont.pointSize();
		fontItalic = m_selFont.italic();
		fontWidth =  m_selFont.weight();

	}
}

void QSetupDialog::changeStyle(const QString &styleName)
{
	QApplication::setStyle(QStyleFactory::create(styleName));
	style = styleName;
	changePalette();
}

void QSetupDialog::changePalette()
{
	QApplication::setPalette(QApplication::palette());
}

void QSetupDialog::setStyle(QString &setStyle) {
	style = setStyle;
	int selStyle = ui.comboStyle->findText(style);
	if (selStyle != -1)
		ui.comboStyle->setCurrentIndex(selStyle);
};



void QSetupDialog::on_pushButton_clicked()
{
	QColor color = QColorDialog::getColor(backgroundColor, this);
	if (color.isValid()) {
		backgroundColor = color;
	}
}

void QSetupDialog::on_pushMainColor_clicked()
{
	QColor color = QColorDialog::getColor(mainColor, this);
	if (color.isValid()) {
		mainColor = color;
	}

}

void QSetupDialog::on_pushSelectErrorColor_clicked()
{
	QColor color = QColorDialog::getColor(errorColor, this);
	if (color.isValid()) {
		errorColor = color;
	}

}

void QSetupDialog::on_pushSelectCallColor_clicked()
{
	QColor color = QColorDialog::getColor(callColor, this);
	if (color.isValid()) {
		callColor = color;	
	}

}