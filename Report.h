#ifndef REPORT_H
#define REPORT_H

#include <QDialog>
#include <QDate>
#include "openoffice/ooxmlengine.h"

namespace Ui {
class Report;
}

class Report : public QDialog
{
    Q_OBJECT

public:
    explicit Report(QWidget *parent = nullptr);
    ~Report();
private:
    Ui::Report *ui;
    OOXMLEngine ooxmlEngine;
    void createReport();
    bool changeTextInDocument(QString key, QString val, bool isNumber = false);
    QDate m_startReportData;
    QDomNodeList m_cells;
    QDomDocument* m_doc;
    int m_flow = 0;
};

#endif // REPORT_H
