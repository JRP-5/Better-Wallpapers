#include "mainwindow.h"
#include <QCoreApplication>
#include <QMessageBox>
#include <QDebug>
#include <QComboBox>
#include <QBoxLayout>
#include <QLabel>
#include <iostream>
#include <QPushButton>
#include "wallpaper_utils.h"
#include "favourites.h"


MainWindow::MainWindow(QWidget *parent, WallpaperOptions *wallpaperOptions)
    : QMainWindow(parent)
{
    this->options = wallpaperOptions;
    // Create the main layout container
    QVBoxLayout *container = new QVBoxLayout();
    //Create the layout to store the wallpaper source
    QHBoxLayout *sourceLayout = new QHBoxLayout();
    container->addLayout(sourceLayout, 0);

    //Create the label for the potd drop down
    QLabel *label = new QLabel("Picture of the day source");
    sourceLayout->addWidget(label, 0, Qt::AlignCenter);
    // Picture of the day drop down
    QStringList commands = { "Bing", "Unsplash", "Nasa" };
    QComboBox* combo = new QComboBox();
    combo->addItems(commands);
    connect( combo, &QComboBox::currentTextChanged, this, &MainWindow::sourceChanged);
    //Add it to the layout
    sourceLayout->addWidget(combo, 0, Qt::AlignCenter);


    // Create a region selection for bing region
    // Create a widget so we can show and hide it
    QWidget *regionWidget = new QWidget();
    regionWidget->setObjectName("region-widget");
    QHBoxLayout *regionLayout = new QHBoxLayout();
    regionWidget->setLayout(regionLayout);
    container->addWidget(regionWidget, 0);

    //Create a label for the regions
    QLabel *regionLabel = new QLabel("Bing Region");
    regionLayout->addWidget(regionLabel, 0, Qt::AlignCenter);
    //Create a combo box for all the regions and add the functionality
    QStringList regions = { "USA", "Great Britain", "China", "Japan", "Canada", "Australia", "New Zealand", "Germany", "Spain", "France", "Italy", "Brazil" };
    QComboBox* regionChoice = new QComboBox();
    regionChoice->addItems(regions);
    connect(regionChoice, &QComboBox::currentTextChanged, this, &MainWindow::bingRegionChanged);
    regionLayout->addWidget(regionChoice, 0, Qt::AlignCenter);

    // Create a button to add the current wallpaper to favourites
    QPushButton *favouriteButton = new QPushButton("Add Current Wallpaper\nto Favourites", this);
    favouriteButton->setFixedSize(150, 50);
    container->addWidget(favouriteButton, 0, Qt::AlignCenter);
    favouriteButton->show();
    QObject::connect(favouriteButton, &QPushButton::clicked, [this](){
        bool res = favouriteCurrentImg(this->options);
    });


    // Create a button to toggle between running the app on startup and not
    QPushButton *startupButton = new QPushButton("Run on startup");
    // Set the colour of the button depending on whether the shortcut exists
    if(shortcutExists()){
        startupButton->setStyleSheet("QPushButton{background-color:rgb(60, 179, 113);}");
    }
    else{
        startupButton->setStyleSheet("QPushButton{background-color:rgb(245, 32, 32);}");
    }
    startupButton->setFixedSize(100, 40);
    container->addWidget(startupButton, 0, Qt::AlignLeft);
    //startupButton->show();
    std::wstring exePath = this->options->jsonPath.toStdWString();
    QObject::connect(startupButton, &QPushButton::clicked, [startupButton, exePath](){
        bool isStartup = toggleShortcut(exePath);
        if(isStartup){
            startupButton->setStyleSheet("QPushButton{background-color:rgb(60, 179, 113);}");
        }
        else{
            startupButton->setStyleSheet("QPushButton{background-color:rgb(245, 32, 32);}");
        }
    });

    // Create a widget to contain our layout
    QWidget *window = new QWidget(this);
    window->setLayout(container);
    // Add the widget to the window
    this->setCentralWidget(window);

    combo->setCurrentIndex(commands.indexOf(options->potdSource));
    regionChoice->setCurrentIndex(regions.indexOf(options->bingRegion));

}


void MainWindow::sourceChanged(const QString& newSource)
{
    if (this->options) { // Check if options is not null
        this->options->potdSource = newSource;
        this->options->saveJson();
        checkForNewImg(this->options, "-1");
    }
    // If user has selected bing
    bool shouldShow = 0;
    if(newSource.compare("Bing") == 0){
        shouldShow = 1;
    }
    // Show or hide the widget with the bing region selection
    QList<QWidget*> allWidgets = this->findChildren<QWidget*>();
    for (QWidget *widget : allWidgets) {
        if(widget->objectName().compare("region-widget") == 0){
            widget->setVisible(shouldShow);
            break;
        }
    }
}
void MainWindow::bingRegionChanged(const QString& newRegion){
    if (this->options) { // Check if options is not null
        this->options->bingRegion = newRegion;
        this->options->saveJson();

        checkForNewImg(this->options, "-1");

    }
}

