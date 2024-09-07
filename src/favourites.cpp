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
#include <QRandomGenerator>
#include <sys/stat.h>
#include <curl/curl.h>
static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

std::string getFavouriteNewImg(QString latestDate, QString path){
    // If we already have the latest image don't get a new one
    if(getCurrentDate().compare(latestDate) <= 0){
        return "";
    }
    QFile file(path + "favourites.json");
    if (!file.open(QIODevice::ReadOnly)) {
        // If we can't read the favourites return nothing
        qDebug() << "Failed to read favourites files";
        return "";
    }
    QJsonDocument document = QJsonDocument().fromJson(file.readAll());
    file.close();
    // If the file doesn't contain valid JSON and is not new return false
    if (!document.isObject()) {
        qDebug() << "favourites.json is not valid JSON";
        return "";
    }
    QJsonObject json = document.object();
    QJsonArray arr;
    // Get the favourites array if it exists
    if(json.contains("favourites") && json["favourites"].isArray()){
        arr = json["favourites"].toArray();
    }
    if(arr.empty()){
        return "";
    }
    // Now get a random element of the favourites array
    int randInt = QRandomGenerator::global()->bounded(0, arr.size());
    QString URL = arr[randInt].toString();
    // Now we have the URL fetch the photo
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, URL.toStdString().c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_CAINFO, (path + "curl-ca-bundle.crt").toStdString().c_str());
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, (path + "libcurl-agent/1.0").toStdString().c_str());
    // Allow redirects
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION , 1);
    // Call the file currentDate.jpg so we can delete it when it hasn't been used in a while

    // Now we'll store the image

    // If favourites directory doesn't exist create it
    struct stat st;
    if(stat( (path + "images/Random-Favourite").toStdString().c_str(), &st) == -1) {
        //Check if images exists
        if(stat((path + "images").toStdString().c_str(),&st) == -1){
            mkdir((path + "images").toStdString().c_str());
        }
        mkdir((path + "images/Random-Favourite").toStdString().c_str());
    }
    QString pageFileName = path + "images/Random-Favourite/" + getCurrentDate() + ".jpg";
    /* open the file */
    FILE *pagefile = fopen(pageFileName.toStdString().c_str(), "wb");
    if (pagefile)
    {
        /* write the page body to this file handle */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
        /* get it! */
        curl_easy_perform(curl_handle);
        /* close the header file */
        fclose(pagefile);
    }
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    return pageFileName.toStdString();
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
    else if(options->potdSource == "Random-Favourite"){
        return true;
    }

    if(URL == ""){
        qDebug() << "Failed to retrieve URL of current image to add to favourites";
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
        // close and re-open file because ReadWrite doesn't overwrite
        file.close();
        file.open(QIODevice::WriteOnly);
        QJsonObject json = document.object();
        QJsonArray arr;
        // Make sure the json contains favourites and it is an array
        // If so add the existing entries to arr
        if(json.contains("favourites") && json["favourites"].isArray()){
            arr = json["favourites"].toArray();
        }
        // Add the new URL to the array if it is not already present
        if(!arr.contains(URL)){
            arr.append(URL);
        }
        // Reassign the new arr
        json["favourites"] = arr;
        // Save the file
        file.write(QJsonDocument(json).toJson());
        file.close();
        return true;
    }
}
