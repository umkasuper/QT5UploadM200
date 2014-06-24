#ifndef QHISTRYLINE_H
#define QHISTRYLINE_H

#include <QLineEdit>
#include <QList>
#include <QString>

class QHistryLine : public QLineEdit
{
	Q_OBJECT

public:
    QHistryLine(QWidget *parent);
    ~QHistryLine();
	QList <QString> *getListPtr() {return &listHistory;};
private:
   QList <QString> listHistory;
   int current;
   void keyPressEvent( QKeyEvent * e );
signals:
	void closed();
};

#endif // QHISTRYLINE_H
