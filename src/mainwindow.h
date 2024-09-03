#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include "wallpaper_utils.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, WallpaperOptions *wallpaperOptions = new WallpaperOptions());
    QString getPOTDSource();
    QString getBingRegion();


public slots:
    void sourceChanged(const QString& command_text);
    void bingRegionChanged(const QString& command_text);

private:
     WallpaperOptions *options;
};
#endif // MAINWINDOW_H
