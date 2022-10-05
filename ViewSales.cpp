#include "ViewSales.h"
#include "ui_ViewSales.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QModelIndex>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>

#include "BoolDelegates.h"

ViewSales::ViewSales(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewSales)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    m_modelSales = new QSqlQueryModel;
    m_modelChecks = new QSqlQueryModel;
    ui->tableViewSales->setModel(m_modelSales);

    connect(ui->buttonSearch, &QPushButton::clicked,
            this, &ViewSales::selectSales);
    connect(ui->tableViewSales, &QTableView::clicked,
            this, &ViewSales::salesSelected);
    connect(ui->buttonPurchaseReturn, &QPushButton::clicked,
            this, &ViewSales::returnPurchase);
    connect(ui->buttonPrintReport, &QPushButton::clicked,
            this, &ViewSales::printReport);
    selectSales();
}

void ViewSales::printReport()
{
    qDebug() << Q_FUNC_INFO;
    QDate dateReport = ui->dateEdit->date();
    QSqlQuery query;
    query.prepare("SELECT name, assign_price, COUNT(*), SUM(sell_price) FROM kassa.checks "
                  "WHERE date BETWEEN :date_start AND :date_end "
                  "GROUP BY name, assign_price ORDER BY name, assign_price ASC;");
    query.bindValue(":date_start", dateReport.toString("yyyy-MM-dd"));
    query.bindValue(":date_end", dateReport.addDays(1).toString("yyyy-MM-dd"));
    if (query.exec()) {
        QString nameReportFile = QString("C:/Report/Report_%1.txt").
                arg(QDateTime::currentDateTime().
                    toString(Qt::DateFormat::ISODate).replace(":", "-"));
        QFile file(nameReportFile);
        file.open(QIODevice::WriteOnly);
        QTextStream stream(&file);
        if (m_flow == 1) {
            stream << QString("Первый этаж").toUtf8() << "\r\n";
        } else if (m_flow == 2) {
            stream << QString("Второй этаж").toUtf8() << "\r\n";
        }
        stream <<  dateReport.toString("dd-MM-yyyy") << "\r\n";
        int sumItog = 0;
        while (query.next()) {
            QString name = query.value(0).toString();
            name.resize(10, QChar(32));
            QString assingPrice = QString::number(query.value(1).toInt());
            assingPrice.resize(6, QChar(32));
            QString count = QString::number(query.value(2).toInt());
            count.resize(5, QChar(32));
            QString sum = QString::number(query.value(3).toInt());
            sumItog += query.value(3).toInt();
            if (file.isOpen()) {
                stream << name << " | " << assingPrice
                       << " | " << count << " | " << sum << "\r\n";
            }
            stream << "------------------------------------" << "\r\n";
        }
        stream << QString("Сумма итого: ").toUtf8() << QString::number(sumItog) << "\r\n";

        query.prepare("SELECT SUM(sell_price) FROM kassa.checks "
                      "WHERE date_return BETWEEN :date_start AND :date_end AND is_return = true ");
        query.bindValue(":date_start", dateReport.toString("yyyy-MM-dd"));
        query.bindValue(":date_end", dateReport.addDays(1).toString("yyyy-MM-dd"));
        query.exec();
        int sumReturn = 0;
        while (query.next()) {
            sumReturn = query.value(0).toInt();
        }
        stream << QString("Возвратов было: ").toUtf8() << QString::number(sumReturn) << "\r\n";


        query.prepare("SELECT * FROM kassa.checks "
                      "WHERE date_return BETWEEN :date_start AND :date_end "
                      "AND is_return = true;");
        query.bindValue(":date_start", dateReport.toString("yyyy-MM-dd"));
        query.bindValue(":date_end", dateReport.addDays(1).toString("yyyy-MM-dd"));
        query.exec();
        while (query.next()) {
            QString idCheck = QString::number(query.value(0).toInt());
            QString name = query.value(1).toString();
            QString assignPrice = query.value(2).toString();
            QString dateP = query.value(5).toDate().toString("dd-MM-yyyy");
            stream << QString("ID позиции ").toUtf8() << idCheck << " " << name
                   <<  " " << assignPrice
                   << QString("  Дата продажи: ").toUtf8() << dateP << "\r\n";
        }

        if (QDate::currentDate() == dateReport) {
            int s = sumItog - sumReturn;
            stream << QString("Итого в кассе: ").toUtf8() << QString::number(s) << "\r\n";
        }
        file.close();
        QUrl url("file:/" + nameReportFile);
        QDesktopServices::openUrl(url);
    }
}

