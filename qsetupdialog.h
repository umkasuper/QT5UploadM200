#ifndef QSETUPDIALOG_H
#define QSETUPDIALOG_H

#include <QDialog>
#include <QColor>
#include "ui_qsetupdialog.h"

class QSetupDialog : public QDialog
{
    Q_OBJECT

public:
    QSetupDialog(QWidget *parent = 0);
    ~QSetupDialog();
	void setTimeReboot(const QString &str) {ui.lineEdit->setText(str);};
	void setTimeErase(const QString &str) {ui.lineEdit_2->setText(str);};
	void setTimeBlock(const QString &str) {ui.lineEdit_3->setText(str);};
	
	void setFontName(const QString &name) {fontName = name;};
	void setFontSize(unsigned int size) {fontSize = size;};
	void setFontItalic(bool italic) {fontItalic = italic;};
	void setFontWidth(int width) {fontWidth = width;}
	void setBackgroundColor(QColor &bkColor) {backgroundColor = bkColor;}
	void setMainColor(QColor &mnColor) {mainColor = mnColor;}
	void setErrorColor(QColor &erColor) {errorColor = erColor;}
	void setCallColor(QColor &clColor) {callColor = clColor;}

	QString getTimeReboot() {return ui.lineEdit->text();};
	QString getTimeErase() {return ui.lineEdit_2->text();};
	QString getTimeBlock() {return ui.lineEdit_3->text();};
	
	void setStyle(QString &setStyle);// {style = setStyle;};
	QString getStyle() {return style;};

	QString getFontName() {return fontName;};
	unsigned int getFontSize() {return fontSize;};
	bool getFontItalic() {return fontItalic;};
	int getFontWidth() {return fontWidth;};
	QColor getBackgroundColor() {return backgroundColor;};
	QColor getMainColor() {return mainColor;};
	QColor getErrorColor() {return errorColor;};
	QColor getCallColor() {return callColor;};

	void changePalette();
private:
    Ui::QSetupDialogClass ui;
	QString fontName;
	QString style;
	unsigned int fontSize;
	int fontWidth;
	bool fontItalic;
	QColor backgroundColor;
	QColor mainColor;	
	QColor errorColor;
	QColor callColor;

private slots:
	void on_pushSelectCallColor_clicked();
	void on_pushSelectErrorColor_clicked();
	void on_pushMainColor_clicked();
	void on_pushButton_clicked();
	void on_pushSetterminalFont_clicked();
	void changeStyle(const QString &styleName);

};

#endif // QSETUPDIALOG_H
