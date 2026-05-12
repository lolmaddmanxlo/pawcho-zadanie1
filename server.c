#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

// KONFIGURACJA
#define PORT 8080
#define AUTHOR "Maciej Łukasiewicz"

// Funkcja pobierająca pogodę przez czyste gniazda TCP
void get_weather(const char* city, char* result) {
    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    server = gethostbyname("wttr.in");
    if (server == NULL) {
        strcpy(result, "Blad DNS");
        return;
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(80);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        strcpy(result, "Blad polaczenia");
        return;
    }

    // Budowanie zapytania HTTP
    char request[256];
    sprintf(request, "GET /%s?format=%%t+%%C HTTP/1.1\r\nHost: wttr.in\r\nUser-Agent: curl\r\nConnection: close\r\n\r\n", city);
    send(sockfd, request, strlen(request), 0);

    // Odbieranie odpowiedzi
    char buffer[1024];
    int n, header_done = 0;
    while ((n = recv(sockfd, buffer, 1023, 0)) > 0) {
        buffer[n] = '\0';
        char* body = strstr(buffer, "\r\n\r\n");
        if (body) {
            strcpy(result, body + 4);
            header_done = 1;
        } else if (header_done) {
            strcat(result, buffer);
        }
    }
    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "-check") == 0) {
        return 0;
    }

    // LOGI STARTOWE
    time_t now = time(NULL);
    char *date_str = ctime(&now);

    printf("========================================\n");
    printf("Data uruchomienia: %s", date_str);
    printf("Autor programu: %s\n", AUTHOR);
    printf("Serwer nasluchuje na porcie TCP: %d\n", PORT);
    printf("========================================\n");
    
    // Wymuszenie wypchnięcia bufora do logów Dockera
    fflush(stdout);

    // KONFIGURACJA SERWERA
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 5);

    while(1) {
        int new_socket = accept(server_fd, NULL, NULL);
        char request_buffer[1024] = {0};
        read(new_socket, request_buffer, 1024);

        char response[2048];
        char weather_data[512] = {0};

        // OBSŁUGA ŻĄDAŃ
        if (strstr(request_buffer, "GET /pogoda?miasto=")) {
            char city[50] = {0};
            char* start = strstr(request_buffer, "miasto=") + 7;
            char* end = strchr(start, ' ');
            if (end) *end = '\0';
            strncpy(city, start, 49);

            get_weather(city, weather_data);

            sprintf(response, 
                "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n"
                "<html><body><h2>Pogoda dla %s:</h2>"
                "<p style='font-size:20px;'>%s</p>"
                "<br><a href='/'>Powrót</a></body></html>", city, weather_data);
        } else {
            // Główna strona z interfejsem wyboru
            sprintf(response, 
                "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n"
                "<html><body><h2>Aplikacja Pogodowa</h2>"
                "<form action='/pogoda' method='GET'>"
                "<label>Wybierz miasto (np. Lublin, Warsaw, Tokyo):</label><br><br>"
                "<input type='text' name='miasto' value='Lublin'>"
                "<button type='submit'>Zatwierdź wybór</button>"
                "</form></body></html>");
        }

        send(new_socket, response, strlen(response), 0);
        close(new_socket);
    }

    return 0;
}
