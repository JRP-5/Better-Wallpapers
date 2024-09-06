#include <string>
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QObject>
#include <QtCore>

std::string getFavouriteNewImg(QString path){
    QFile file(path + "favourites.json");
    if (!file.open(QIODevice::ReadOnly)) {
        // If we can't read the favourites return nothing
        qDebug() << "Failed to read favourites files";
        return "";
    }
    else{
        QJsonObject json = QJsonDocument().fromJson(file.readAll()).object();
    }
    // TODO
    file.close();
    return "";
}
