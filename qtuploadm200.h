#ifndef QTUPLOADM200_H
#define QTUPLOADM200_H

#include <QWidget>
#include "ui_qtuploadm200.h"
#include "qterminal.h"
#include "QComPort.h"
#include <QTimer>
#include <QtXml/QDomDocument>
#include <QTextStream>
#include <QFile>
#include <QColor>

#define MAX_BUTTONS 6

union CDWordAsArray {
	DWORD m_Addres;
	char m_ByteArray[4];
};

class QTUpLoadM200 : public QWidget
{
    Q_OBJECT

public:
    QTUpLoadM200(QWidget *parent = 0);
    ~QTUpLoadM200();
	void addFile(QString &fileName);
public slots:
	void closeTerminal();

private:
    Ui::QTUpLoadM200Class ui;
	QTerminal *terminal;
	QComPort comPortThread;
	FILE *file;
	QString name[MAX_BUTTONS];
	QString command[MAX_BUTTONS];

	unsigned int timeReboot;
	unsigned int timeErase;
	unsigned int timeBlock;

	unsigned int blockCount ; // Номер блока
	unsigned int m_BlockSize; // Размер блока (максимальный)
	long m_FileSize; // Размер файла
	unsigned int m_Offset; // Сколько байт передано
	unsigned int m_StartAddres; // Адрес загрузки
	unsigned int m_CurrBlockSize; // Текущий отправленный блок
	unsigned int errorCount; // Счетчик ошибок

	bool fTransfer;
	unsigned char stageTransfer;
	void transfer(unsigned char *pBuffer, int len);
	FILE* fileOpen(QString &fileName);
	void fileClose();
	int OnSendBlock();
	QTimer *timer;

	QString fontName;
	bool fontItalic;
	int fontWidth;
	QString style;

	unsigned int fontSize;
	unsigned int terminalX;
	unsigned int terminalY;
	unsigned int terminalWidth;
	unsigned int terminalHeight;
	QFile *openXML(QString &fileName, QDomDocument &doc, QDomElement &root);
	void getElement(QDomDocument &doc, QDomElement &root, QString &elementName, QDomElement &element);
	void saveXML(QDomDocument &doc, QFile *file);

	QColor backgroundColor;
	QColor mainColor;
	QColor errorColor;
	QColor callColor;

private slots:
	void on_pushAbout_clicked();
	void on_pushTransfer_clicked();
	void on_pushSelectFile_clicked();
	void on_pushOption_clicked();
	void on_checkBox_stateChanged(int);
	void on_lineEditReturnPressed();
	void on_recvDataComPort(unsigned char *pBuffer, unsigned int len);
	void terminalKeyPressed(QKeyEvent *e);
	void timeOut();
	void transferStart();
	void buttonChanged(unsigned int number, QString &m_name, QString &m_command);
	void on_command(QString &m_command);
	void activatedComPort (const QString & name);
	void activatedSelectFile(int index);
	void activatedSelectType(const QString & name);

protected:
	//void hideEvent ( QHideEvent * event );
	void closeEvent ( QCloseEvent * event ) ;

};

#endif // QTUPLOADM200_H
