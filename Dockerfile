# ETAP 1 Budowanie i przygotowanie komponentów
FROM golang:1.22-alpine AS builder

# Instalacja UPX do kompresji oraz certyfikatow do HTTPS
RUN apk add --no-cache upx ca-certificates

WORKDIR /app
COPY server.go .

# Kompilacja
RUN CGO_ENABLED=0 GOOS=linux go build -trimpath -ldflags="-s -w" -o server server.go

# Kompresja pliku wykonywalnego
RUN upx --best --lzma server

# ETAP 2 Obraz finalny
FROM scratch

LABEL org.opencontainers.image.authors="Maciej Łukasiewicz"

# certyfikaty bezpieczeństwa dla żądań HTTPS
COPY --from=builder /etc/ssl/certs/ca-certificates.crt /etc/ssl/certs/

# Kopia aplikacji
COPY --from=builder /app/server /server

EXPOSE 8080

HEALTHCHECK --interval=30s --timeout=3s CMD ["/server", "-check"]

CMD ["/server"]
