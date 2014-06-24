#include "qtuploadm200.h"
#include "qsetupdialog.h"
#include "qaboutdialog.h"
#include <QFileDialog>
#include <QKeyEvent>
#include <QTimer>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QStyleFactory>

QTUpLoadM200::QTUpLoadM200(QWidget *parent)
: QWidget(parent)
{
	ui.setupUi(this);
	terminal = NULL;
	QGridLayout *layout = new QGridLayout; 
	layout->setColumnStretch(0, 1);
	layout->addWidget(ui.checkBox,0,0);	
	layout->addWidget(ui.pushOption,0,1);	
	layout->addWidget(ui.comboSelectFile,1,0);	
	layout->addWidget(ui.pushSelectFile,1,1);	
	layout->addWidget(ui.comboSelectType,2,0);	
	layout->addWidget(ui.pushTransfer,2,1);	

	QHBoxLayout *line4 = new QHBoxLayout;
	line4->addWidget(ui.comboComPort);  
	line4->addWidget(ui.progressBar);   //QProgressBar
	layout->addLayout(line4,3,0);   
	layout->addWidget(ui.pushAbout,3,1);	

	connect(ui.comboComPort, SIGNAL(activated ( const QString & )), this, SLOT(activatedComPort ( const QString & )));
	connect(ui.comboSelectFile, SIGNAL(activated ( int )), this, SLOT(activatedSelectFile ( int )));
	connect(ui.comboSelectType, SIGNAL(activated ( const QString &  )), this, SLOT(activatedSelectType ( const QString &  )));
	setLayout(layout);
	((QProgressBar *)ui.progressBar)->setValue(0); 
	((QProgressBar *)ui.progressBar)->setMinimum(0);
	((QProgressBar *)ui.progressBar)->setMaximum(100);

	comPortThread.fillPortList(ui.comboComPort);
	//comPortThread.start();
	connect (&comPortThread , SIGNAL(recvDataComPort(unsigned char *,  unsigned int)), this, SLOT(on_recvDataComPort(unsigned char *,  unsigned int)));
	fTransfer = false;
	stageTransfer = 0;
	file = NULL;

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timeOut()));
	timer->setSingleShot(true);
	for (int i = 0; i < MAX_BUTTONS; i++)
	{
		name[i] = "Empty";
		command[i] =  "";
	}


	// Тут все считываем из xml
	// Все будет совместимо с unix like 
	QFile file(QDir::tempPath() + "properts.xml");

	// И остальные настройки то же инициализируем
	timeBlock = 5000;
	timeReboot = 100000;
	timeErase = 100000;
	// Настройки терминала
	timeErase = 100000;
	fontName = "Times";
	fontItalic = false;
	fontWidth = -1;
	fontSize = 12;
	terminalX = 50;
	terminalY = 50;
	terminalWidth = 500;
	terminalHeight = 200;

	backgroundColor = Qt::white;
	mainColor =	Qt::black;
	errorColor = Qt::red;
	callColor = Qt::blue;

	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		return;
	}
	// Файл существует, читаем из него настройки
	QDomDocument domDocument; // Для работы с настройками
	QString errorStr;
	int errorLine;
	int errorColumn;
	
	file.seek(0);
	domDocument.setContent( QString("") );
	if (!domDocument.setContent(&file, true, &errorStr, &errorLine,	&errorColumn)) {
			QMessageBox::information(window(), tr("DOM properts"),
				tr("Parse error at line %1, column %2:\n%3")
				.arg(errorLine)
				.arg(errorColumn)
				.arg(errorStr));
			return ;
		}

	QDomElement root = domDocument.documentElement();
	// Можно проверить версию конфиги
	bool ok;
	
	root = root.firstChildElement();
	int xm200, ym200, widthm200, heightm200;
	bool bX = false, bY = false, bW = false, bH = false;
	int red, green, blue;

	while (!root.isNull()) {
		if (root.tagName() == "reboot_time") 
			if (root.hasAttribute("value")) {
				timeReboot = root.attribute("value").toInt(&ok, 10);
				if (!ok)
					timeReboot = 100000;
			}

		if (root.tagName() == "erase_time") 
			if (root.hasAttribute("value")) {
				timeErase = root.attribute("value").toInt(&ok, 10);
				if (!ok)
					timeErase = 100000;
			}

		if (root.tagName() == "block_time") 
			if (root.hasAttribute("value")) {
				timeBlock = root.attribute("value").toInt(&ok, 10);
				if (!ok)
					timeBlock = 5000;
			}

		if (root.tagName() == "terminal_setup") 
		{
			if (root.hasAttribute("size")) {
				fontSize = root.attribute("size").toInt(&ok, 10);
				if (!ok)
					fontSize = 12;
			}
			if (root.hasAttribute("name")) {
				fontName = root.attribute("name");
			}
			if (root.hasAttribute("italic")) {
				if (root.attribute("italic") == "YES")
					fontItalic = true;
				else fontItalic = false;
			}
			if (root.hasAttribute("width")) {
				fontWidth = root.attribute("width").toInt(&ok, 10);
				if (!ok)
					fontWidth = -1;
			}

			if (root.hasAttribute("terminalX")) {
				terminalX = root.attribute("terminalX").toInt(&ok, 10);
				if (!ok)
					terminalX = 0;
			}
			if (root.hasAttribute("terminalY")) {
				terminalY = root.attribute("terminalY").toInt(&ok, 10);
				if (!ok)
					terminalY = 0;
			}
			if (root.hasAttribute("terminalWidth")) {
				terminalWidth = root.attribute("terminalWidth").toInt(&ok, 10);
				if (!ok)
					terminalWidth = 500;
			}
			if (root.hasAttribute("terminalHeight")) {
				terminalHeight = root.attribute("terminalHeight").toInt(&ok, 10);
				if (!ok)
					terminalHeight = 200;
			}

		}
		if (root.tagName() == "M200Window") 
		{
			if (root.hasAttribute("x")) {
				xm200 = root.attribute("x").toInt(&ok, 10);
				if (ok)
					bX = true;
			}
			if (root.hasAttribute("y")) {
				ym200 = root.attribute("y").toInt(&ok, 10);
				if (ok)
					bY = true;
			}
			if (root.hasAttribute("width")) {
				widthm200 = root.attribute("width").toInt(&ok, 10);
				if (ok)
					bW = true;
			}
			if (root.hasAttribute("height")) {
				heightm200 = root.attribute("height").toInt(&ok, 10);
				if (ok)
					bH = true;
			}
		}

		if (root.tagName() == "history") 
		{
			for (int i = 0; i < 4; i++) {
				if (root.hasAttribute(QString("histor_%1").arg(i + 1))) 
					ui.comboSelectFile->insertItem(i,root.attribute(QString("histor_%1").arg(i + 1)));
			}
			if (root.hasAttribute(QString("current")))
			{
				int current = root.attribute("current").toInt(&ok, 10);
				if (ok)
					ui.comboSelectFile->setCurrentIndex(current);
			}

		}
		if (root.tagName() == "style") 
		{
			if (root.hasAttribute("name")) {
				style = root.attribute("name");
				QApplication::setStyle(QStyleFactory::create(style));
			}
		}
		if (root.tagName() == "buttons") 
		{
			for (int i = 0; i < MAX_BUTTONS; i++) {
				if (root.hasAttribute(QString("name_%1").arg(i + 1))) {
					name[i] = root.attribute(QString("name_%1").arg(i + 1));
				}
				if (root.hasAttribute(QString("command_%1").arg(i + 1))) {
					command[i] = root.attribute(QString("command_%1").arg(i + 1));
				}
			}
		}
		if (root.tagName() == "comport") 
		{
			if (root.hasAttribute("value")) {
				int com = ui.comboComPort->findText(root.attribute("value"));
				if (com != -1)
					ui.comboComPort->setCurrentIndex(com);				
			}
		}
		if (root.tagName() == "selected_type")
		{
			if (root.hasAttribute("type")) {
				int type = ui.comboSelectType->findText(root.attribute("type"));
				if (type != -1)
					ui.comboSelectType->setCurrentIndex(type);
			}
		}
		if (root.tagName() == "backgroundColor")
		{
			bool error = false;
			if (root.hasAttribute("red")) {
				red = root.attribute("red").toInt(&ok, 10);
				if (!ok)
					error = true;
			} else error = true;
			if (root.hasAttribute("blue")) {
				blue = root.attribute("blue").toInt(&ok, 10);
				if (!ok)
					error = true;
			} else error = true;
			if (root.hasAttribute("green")) {
				green = root.attribute("green").toInt(&ok, 10);
				if (!ok)
					error = true;
			} else error = true;
			if (!error)
				backgroundColor.setRgb(red, green, blue);
		}	

		if (root.tagName() == "mainColor")
		{
			bool error = false;
			int red, green, blue;
			if (root.hasAttribute("red")) {
				red = root.attribute("red").toInt(&ok, 10);
			} else error = true;
			if (root.hasAttribute("green")) {
				green = root.attribute("green").toInt(&ok, 10);
			} else error = true;
			if (root.hasAttribute("blue")) {
				blue = root.attribute("blue").toInt(&ok, 10);
			} else error = true;
			if (!error)
				mainColor.setRgb(red, green, blue);
		}	
		if (root.tagName() == "errorColor")
		{
			bool error = false;
			int red, green, blue;
			if (root.hasAttribute("red")) {
				red = root.attribute("red").toInt(&ok, 10);
			} else error = true;
			if (root.hasAttribute("green")) {
				green = root.attribute("green").toInt(&ok, 10);
			} else error = true;
			if (root.hasAttribute("blue")) {
				blue = root.attribute("blue").toInt(&ok, 10);
			} else error = true;
			if (!error)
				errorColor.setRgb(red, green, blue);
		}	
		if (root.tagName() == "callColor")
		{
			bool error = false;
			int red, green, blue;
			if (root.hasAttribute("red")) {
				red = root.attribute("red").toInt(&ok, 10);
			} else error = true;
			if (root.hasAttribute("green")) {
				green = root.attribute("green").toInt(&ok, 10);
			} else error = true;
			if (root.hasAttribute("blue")) {
				blue = root.attribute("blue").toInt(&ok, 10);
			} else error = true;
			if (!error)
				callColor.setRgb(red, green, blue);
		}	

		root = root.nextSiblingElement();
	}
	if (bX && bY && bW && bH)
		setGeometry(xm200, ym200, widthm200, heightm200);
}

