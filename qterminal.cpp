#include "qterminal.h"
#include "qsetbuttondlg.h"
#include "QTerminalBrowser.h"
#include "qhistorylistwidget.h"
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFontDialog>
#include <QList>

QTerminal::QTerminal(QWidget *parent, QColor &backgroundColor, QColor &mainColor, QColor &errorColor, QColor &callColor)
: QWidget(parent)
{
	ui.setupUi(this);
	this->backgroundColor = backgroundColor;
	this->mainColor = mainColor;
	this->errorColor = errorColor;
	this->callColor = callColor;

	textOutTerminal = new QTerminalBrowser(this, backgroundColor);
	textOutTerminal->setObjectName(QString::fromUtf8("textOutTerminal"));
	//textOutTerminal->setBackground(Qt::SolidPattern);
	//textOutTerminal->setBackgroundRole(QPalette::Shadow);
	//textOutTerminal->setBackgroundMode ()
	//textOutTerminal->update() ;


	//textOutTerminal->get	
	QRect rect = frameGeometry ();
	textOutTerminal->setGeometry(QRect(0, 0, rect.width() - 10, rect.height() - 50));
	QRect rect2 = ui.lineEdit->frameGeometry();
	ui.lineEdit->setGeometry(QRect(0, rect.height() - 50, rect.width() - 10, rect2.height()));
	connect(textOutTerminal, SIGNAL( keyPress(QKeyEvent *)), this, SLOT(terminalKeyPressed(QKeyEvent *)));

	connect(textOutTerminal->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(verticalScrollChanged(int)));
	//setFontButton = new QPushButton(tr("SetFont"));
	QHBoxLayout *buttonSetButton = new QHBoxLayout;
	buttonSetButton->addWidget(ui.pushSetButton1);
	buttonSetButton->addWidget(ui.pushSetButton2);
	buttonSetButton->addWidget(ui.pushSetButton3);
	buttonSetButton->addWidget(ui.pushSetButton4);
	buttonSetButton->addWidget(ui.pushSetButton5);
	buttonSetButton->addWidget(ui.pushSetButton6);

	connect(ui.pushSetButton1, SIGNAL(rightPressed(QSetButton *)), this, SLOT(rightPressed(QSetButton *)));
	connect(ui.pushSetButton2, SIGNAL(rightPressed(QSetButton *)), this, SLOT(rightPressed(QSetButton *)));
	connect(ui.pushSetButton3, SIGNAL(rightPressed(QSetButton *)), this, SLOT(rightPressed(QSetButton *)));
	connect(ui.pushSetButton4, SIGNAL(rightPressed(QSetButton *)), this, SLOT(rightPressed(QSetButton *)));
	connect(ui.pushSetButton5, SIGNAL(rightPressed(QSetButton *)), this, SLOT(rightPressed(QSetButton *)));
	connect(ui.pushSetButton6, SIGNAL(rightPressed(QSetButton *)), this, SLOT(rightPressed(QSetButton *)));

	connect(ui.pushSetButton1, SIGNAL(click()), this, SLOT(on_pushSetButton1_clicked()));
	connect(ui.pushSetButton2, SIGNAL(click()), this, SLOT(on_pushSetButton2_clicked()));
	connect(ui.pushSetButton3, SIGNAL(click()), this, SLOT(on_pushSetButton3_clicked()));
	connect(ui.pushSetButton4, SIGNAL(click()), this, SLOT(on_pushSetButton4_clicked()));
	connect(ui.pushSetButton5, SIGNAL(click()), this, SLOT(on_pushSetButton5_clicked()));
	connect(ui.pushSetButton6, SIGNAL(click()), this, SLOT(on_pushSetButton6_clicked()));
	connect(ui.lineEdit, SIGNAL(closed()), this, SLOT(on_edit_send_closed()));

	((QSetButton *)ui.pushSetButton1)->setNumber(1);
	((QSetButton *)ui.pushSetButton2)->setNumber(2);
	((QSetButton *)ui.pushSetButton3)->setNumber(3);
	((QSetButton *)ui.pushSetButton4)->setNumber(4);
	((QSetButton *)ui.pushSetButton5)->setNumber(5);
	((QSetButton *)ui.pushSetButton6)->setNumber(6);

	QHBoxLayout *buttonLayout = new QHBoxLayout;

	buttonLayout->addWidget(ui.lineEdit);
	buttonLayout->addWidget(ui.pushHistory);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(textOutTerminal);

	layout->addLayout(buttonSetButton);
	layout->addLayout(buttonLayout);

	setLayout(layout);

	setupEditor();

	textOutTerminal->setReadOnly(false);
	textOutTerminal->setFocus();
	scroll = true;
	m_setup = NULL;
	listWidget = NULL;
	listWidget = new QHistoryListWidget(this);
	listWidget->hide();
	QWidget::setWindowFlags(Qt::Window);

}

