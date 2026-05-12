# Zad 1.
Analiza podatności na zagrożenia w oparciu o Docker Scout

polecenie: docker scout cves lolmaddmanxlo/lab8-pogoda:v1-dod
<img width="832" height="423" alt="image" src="https://github.com/user-attachments/assets/d586d20d-4c79-4b38-a550-ddaa4d656686" />

Narzędzie wykazało 0 podatności (Critical: 0, High: 0, Medium: 0, Low: 0).
Brak wykrytych podatności wynika z zastosowania obrazu bazowego scratch oraz statycznej kompilacji aplikacji w języku C. Obraz nie zawiera powłoki systemowej, menedżerów pakietów ani zbędnych bibliotek dynamicznych, co redukuje powierzchnię ataku do absolutnego minimum.

# 1. Zbudowany obraz kontenera zgodny z OCI zawierający aplikację opracowaną w części obowiązkowej zadania (punkt nr 1), który jest przeznaczony na platformy sprzętowe: linux/arm64 oraz linux/amd64.
docker buildx create --name mybuilder --driver docker-container --use

docker buildx inspect --bootstrap
<img width="966" height="96" alt="image" src="https://github.com/user-attachments/assets/f0af27c1-b05e-49c5-ac04-156700a8c968" />


# 2. Proces budowania

docker buildx build \
  --platform linux/amd64,linux/arm64 \
  --secret id=gh_token,src=gh_token.txt \
  -t lolmaddmanxlo/lab8-pogoda:v1-dod \
  --cache-to type=registry,ref=lolmaddmanxlo/lab8-pogoda:cache,mode=max \
  --cache-from type=registry,ref=lolmaddmanxlo/lab8-pogoda:cache \
  --push .

<img width="1435" height="993" alt="image" src="https://github.com/user-attachments/assets/aeba2598-3f4d-4f7a-a528-7011645db822" />
Jedyny występujący błąd wynika z pierwszego uruchomienia z nowym tagiem cache – Docker informuje, że jeszcze nie znalazł niczego do pobrania. Nie jest to nic złego

Ponowne użycie polecenia buildx w celu potwierdzenia poprawnego działania:
<img width="1893" height="982" alt="image" src="https://github.com/user-attachments/assets/511936b6-7122-4849-9773-25f7f944da44" />
W tym przypadku błąd się nie pokazuje czyli wszystko działa jak powinno

# 3. Weryfikacja (Potwierdzenie manifestu)

docker buildx imagetools inspect lolmaddmanxlo/lab8-pogoda:v1-dod

<img width="1044" height="456" alt="image" src="https://github.com/user-attachments/assets/7e726820-8366-48f5-b188-7d04bee4eeec" />