void QTUpLoadM200::closeTerminal()
{
	ui.checkBox->setChecked(false);
}

QTUpLoadM200::~QTUpLoadM200()
{
}


void QTUpLoadM200::on_checkBox_stateChanged(int)
{
	if (ui.checkBox->checkState() == Qt::Checked) 
	{
		if (!comPortThread.openComPort(ui.comboComPort->currentText())) {
			ui.checkBox->setCheckState(Qt::Unchecked);
			return;
		}
		ui.comboComPort->setEnabled(false);
		comPortThread.start();	// Запускаем процесс
		terminal = new QTerminal(this, backgroundColor, mainColor, errorColor, callColor);

		connect(terminal, SIGNAL(closed()), this, SLOT(closeTerminal()));

		connect (terminal->getLineEdit() , SIGNAL(returnPressed()), this, SLOT(on_lineEditReturnPressed()));
		connect(terminal, SIGNAL( keyPress(QKeyEvent *)), this, SLOT(terminalKeyPressed(QKeyEvent *)));
		connect(terminal, SIGNAL(transferStart()), this, SLOT(transferStart()));
		connect(terminal, SIGNAL(buttonChanged(unsigned int, QString&, QString&)), this, SLOT(buttonChanged(unsigned int, QString&, QString&)));	
		connect(terminal, SIGNAL(command(QString&)), this, SLOT(on_command(QString&)));	

		terminal->setTerminalOutFont(QFont(fontName, fontSize, fontWidth, fontItalic));
		terminal->setGeometry(terminalX, terminalY, terminalWidth,terminalHeight);
		terminal->SetButtons(name, command, MAX_BUTTONS);
		terminal->getTerminal()->setFocus();
		terminal->show(); 

	} else
	{
		ui.comboComPort->setEnabled(true);
		setFocus();
		if (terminal != NULL)
		{
			QRect m_termSize = terminal-> geometry ();
			terminalX = m_termSize.x();
			terminalY = m_termSize.y();
			terminalWidth = m_termSize.width();
			terminalHeight = m_termSize.height();
			// Сохраняем положение на экране
			QDomDocument domDocument; // Для работы с настройками
			QDomElement root;
			QDomElement element;

			QFile *file = openXML( QString(QDir::tempPath() + "properts.xml"), domDocument, root);
			if (file != NULL) 
			{
				getElement(domDocument,root, QString("terminal_setup"),element);

				element.setAttribute("terminalX",QString("%1").arg(terminalX));
				element.setAttribute("terminalY",QString("%1").arg(terminalY));
				element.setAttribute("terminalWidth",QString("%1").arg(terminalWidth));
				element.setAttribute("terminalHeight",QString("%1").arg(terminalHeight));
				
				saveXML(domDocument, file);
			}				
			terminal->hide(); 
			delete terminal;
		}
		if (!fTransfer)
			comPortThread.closeComPort();

		terminal = NULL;
	}

}



