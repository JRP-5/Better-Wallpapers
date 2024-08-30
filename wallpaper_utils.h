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
std::string checkForNewImg(WallpaperOptions *options, std::string path, QString date);
WallpaperOptions* getJsonFromPath(const QString &jsonPath);
void wallpaper_loop(WallpaperOptions *options, std::string path);
#endif // WALLPAPER_UTILS_H
