#ifndef GENERATORREPORTDIALOG_H
#define GENERATORREPORTDIALOG_H

#include <QDialog>

namespace Ui {
class GeneratorReportDialog;
}

class GeneratorReportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GeneratorReportDialog(QWidget *parent = nullptr);
    ~GeneratorReportDialog();

private:
    Ui::GeneratorReportDialog *ui;
private slots:
    void generare();
};

#endif // GENERATORREPORTDIALOG_H