void QTUpLoadM200::on_pushOption_clicked()
{
	// Открываем диалог с опциями
	QSetupDialog m_setup;

	m_setup.setTimeBlock(QString("%1").arg(timeBlock,0,10));
	m_setup.setTimeErase(QString("%1").arg(timeErase,0,10));
	m_setup.setTimeReboot(QString("%1").arg(timeReboot,0,10));
	m_setup.setFontName(fontName);
	m_setup.setFontSize(fontSize);
	m_setup.setFontItalic(fontItalic);
	m_setup.setFontWidth(fontWidth);
	m_setup.setStyle(style);
	m_setup.setBackgroundColor(backgroundColor);
	m_setup.setMainColor(mainColor);
	m_setup.setErrorColor(errorColor);
	m_setup.setCallColor(callColor);

	if (m_setup.exec() != QDialog::Accepted)
		return;
	// Сохраняем настройки где только можно!!!
	// Записываем все в xml, так будет лучше все перетираем
	
	QDomDocument domDocument; // Для работы с настройками
	QDomElement root;
	QDomElement element;
	QFile *file = openXML(QString(QDir::tempPath() + "properts.xml"), domDocument, root);
	if (file != NULL) 
	{
		QDomElement reboot_time;
		getElement(domDocument,root, QString("reboot_time"),reboot_time);
		reboot_time.setAttribute( "value", m_setup.getTimeReboot());    
		
		QDomElement erase_time;
		getElement(domDocument,root, QString("erase_time"),erase_time);
		erase_time.setAttribute( "value", m_setup.getTimeErase());    

		QDomElement block_time;
		getElement(domDocument,root, QString("block_time"),block_time);
		block_time.setAttribute("value", m_setup.getTimeBlock());

		QDomElement font;
		getElement(domDocument,root, QString("terminal_setup"),font);
		font.setAttribute("name", m_setup.getFontName());
		font.setAttribute("size",QString("%1").arg(m_setup.getFontSize(),0,10));
		font.setAttribute("italic", m_setup.getFontItalic() ? "YES" : "NO");
		font.setAttribute("width", QString("%1").arg(m_setup.getFontWidth(),0,10));
		
		// Еще тут сохраняем стиль окон, 
		QDomElement style;
		getElement(domDocument,root, QString("style"),style);
		style.setAttribute("name", m_setup.getStyle());

		QDomElement bgColor; 
		QColor selectColor = m_setup.getBackgroundColor();
		backgroundColor = selectColor;
		int r,g,b;
		selectColor.getRgb(&r, &g, &b);
		getElement(domDocument,root, QString("backgroundColor"),bgColor);
		bgColor.setAttribute("red", QString("%1").arg(r,0,10));
		bgColor.setAttribute("green", QString("%1").arg(g,0,10));
		bgColor.setAttribute("blue", QString("%1").arg(b,0,10));

		QDomElement mnColor; 
		selectColor = m_setup.getMainColor();
		mainColor = selectColor;
		selectColor.getRgb(&r, &g, &b);
		getElement(domDocument,root, QString("mainColor"),mnColor);
		mnColor.setAttribute("red", QString("%1").arg(r,0,10));
		mnColor.setAttribute("green", QString("%1").arg(g,0,10));
		mnColor.setAttribute("blue", QString("%1").arg(b,0,10));

		QDomElement erColor; 
		selectColor = m_setup.getErrorColor();
		errorColor = selectColor;
		selectColor.getRgb(&r, &g, &b);
		getElement(domDocument,root, QString("errorColor"),erColor);
		erColor.setAttribute("red", QString("%1").arg(r,0,10));
		erColor.setAttribute("green", QString("%1").arg(g,0,10));
		erColor.setAttribute("blue", QString("%1").arg(b,0,10));

		QDomElement clColor; 
		selectColor = m_setup.getCallColor();
		callColor = selectColor;
		selectColor.getRgb(&r, &g, &b);
		getElement(domDocument,root, QString("callColor"),clColor);
		clColor.setAttribute("red", QString("%1").arg(r,0,10));
		clColor.setAttribute("green", QString("%1").arg(g,0,10));
		clColor.setAttribute("blue", QString("%1").arg(b,0,10));

		saveXML(domDocument, file);
	}
	
	// Запоминаем настройки в наших переменных
	// Если проблемы, то по дефолту
	bool ok;
	timeBlock = m_setup.getTimeBlock().toInt(&ok, 10); 
	if (!ok)
		timeBlock = 5000;
	
	timeReboot =  m_setup.getTimeReboot().toInt(&ok, 10) ;
	if (!ok)
		timeReboot = 60000;
	
	timeErase = m_setup.getTimeErase().toInt(&ok, 10);
	if (!ok)
		timeReboot = 100000;

	fontName = m_setup.getFontName();
	fontSize = m_setup.getFontSize();
	fontItalic = m_setup.getFontItalic();
	fontWidth = m_setup.getFontWidth();
	if (terminal != NULL)
		terminal->setTerminalOutFont(QFont(fontName, fontSize,fontWidth, fontItalic));

}

