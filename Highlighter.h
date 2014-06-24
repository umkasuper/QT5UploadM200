#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QHash>
#include <QString>
#include <QStringList>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextEdit>

class Highlighter : public QObject
{

	Q_OBJECT

public:
	Highlighter(QObject *parent = 0);
	~Highlighter(void);

	void addToDocument(QTextDocument *doc);
	void setEditor(QTextEdit *edit);
	void addMapping(const QString &pattern, const QTextCharFormat &format);
private slots:
	void highlight(int from, int removed, int added);


private:
	void highlightBlock(QTextBlock block);

	QHash<QString,QTextCharFormat> mappings;
	QTextEdit *editor;

};


#endif