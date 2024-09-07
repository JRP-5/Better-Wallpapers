#include <string>
#include <QString>
#include <curl/curl.h>
#include <iostream>
#include "wallpaper_utils.h"
#include <sys/stat.h>
#include <QDebug>

struct MemoryStruct {
    char *memory;
    size_t size;
};
static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
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
QString getUnsplashURL(QString path){
    curl_global_init(CURL_GLOBAL_ALL);
    /* init the curl session */
    CURL *curl_handle = curl_easy_init();
    CURLcode res;

    struct MemoryStruct chunk;

    chunk.memory = (char*)malloc(1);  /* grown as needed by the realloc above */
    chunk.memory[0] = (char)0;
    chunk.size = 0;    /* no data at this point */

    /* specify URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, "https://unsplash.com/collections/1459961/photo-of-the-day-(archive)");

    /* Add a certificate */
    curl_easy_setopt(curl_handle, CURLOPT_CAINFO, (path + "curl-ca-bundle.crt").toStdString().c_str());
    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    /* some servers do not like requests that are made without a user-agent
     field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, (path + "libcurl-agent/1.0").toStdString().c_str());

    /* get it! */
    res = curl_easy_perform(curl_handle);
    if(res != 0){
        return "";
    }
    qDebug() << chunk.memory;
    int pos = std::string(chunk.memory).find("Download this image");
    // Search for href keyword
    int hrefEnd = 0;
    for(int i = pos + 19; i < pos + 19 + 200; i++){
        if(chunk.memory[i] == 'h' && chunk.memory[i+1] == 'r' && chunk.memory[i+2] == 'e' && chunk.memory[i+3] == 'f'){
            hrefEnd = i+4;
            break;
        }
    }
    // Now get the href itself
    bool found1 = false;
    bool found2 = false;
    int counter = hrefEnd;
    int urlCounter = 0;
    char URL[200];
    while(!found2 && counter < hrefEnd + 200){
        if(chunk.memory[counter] == '"'){
            if(!found1){
                found1 = true;
                counter++;
            }
        }
        if(chunk.memory[counter] == '?'){
            found2 = true;
            break;
        }
        if(found1){
            URL[urlCounter] = chunk.memory[counter];
            urlCounter++;
        }
        counter++;
    }
    URL[urlCounter] = (char)0;

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    return URL;
}

// Function to download the latest unslpash image given the latest date of the current one
std::string getUnsplashNewImg(QString latestDate, QString path){
    // If we already have the latest image don't get a new one
    if(getCurrentDate().compare(latestDate) <= 0){
        return "";
    }

    curl_global_init(CURL_GLOBAL_ALL);
    /* init the curl session */
    CURL *curl_handle = curl_easy_init();
    CURLcode res;
    QString URL = getUnsplashURL(path);
    /* specify URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, URL.toStdString().c_str());

    /* Add a certificate */
    curl_easy_setopt(curl_handle, CURLOPT_CAINFO, (path + "curl-ca-bundle.crt").toStdString().c_str());

    /* some servers do not like requests that are made without a user-agent
     field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, (path + "libcurl-agent/1.0").toStdString().c_str());


    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
    // Allow redirects
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION , 1);
    // Generate the path to the image + it's name
    QString filePath = path + "images/unsplash/" + getCurrentDate() + ".jpg";

    // Now we'll store the image

    // If unsplash directory doesn't exist create it
    struct stat st;
    if(stat( (path + "images/unsplash").toStdString().c_str(), &st) == -1) {
        //Check if images exists
        if(stat((path + "images").toStdString().c_str(),&st) == -1){
            mkdir((path + "images").toStdString().c_str());
        }
        mkdir((path + "images/unsplash").toStdString().c_str());
    }
    /* open the file */
    FILE *pagefile = fopen(filePath.toStdString().c_str(), "wb");
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
    return filePath.toStdString();
}
