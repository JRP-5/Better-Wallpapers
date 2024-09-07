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
#include <shlobj.h>
#include "unsplash.h"
#include "nasa.h"
#include "favourites.h"

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
// Gets the current date in the format YYYYMMDD
QString getCurrentDate(){
    time_t rawtime = time(0);
    struct tm * timeinfo;
    timeinfo = localtime(&rawtime);
    char date[9];
    strftime(date, sizeof(date), "%Y%m%d", timeinfo);
    return QString(date);
}
// Sets the wallpaper to the image given in the path
// Windows specific
int setPhoto(string imgPath){
    std::wstring wideImgPath = std::wstring(imgPath.begin(), imgPath.end());
    return SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void *)wideImgPath.c_str(), SPIF_UPDATEINIFILE);
}
// Checks for a new image from the given source and sets and saves it if a new one is found
void checkForNewImg(WallpaperOptions *options, QString date){
    string imgPath = "";
    if(options->potdSource == "Bing"){
        imgPath = getBingNewImg(options->bingRegion, date, options->jsonPath);
    }
    else if(options->potdSource == "Unsplash"){
        imgPath = getUnsplashNewImg(date, options->jsonPath);
    }
    else if(options->potdSource == "Nasa"){
        imgPath = getNasaNewImg(date, options->jsonPath);
    }
    else if(options->potdSource == "Random-Favourite"){
        imgPath = getFavouriteNewImg(date, options->jsonPath);
    }

    if(imgPath != ""){
        setPhoto(imgPath);
    }
}
void WallpaperOptions::saveJson(){
    // Make our json object to be saved
    QJsonObject json;
    json["potdSource"] = potdSource;
    json["bingRegion"] = bingRegion;
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
        qDebug() << "Unable to read options.json";
    }
    else{

        QJsonDocument document = QJsonDocument().fromJson(file.readAll());
        // If the file doesn't contain valid JSON return false
        if (!document.isObject()) {
            qDebug() << "options.json is not valid JSON";
            data->jsonPath = path;
        }
        else{
            QJsonObject json = document.object();
            // Convert the Json object to a struct
            data->potdSource = json["potdSource"].toString();
            data->bingRegion = json["bingRegion"].toString();
            data->jsonPath = path;
        }
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
                if(fileDate > latestDate.toStdString() || latestDate == "-1"){
                    latestDate = QString::fromStdString(fileDate);
                }
            }

        }
        checkForNewImg(options, latestDate);
        QThread::sleep(5);
    }
}
std::wstring GetStartupFolderPath(){
    PWSTR pszPath;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Startup,
                                      0,     // no special options required
                                      NULL,  // no access token required
                                      &pszPath);
    if (SUCCEEDED(hr))
    {
        // The function succeeded, so copy the returned path to a
        // C++ string, free the memory allocated by the function,
        // and return the path string.
        std::wstring path(pszPath);
        CoTaskMemFree(static_cast<LPVOID>(pszPath));
        return path;
    }
    else{
        qDebug() << "Failed to locate startup folder";
        return std::wstring();
    }
}
bool shortcutExists(){
    std::wstring startupPath = GetStartupFolderPath();
    if(startupPath.empty()){
        return false;
    }
    // Generate the shortcut's path
    std::wstring shortcutPath = startupPath + L"\\BetterWallpapers.lnk";

    // Check if the file exists.
    if (GetFileAttributes(shortcutPath.c_str()) != INVALID_FILE_ATTRIBUTES)
    {
        return true;
    }
    return false;

}
bool addShortcutToStartup(std::wstring exePath){
    std::wstring startupPath = GetStartupFolderPath();
    if(startupPath.empty()){
        return false;
    }
    HRESULT hres;
    IShellLink* psl;
    // Initialize the COM library.
    hres = CoInitialize(NULL);
    if (FAILED(hres))
    {
        return false;
    }
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres)){
        IPersistFile* ppf;

        // Set the path to the shortcut target
        wchar_t exe[] = L"/BetterWallpapers.exe";
        psl->SetPath((exePath + exe).c_str());

        // Query IShellLink for the IPersistFile interface, used for saving the
        // shortcut in persistent storage.
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
        bool success = false;
        if (SUCCEEDED(hres))
        {
            // Generate the shortcut's path
            std::wstring shortcutExePath = std::wstring(startupPath) + L"\\BetterWallpapers.lnk";

            // Save the link by calling IPersistFile::Save.
            hres = ppf->Save(shortcutExePath.c_str(), TRUE);
            ppf->Release();
            success = true;
        }
        psl->Release();
        return success;
    }
    return hres;
}
// Function to delete the executable in the startup folder (if it exists)
// Returns whether a shortcut to the executable is now in the startup folder
bool deleteShortcutInStartupFolder()
{
    std::wstring startupPath = GetStartupFolderPath();
    if(startupPath.empty()){
        // If we can't find the startuop folder we assume our executable isn't in there
        return false;
    }
    // Generate the shortcut's path
    std::wstring shortcutPath = startupPath + L"\\BetterWallpapers.lnk";

    // Check if the file exists.
    if (GetFileAttributes(shortcutPath.c_str()) != INVALID_FILE_ATTRIBUTES)
    {
        // Delete the shortcut file.
        return !(DeleteFile(shortcutPath.c_str()) != 0);
    }

    // If we can't find the startuop folder we assume our executable isn't in there
    return false;
}
// Function to toggle whether the executable exists in the startup folder
// Returns whether the shortcut exists (true) or doesn't or can't access the folder(false)
bool toggleShortcut(std::wstring exePath){
    if(shortcutExists()){
        return deleteShortcutInStartupFolder();
    }
    return addShortcutToStartup(exePath);
}
