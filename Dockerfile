# syntax=docker/dockerfile:1
# rozszerzony frontend BuildKit

# ETAP 1: Builder
FROM alpine:latest AS builder

# Instalacja narzędzi
RUN apk add --no-cache gcc musl-dev upx git

WORKDIR /app

# Pobieranie kod bezpośrednio z GitHub przy użyciu tokenu przekazanego jako secret
RUN --mount=type=secret,id=gh_token \
    git clone https://$(cat /run/secrets/gh_token)@github.com/lolmaddmanxlo/pawcho-zadanie1.git .

# Kompilacja statyczna
RUN gcc -static -Os -s -ffunction-sections -fdata-sections -Wl,--gc-sections server.c -o server && \
    upx --ultra-brute server

# Finalny obraz
FROM scratch

# Metadane
LABEL org.opencontainers.image.authors="Maciej Łukasiewicz"

COPY --from=builder /app/server /server
USER 1001:1001
EXPOSE 8080
HEALTHCHECK --interval=30s --timeout=3s CMD ["/server", "-check"]

CMD ["/server"]
