#include <string>
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QObject>
#include <QtCore>
#include "wallpaper_utils.h"
#include "nasa.h"
#include "unsplash.h"
#include "bing_wallpaper.h"
#include <vector>

std::string getFavouriteNewImg(QString path){
    QFile file(path + "favourites.json");
    if (!file.open(QIODevice::ReadOnly)) {
        // If we can't read the favourites return nothing
        qDebug() << "Failed to read favourites files";
        return "";
    }
    else{
        QJsonObject json = QJsonDocument().fromJson(file.readAll()).object();
    }
    // TODO
    file.close();
    return "";
}
// Function which saves the URL of the current picture to favourites.json
bool favouriteCurrentImg(WallpaperOptions* options){
    QString URL = "";
    // Get the URL depending on the current source
    if(options->potdSource == "Bing"){
        URL = getBingURL(options->bingRegion, options->jsonPath);
    }
    else if(options->potdSource == "Unsplash"){
        URL = getUnsplashURL(options->jsonPath);
    }
    else if(options->potdSource == "Nasa"){
        URL = getNasaURL(options->jsonPath);
    }

    if(URL == ""){
        qDebug() << options->potdSource;
        return false;
    }
    QFile file(options->jsonPath + "favourites.json");
    bool exists = file.exists();
    // If we cannot read and write to the file return false
    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << "Unable to read/write to favourites.json";
        file.close();
        return false; // Return a default WallpaperData object
    }
    else{
        QJsonDocument document = QJsonDocument().fromJson(file.readAll());
        // If the file doesn't contain valid JSON and is not new return false
        if (!document.isObject() && exists) {
            qDebug() << "favourites.json is not valid JSON";
            return false;
        }
        file.close();
        file.open(QIODevice::WriteOnly);
        QJsonObject json = document.object();
        QJsonArray arr;
        // Make sure the json contains favourites and it is an array
        // If so add the existing entries to arr
        if(json.contains("favourites") && json["favourites"].isArray()){
            arr = json["favourites"].toArray();
        }
        // Add the new URL to the array
        if(!arr.contains(URL)){
            arr.append(URL);
        }
        json["favourites"] = arr;
        file.write(QJsonDocument(json).toJson());
        file.close();
        return true;
    }
}