void QTUpLoadM200::on_pushSelectFile_clicked()
{
	
	QString fileName = ui.comboSelectFile->currentText();
	QString curranePath = QDir::currentPath();
	if (!fileName.isEmpty())
		curranePath = fileName;
	
	QString directory = QFileDialog::getOpenFileName(
		this,
		"Choose a file",
		curranePath,
		"Binary (*.bn1);;All files (*.*)");
	addFile(directory);
}

void QTUpLoadM200::addFile(QString &fileName)
{
	if (fileName.isEmpty())
		return;

	int index = ui.comboSelectFile->findText(fileName);
	if (index != -1) {
		ui.comboSelectFile->setCurrentIndex(index);
		activatedSelectFile(index);
		return;
	}
	ui.comboSelectFile->insertItem(0, fileName);
	ui.comboSelectFile->setCurrentIndex(0);
	// Записать в историю на диск
	QDomDocument domDocument; // Для работы с настройками
	QDomElement root;
	QDomElement element;
	QFile *file = openXML(QString(QDir::tempPath() + "properts.xml"), domDocument, root);
	if (file != NULL) 
	{
		getElement(domDocument,root, QString("history"),element);
		for (int i = 0; i < 4; i++)
			element.setAttribute(QString("histor_%1").arg(i + 1,0,10), ui.comboSelectFile->itemText(i));
		saveXML(domDocument, file);
	}

}
void QTUpLoadM200::timeOut()
{
	if (!fTransfer)
		return;

	switch (stageTransfer)
	{
	case 0: // Не дождались перезагрузки станции
	case 1: 
		{
			//QMessageBox::Ok
			on_pushTransfer_clicked(); // Закрываем все
			QMessageBox::warning(this, tr("error"),
				tr("Could not reboot station,\n"
				"may be problems whis RS232\n"),
				QMessageBox::Ok | QMessageBox::Default,
				0);			
		};break;
	case 2: // Не дождались стирания файла
		{
			on_pushTransfer_clicked(); // Закрываем все
			QMessageBox::warning(this, tr("error"),
				tr("Could not erase flash,\n"),
				QMessageBox::Ok | QMessageBox::Default,
				0);			
		};break;
	case 3: // Не смогли записать блок
		{
			transfer((unsigned char *)"W", 1);
		};break;
	}
}

