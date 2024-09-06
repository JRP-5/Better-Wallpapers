#include <QString>
#include <string>
#include <curl/curl.h>
#include "wallpaper_utils.h"
#include <sys/stat.h>
#include <iostream>

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
QString getNasaURL(QString path){
    /* init the curl session */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl_handle = curl_easy_init();;
    CURLcode res;

    struct MemoryStruct chunk;

    chunk.memory = (char*)malloc(1);  /* grown as needed by the realloc above */
    chunk.memory[0] = (char)0;
    chunk.size = 0;    /* no data at this point */


    /* specify URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, "https://www.nasa.gov/image-of-the-day/");

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

    curl_easy_cleanup(curl_handle);
    curl_global_init(CURL_GLOBAL_ALL);
    if(res != 0){
        return "";
    }
    int pos = std::string(chunk.memory).find("hds-gallery-item-single hds-gallery-image");
    // Search for src attribute
    int srcEnd = 0;
    for(int i = pos + strlen("hds-gallery-item-single hds-gallery-image"); i < pos + 600; i++){
        if(chunk.memory[i] == 's' && chunk.memory[i+1] == 'r' && chunk.memory[i+2] == 'c'){
            srcEnd = i+3;
            break;
        }
    }
    // Now get the src itself
    bool found1 = false;
    int counter = srcEnd;
    int urlCounter = 0;
    char URL[300];
    while(counter < srcEnd + 300){
        if(chunk.memory[counter] == '"'){
            if(!found1){
                found1 = true;
                counter++;
            }
        }
        if(chunk.memory[counter] == '"'){
            break;
        }
        if(found1){
            URL[urlCounter] = chunk.memory[counter];
            urlCounter++;
        }
        counter++;
    }
    URL[urlCounter] = (char)0;
    return QString(URL);
}
std::string getNasaNewImg(QString latestDate, QString path){
    // If we already have the latest image don't get a new one
    if(getCurrentDate().compare(latestDate) <= 0){
        return "";
    }
    /* init the curl session */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl_handle = curl_easy_init();
    CURLcode res;

    QString URL = getNasaURL(path);
    if(URL == ""){
        return "";
    }
    //Now we have the URL download the image
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
    QString filePath = path + "images/nasa/" + getCurrentDate() + ".jpg";

    // Now we'll store the image

    // If nasa directory doesn't exist create it
    struct stat st;
    if(stat( (path + "images/nasa").toStdString().c_str(), &st) == -1) {
        //Check if images exists
        if(stat((path + "images").toStdString().c_str(),&st) == -1){
            mkdir((path + "images").toStdString().c_str());
        }
        mkdir((path + "images/nasa").toStdString().c_str());
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

