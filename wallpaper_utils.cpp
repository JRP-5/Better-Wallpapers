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
string checkForNewImg(WallpaperOptions *options, string path, QString date){

    if(options->potdSource == "Bing"){
        string imgPath = getNewImg(options->bingRegion, date, path);
        if(imgPath != ""){
            setPhoto(imgPath);
        }
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

    QFile file(jsonPath);
    // If we can read the file
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonDoc.toJson());
    }
    file.close();

}
//Method to initialize the struct from a path to the json file
WallpaperOptions* getJsonFromPath(const QString &path) {
    WallpaperOptions *data = new WallpaperOptions;
    QFile file(path);
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
void wallpaper_loop(WallpaperOptions *options, string path){
    while(1){
        // Finding the latest date for which we have a picture
        QString latestDate = "-1";
        for (const auto & entry : fs::directory_iterator(path + "images/" + options->potdSource.toStdString())){
            string fileDate = entry.path().filename().string().substr(0, 8);
            if(fileDate < latestDate.toStdString() || latestDate == "-1"){
                latestDate = QString::fromStdString(fileDate);
            }
        }
        checkForNewImg(options, path, latestDate);
        QThread::sleep(5);
    }
}
