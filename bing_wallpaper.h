#ifndef BING_WALLPAPER_H
#define BING_WALLPAPER_H

#include <string>
std::string saveTodayPhoto(std::string region, std::string path);
int setPhoto(std::string imgPath);
void wallpaperLoop(struct wallpaperOption *options);
#endif // BING_WALLPAPER_H
