#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <GeneratorReportDialog.h>

#include "ViewSales.h"
#include "Report.h"
#include "global.h"

const QString NAME_DISCOUNT = "name_discount";
const QString NAME_TYPE = "name_type";
const QString NAME_ASSIGN_PRICE = "name_assign_price";
const QString NAME_SELL_PRICE = "name_sell_price";
const QString NAME_DICOUNT_1PLUS1 = "1+1";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#ifdef QT_NO_DEBUG
    showMaximized();
#endif
    connect(ui->buttonOuterwear, &QPushButton::clicked,
            this, &MainWindow::fullPriceButton);
    connect(ui->buttonShoes, &QPushButton::clicked,
            this, &MainWindow::fullPriceButton);
    connect(ui->buttonSkirt, &QPushButton::clicked,
            this, &MainWindow::fullPriceButton);
    connect(ui->buttonTrifle, &QPushButton::clicked,
            this, &MainWindow::fullPriceButton);
    connect(ui->buttonWoman, &QPushButton::clicked,
            this, &MainWindow::fullPriceButton);
    connect(ui->buttonTrousers, &QPushButton::clicked,
            this, &MainWindow::fullPriceButton);
    connect(ui->buttonChildren, &QPushButton::clicked,
            this, &MainWindow::fullPriceButton);
    connect(ui->buttonMen, &QPushButton::clicked,
            this, &MainWindow::fullPriceButton);
    connect(ui->buttonNew, &QPushButton::clicked,
            this, &MainWindow::fullPriceButton);


    connect(ui->buttonSaveBuy, &QPushButton::clicked,
            this, &MainWindow::saveBuy);
    connect(ui->buttonDiscount, &QPushButton::clicked,
            this, &MainWindow::openDiscount);
    connect(ui->buttonCloseDiscount, &QPushButton::clicked,
            this, &MainWindow::closeDiscount);
    connect(ui->buttonDeleteCurrentLine, &QPushButton::clicked,
            this, &MainWindow::deleteCurrentLine);

    connect(ui->buttonDiscount1plus1, &QPushButton::clicked,
            this, &MainWindow::discount1plus1);
    connect(ui->buttonDiscount10, &QPushButton::clicked,
            this, &MainWindow::discount10);
    connect(ui->buttonDiscount25, &QPushButton::clicked,
            this, &MainWindow::discount25);
    connect(ui->buttonDiscount30, &QPushButton::clicked,
            this, &MainWindow::discount30);
    connect(ui->buttonDiscount50, &QPushButton::clicked,
            this, &MainWindow::discount50);
    connect(ui->actionViewSales, &QAction::triggered,
            this, &MainWindow::openViewSales);
    connect(ui->actionPrintReport, &QAction::triggered,
            this, &MainWindow::openReport);
    connect(ui->actionGeneratorReport, &QAction::triggered,
            this, &MainWindow::openGeneratorReport);

    ui->groupDiscount->setVisible(false);
    ui->frameMainControls->setEnabled(false);

    initBase();
}

void MainWindow::setFlow(int val)
{
    if (val == 1) {
        ui->buttonWoman->setVisible(false);
        ui->buttonSkirt->setVisible(false);
    } else if (val == 2) {
        ui->buttonMen->setVisible(false);
        ui->buttonChildren->setVisible(false);
    }
    m_flow = val;
}

void MainWindow::openReport()
{
    Report *report = new Report(this);
    report->setModal(true);
    report->setAttribute(Qt::WA_DeleteOnClose);
    report->setWindowFlags((report->windowFlags() & ~Qt::WindowContextHelpButtonHint)
                           | Qt::WindowMaximizeButtonHint);
    report->show();
}

void MainWindow::openGeneratorReport()
{
    auto *generator = new GeneratorReportDialog(this);
    generator->setModal(true);
    generator->setAttribute(Qt::WA_DeleteOnClose);
    generator->show();
}

void MainWindow::openViewSales()
{
    ViewSales *viewSales = new ViewSales(this);
    viewSales->setModal(true);
    viewSales->setAttribute(Qt::WA_DeleteOnClose);
    viewSales->setWindowFlags((viewSales->windowFlags() & ~Qt::WindowContextHelpButtonHint)
                              | Qt::WindowMaximizeButtonHint);
    viewSales->show();
    viewSales->setFlow(m_flow);
    viewSales->selectSales();
}

