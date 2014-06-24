#ifndef QSETBUTTON_H
#define QSETBUTTON_H

#include <QPushButton>
#include <QMouseEvent>

class QSetButton : public QPushButton
{
	Q_OBJECT

public:
    QSetButton(QWidget *parent);
    ~QSetButton();
	void setName(QString &m_setName) {m_name = m_setName; setText(m_name);};
	void setCommand(QString &m_setCommand) {m_command = m_setCommand;};
	void setNumber(unsigned int num) {number = num;}
	unsigned int getNumber() {return number;}

	QString getName() {return m_name;};
	QString getCommand() {return m_command;};

private:
	//void mousePressEvent ( QMouseEvent * event );
	void mouseReleaseEvent( QMouseEvent * event );
	QString m_name;
	QString m_command;
	unsigned int number;

signals:
	void rightPressed(QSetButton *sender);

};

#endif // QSETBUTTON_H
