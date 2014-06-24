#ifndef QTERMINAL_H
#define QTERMINAL_H

#include <QWidget>
#include <QTextCursor>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextBrowser>
#include <QListWidget>
#include <QListWidgetItem>

#include "ui_qterminal.h"
#include "Highlighter.h"
#include "qsetbutton.h"
#include "qsetbuttondlg.h"
#include "QTerminalBrowser.h"

class QTerminal : public QWidget
{
	Q_OBJECT

public:
	QTerminal(QWidget *parent, QColor &backgroundColor, QColor &mainColor, QColor &errorColor, QColor &callColor);
//	QTerminal(QWidget *parent = 0);
	~QTerminal();
	void closeEvent ( QCloseEvent * e );
	QLineEdit *getLineEdit() {return ui.lineEdit;};
	void displayData(QString rcvString, bool insertBlock);
	void setTerminalOutFont(const QFont &setFont);
	void SetButtons(QString *name, QString *command, unsigned int count);
	QTextBrowser *getTerminal() {return textOutTerminal;};

private:
	void setupEditor();

	Ui::QTerminalClass ui;
	QTerminalBrowser *textOutTerminal;
	bool scroll;
	Highlighter highlighter;
	void parseButton(QSetButton *sender);
	QColor backgroundColor;
	QColor mainColor;
	QColor errorColor;
	QColor callColor;
signals:
	void closed();
	void keyPress(QKeyEvent * e);
	void transferStart();
	void buttonChanged(unsigned int number, const QString &m_name, const QString &m_command);
	void command(const QString &m_command);

private slots:
	void on_pushHistory_clicked();
	void on_pushSetButton6_clicked();
	void on_pushSetButton5_clicked();
	void on_pushSetButton2_clicked();
	void on_pushSetButton4_clicked();
	void on_pushSetButton3_clicked();
	void on_pushSetButton1_clicked();
	void terminalKeyPressed(QKeyEvent *e);
	void verticalScrollChanged(int value);
	void rightPressed(QSetButton *sender);
	void buttonSetupClosed(QSetButton *sender, bool res);
	void itemSelected(QListWidgetItem *item);
	void listClose();
	void on_edit_send_closed();

private:
	QSetButtonDlg *m_setup;
	QListWidget *listWidget;

};

#endif // QTERMINAL_H
