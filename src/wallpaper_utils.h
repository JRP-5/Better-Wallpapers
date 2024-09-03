#ifndef WALLPAPER_UTILS_H
#define WALLPAPER_UTILS_H
#include <QString>
void deleteOldImgs(std::string path, int days);

struct WallpaperOptions {
    QString potdSource = "Bing";
    QString bingRegion = "USA";
    bool changed;
    QString jsonPath;
    void saveJson();
};
WallpaperOptions* getJsonFromPath(const QString &jsonPath);
void checkForNewImg(WallpaperOptions *options, QString date);
void wallpaper_loop(WallpaperOptions *options);
bool addShortcutToStartup(std::wstring exePath);
bool deleteShortcutInStartupFolder();
QString getCurrentDate();
#endif // WALLPAPER_UTILS_H
