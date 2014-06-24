#include ".\qterminalbrowser.h"
#include <QPainter>

QTerminalBrowser::QTerminalBrowser(QWidget *parent, QColor &backgroundColor) : QTextBrowser(parent)
{
	this->backgroundColor = backgroundColor;
	scroll = true;
}

QTerminalBrowser::~QTerminalBrowser(void)
{
}

void QTerminalBrowser::keyPressEvent( QKeyEvent * e )
{
	emit keyPress(e);
}

void QTerminalBrowser::paintEvent(QPaintEvent *event)
{
	QRect rect = viewport()->rect(); //cursorRect();
//	rect.setX(0);
//	rect.setWidth(viewport()->width());
	QPainter painter(viewport());
//	int r,g,b,a;
//	backgroundColor.getRgb(&r,&g,&b,&a);
//	if (scroll)
//		backgroundColor.setRgb(r,g,b,255);
//	else
//		backgroundColor.setRgb(r,g,b,128);
	const QBrush brush(backgroundColor); // Тут взять из настроек
	painter.fillRect(rect, brush);
	painter.end();
	QTextBrowser::paintEvent(event);	
}