bool MainWindow::initBase()
{
    QSqlDatabase m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setDatabaseName("sh_base");
    m_db.setHostName("127.0.0.1");
    m_db.setUserName("sh");
    m_db.setPassword("vfSdgdxzv");    //vfSdgdxzv //2300
    if (!m_db.open()) {
        qDebug() << "fail connect DB";
        ui->groupCategory->setEnabled(false);
        ui->statusBar->showMessage("Ошибка подключения к базе данных");
        return false;
    }
    qDebug() << "connect DB success";
    ui->statusBar->showMessage("Подключение к базе данных завершилось успешно");
    return true;
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::calcSumm()
{
    m_summ = 0;
    for (int i = 0; i < ui->listBuy->count(); ++i) {
        QMap<QString, QVariant> data = ui->listBuy->item(i)->data(Qt::UserRole).toMap();
        m_summ += data[NAME_SELL_PRICE].toUInt();
    }
}

void MainWindow::updateSumm()
{
    calcSumm();
    ui->labelSumm->setText(QString::number(m_summ));
}

void MainWindow::saveBuy()
{
    updateSumm();
    if (m_summ == 0)
        return;
    //sql
    QSqlQuery query;
    query.prepare("INSERT INTO kassa.sales (sum) VALUES (:sum) RETURNING id;");
    query.bindValue(":sum", m_summ);
    if (!query.exec()) {
        errorDB();
        return;
    }
    query.next();
    qint64 idSales = query.value(0).toInt();
    qDebug() << "save in sales" << idSales;
    for (int i = 0; i < ui->listBuy->count(); ++i) {
        QMap<QString, QVariant> data = ui->listBuy->item(i)->data(Qt::UserRole).toMap();
        query.prepare("INSERT INTO kassa.checks (name, assign_price, discount, sell_price, id_sales)"
                      "VALUES (:name, :assing_price, :discount, :sell_price, :id_sales) RETURNING id;");
        query.bindValue(":name", data[NAME_TYPE].toString());
        query.bindValue(":assing_price", data[NAME_ASSIGN_PRICE].toUInt());
        QString discount = data[NAME_DISCOUNT].toString();
        query.bindValue(":discount", discount.isEmpty() ? "" : discount);
        query.bindValue(":sell_price", data[NAME_SELL_PRICE].toUInt());
        query.bindValue(":id_sales", idSales);
        if (!query.exec()) {
            qDebug() << query.lastError().text();
            errorDB();
            return;
        }
        query.next();
        qint64 idCheck = query.value(0).toInt();
        qDebug() << "save in check" << idCheck;
    }

    ui->statusBar->showMessage(QString("Покупка № %1 успешно сохранена").arg(idSales));
    ui->frameMainControls->setEnabled(false);
    ui->listBuy->clear();
    ui->labelSumm->setText("0.00");
    closeDiscount();
}

void MainWindow::errorDB()
{
    QMessageBox::critical(this, "Критическая ошибка", "Невозможно сохранить чек. "
                                                      "Перезагрузите компьютер");
}

void MainWindow::closeDiscount()
{
    ui->groupDiscount->setVisible(false);
    ui->groupBoxPrice->setVisible(true);
    ui->groupCategory->setVisible(true);
    ui->checkBoxAllPosition->setChecked(false);
}

void MainWindow::openDiscount()
{
    ui->groupDiscount->setVisible(true);
    ui->groupBoxPrice->setVisible(false);
    ui->groupCategory->setVisible(false);
}

void MainWindow::deleteCurrentLine()
{
    delete ui->listBuy->currentItem();
    updateSumm();
    if (ui->listBuy->count() == 0)
        ui->frameMainControls->setEnabled(false);
}

void MainWindow::selectPrice()
{
    ui->statusBar->clearMessage();
    ui->frameMainControls->setEnabled(true);
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button) {
        //button->setStyleSheet("QPushButton { background-color: red; }");
        QListWidgetItem *item = new QListWidgetItem(QString("%1 : %2").arg(m_currentType, button->text()));
        QMap<QString, QVariant> data;
        data[NAME_TYPE] = m_currentType;
        ushort currentPrice = button->text().toUShort();
        data[NAME_ASSIGN_PRICE] = currentPrice;
        data[NAME_SELL_PRICE] = currentPrice;
        item->setData(Qt::UserRole, data);
        ui->listBuy->addItem(item);
        updateSumm();
    }
}

