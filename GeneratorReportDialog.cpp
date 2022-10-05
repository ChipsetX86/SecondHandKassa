#include "GeneratorReportDialog.h"
#include "ui_GeneratorReportDialog.h"

#include <QFile>
#include <QTextStream>

#include "OdsDocumentParts.h"
#include "global.h"

GeneratorReportDialog::GeneratorReportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeneratorReportDialog)
{
    ui->setupUi(this);
    connect(ui->buttonStartGenerate, &QPushButton::clicked,
            this, &GeneratorReportDialog::generare);
}

GeneratorReportDialog::~GeneratorReportDialog()
{
    delete ui;
}

void GeneratorReportDialog::generare()
{
    QFile file(ui->editPathXml->text());
    if (!file.open(QFile::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << ODS_HEADER;
    int counterRows = 0;

    for (auto it = typeNamesPrices.cbegin(); it != typeNamesPrices.cend(); ++it) {

        if (ui->flow1radioButton->isChecked() && (it.key() == "Женское" || it.key() == "Юбки")) {
            continue;
        }

        if (ui->flow2radioButton->isChecked() && (it.key() == "Мужское" || it.key() == "Детское")) {
            continue;
        }

        foreach (auto row, TABLE_HEADER) {
            out << row.replace("#NAME", it.key());
        }

        counterRows += 5;

        QString tableEnd = TABLE_END;
        tableEnd = tableEnd.replace(FORMULA_INDEX_START_LABEL, QString::number(counterRows));

        for (auto itPrice = it.value().cbegin(); itPrice != it.value().cend(); ++itPrice) {
            QString row = TABLE_ROW;
            out << row.replace(NAME_LABEL, it.key()).
                   replace(PRICE_LABEL, QString::number(*itPrice)).
                   replace(FORMULA_INDEX_LABEL, QString::number(counterRows));
            counterRows++;
        }

        out << tableEnd.replace(FORMULA_INDEX_END_LABEL, QString::number(counterRows - 1));
    }

    out << OSD_FOOTER;
}
