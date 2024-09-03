#ifndef BING_WALLPAPER_H
#define BING_WALLPAPER_H

#include <string>
#include <QString>
std::string getBingNewImg(QString region, QString date, QString path);
int setPhoto(std::string imgPath);
#endif // BING_WALLPAPER_H
