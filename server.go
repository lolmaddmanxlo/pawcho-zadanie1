package main

import (
	"fmt"
	"io"
	"log"
	"net/http"
	"net/url"
	"os"
	"time"
)

// pobieranie prawdziwej pogody z serwisu wttr.in
func pobierzPogode(miasto string) string {
	// Kodowanie nazwy miasta
	bezpieczneMiasto := url.QueryEscape(miasto)
	// Odpytywanie API
	zapytanie := "https://wttr.in/" + bezpieczneMiasto + "?format=%t+%C"
	
	resp, err := http.Get(zapytanie)
	if err != nil {
		return "Błąd połączenia z serwisem pogodowym"
	}
	defer resp.Body.Close()

	if resp.StatusCode != 200 {
		return "Brak danych dla tej lokalizacji"
	}

	body, err := io.ReadAll(resp.Body)
	if err != nil {
		return "Błąd odczytu danych pogodowych"
	}

	return string(body)
}

func main() {
	port := "8080"

	// HEALTHCHECK
	if len(os.Args) > 1 && os.Args[1] == "-check" {
		resp, err := http.Get("http://127.0.0.1:" + port + "/")
		if err != nil || resp.StatusCode != 200 {
			os.Exit(1)
		}
		os.Exit(0)
	}

	// DANE DO LOGÓW
	author := "Maciej Łukasiewicz"
	startupDate := time.Now().Format("2006-01-02 15:04:05")

	// LOGI STARTOWE
	fmt.Printf("Data uruchomienia: %s\n", startupDate)
	fmt.Printf("Autor programu: %s\n", author)
	fmt.Printf("Aplikacja nasluchuje na porcie TCP: %s\n", port)
	fmt.Println("=======================")

	// SERWER HTTP
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "text/html; charset=utf-8")

		// Główna strona z predefiniowaną listą
		if r.URL.Path == "/" {
			html := `
			<h2>Wybierz lokalizacje z listy:</h2>
			<form action="/pogoda" method="GET">
				<select name="miasto">
					<option value="Lublin">Polska, Lublin</option>
					<option value="Warsaw">Polska, Warszawa</option>
					<option value="Madrid">Hiszpania, Madryt</option>
					<option value="Tokyo">Japonia, Tokio</option>
					<option value="New York">USA, Nowy Jork</option>
				</select>
				<button type="submit">Sprawdz aktualna pogode</button>
			</form>`
			fmt.Fprint(w, html)
			return
		}
		
		// Strona z wynikami
		if r.URL.Path == "/pogoda" {
			miasto := r.URL.Query().Get("miasto")
			if miasto == "" {
				fmt.Fprint(w, `Brak wybranego miasta. <a href="/">Powrot</a>`)
				return
			}

			// Pobranie real danych
			aktualnaPogoda := pobierzPogode(miasto)
			
			fmt.Fprintf(w, `
				<h2>Aktualna pogoda dla: %s</h2>
				<p style="font-size: 24px; color: #2c3e50;"><strong>%s</strong></p>
				<br><a href="/">Powrot</a>
			`, miasto, aktualnaPogoda)
			return
		}
			
		w.WriteHeader(http.StatusNotFound)
		fmt.Fprint(w, "Nie znaleziono (404)")
	})

	log.Fatal(http.ListenAndServe(":"+port, nil))
}