void ViewSales::setFlow(int val)
{
    m_flow = val;
}

void ViewSales::salesSelected(const QModelIndex &index)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM kassa.checks WHERE id_sales = :id ORDER BY id ASC;");
    query.bindValue(":id", m_modelSales->data(m_modelSales->index(index.row(), 0)));
    query.exec();
    m_modelChecks->setQuery(query);
    ui->tableViewChecks->setModel(m_modelChecks);
    ui->tableViewChecks->hideColumn(5);
    ui->tableViewChecks->hideColumn(6);
    m_modelChecks->setHeaderData(0, Qt::Horizontal, QString("Номер"));
    m_modelChecks->setHeaderData(1, Qt::Horizontal, QString("Имя"));
    m_modelChecks->setHeaderData(2, Qt::Horizontal, QString("Нач. цена"));
    m_modelChecks->setHeaderData(3, Qt::Horizontal, QString("Скидка"));
    m_modelChecks->setHeaderData(4, Qt::Horizontal, QString("Пр. цена"));
    m_modelChecks->setHeaderData(7, Qt::Horizontal, QString("Возврат"));
    m_modelChecks->setHeaderData(8, Qt::Horizontal, QString("Дата возврата"));
    ui->tableViewChecks->setItemDelegateForColumn(7, new BoolDelegates(ui->tableViewChecks));

}

void ViewSales::returnPurchase()
{
    QVariant id = m_modelChecks->data(
                m_modelChecks->index(ui->tableViewChecks->currentIndex().row(), 0));
    QVariant idSales = m_modelChecks->data(
                m_modelChecks->index(ui->tableViewChecks->currentIndex().row(), 6));
    QVariant currentPrice = m_modelChecks->data(
                m_modelChecks->index(ui->tableViewChecks->currentIndex().row(), 4));

    auto mesRepl = QMessageBox::question(this, "Вопрос",
                          QString("Вы действительно хотите выполнить возврат товара %1?").arg(""),
                          QMessageBox::Yes |
                          QMessageBox::No);
    if(mesRepl == QMessageBox::No) {
        return;
    }
    QSqlQuery query;
    query.prepare("UPDATE kassa.checks SET is_return = true, date_return = now() "
                  "WHERE id = :id AND is_return = false RETURNING is_return;");
    query.bindValue(":id", id);
    if (!query.exec()) {
        qDebug() << query.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Ошибка выполнения запроса");
        return;
    }

    if (query.next()) {
        query.prepare("UPDATE kassa.sales SET sum = (sum - :dec) WHERE id = :id;");
        query.bindValue(":id", idSales);
        query.bindValue(":dec", currentPrice);
        query.exec();
        QMessageBox::information(this, "Возврат", "Возврат успешно оформлен");
        salesSelected(ui->tableViewChecks->currentIndex());
    } else {
        QMessageBox::critical(this, "Возврат", "Нет позиции для возврата");
    }

}

void ViewSales::selectSales()
{
    qDebug() << Q_FUNC_INFO;
    QDate selectDate = ui->dateEdit->date();
    QSqlQuery query;
    query.prepare("SELECT * FROM kassa.sales WHERE date BETWEEN :start AND :end ORDER BY date ASC;");
    query.bindValue(":start", selectDate.toString("yyyy-MM-dd"));
    query.bindValue(":end", selectDate.addDays(1).toString("yyyy-MM-dd"));
    query.exec();
    m_modelSales->setQuery(query);
    ui->tableViewSales->setModel(m_modelSales);
    m_modelSales->setHeaderData(0, Qt::Horizontal, QString("Номер"));
    m_modelSales->setHeaderData(1, Qt::Horizontal, QString("Дата"));
    m_modelSales->setHeaderData(2, Qt::Horizontal, QString("Сумма"));

    QSqlQuery querySum;
    querySum.prepare("SELECT SUM(sum) as sumSales "
                     "FROM kassa.sales "
                     "WHERE date BETWEEN :start AND :end");
    querySum.bindValue(":start", selectDate.toString("yyyy-MM-dd"));
    querySum.bindValue(":end", selectDate.addDays(1).toString("yyyy-MM-dd"));
    querySum.exec();
    querySum.next();
    ui->labelSumm->setText(querySum.value(0).toString());
}

ViewSales::~ViewSales()
{
    delete ui;
}
