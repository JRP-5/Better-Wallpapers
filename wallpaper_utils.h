#ifndef WALLPAPER_UTILS_H
#define WALLPAPER_UTILS_H
#include <QString>
//int deleteOldImgs(std::string path, int days);
struct WallpaperOptions {
    QString potdSource;
    QString bingRegion;
    bool changed;
    QString jsonPath;
    void saveJson();
};
WallpaperOptions* getJsonFromPath(const QString &jsonPath);
void wallpaper_loop(WallpaperOptions *options);
#endif // WALLPAPER_UTILS_H
