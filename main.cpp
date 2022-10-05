#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QCommandLineParser parserCommandLine;
    QCommandLineOption debugNameOption(QStringList() << "f" << "flow",
                                      "flow", "flow",
                                       QString::number(0));
    parserCommandLine.addOption(debugNameOption);
    parserCommandLine.process(a);
    QString clFlow = parserCommandLine.value(debugNameOption);
    if (!clFlow.isEmpty()) {
        w.setFlow(clFlow.toUShort());
    }

    return a.exec();
}
