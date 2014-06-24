#ifndef QSETBUTTONDLG_H
#define QSETBUTTONDLG_H

#include <QWidget>
#include "ui_qsetbuttondlg.h"
#include "qsetbutton.h"

class QSetButtonDlg : public QWidget
{
    Q_OBJECT

public:
    QSetButtonDlg(QWidget *parent = 0);
    ~QSetButtonDlg();

	void setButton(QSetButton *sender) {button = sender;};
	QString getName() {return ui.lineName->text();} 
	QString getCommand() {return ui.lineCommand->text();}
	void setName(const QString &m_name) {ui.lineName->setText(m_name);}
	void setCommand(const QString &m_command) {ui.lineCommand->setText(m_command);}

private:
    Ui::QSetButtonDlgClass ui;
	QSetButton *button;

signals:
	void closed(QSetButton *sender, bool res);

private slots:
	void on_pushCancel_clicked();
	void on_pushOk_clicked();
};

#endif // QSETBUTTONDLG_H
