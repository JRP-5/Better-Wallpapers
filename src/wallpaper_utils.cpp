#include <filesystem>
#include <string>
#include <ctime>
#include <cstring>
#include <QThread>
#include <QtCore>
#include <QObject>
#include <iostream>
#include "wallpaper_utils.h"
#include "bing_wallpaper.h"
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
namespace fs = std::filesystem;

void deleteOldImgs(string path, int days){
    // Get the time now
    time_t rawtime = time(0);
    struct tm * timeinfo;
    //Take 4 days off the current time
    rawtime -= 60*60*24*days;
    // Convert the time to a tm object
    timeinfo = localtime(&rawtime);
    char oldDate[9];
    // Format the date to match how we save our images
    strftime(oldDate, sizeof(oldDate), "%Y%m%d", timeinfo);
    // For every file in the path
    for (const auto & entry : fs::directory_iterator(path)){
        // If the image is older than the date delete it
        string fileDate = entry.path().filename().string().substr(0, 8);
        if(strcmp(oldDate, fileDate.c_str()) >= 0){
            remove(entry.path().string().c_str());
        }
    }
}
// Sets the wallpaper to the image given in the path
// Windows specific
int setPhoto(string imgPath){
    std::wstring wideImgPath = std::wstring(imgPath.begin(), imgPath.end());
    return SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void *)wideImgPath.c_str(), SPIF_UPDATEINIFILE);
}
// Checks for a new image from the given source and sets and saves it if a new one is found
string checkForNewImg(WallpaperOptions *options, QString date){

    if(options->potdSource == "Bing"){
        string imgPath = getBingNewImg(options->bingRegion, date, options->jsonPath);

        if(imgPath != ""){
            setPhoto(imgPath);
        }
    }
    else if(options->potdSource == "Unslpash"){

    }
    return "";
}
void WallpaperOptions::saveJson(){
    // Make our json object to be saved
    QJsonObject json;
    json["potdSource"] = potdSource;
    json["bingRegion"] = bingRegion;
    json["changed"] = changed;
    QJsonDocument jsonDoc(json);

    QFile file(jsonPath + "options.json");
    // If we can read the file
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonDoc.toJson());
    }
    file.close();

}
//Method to initialize the struct from a path to the json file
WallpaperOptions* getJsonFromPath(const QString &path) {
    WallpaperOptions *data = new WallpaperOptions;
    QFile file(path + "options.json");
    if (!file.open(QIODevice::ReadOnly)) {
        data->jsonPath = path; // Return a default WallpaperData object
    }
    else{
        QJsonObject json = QJsonDocument().fromJson(file.readAll()).object();
        // Conver the Json object to a struct
        data->potdSource = json["potdSource"].toString();
        data->bingRegion = json["bingRegion"].toString();
        data->changed = json["changed"].toBool();
        data->jsonPath = path;
    }

    file.close();
    return data;
}
void wallpaper_loop(WallpaperOptions *options){
    while(1){
        // Finding the latest date for which we have a picture
        QString latestDate = "-1";
        struct stat st;
        if(stat( (options->jsonPath + "images/" + options->potdSource).toStdString().c_str(), &st) == 0){
            for (const auto & entry : fs::directory_iterator((options->jsonPath + "images/" + options->potdSource).toStdString())){
                string fileDate = entry.path().filename().string().substr(0, 8);
                if(fileDate < latestDate.toStdString() || latestDate == "-1"){
                    latestDate = QString::fromStdString(fileDate);
                }
            }
            checkForNewImg(options, latestDate);
        }

        QThread::sleep(5);
    }
}
