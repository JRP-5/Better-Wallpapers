#ifndef BING_WALLPAPER_H
#define BING_WALLPAPER_H

#include <string>
#include <QString>
std::string getBingNewImg(QString region, QString date, std::string path);
int setPhoto(std::string imgPath);
#endif // BING_WALLPAPER_H
