Projekt zaliczeniowy SK2
Bartosz Paulewicz, Marcin Zieleń

Temat: Prosty serwer HTTP zgodny z RFC 2616

Serwer programu jest napisany w C++ i korzysta on z oddzielnych plików z funkcjami i stałymi.
Obsługuje polecenia GET, HEAD, OPTIONS, PUT oraz DELETE.
Klient jest aplikacją webową. Interfejs graficzny pozwala na edytowanie i usuwanie każdego pliku i folderu
znajdującego się w katalogu głównym (z pewnymi wyjątkami w postaci np. pliku serwera), ich wyświetlanie
oraz tworzenie nowych za pomocą pojedynczych kliknięć. 

Kompilacja i uruchomienie serwera zawarte są w pliku. Wystarczy wpisać "make" w konsolę będąc w podkatalogu z serwerem.
Aby połączyć się z serwerem należy wpisać w przeglądarkę adres IP komputera z serwerem i port, na którym
owy działa (domyślnie port 1234), łamane na "index.html", np.: "192.168.10.2:1234/index.html".

| Plik | Opis |
| --- | --- |
| server.cpp  | Serwer  |
| Connection.cpp  | Klasa „Connection” oraz powiązane z nią funkcje służące do obsługi połączeń  |
| constants.cpp  | Stałe, wektory, słowniki, zmienne globalne wspólne dla wszystkich połączeń  |
| functions.cpp  | Globalne funkcje pomocnicze  |
| index.html  | Główny widok aplikacji  |
| Makefile  | Plik z dyrektywami kompilacji  |
