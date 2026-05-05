#a) Zbudowanie opracowanego obrazu kontenera
docker build -t lolmaddmanxlo/lab8_weather:v1 .

#b) Uruchomienie kontenera na podstawie zbudowanego obrazu
docker run -d -p 8080:8080 --name kontener_pogoda lolmaddmanxlo/lab8_weather:v1

#c) Uzyskanie informacji z logów aplikacji
docker logs kontener_pogoda

#d) Sprawdzenie warstw oraz rozmiaru zbudowanego obrazu
docker images lolmaddmanxlo/lab8_weather:v1

docker history lolmaddmanxlo/lab8_weather:v1
