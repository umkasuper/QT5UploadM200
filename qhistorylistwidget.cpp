#include "qhistorylistwidget.h"
#include <QKeyEvent>

QHistoryListWidget::QHistoryListWidget(QWidget *parent)
	: QListWidget(parent)
{

}

QHistoryListWidget::~QHistoryListWidget()
{

}

void QHistoryListWidget::keyPressEvent( QKeyEvent * e )
{	
	if (e->key() == Qt::Key_Escape)
	{
		emit listClose();
	}
	QListWidget::keyPressEvent(e);

}

void QHistoryListWidget::focusOutEvent ( QFocusEvent * event )  
{
	emit listClose();
	QListWidget::focusOutEvent (event);
}