#include "qsetbuttondlg.h"

QSetButtonDlg::QSetButtonDlg(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	//QWidget::setWindowFlags(Qt::Drawer);
	// �� �� ��� ��������, �������� ������ � �������
}

QSetButtonDlg::~QSetButtonDlg()
{

}
void QSetButtonDlg::on_pushOk_clicked()
{
	emit closed(button, true);
}

void QSetButtonDlg::on_pushCancel_clicked()
{
	emit closed(button, false);
}
