#ifndef WALLPAPER_UTILS_H
#define WALLPAPER_UTILS_H
#include <QString>
void deleteOldImgs(std::string path, int days);

struct WallpaperOptions {
    QString potdSource = "Bing";
    QString bingRegion = "USA";
    QString jsonPath;
    void saveJson();
};
WallpaperOptions* getJsonFromPath(const QString &jsonPath);
void checkForNewImg(WallpaperOptions *options, QString date);
void wallpaper_loop(WallpaperOptions *options);
QString getCurrentDate();
bool toggleShortcut(std::wstring exePath);
bool shortcutExists();
#endif // WALLPAPER_UTILS_H
