#include "Report.h"
#include "ui_Report.h"
#include "openoffice/ooxmlengine.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QVariant>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QDesktopServices>
#include "global.h"

const static ushort maxDayInReport = 14;

Report::Report(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Report)
{
    ui->setupUi(this);
    ui->buttonOpenReport->setVisible(false);
    connect(ui->buttonCreateReport, &QPushButton::clicked,
            this, &Report::createReport);
}

Report::~Report()
{
    delete ui;
}

bool Report::changeTextInDocument(QString key, QString val, bool isNumber)
{
    for (int i = 0; i < m_cells.count(); i++) {
        if (m_cells.at(i).firstChild().nodeValue() == key) {
            m_cells.at(i).firstChild().setNodeValue(val);
            if (!val.isEmpty()) {
                m_cells.at(i).parentNode().attributes().namedItem("office:value-type").setNodeValue(isNumber ? "float" : "string");
                m_doc->createAttribute("office:value-type");
                QDomAttr domValue = m_doc->createAttribute("office:value");
                domValue.setValue(val);
                m_cells.at(i).parentNode().attributes().setNamedItem(domValue);
            }
            return true;
        }
    }
    return false;
}

void Report::createReport()
{
    QDir dir;
    dir.mkdir("C:/Report/");
    QString nameReportFile =
            QString("C:/Report/Report_%1.ods").
            arg(QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate).replace(":", "-"));
    qDebug() << qApp->applicationDirPath() + "/report.ods" << nameReportFile;
    qDebug() << QFile::copy(qApp->applicationDirPath() + "/report.ods", nameReportFile);
    m_startReportData = ui->calendarWidget->selectedDate();
    if (ooxmlEngine.open(nameReportFile)) {
        qDebug() << "Open file success";
        m_doc = ooxmlEngine.getDomDocument();
        m_cells = m_doc->elementsByTagName("text:p");
        for (auto currentType: typeNames) {
            this->setWindowTitle("Обработка " + currentType);
            QSqlQuery query;
            query.prepare("SELECT COUNT(*), assign_price, "
                          "Extract(day from date) as day FROM kassa.checks "
                          "WHERE name = :name_type AND date BETWEEN :start AND :end AND is_return = false "
                          "GROUP BY assign_price, day ORDER BY day ASC");
            query.bindValue(":name_type", currentType);
            query.bindValue(":start", m_startReportData.toString("yyyy-MM-dd"));
            query.bindValue(":end", m_startReportData.
                            addDays(maxDayInReport + 1).toString("yyyy-MM-dd"));
            if (!query.exec()) {
                qDebug() << query.lastError().text();
                QMessageBox::critical(this, "Ошибка", "Ошибка выполнения запроса");
                return;
            }

            //заполнение заголовка
            QMap<int, int> numberDay;
            for (int i = 0; i < maxDayInReport; ++i) {
                QString t1("[" + currentType + "Дата%1]");
                t1 = t1.arg(i + 1);
                int day = m_startReportData.addDays(i).day();
                numberDay[day] = i + 1;
                changeTextInDocument(t1, QString::number(day));
            }

            QString td1("[" + currentType + "МесяцГод]");
            changeTextInDocument(td1, m_startReportData.toString("MM-yyyy"), false);

            QString t1("[" + currentType + "Дата%1-%2]");
            while (query.next()) {
                int count = query.value(0).toInt();
                int assingPrice = query.value(1).toInt();
                int day = query.value(2).toInt();
                changeTextInDocument(t1.arg(QString::number(numberDay[day]),
                                            QString::number(assingPrice)),
                                     QString::number(count), true);
            }
            //clear
            for (auto val: typeNamesPrices[currentType]) {
                for (int i = 0; i < maxDayInReport; ++i) {
                    changeTextInDocument(t1.arg(QString::number(i + 1),
                                                QString::number(val)), "", false);
                }
            }
            //summ
            QSqlQuery querySumm;
            querySumm.prepare("SELECT SUM(sell_price), assign_price FROM kassa.checks "
                              "WHERE name = :name_type AND date BETWEEN :start AND :end AND is_return = false "
                              "GROUP BY assign_price");
            querySumm.bindValue(":name_type", currentType);
            querySumm.bindValue(":start", m_startReportData.toString("yyyy-MM-dd"));
            querySumm.bindValue(":end", m_startReportData.
                                addDays(maxDayInReport + 1).toString("yyyy-MM-dd"));
            if (!querySumm.exec()) {
                QMessageBox::critical(this, "Ошибка", "Ошибка выполнения запроса");
                return;
            }
            QString t2("[" + currentType + "Сумма%1]");
            while (querySumm.next()) {
                int sum = querySumm.value(0).toInt();
                int assingPrice = querySumm.value(1).toInt();
                changeTextInDocument(t2.arg(QString::number(assingPrice)),
                                     QString::number(sum), true);
            }
            //clear sum
            for (auto val: typeNamesPrices[currentType]) {
                changeTextInDocument(t2.arg(QString::number(val)), "", false);
            }
        }

        ooxmlEngine.close();
        QMessageBox::information(this, "Информация", "Отчет готов");
        QUrl url("file:/" + nameReportFile);
        QDesktopServices::openUrl(url);
    } else {
        qDebug() << "Not open file";
    }

}