QTerminal::~QTerminal()
{
	if (m_setup != NULL)
		delete m_setup;	

}

void QTerminal::closeEvent ( QCloseEvent * e )
{
	emit closed();
}

void QTerminal::displayData(QString rcvString, bool insertBlock)
{
	QTextDocument *td = textOutTerminal->document(); 
	QTextCursor tc(td->end()); 
	tc.movePosition(QTextCursor::End);

	QTextCharFormat boldFormat;

	boldFormat.setForeground(mainColor);
//	boldFormat.setBackground(Qt::SolidPattern);

	tc.insertText(rcvString, boldFormat); 
	if (insertBlock)
		tc.insertBlock();

	QScrollBar *vertScroll = textOutTerminal->verticalScrollBar();
	if (scroll)
		vertScroll->setValue( vertScroll->maximum() ); 
}

void QTerminal::terminalKeyPressed(QKeyEvent *e)
{
	QScrollBar *vertScroll = textOutTerminal->verticalScrollBar();
	int pos = vertScroll->value();
	if (e->key() == Qt::Key_Escape)
	{
		emit closed();
	}
	else
	if (e->key() == Qt::Key_PageUp)
	{
		vertScroll->setValue(pos - 80);	
	}
	else
	if (e->key() == Qt::Key_Up) // Нажата вверх
	{
		vertScroll->setValue(pos - 10);		
	} 
	else
	if (e->key() == Qt::Key_Down) // Нажата вниз
	{
		vertScroll->setValue(pos + 10);			
	} 
	else
	if (e->key() ==Qt::Key_PageDown)
	{
		vertScroll->setValue(pos + 80);	
	}
	else
		emit keyPress(e);
}


void QTerminal::verticalScrollChanged(int value)
{
	QScrollBar *vertScroll = textOutTerminal->verticalScrollBar();
	//vertScroll->setValue( vertScroll->maximum() ); 

	if (value != vertScroll->maximum())
		scroll = false;
	else scroll = true;
	textOutTerminal->setScroll(scroll);

}

void QTerminal::setupEditor()
{

	//  QTextCharFormat defaultFormat;
	//  defaultFormat.setFontFamily("Courier");
	//defaultFormat.setFontPointSize(10);

	QTextCharFormat variableFormat;// = defaultFormat;
	variableFormat.setFontWeight(QFont::Bold);
	variableFormat.setForeground(errorColor);
	highlighter.addMapping("(INFOW:|WARNING:)[^\n]*", variableFormat);

	QTextCharFormat variableFormat2;// = defaultFormat;
	variableFormat2.setFontWeight(QFont::Bold);
	variableFormat2.setForeground(callColor);
	highlighter.addMapping("(SEIZ\\b|CALL\\b|BUSY\\b|RLSI\\b|RLSO\\b|DISA_in_CALL\\b)[^\n]*", variableFormat2);
/*
	QTextCharFormat variableFormat3;// = defaultFormat;
	variableFormat3.setFontWeight(QFont::Bold);
	variableFormat3.setForeground(Qt::darkGreen);
	highlighter.addMapping("\\b(CALL|BUSY)\\b", variableFormat3);

	QTextCharFormat variableFormat4;// = defaultFormat;
	variableFormat4.setFontWeight(QFont::Bold);
	variableFormat4.setForeground(Qt::darkYellow);
	highlighter.addMapping("\\b(RLSI|RLSO)\\b", variableFormat4);
*/
	/*
	QTextCharFormat singleLineCommentFormat = defaultFormat;
	singleLineCommentFormat.setBackground(QColor("#77ff77"));
	highlighter.addMapping("#[^\n]*", singleLineCommentFormat);
	QTextCharFormat quotationFormat = defaultFormat;
	quotationFormat.setBackground(Qt::cyan);
	quotationFormat.setForeground(Qt::blue);
	highlighter.addMapping("\".*\"", quotationFormat);

	QTextCharFormat functionFormat = defaultFormat;
	functionFormat.setFontItalic(true);
	functionFormat.setForeground(Qt::blue);
	highlighter.addMapping("\\b[a-z0-9_]+\\(.*\\)", functionFormat);
	*/
	//editor = new QTextEdit;
	//editor->setFont(defaultFormat.font());
	//editor->document()->setDefaultFont(defaultFormat.font());
	highlighter.addToDocument(textOutTerminal->document());
	highlighter.setEditor(textOutTerminal);

}

void QTerminal::parseButton(QSetButton *sender)
{
	if (sender->getCommand().isEmpty())
		return;
	if (sender->getCommand() == QString("upload")) {
		emit transferStart();
		return;
	}
	emit command(sender->getCommand());
}
void QTerminal::on_pushSetButton1_clicked()
{
	parseButton((QSetButton *)ui.pushSetButton1);
}

