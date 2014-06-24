#ifndef COMPORT_H
#define COMPORT_H

#include <QMutex>
#include <QSize>
#include <QThread>
#include <QWaitCondition>
#include <QComboBox>

#ifdef WIN32
#include <Windows.h>
#endif

class QComPort : public QThread
{
	Q_OBJECT

public:
	QComPort(QObject *parent = 0);
	~QComPort(void);

	bool openComPort(QString comName);
	void closeComPort();
	void fillPortList(QComboBox *portList);
	bool sendData(char *buffer, unsigned int len);
	void recvComplite();

signals:
	void recvDataComPort(unsigned char *pBuffer,  unsigned int len); 

protected:
	void run();

#ifdef WIN32
	HANDLE com;
#endif
	QMutex mutex;
	QWaitCondition condition;


};

#endif
