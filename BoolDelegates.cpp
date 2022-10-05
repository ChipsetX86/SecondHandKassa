#include "BoolDelegates.h"

BoolDelegates::BoolDelegates(QObject *parent):
    QStyledItemDelegate(parent)
{
}

QString BoolDelegates::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(locale)
    return value.toBool() ? "Да" : "Нет";    
}
