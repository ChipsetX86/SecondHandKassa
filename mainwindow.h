#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QSqlDatabase>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    void fullPriceButton();
    void selectPrice();
    void saveBuy();
    void openDiscount();
    void closeDiscount();
    void deleteCurrentLine();
    bool initBase();
    void calcSumm();
    void updateSumm();
    void errorDB();

    void discount1plus1();
    void discount10();
    void discount25();
    void discount30();
    void discount50();
    void setDiscount(ushort);
    void openViewSales();
    void openReport();
    void openGeneratorReport();
public:
    void setFlow(int);
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QVector<QPushButton *> buttons;
    QString m_currentType;
    ushort m_summ = 0;
    QSqlDatabase m_db;
    int m_flow = 0;
};

#endif // MAINWINDOW_H
