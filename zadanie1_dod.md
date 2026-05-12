# 1. Zbudowany obraz kontenera zgodny z OCI zawierający aplikację opracowaną w części obowiązkowej zadania (punkt nr 1), który jest przeznaczony na platformy sprzętowe: linux/arm64 oraz linux/amd64.
docker buildx create --name mybuilder --driver docker-container --use

docker buildx inspect --bootstrap

# 2. Proces budowania

docker buildx build \
  --platform linux/amd64,linux/arm64 \
  --secret id=gh_token,src=gh_token.txt \
  -t lolmaddmanxlo/lab8-pogoda:v1-dod \
  --cache-to type=registry,ref=lolmaddmanxlo/lab8-pogoda:cache,mode=max \
  --cache-from type=registry,ref=lolmaddmanxlo/lab8-pogoda:cache \
  --push .

# 3. Weryfikacja (Potwierdzenie manifestu)

docker buildx imagetools inspect lolmaddmanxlo/lab8-pogoda:v1-dod

