#include <filesystem>
#include <string>
#include <ctime>
#include <cstring>
#include <QThread>
#include <QtCore>
#include <QObject>
#include <iostream>
#include "wallpaper_utils.h"

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
// struct WallpaperOptions {
//     QString potdSource = "Bing";
//     QString bingRegion = "USA";
//     bool changed = false;
//     QString jsonPath;

// };

void WallpaperOptions::saveJson(){
    // Make our json object to be saved
    qDebug() << jsonPath ;
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
        cout << "ehre" << endl;
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

        QThread::sleep(5);
    }
}
