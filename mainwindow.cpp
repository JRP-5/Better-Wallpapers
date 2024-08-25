#include "mainwindow.h"
#include <QCoreApplication>
#include <QMessageBox>
#include <QDebug>
#include <iostream>
#include <QComboBox>
 #include <QBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , trayIcon(new QSystemTrayIcon(this))
{
    // Tray icon menu
    auto menu = this->createMenu();
    this->trayIcon->setContextMenu(menu);

    // App icon
    auto appIcon = QIcon::fromTheme(QIcon::ThemeIcon::EditUndo);
    this->trayIcon->setIcon(appIcon);
    this->setWindowIcon(appIcon);

    // Displaying the tray icon
    this->trayIcon->show();     // Note: without explicitly calling show(), QSystemTrayIcon::activated signal will never be emitted!

    // Interaction
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);

    //Create a grid layout
    QHBoxLayout *layout = new QHBoxLayout();


    // POTD Drop down
    QStringList commands = { "Bing", "Wikimedia", "Nasa Earth Observatory", "Nasa" };
    QComboBox* combo = new QComboBox();


    combo->addItems(commands);
    connect( combo, &QComboBox::currentTextChanged, this, &MainWindow::commandChanged);

    layout->addWidget(combo, 0, Qt::AlignCenter);

    QWidget* widget = new QWidget();
    widget->setLayout(layout);
    //this->setCentralWidget(widget);
    //QWidget *w = new QWidget;
    widget->resize(500,500);
    widget->show();
}

QMenu* MainWindow::createMenu()
{
    // App can exit via Quit menu
    auto quitAction = new QAction("&Quit", this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    auto menu = new QMenu(this);
    menu->addAction(quitAction);

    return menu;
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason_)
{
    switch (reason_) {
    case QSystemTrayIcon::Trigger:
        // std::cout << "here";
        // this->trayIcon->showMessage("Hello", "You clicked me!");
        this->show();
        break;
    default:
        ;
    }
}
void MainWindow::commandChanged(const QString& command_text)
{
    std::cout << qPrintable(command_text) << std::endl;
    //Do the logic based on command_text
}
