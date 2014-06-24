#ifndef QABOUTDIALOG_H
#define QABOUTDIALOG_H

#include <QDialog>
#include "ui_qaboutdialog.h"

class QAboutDialog : public QDialog
{
    Q_OBJECT

public:
    QAboutDialog(QWidget *parent = 0);
    ~QAboutDialog();

private:
    Ui::QAboutDialogClass ui;
};

#endif // QABOUTDIALOG_H
