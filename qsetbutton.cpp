#include "qsetbutton.h"

QSetButton::QSetButton(QWidget *parent)
	: QPushButton(parent)
{
	setName(QString("Empty"));
	setCommand(QString(""));
}

QSetButton::~QSetButton()
{

}
/*
void QSetButton::mousePressEvent ( QMouseEvent * event )
{
	if ( event->button() == Qt::RightButton ) 
	{
		// ������ ��������� � ������
		//emit rightPressed();
	}
}
*/
void QSetButton::mouseReleaseEvent( QMouseEvent * event )
{

	if ( event->button() == Qt::RightButton ) 
	{
		// ������ ��������� � ������
		emit rightPressed(this);
		return;
	} 

	QPushButton::mouseReleaseEvent(event);
	//((QPushButton *) this)->mouseReleaseEvent(event);
}