void MainWindow::fullPriceButton()
{
    m_currentType = qobject_cast<QPushButton *>(sender())->text();
    for (auto button: buttons) {
        delete button;
    }
    buttons.clear();

    QVector<ushort> currentPrice;
    if (sender()->objectName() == "buttonOuterwear") {
        currentPrice = priceOuterwear;
    }
    if (sender()->objectName() == "buttonSkirt") {
        currentPrice = priceSkirt;
    }
    if (sender()->objectName() == "buttonShoes") {
        currentPrice = priceShoes;
    }
    if (sender()->objectName() == "buttonTrifle") {
        currentPrice = priceTrifle;
    }
    if (sender()->objectName() == "buttonWoman") {
        currentPrice = priceWoman;
    }
    if (sender()->objectName() == "buttonTrousers") {
        currentPrice = priceTrousers;
    }
    if (sender()->objectName() == "buttonChildren") {
        currentPrice = priceChildren;
    }
    if (sender()->objectName() == "buttonMen") {
        currentPrice = priceMen;
    }
    if (sender()->objectName() == "buttonNew") {
        currentPrice = priceNew;
    }

    int count = 0;
    int lines = 0;
    std::sort(currentPrice.begin(), currentPrice.end(), std::less<ushort>());
    for (auto val: currentPrice) {
        QPushButton *button = new QPushButton(ui->groupBoxPrice);
        button->setMinimumHeight(30);
        button->setFont(QFont("Times", 12));
        connect(button, &QPushButton::clicked, this, &MainWindow::selectPrice);
        buttons.append(button);
        ui->layoutButtonPrice->addWidget(button, lines, count % 4);
        count++;
        button->setText(QString::number(val));
        if (count % 4 == 0) lines++;
    }

}

void MainWindow::setDiscount(ushort val)
{
    QListWidgetItem *item = ui->listBuy->currentItem();
    if (!item && !ui->checkBoxAllPosition->isChecked()) {
        QMessageBox::warning(this, "Предупреждение", "Не выбрана строка");
        return;
    }

    for (int i = 0; i < ui->listBuy->count(); ++i) {
        QMap<QString, QVariant> data = ui->listBuy->item(i)->data(Qt::UserRole).toMap();
        if (data[NAME_DISCOUNT].toString() == NAME_DICOUNT_1PLUS1) {
            QMessageBox::warning(this, "Предупреждение", "В чеке уже есть скидка 1+1. "
                                                         "Нельзя применить ещё одну");
            return;
        }
    }

    int start;
    int end;
    if (ui->checkBoxAllPosition->isChecked()) {
        start = 0;
        end = ui->listBuy->count();
    } else {
        start = ui->listBuy->currentRow();
        end = ui->listBuy->currentRow() + 1;
    }

    for (int i = start; i < end; ++i) {
        QListWidgetItem *item = ui->listBuy->item(i);
        QMap<QString, QVariant> data = item->data(Qt::UserRole).toMap();
        uint assingPrice = data[NAME_ASSIGN_PRICE].toUInt();
        data[NAME_SELL_PRICE] = assingPrice - round(assingPrice*1.0*(val/100.0));
        data[NAME_DISCOUNT] = QString::number(val);
        item->setText(QString("%1 : %2 Скидка %3 Итог: %4").arg(data[NAME_TYPE].toString(),
                                                                data[NAME_ASSIGN_PRICE].toString(),
                                                                data[NAME_DISCOUNT].toString(),
                                                                data[NAME_SELL_PRICE].toString()));
        item->setData(Qt::UserRole, data);
    }

    updateSumm();
}

void MainWindow::discount1plus1()
{
    if (ui->listBuy->count() == 3) {
        for (int i = 0; i < ui->listBuy->count(); ++i) {
            QMap<QString, QVariant> data = ui->listBuy->item(i)->data(Qt::UserRole).toMap();
            if (!data[NAME_DISCOUNT].toString().isEmpty()) {
                QMessageBox::warning(this, "Предупреждение", "В чеке уже есть скидка, 1+1 нельзя применить");
                return;
            }
        }

        QListWidgetItem *minPrice = ui->listBuy->item(0);
        uint minAssingPrice = (ui->listBuy->item(0)->data(Qt::UserRole).toMap())[NAME_ASSIGN_PRICE].toUInt();
        for (int i = 1; i < ui->listBuy->count(); ++i) {
            QMap<QString, QVariant> data = ui->listBuy->item(i)->data(Qt::UserRole).toMap();
            uint currentAssingPrice = data[NAME_ASSIGN_PRICE].toUInt();
            if (minAssingPrice > currentAssingPrice) {
                minAssingPrice = currentAssingPrice;
                minPrice = ui->listBuy->item(i);
            }
        }

        QMap<QString, QVariant> data = minPrice->data(Qt::UserRole).toMap();
        data[NAME_SELL_PRICE] = 0;
        data[NAME_DISCOUNT] = NAME_DICOUNT_1PLUS1;
        minPrice->setData(Qt::UserRole, data);
        minPrice->setText(QString("%1 : Скидка 1+1").arg(data[NAME_TYPE].toString()));
        updateSumm();

    } else {
        QMessageBox::warning(this, "Предупреждение", "Для скидки 1+1 в чеке должно быть 3 вещи");
    }
}

void MainWindow::discount10()
{
    setDiscount(10);
}

void MainWindow::discount25()
{
    setDiscount(25);
}

void MainWindow::discount30()
{
    setDiscount(30);
}

void MainWindow::discount50()
{
    setDiscount(50);
}
