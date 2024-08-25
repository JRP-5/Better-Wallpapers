#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    QString getPOTDSource();
    QString getBingRegion();


public slots:
    void sourceChanged(const QString& command_text);
    void bingRegionChanged(const QString& command_text);

private:
    QString potdSource;
    QString bingRegion;
};
#endif // MAINWINDOW_H
