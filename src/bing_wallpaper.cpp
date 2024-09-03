#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
 #include <QJsonDocument>
#include <iostream>
#include <filesystem>

using namespace std;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

struct MemoryStruct
{
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr)
    {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}
// Function to get the url of the new image given current most recent one, if a new one is needed
string getBingNewImg(QString longRegion, QString date, QString path){
    map<QString, string> regions{ { "USA","en-US"},
            {"Great Britain", "en-GB"},
            {"China", "zh-CN"},
            {"Japan", "ja-JP"},
            {"Canada", "en-CA"},
            {"Australia", "en-AU"},
            {"New Zealand", "en-NZ"},
            {"Germany", "de-DE"},
            {"Spain", "es-ES"},
            {"France", "fr-FR"},
            {"Italy", "it-IT"},
            {"Brazil", "pt-BR"}};
    string region = regions[longRegion];
    struct stat sta;
    date = "-1";
    // Getting the newest picture date from the current region
    if(stat( (path + "images/bing").toStdString().c_str(), &sta) == 0){
        // Iterate through every image in the directory
        for (const auto & entry : std::filesystem::directory_iterator((path + "images/bing").toStdString())){
            string fileDate = entry.path().filename().string().substr(0, 8);
            // If the date is more than the latest date change the latest date to it
            if((fileDate > date.toStdString() || date == "-1") && entry.path().filename().string().find(region) != std::string::npos){
                date = QString::fromStdString(fileDate);
            }
        }
    }

    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl_handle = curl_easy_init();
    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory = (char*)malloc(1); /* grown as needed by the realloc above */
    chunk.memory[0] = (char)0;
    chunk.size = 0;
    string jsonURL = "https://www.bing.com/HPImageArchive.aspx?format=js&idx=0&n=1&mkt=";
    //Add in the region to our request
    string jsonURLRegion = jsonURL + region;

    curl_easy_setopt(curl_handle, CURLOPT_URL, jsonURLRegion.c_str());
    /* disable progress meter, set to 0L to enable it */
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_CAINFO, (path + "curl-ca-bundle.crt").toStdString().c_str());
    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, (path + "libcurl-agent/1.0").toStdString().c_str());
    curl_easy_perform(curl_handle);



    QJsonDocument data = QJsonDocument::fromJson(QString(chunk.memory).toUtf8());
    if(data.isNull()){
        curl_easy_cleanup(curl_handle);
        curl_global_cleanup();
        return "";
    }
    QString imgDate =  data["images"][0]["startdate"].toString();
    if(imgDate.compare(date) <= 0){
        curl_easy_cleanup(curl_handle);
        curl_global_cleanup();
        return "";
    }

    QString url = data["images"][0]["url"].toString();

    // Get the UHD version of the URL by replaceing the 1920x1080 with UHD
    QString uhdUrl = url.replace("1920x1080", "UHD");
    // Add the bing bit to the start
    QString fullUHDurl = "https://www.bing.com" + uhdUrl;

    // Fetch the photo from the url
    /* set URL to get here */
    curl_easy_setopt(curl_handle, CURLOPT_URL, fullUHDurl.toStdString().c_str());
    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

    // Get the image path includin the bing directory
    QString imagesBing = path + "images/bing/";

    QString pageFileName = imagesBing + imgDate + QString::fromStdString(region) + ".jpg";
    // Now we'll store the image

    // If bing directory doesn't exist create it
    struct stat st;
    if(stat( (path + "images/bing").toStdString().c_str(), &st) == -1) {
        //Check if images exists
        if(stat((path + "images").toStdString().c_str(),&st) == -1){
            mkdir((path + "images").toStdString().c_str());
        }
        mkdir((path + "images/bing").toStdString().c_str());
    }
    /* open the file */
    FILE *pagefile = fopen(pageFileName.toStdString().c_str(), "wb");
    if (pagefile)
    {
        /* write the page body to this file handle */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);

        /* get it! */
        curl_easy_perform(curl_handle);

        /* close the header file */
        fclose(pagefile);
    }
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    return pageFileName.toStdString();


}


