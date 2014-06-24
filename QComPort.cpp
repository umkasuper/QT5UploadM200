#include "QComPort.h"

#include <QtGui>

QComPort::QComPort(QObject *parent) :
QThread(parent)

{
#ifdef WIN32
	com = INVALID_HANDLE_VALUE;
#endif
}

QComPort::~QComPort(void)
{
	mutex.lock();
	condition.wakeOne();
	mutex.unlock();

	//wait();
}

void QComPort::fillPortList(QComboBox *portList)
{
#ifdef WIN32
	HKEY hkResult;
	if (RegOpenKey(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", &hkResult) == ERROR_SUCCESS)
	{
		char lpValueName[50];
		DWORD dwValueNameSize = 50;
		DWORD dwType = 0;
		char lpValue[100];
		DWORD dwBufferDataSize = 100;
		DWORD dwIndex = 0;
		while(RegEnumValue(hkResult, dwIndex, lpValueName, &dwValueNameSize,NULL,&dwType,(BYTE*)lpValue,&dwBufferDataSize) == ERROR_SUCCESS)
		{
			dwIndex ++;
			dwValueNameSize = 50;
			dwBufferDataSize = 100;
			portList->addItem(lpValue);
		}
		RegCloseKey(hkResult);
	}
#endif
}

unsigned char pBuffer[501];
#ifdef WIN32
OVERLAPPED ov;
unsigned int dwBytesRead;
#endif

void QComPort::run()
{
	// тут сканируем наш ком порт с помощью WinApi

	//dwBytesRead = 0;

	forever {
#ifdef WIN32
		ov.OffsetHigh=0;
		ov.Offset=0;
		if (com != INVALID_HANDLE_VALUE)
		{
			// Все реально открыто
			dwBytesRead = 0;
			COMSTAT Win_ComStat;
			DWORD Win_ErrorMask=0;
			int retVal=0;
			ClearCommError(com, &Win_ErrorMask, &Win_ComStat);
			if (Win_ComStat.cbInQue && (!ReadFile(com, (void*)pBuffer, 500, &dwBytesRead, NULL) || dwBytesRead==0))
				dwBytesRead = 0;

			if (dwBytesRead != 0)
			{
				// Что то пишло
				pBuffer[dwBytesRead] = 0;
				mutex.lock();
				emit recvDataComPort(pBuffer,  dwBytesRead);
				condition.wait(&mutex);
				mutex.unlock();
			}
		}
#endif
		msleep(150);
	}
}

bool QComPort::openComPort(QString comName)
{  // std::string.c_str();
#ifdef WIN32
	char name[50];
	if (com != INVALID_HANDLE_VALUE) // Все уже ок
		return true;
	strcpy(name, (char *)comName.toStdString().c_str());

	com = CreateFile(name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING,0, NULL);
	if (com != INVALID_HANDLE_VALUE)
	{
		DCB cDCB;
		GetCommState(com, &cDCB);
		cDCB.fAbortOnError = FALSE;
		cDCB.fNull = FALSE;
		cDCB.BaudRate = CBR_38400;
		cDCB.ByteSize = 8;
		cDCB.Parity = NOPARITY;
		cDCB.StopBits = TWOSTOPBITS;
		cDCB.fRtsControl = 0;
		cDCB.fDtrControl = 0;

		//для SetCommTimeouts
		COMMTIMEOUTS cTO;
		GetCommTimeouts(com, &cTO);
		cTO.ReadIntervalTimeout = 10;
		cTO.ReadTotalTimeoutMultiplier = 0;
		cTO.ReadTotalTimeoutConstant = 0;
		cTO.WriteTotalTimeoutMultiplier = 0;
		cTO.WriteTotalTimeoutConstant = 500;

		SetCommState(com, &cDCB);
		SetCommTimeouts(com, &cTO);
		return true;
	}
#endif
	return false;
}
void QComPort::closeComPort()
{
#ifdef WIN32
	CloseHandle(com);
	com = INVALID_HANDLE_VALUE;
#endif
}

bool QComPort::sendData(char *buffer, unsigned int len)
{
#ifdef WIN32
	OVERLAPPED ov;
	ov.OffsetHigh=0;
	ov.Offset=0;
	if (com != INVALID_HANDLE_VALUE)
	{
		DWORD dwBytesWrite = 0;
		WriteFile(com, buffer, len, &dwBytesWrite, NULL);
		//GetOverlappedResult(com, &ov, &dwBytesWrite, true);
		if (len != dwBytesWrite)
			return false;
	}
#endif
	return true;
}

void QComPort::recvComplite()
{
	QMutexLocker locker(&mutex);
	condition.wakeOne();
}
