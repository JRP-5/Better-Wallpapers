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
std::string checkForNewImg(WallpaperOptions *options, QString date);
WallpaperOptions* getJsonFromPath(const QString &jsonPath);
void wallpaper_loop(WallpaperOptions *options);
bool addShortcutToStartup(std::wstring exePath);
#endif // WALLPAPER_UTILS_H