void QTUpLoadM200::on_pushTransfer_clicked()
{
	QString fileName = ui.comboSelectFile->currentText();
	QString selectedType = ui.comboSelectType->currentText();

	if (fileName.isEmpty())
		return;
	if (selectedType.isEmpty())
		return;

	if (!fTransfer) {
		// Открываем файл
		if (fileOpen(fileName) == NULL)
			return;
		ui.pushTransfer->setText(QApplication::translate("QTUpLoadM200", "Cancel"));
		fTransfer = true;
	} else {
		ui.pushTransfer->setText(QApplication::translate("QTUpLoadM200", "Transfer"));
		((QProgressBar *)ui.progressBar)->setValue(0);
		fTransfer = false;
		fileClose();
		timer->stop();
		if (terminal == NULL)
			comPortThread.closeComPort();
		return;
	}


	// Запускаем процедуру загрузки в станцию
	if (comPortThread.openComPort(ui.comboComPort->currentText()));
	comPortThread.start();	// Запускаем процесс

	comPortThread.sendData("q\r\n", 3);
	for (int k = 0; k < 100000; k++)
		for (int l = 0; l < 100000; l++);
	// Паузу
	comPortThread.sendData("reset\r\n", 7);
	stageTransfer = 0;

	// Ждем 5 звездочек в течении заданного таймера
	//QTimer::singleShot(60000, this, SLOT(timeOut()));
	timer->start(timeReboot);

}
#pragma optimize( "", off )
void QTUpLoadM200::transfer(unsigned char *pBuffer, int len)
{
	static char prevStr[200];
	QString type = ui.comboSelectType->currentText();
	int typeLoad = 0;
	if (type == "SMP")
		typeLoad = 1;
	if (type == "ADSP")
		typeLoad = 2;
	if (type == "MUSIC(OFFICE)")
		typeLoad = 3;

	static unsigned int charCounter = 0;
	switch (stageTransfer)
	{
	case 0:
		{
			strcpy(prevStr,""); // Зануляем старую строку
			for (int i = 0; i < len; i++)
			{

				if (typeLoad == 1 || typeLoad == 3) // Загрузка SMP
				{
					if (pBuffer[i] == '*')
						charCounter++;
					else charCounter = 0;
				}
				if (typeLoad == 2) // Загрузка ADSP
				{
					if (pBuffer[i] == '.')
						charCounter++;
					else charCounter = 0;
				}

				if (charCounter >=5) 
				{
					stageTransfer = 1;
					if (typeLoad == 1 || typeLoad == 3)
						comPortThread.sendData("#\r\n", 3);
					if (typeLoad == 2)
						comPortThread.sendData("$", 1);
					break;
				}
				// Даем #a или #b
			}
		}; break;
	case 1: // Ждем появления '.'
		{
			// Ждем фразу стирание закончено или не закончено			
			for (int i = 0; i < len; i++)
			{
				if (pBuffer[i] == '.') 
				{
					timer->stop();
					timer->start(timeErase);
					stageTransfer = 2;
					if (typeLoad == 1)					
						comPortThread.sendData("#a\r\n", 4);
					if (typeLoad == 2)					
						comPortThread.sendData("#b\r\n", 4);
					if (typeLoad == 3)					
						comPortThread.sendData("#b\r\n", 4);
					break;
				}
			}	
		}; break;
	case 2:
		{
			// Ждем фразу стирание закончено или не закончено			
			if (typeLoad == 1 || typeLoad == 3)
			{
				if ((strlen(prevStr) + len) < 200) // еще можно добавлять
					strncat(prevStr, (const char *)pBuffer, len);
				else 
					strcpy(prevStr, "");
				if (strstr(prevStr, "Стирание успешно закончено") != NULL) 
				{
					stageTransfer = 3;
					transfer((unsigned char *)"S", 1); // Искусственно кидаем подтверждение начинаем передачу
				}
				if (strstr(prevStr, "Стирание не закончено") != NULL) 
				{
					fTransfer = false; // Все конец или пришел таймер
					// Выдача сообщения
				}
			}
			if (typeLoad == 2)
			{
				for (int i = 0; i < len; i++)
				{
					if (pBuffer[i] == '.') 
					{
						stageTransfer = 3;
						transfer((unsigned char *)"S", 1); // Искусственно кидаем подтверждение начинаем передачу
					}
				}
			}
		}; break;
	case 3: // Собственно сама передача файла
		{
			if (pBuffer[0] == 'S') // начало передачи
			{
				timer->stop();
				timer->start(timeBlock); 
				if (typeLoad == 2)
				{
					comPortThread.sendData("\xff", 1);
					return;
				}
				OnSendBlock();
			}
			if ((pBuffer[0] == 'V') || (pBuffer[0] == 'W')) // очевидная ошибка записи
			{
				// Пересылаем последний блок	
				OnSendBlock();
				timer->stop();
				timer->start(timeBlock);
				errorCount++;
				if (errorCount > 5) 
				{
					on_pushTransfer_clicked();
					QMessageBox::warning(this, tr("error"),
						tr("Could not transfer block data,\n"),
						QMessageBox::Ok | QMessageBox::Default,
						0);
					return;

				}
			}

			if (pBuffer[0] == 'U') // Типа все ок идет ответ пришел, блок записан
			{
				errorCount = 0;
				m_Offset += m_CurrBlockSize;
				blockCount++;
				if (OnSendBlock() == 2) // Подтвердили последний кусок
				{	// Можно все закрыть и выйти
					comPortThread.sendData("q\r", 2);
					for (int k = 0; k < 1000; k++)
						for (int l = 0; l < 1000; l++);
					comPortThread.sendData("q\r", 2);
					for (int k = 0; k < 1000; k++)
						for (int l = 0; l < 1000; l++);
					//comPortThread.sendData("q\r", 2);
					on_pushTransfer_clicked();
					return;
				}
				//Все конец	
				// Тут же позиционируем скроллер
				timer->stop();
				timer->start(timeBlock);

				((QProgressBar *)ui.progressBar)->setValue((int)(((float)m_Offset/m_FileSize)*100));

			}
		};break;
	}
}
#pragma optimize( "", on )

