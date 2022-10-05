#ifndef BOOLDELEGATES_H
#define BOOLDELEGATES_H

#include <QStyledItemDelegate>

class BoolDelegates: public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit BoolDelegates(QObject *parent = nullptr);
    QString displayText(const QVariant &value, const QLocale &locale) const;
};

#endif // BOOLDELEGATES_H