void QTerminal::setTerminalOutFont(QFont &setFont)
{
	textOutTerminal->setFont(setFont);
	textOutTerminal->repaint();
}

void QTerminal::rightPressed(QSetButton *sender)
{
	if (m_setup != NULL)
		delete m_setup;
	m_setup = new QSetButtonDlg(this);
	connect(m_setup, SIGNAL(closed(QSetButton*,bool)), this, SLOT(buttonSetupClosed(QSetButton*,bool)));
	m_setup->setName(sender->getName());
	m_setup->setCommand(sender->getCommand());

	m_setup->setButton(sender);
	QRect m_ButtonSize = sender->geometry();
	QRect m_SetupSize = m_setup->geometry();
	QRect m_TermSize = geometry();

	unsigned int x = m_ButtonSize.x();
	if (m_ButtonSize.x() + m_SetupSize.width() > m_TermSize.width())
		x = m_TermSize.width() - m_SetupSize.width();
	m_setup->setGeometry(x, m_ButtonSize.y() - m_SetupSize.height(), m_SetupSize.width(), m_SetupSize.height());

	m_setup->show();
}

void QTerminal::buttonSetupClosed(QSetButton *sender, bool res)
{
	if (m_setup == NULL)
		return;
	QString m_name = m_setup->getName();
	QString m_command = m_setup->getCommand();
	delete m_setup;
	m_setup = NULL;
	if (!res)
		return;
	sender->setName(m_name);
	sender->setCommand(m_command);
	// Сообщение на верх
	emit buttonChanged(sender->getNumber(), m_name, m_command);
}

void QTerminal::SetButtons(QString *name, QString *command, unsigned int count)
{

	((QSetButton *)ui.pushSetButton1)->setName(name[0]);
	((QSetButton *)ui.pushSetButton2)->setName(name[1]);
	((QSetButton *)ui.pushSetButton3)->setName(name[2]);
	((QSetButton *)ui.pushSetButton4)->setName(name[3]);
	((QSetButton *)ui.pushSetButton5)->setName(name[4]);
	((QSetButton *)ui.pushSetButton6)->setName(name[5]);

	((QSetButton *)ui.pushSetButton1)->setCommand(command[0]);
	((QSetButton *)ui.pushSetButton2)->setCommand(command[1]);
	((QSetButton *)ui.pushSetButton3)->setCommand(command[2]);
	((QSetButton *)ui.pushSetButton4)->setCommand(command[3]);
	((QSetButton *)ui.pushSetButton5)->setCommand(command[4]);
	((QSetButton *)ui.pushSetButton6)->setCommand(command[5]);	
}

void QTerminal::on_pushSetButton3_clicked()
{
	parseButton((QSetButton *)ui.pushSetButton3);

}

void QTerminal::on_pushSetButton4_clicked()
{
	parseButton((QSetButton *)ui.pushSetButton4);

}

void QTerminal::on_pushSetButton2_clicked()
{
	parseButton((QSetButton *)ui.pushSetButton2);

}

void QTerminal::on_pushSetButton5_clicked()
{
	parseButton((QSetButton *)ui.pushSetButton5);

}

void QTerminal::on_pushSetButton6_clicked()
{
	parseButton((QSetButton *)ui.pushSetButton6);

}

void QTerminal::on_pushHistory_clicked()
{
	if (listWidget->isVisible()) { 
		listClose();
		return;
	}
	QRect rectlineEdit = ui.lineEdit->geometry();
	
	listWidget->setGeometry(rectlineEdit.x(), rectlineEdit.y() - 100, rectlineEdit.width(), 100);
	QList <QString>*listItem = ui.lineEdit->getListPtr();
	
	listWidget->clear(); 
	for (int i = 0; i < listItem->count(); i++)
		listWidget->addItem(listItem->at(i));
	connect(listWidget, SIGNAL(itemDoubleClicked (QListWidgetItem *)), this, SLOT(itemSelected(QListWidgetItem *)));
	connect(listWidget, SIGNAL(itemEntered (QListWidgetItem *)), this, SLOT(itemSelected(QListWidgetItem *)));
	connect(listWidget, SIGNAL(listClose()), this, SLOT(listClose()));
						
	listWidget->show();
	ui.pushHistory->setText("|");
	listWidget->setFocus();
}

void QTerminal::itemSelected(QListWidgetItem *item)
{
	ui.lineEdit->setText(item->text());
	listClose();
}

void QTerminal::listClose()
{
	listWidget->hide();
//	listWidget->close();
//	delete listWidget;
//	listWidget = NULL;
	ui.pushHistory->setText("^");
}

void QTerminal::on_edit_send_closed()
{
	emit closed();
}
