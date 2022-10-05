#ifndef VIEWSALES_H
#define VIEWSALES_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class ViewSales;
}

class ViewSales : public QDialog
{
    Q_OBJECT
    void salesSelected(const QModelIndex &index);
    void returnPurchase();
    void printReport();
public:
    explicit ViewSales(QWidget *parent = nullptr);
    ~ViewSales();
    void selectSales();
    void setFlow(int);
private:
    Ui::ViewSales *ui;
    QSqlQueryModel *m_modelSales;
    QSqlQueryModel *m_modelChecks;
    int m_flow = 0;
};

#endif // VIEWSALES_H
