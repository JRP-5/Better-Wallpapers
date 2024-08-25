#include "mainwindow.h"
#include <iostream>
#include <QApplication>
QMainWindow *mainWindow;
// Function called when the system tray icon is left-clicked
void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    // Check if the activation reason is a left-click
    if (reason == QSystemTrayIcon::Trigger) {
        mainWindow = new MainWindow();
        mainWindow->resize(500, 500);
        mainWindow->show();
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    // Create the system tray icon
    QSystemTrayIcon trayIcon = QIcon::fromTheme(QIcon::ThemeIcon::EditUndo);;

    // Create a menu for the tray icon
    QMenu trayMenu;
    QAction *quitAction = new QAction("Quit", &trayMenu);
    QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);
    trayMenu.addAction(quitAction);
    trayIcon.setContextMenu(&trayMenu);

    // Add left click functionality to the icon
    QObject::connect(&trayIcon,  &QSystemTrayIcon::activated, &app, &onTrayIconActivated);
    // Show the tray icon
    trayIcon.show();




    app.exec();
    return 0;
}
