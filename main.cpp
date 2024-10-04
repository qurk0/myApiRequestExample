#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h>

using json = nlohmann::json;

// https://api.kinopoisk.dev/ - адрес, куда отправляется запрос
// Допустим будем делать запрос : https://api.kinopoisk.dev/v1.4/movie?rating.kp=7-10&genres.name=криминал

size_t WriteToFile(void* content, size_t size, size_t nmemb, std::ofstream* file)
{
    size_t totalSize = size * nmemb;

    file->write((char*)content, totalSize);

    return totalSize;
}

int main(int argc, char* argv[]) 
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();
    CURLcode res;

    if(curl)
    {
        // Файл куда мы сохраним результат запроса на сервер
        std::ofstream file("output.json");

        //Ссылка для обращения к апи неофициал. кинопоиска
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.kinopoisk.dev/v1.4/movie/random?notNullFields=name");
        
        // Для корректной работы запроса необходимо передать апи-ключ, который выдает их телеграм-бот. Допустим я супер-добрый и дам вам свой апи-ключ
        struct curl_slist *headers = nullptr;
        
        headers = curl_slist_append(headers, "accept: application/json");
        headers = curl_slist_append(headers, "X-API-KEY: HDE7V3E-MK04HG7-QZ8SGB7-6RRB61F");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) 
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else 
        {
            std::cout << "Запрос выполнен успешно, данные сохранены в output.json" << std::endl;
        }

        curl_slist_free_all(headers);
        file.close();
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    std::ifstream newFile("output.json");
    std::ofstream prettyFile("prettyOut.json");

    json data = json::parse(newFile);
    prettyFile << std::setw(4) << data << std::endl;

    return 0;
}
