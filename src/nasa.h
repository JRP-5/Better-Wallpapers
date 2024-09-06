#ifndef NASA_H
#define NASA_H
#include <QString>
#include <string>

std::string getNasaNewImg(QString latestDate, QString path);
QString getNasaURL(QString path);
#endif // NASA_H
