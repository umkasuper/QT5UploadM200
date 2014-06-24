#ifndef QHISTORYLISTWIDGET_H
#define QHISTORYLISTWIDGET_H

#include <QListWidget>

class QHistoryListWidget : public QListWidget
{
	Q_OBJECT

public:
    QHistoryListWidget(QWidget *parent);
    ~QHistoryListWidget();
signals:
	void listClose();

private:
	void keyPressEvent( QKeyEvent * e );
	void focusOutEvent ( QFocusEvent * event ) ;
    
};

#endif // QHISTORYLISTWIDGET_H
