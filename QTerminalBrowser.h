#ifndef QTERMINALBROWSER_H
#define QTERMINALBROWSER_H

#include <QTextBrowser>
#include <QColor>

class QTerminalBrowser : public QTextBrowser
{
	Q_OBJECT
	QColor backgroundColor;
	bool scroll;
public:
	void paintEvent(QPaintEvent *event);
public:
	QTerminalBrowser(QWidget *parent, QColor &backgroundColor);
	~QTerminalBrowser(void);

	void keyPressEvent( QKeyEvent * e );
	void setScroll(bool scroll) {this->scroll = scroll;};
signals:
	void keyPress(QKeyEvent * e);
};

#endif