void QTUpLoadM200::on_recvDataComPort(unsigned char *pBuffer, unsigned int len)
{
	//static QString prevString;
	QString str((char *)pBuffer);

	if (fTransfer)
	{
		transfer(pBuffer, len);	
	}
	comPortThread.recvComplite(); // Освобождаем порт

	if (terminal != NULL)
	{
		str.replace(QString("\r\n"),QString("\r"));
		str.replace(QString("\n"),QString("\r"));
		QStringList list1 = str.split("\r");
        int i = 0;
        for (i = 0; i < list1.size() - 1; i++)
			terminal->displayData(QString(list1.at(i)), true);

		terminal->displayData(list1.at(i), false);
	}
}

void QTUpLoadM200::on_lineEditReturnPressed()
{
	// Если идет заливка то ни чего не передаем
	if (fTransfer)
		return;

	if (terminal == NULL)
		return;
	QLineEdit *lineEdit = terminal->getLineEdit();
	QString sendString  = lineEdit->text();
	sendString = sendString + '\r';//+'\n';
	char name[50];
	strcpy(name, (char *)sendString.toStdString().c_str());
	terminal->getLineEdit()->setText(QString(""));

	comPortThread.sendData(name, sendString.size());
}

void QTUpLoadM200::terminalKeyPressed(QKeyEvent *e)
{
	// Если идет заливка то ни чего не передаем
	if (fTransfer)
		return;
	int buff;
	char name[50];
	strcpy(name, (char *)e->text().toStdString().c_str());
	if (strlen(name) == 0)
		return;
	comPortThread.sendData((char *)name, 1);
}

