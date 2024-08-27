#include <filesystem>
#include <string>
#include <ctime>
#include <cstring>
#include <QThread>
#include <QtCore>
#include <QObject>
using namespace std;
namespace fs = std::filesystem;

void deleteOldImgs(string path, int days){
    // Get the time now
    time_t rawtime = time(0);
    struct tm * timeinfo;
    //Take 4 days off the current time
    rawtime -= 60*60*24*days;
    // Convert the time to a tm object
    timeinfo = localtime(&rawtime);
    char oldDate[9];
    // Format the date to match how we save our images
    strftime(oldDate, sizeof(oldDate), "%Y%m%d", timeinfo);
    // For every file in the path
    for (const auto & entry : fs::directory_iterator(path)){
        // If the image is older than the date delete it
        string fileDate = entry.path().filename().string().substr(0, 8);
        if(strcmp(oldDate, fileDate.c_str()) >= 0){
            remove(entry.path().string().c_str());
        }
    }
}
void wallpaper_loop(){
    while(1){

        QThread::sleep(5);
    }
}
