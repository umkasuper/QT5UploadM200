#include "qhistryline.h"
#include <QKeyEvent>

QHistryLine::QHistryLine(QWidget *parent)
	: QLineEdit(parent)
{
	current = 0;
}

QHistryLine::~QHistryLine()
{

}

void QHistryLine::keyPressEvent( QKeyEvent * e )
{	
	if (e->key() == Qt::Key_Return)
	{
		if (!text().isEmpty()) { 
			int index = listHistory.indexOf(text());
			if (index != -1)// Нашли такой элемент, ставим его первым
				listHistory.removeAt(index);
			listHistory.insert(0, text());
			index = listHistory.indexOf(text());			
			current = index - 1; 
		}
	}
	else
	if (e->key() == Qt::Key_Down)
	{
		if (current > 0) {
			current--;
			setText(listHistory.at(current));		
		} 
		selectAll();
	}
	else
	if (e->key() == Qt::Key_Up) // Нажата вверх
	{
		if (current < listHistory.count()) {
			if (!(current == (listHistory.count() - 1)))
				current++;
			setText(listHistory.at(current));
		}
		selectAll();
	} 
	

	if (e->key() == Qt::Key_Escape)
	{
		emit closed();
	} else
		QLineEdit::keyPressEvent(e);


}