void QTUpLoadM200::transferStart()
{
	QString fileName = ui.comboSelectFile->currentText();
	if (fileName.isEmpty())
		return;
	if (fileOpen(fileName) == NULL)
		return;
	fTransfer = true;
	stageTransfer = 3;
	ui.pushTransfer->setText(QApplication::translate("QTUpLoadM200", "Cancel"));
	transfer((unsigned char *)"S", 1);
}

FILE * QTUpLoadM200::fileOpen(QString &fileName)
{

	if (file != NULL)
		fclose(file);
	file = fopen(fileName.toStdString().c_str(),"rb");
	if (file == NULL)
		return NULL;
	if (fseek(file,0,SEEK_END) != 0)
		return NULL;
	m_FileSize = ftell(file);
	if (m_FileSize == -1L)
		return NULL;
	fseek(file,0,SEEK_SET);
	
	blockCount = 0; // Номер блока
	m_BlockSize = 1024; // Размер блока

	m_Offset = 0;
	QString type = ui.comboSelectType->currentText();
	if (type == "SMP")
		m_StartAddres = 0x100000;
	if (type == "ADSP")
		m_StartAddres = 0xa0000;
	if (type == "MUSIC(OFFICE)")
		m_StartAddres = 0x1c0000;
	errorCount = 0;
	return file;
}

void QTUpLoadM200::fileClose()
{
	if (file != NULL)
		fclose(file);
	errorCount = 0;
}

int QTUpLoadM200::OnSendBlock()
{
	// i - номер блока
	// m_BlockSize - оазмер блока
	// m_Offset текущее положение в фале
	//unsigned int m_CurrBlockSize = 0;
start_send:

	if (m_FileSize - blockCount * m_BlockSize < m_BlockSize)
		m_CurrBlockSize = m_FileSize - m_Offset;
	else 
		m_CurrBlockSize = m_BlockSize;

	CDWordAsArray m_Addres;
	char *m_Buffer = new char [m_CurrBlockSize + 6 + 1]; // +6 байт для опистателя блока +1 в конце для CRC
	m_Buffer[0] = (char)0xF1;

	m_Addres.m_Addres = m_StartAddres + m_Offset;

	m_Buffer[1] = m_Addres.m_ByteArray[2]; 
	m_Buffer[2] = m_Addres.m_ByteArray[1]; 
	m_Buffer[3] = m_Addres.m_ByteArray[0]; 

	m_Buffer[4] = (char)(m_CurrBlockSize & 0xFF);
	m_Buffer[5] = (char)(m_CurrBlockSize >> 8);
	
	// Читаем из файла кусок
	int res = 0; // 0 - блок послан, 1 - блок не послан, 2 - конец
	fseek(file,blockCount * m_BlockSize,SEEK_SET );
	int readed = fread(m_Buffer + 6,  sizeof( char ), m_CurrBlockSize, file);
	if (readed != m_CurrBlockSize) {
		return 2;		
	}
	// Создаем контрольну сумму
	char CheckSum = 0;
	int j = 0;
    for (j = 0; j < m_CurrBlockSize + 6; j++)
		CheckSum += m_Buffer[j];

	CheckSum = ~CheckSum + 1;               
	m_Buffer[m_CurrBlockSize + 6] = CheckSum; 

	BOOL m_TransOk = FALSE;
	for (j = 0; j < m_CurrBlockSize; j++) {
		if ((unsigned char)m_Buffer[j + 6] != (unsigned char)0xFF) {
			m_TransOk = TRUE;
			break;
		}
	}
	if (!m_TransOk)
	{
		m_Offset += m_CurrBlockSize;
		blockCount++;
		// Тут же позиционируем скроллер
		goto start_send;
	}

	if (m_TransOk) {
		// Отправляем все  в порт
		comPortThread.sendData((char *)m_Buffer, m_CurrBlockSize + 6 + 1);
	} 

	delete m_Buffer; // Буффер нафиг

	return res;
}


//void QTUpLoadM200::hideEvent ( QHideEvent * event )
//{
//	ui.checkBox->setChecked(false);
//	QWidget::hideEvent (event);
//}

