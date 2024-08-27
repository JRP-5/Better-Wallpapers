#ifndef WALLPAPER_UTILS_H
#define WALLPAPER_UTILS_H
#include <QThread>
#include <string>
int deleteOldImgs(std::string path, int days);
void wallpaper_loop();
#endif // WALLPAPER_UTILS_H
