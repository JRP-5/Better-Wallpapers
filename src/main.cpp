#include "mainwindow.h"
#include <QApplication>
#include <QCoreApplication>
#include <QPushButton>
#include <QTimer>
#include "wallpaper_utils.h"
#include <QThread>
#include <iostream>
#include "nasa.h"

QMainWindow *mainWindow;
QApplication *app;
WallpaperOptions *options;
//Function to launch the settings window
void launchWindow(WallpaperOptions *options){
    mainWindow = new MainWindow(0, options);
    mainWindow->setAttribute(Qt::WA_DeleteOnClose);
    mainWindow->resize(500, 500);
    QObject::connect(mainWindow, &QObject::destroyed, [](){
        mainWindow = NULL;
    });
    // Create a button to compltely close the application
    QPushButton *closeButton = new QPushButton("Stop Wallpaper changer", mainWindow);
    closeButton->setAttribute(Qt::WA_DeleteOnClose);
    closeButton->setFixedSize(180, 30);
    closeButton->setGeometry(10, 10, closeButton->width(), closeButton->height());
    closeButton->show();
    QObject::connect(closeButton, &QPushButton::clicked, app, app->quit);
    mainWindow->show();
}

// Function called when the system tray icon is left-clicked
void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    // Check if the activation reason is a left-click
    if (reason == QSystemTrayIcon::Trigger) {

        //qDebug() << mainWindow;
        if(mainWindow == NULL){
            launchWindow(options);
            // TODO FREE WINDOW MEMORY ON CLOSE
        }
        else{
            mainWindow->show();
        }

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

int main(int argc, char *argv[]){
    // Get the executables current path
    std::string path = getExeFolder(argv[0]);

    // Read the user's options from options.json
    QString s = QString::fromStdString(path);
    options = getJsonFromPath(s);

    // Create out Qapplication
    app = new QApplication(argc, argv);
    // Make sure the application doesn't quit when we close the settings window
    app->setQuitOnLastWindowClosed(false);
    // Create the system tray icon
    QSystemTrayIcon trayIcon(QIcon(":/assets/icon.png"));

    // Create a menu for the tray icon
    QMenu trayMenu;
    QAction *quitAction = new QAction("Quit" , &trayMenu);
    QObject::connect(quitAction, &QAction::triggered, app, &QApplication::quit);
    QAction *settingsAction = new QAction("Launch settings" , &trayMenu);
    QObject::connect(settingsAction, &QAction::triggered, app, [](){
        launchWindow(options);
    });
    trayMenu.addAction(quitAction);
    trayMenu.addAction(settingsAction);
    trayIcon.setContextMenu(&trayMenu);

    // Add left click functionality to the icon
    QObject::connect(&trayIcon,  &QSystemTrayIcon::activated, app, &onTrayIconActivated);
    // Show the tray icon
    trayIcon.show();
    options->saveJson();
    //launchWindow(options);


    QThread *thread = QThread::create([]() { wallpaper_loop(options); });
    thread->start();
    int result = app->exec();
    thread->exit();
    return result;
}