void QTUpLoadM200::closeEvent ( QCloseEvent * event ) 
{
	QRect m_termSize = geometry();
	QDomDocument domDocument; // Для работы с настройками
	QDomElement root;
	QDomElement element;
	QFile *file = openXML(QString(QDir::tempPath() + "properts.xml"), domDocument, root);
	if (file != NULL) 
	{
		getElement(domDocument,root, QString("M200Window"),element);
		element.setAttribute(QString("x"),QString("%1").arg(m_termSize.x()));
		element.setAttribute(QString("y"),QString("%1").arg(m_termSize.y()));
		element.setAttribute(QString("width"),QString("%1").arg(m_termSize.width()));
		element.setAttribute(QString("height"),QString("%1").arg(m_termSize.height()));
		saveXML(domDocument, file);
	}


}


void QTUpLoadM200::on_command(QString &m_command)
{
	// Если идет заливка то ничего не делаем
	if (fTransfer)
		return;
	QString sendString  = m_command;
	sendString = sendString + '\r';//+'\n';
	char name[50];
	strcpy(name, (char *)sendString.toStdString().c_str());

	comPortThread.sendData(name, sendString.size());
}


QFile *QTUpLoadM200::openXML(QString &fileName, QDomDocument &doc, QDomElement &root)
{
	QFile *file = new QFile(fileName);
	QString errorStr;
	int errorLine;
	int errorColumn;
	if (file->open(QFile::ReadWrite | QFile::Text)) {
		file->seek(0);
		doc.setContent( QString("") );
		if (!doc.setContent(file, true, &errorStr, &errorLine,	&errorColumn)) 
		{
			QMessageBox::information(window(), tr("DOM properts"),
				tr("Parse error at line %1, column %2:\n%3")
				.arg(errorLine)
				.arg(errorColumn)
				.arg(errorStr));

			root = doc.createElement("properts");
			doc.appendChild(root);
			root.setAttribute( "version", "1.0");  
		} else {
			root = doc.documentElement();
		}								
		return file;
	}
	delete file;
	return NULL;
}

void QTUpLoadM200::getElement(QDomDocument &doc, QDomElement &root, QString &elementName, QDomElement &element)
{
	element = root.firstChildElement(elementName);
	if (element.isNull()) {
		element = doc.createElement(elementName);
		root.appendChild(element);
	}

}
void QTUpLoadM200::saveXML(QDomDocument &doc, QFile *file)
{
	// Собственно запись
	const int IndentSize = 4;
	QString fileName = file->fileName();
	file->close();
	delete file;

	QFile::remove(fileName);

	QFile m_File(fileName);
	if (m_File.open(QFile::ReadWrite | QFile::Text)) {
		m_File.seek(0);
		QTextStream out(&m_File);
		doc.save(out, IndentSize);
		out.flush();
		m_File.close();
	}

}
void QTUpLoadM200::activatedSelectType(const QString & name)
{
	QDomDocument domDocument; // Для работы с настройками
	QDomElement root;
	QDomElement element;
	QFile *file = openXML(QString(QDir::tempPath() + "properts.xml"), domDocument, root);
	if (file != NULL) 
	{
		getElement(domDocument,root, QString("selected_type"),element);
		element.setAttribute(QString("type"),name);
		saveXML(domDocument, file);
	}

}

void QTUpLoadM200::activatedSelectFile(int index)
{

	QDomDocument domDocument; // Для работы с настройками
	QDomElement root;
	QDomElement element;
	QFile *file = openXML(QString(QDir::tempPath() + "properts.xml"), domDocument, root);
	if (file != NULL) 
	{
		getElement(domDocument,root, QString("history"),element);
		element.setAttribute(QString("current"),QString("%1").arg(index));
		saveXML(domDocument, file);
	}
}

void QTUpLoadM200::activatedComPort (const QString & name)
{
	comPortThread.closeComPort();
	comPortThread.openComPort(name);
	// Нужно еще сохранить выбранный ком порт в файле
	QDomDocument domDocument; // Для работы с настройками
	QDomElement root;
	QDomElement element;
	QFile *file = openXML(QString(QDir::tempPath() + "properts.xml"), domDocument, root);
	if (file != NULL) 
	{
		getElement(domDocument,root, QString("comport"),element);
		element.setAttribute(QString("value"),name);
		saveXML(domDocument, file);
	}
}

void QTUpLoadM200::buttonChanged(unsigned int number, QString &m_name, QString &m_command)
{
	QDomDocument domDocument; // Для работы с настройками
	QDomElement root;
	QDomElement element;
	QFile *file = openXML(QString(QDir::tempPath() + "properts.xml"), domDocument, root);
	if (file != NULL) 
	{
		getElement(domDocument,root, QString("buttons"),element);
		element.setAttribute(QString("name_%1").arg(number),m_name);
		element.setAttribute(QString("command_%1").arg(number),m_command);
		saveXML(domDocument, file);
	}	
}


void QTUpLoadM200::on_pushAbout_clicked()
{
	QAboutDialog m_about;

	m_about.exec();
}