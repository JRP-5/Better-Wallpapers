#include "mainwindow.h"
#include <iostream>
#include <QApplication>
#include <QCoreApplication>
#include <QPushButton>
#include "bing_wallpaper.h"

QMainWindow *mainWindow;
QApplication *app;
//Function to launch the settings window
void launchWindow(){
    mainWindow = new MainWindow();
    mainWindow->resize(500, 500);
    mainWindow->show();
    // Create a button to compltely close the application
    QPushButton *closeButton = new QPushButton("Close Wallpaper changer", mainWindow);
    closeButton->setFixedSize(180, 30);
    closeButton->setGeometry(10, 10, closeButton->width(), closeButton->height());
    closeButton->show();
    QObject::connect(closeButton, &QPushButton::clicked, app, app->quit);
}

// Function called when the system tray icon is left-clicked
void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    // Check if the activation reason is a left-click
    if (reason == QSystemTrayIcon::Trigger) {
        launchWindow();
    }
}
std::string getExeFolder(char* exePath){
    int index = -1;
    std::string path = exePath;
    for(int i = strlen(exePath) - 1; i >= 0; i--){
        if(exePath[i] == '\\' || exePath[i] == '/'){
            index = i;
            break;
        }
    }
    if(index != -1){
        return path.substr(0,index+1);
    }
    return "/";
}

int main(int argc, char *argv[])
{
    std::string path = getExeFolder(argv[0]);
    std::string imgPath = saveTodayPhoto("fr-FR", path);
    std::cout << "path" << imgPath<< std::endl;
    if(!(imgPath == "")){
        std::cout << setPhoto(imgPath) << std::endl;
    }
    app = new QApplication(argc, argv);
    app->setQuitOnLastWindowClosed(false);

    // Create the system tray icon
    QSystemTrayIcon trayIcon = QIcon::fromTheme(QIcon::ThemeIcon::EditUndo);;

    // Create a menu for the tray icon
    QMenu trayMenu;
    QAction *quitAction = new QAction("Quit", &trayMenu);
    QObject::connect(quitAction, &QAction::triggered, app, &QApplication::quit);
    trayMenu.addAction(quitAction);
    trayIcon.setContextMenu(&trayMenu);

    // Add left click functionality to the icon
    QObject::connect(&trayIcon,  &QSystemTrayIcon::activated, app, &onTrayIconActivated);
    // Show the tray icon
    trayIcon.show();
    launchWindow();


    app->exec();
    return 0;
}
