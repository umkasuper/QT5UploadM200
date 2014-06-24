#include <QApplication>
#include "qtuploadm200.h"
#include <QTextCodec>
#include <QTranslator>

int main(int argc, char *argv[])
{
	//    setlocale(LC_CTYPE, "Russian");  
	//QTextCodec::setCodecForCStrings( QTextCodec::codecForName("CP1251") ); 


	QApplication a(argc, argv);
	char path[1024]; 
	path[0] = 0;	
#ifndef _DEBUG
//	strcpy(path, argv[0]);
/*	char * pch1 = path;  
	char * pch2;         
	for(; *pch1; pch1++) 
		if(*pch1 == '\\')
			pch2 = pch1; 
	*pch2 = '\0';           
*/
#endif
	
	QString startDir(path);
	
	QTranslator translator;
	translator.load("qtuploadm200_ru", startDir);
	a.installTranslator(&translator);

	QTUpLoadM200 w;
	
	if (argc == 2)
	{		
		QString binFileName(argv[1]);
		w.addFile(binFileName);
	}
	w.show(); 
	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
	a.connect(&w, SIGNAL(closed()), &a, SLOT(quit()));

	return a.exec();
}
