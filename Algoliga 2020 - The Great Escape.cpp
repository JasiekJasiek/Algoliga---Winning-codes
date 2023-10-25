#include <bits/stdc++.h>

using namespace std;

const int SZEROKOSC = 9;
const int WYSOKOSC = 9;
const int MAX_LICZBA_GRACZY = 3;
int LICZBA_GRACZY;
const int LICZBA_KIERUNKOW = 4;
const int LICZBA_ORIENTACJI = 2;

int MOJE_ID;
int PRZECIWNIK_ID;

const int GORA = 0;
const int PRAWO = 1;
const int DOL = 2;
const int LEWO = 3;

const int POZIOMA = 0;
const int PIONOWA = 1;

const int INF = 1<<20;

struct Punkt {
    int kolumna, wiersz;
};

const std::vector<Punkt> KIERUNKI = {
    {0, -1}, // GORA,
    {1, 0}, // PRAWO,
    {0, 1}, // DOL,
    {-1, 0} // LEWO,
};

std::map<int, string> KIERUNEK_DO_STRINGA = {
    {GORA, "UP"},
    {PRAWO, "RIGHT"},
    {DOL, "DOWN"},
    {LEWO, "LEFT"},
};

struct Gracz {
    Punkt pozycja;
    int liczba_scian;
};

struct Sciana {
    Punkt pozycja;
    string orientacja;

    bool operator<(const Sciana& sciana) const {
        return pozycja.kolumna < sciana.pozycja.kolumna;
    }
};

std::vector<Punkt> docelowe_punkty[MAX_LICZBA_GRACZY];

struct Plansza {
    Gracz gracze[MAX_LICZBA_GRACZY];
    bool czy_moge_isc[SZEROKOSC][WYSOKOSC][LICZBA_KIERUNKOW];
    bool sciany[SZEROKOSC][WYSOKOSC][LICZBA_ORIENTACJI];
};

bool czy_na_planszy(Punkt punkt) {
    return punkt.kolumna >= 0 && punkt.kolumna < SZEROKOSC &&
           punkt.wiersz >= 0 && punkt.wiersz < WYSOKOSC;
}

Punkt przesun_punkt(Punkt punkt, int kierunek) {
    Punkt przesuniecie = KIERUNKI[kierunek];
    return {
        punkt.kolumna + przesuniecie.kolumna,
        punkt.wiersz + przesuniecie.wiersz
    };
}

int odwrotny_kierunek(int kierunek) {
    return (kierunek + 2) % LICZBA_KIERUNKOW;
}

// Ta funkcja nie sprawdza, czy usunięcie krawędzi jest bezpieczne. 
void usun_krawedz(Plansza& plansza, Punkt punkt, int kierunek) {
    plansza.czy_moge_isc[punkt.kolumna][punkt.wiersz][kierunek] = 1;
    Punkt sasiad_punktu = przesun_punkt(punkt, kierunek);
    plansza.czy_moge_isc[sasiad_punktu.kolumna][sasiad_punktu.wiersz][odwrotny_kierunek(kierunek)] = 1;
}

bool czy_moge_isc(Plansza& plansza, Punkt punkt, int kierunek) {
    return plansza.czy_moge_isc[punkt.kolumna][punkt.wiersz][kierunek] == 0;
}

struct GraczInfo {
    int ruch;
    int odleglosc;
};

// Zwraca kierunek
GraczInfo BFS(Plansza& plansza, Punkt start, 
        const std::vector<Punkt>& docelowe_punkty) {
    int odleglosci[SZEROKOSC][WYSOKOSC];
    for(int kolumna = 0; kolumna < SZEROKOSC; kolumna++) {
        for(int wiersz = 0; wiersz < WYSOKOSC; wiersz++) {
            odleglosci[kolumna][wiersz] = INF;
        }
    }
    std::queue<Punkt> q;
    for(Punkt punkt : docelowe_punkty) {
        q.push(punkt);
        odleglosci[punkt.kolumna][punkt.wiersz] = 0;
    }
    while(q.size()) {
        Punkt punkt = q.front(); q.pop();
        int odleglosc = odleglosci[punkt.kolumna][punkt.wiersz];
        for(int kierunek = 0; kierunek < LICZBA_KIERUNKOW; kierunek++) {
            if(!czy_moge_isc(plansza, punkt, kierunek)) continue;
            Punkt nowy_punkt = przesun_punkt(punkt, kierunek);
            if(odleglosci[nowy_punkt.kolumna][nowy_punkt.wiersz] < INF) continue;
            odleglosci[nowy_punkt.kolumna][nowy_punkt.wiersz] = odleglosc + 1;
            q.push(nowy_punkt);
        }
    }
    std::vector<pair<int, int>> odleglosci_i_kierunki;
    for(int kierunek = 0; kierunek < LICZBA_KIERUNKOW; kierunek++) {
        if(czy_moge_isc(plansza, start, kierunek)) {
            Punkt nastepny_punkt = przesun_punkt(start, kierunek);
            // cerr << kierunek << ": " << odleglosci[nastepny_punkt.kolumna][nastepny_punkt.wiersz] << endl;
            odleglosci_i_kierunki.push_back({
                odleglosci[nastepny_punkt.kolumna][nastepny_punkt.wiersz],
                kierunek
            });
        }
    }
    GraczInfo info;
    info.odleglosc = INF;
    if (odleglosci_i_kierunki.empty()) {
        return info;
    }
    sort(odleglosci_i_kierunki.begin(), odleglosci_i_kierunki.end());
    int najlepszy_kierunek = odleglosci_i_kierunki.front().second;
    info.ruch = najlepszy_kierunek;
    info.odleglosc = odleglosci_i_kierunki.front().first + 1;
    return info;
}

GraczInfo BFS(Plansza& plansza, int id_gracza) {
    return BFS(plansza, plansza.gracze[id_gracza].pozycja,
               docelowe_punkty[id_gracza]);
}

void inicjalizuj(Plansza& plansza) {
    for(int kolumna = 0; kolumna < SZEROKOSC; kolumna++) {
        for(int wiersz = 0; wiersz < WYSOKOSC; wiersz++) {
            for(int kierunek = 0; kierunek < LICZBA_KIERUNKOW; kierunek++) {
                plansza.czy_moge_isc[kolumna][wiersz][kierunek] = 0;
            }
            for(int orientacja = 0; orientacja < LICZBA_ORIENTACJI; orientacja++) {
                plansza.sciany[kolumna][wiersz][orientacja] = 0;
            }
        }
    }
    for(int kolumna = 0; kolumna < SZEROKOSC; kolumna++) {
        for(int wiersz = 0; wiersz < WYSOKOSC; wiersz++) {
            for(int kierunek = 0; kierunek < LICZBA_KIERUNKOW; kierunek++) {
                if(!czy_na_planszy(przesun_punkt({kolumna, wiersz}, kierunek))) {
                    plansza.czy_moge_isc[kolumna][wiersz][kierunek] = 1;
                }
            }
        }
    }
}

bool czy_moge_dodac_sciane(Plansza& plansza, Sciana sciana) {
    Punkt poczatek = sciana.pozycja;
    // wyjście poza planszę
    if (poczatek.kolumna >= SZEROKOSC) return false;
    if (poczatek.kolumna < 0) return false;
    if (poczatek.wiersz >= WYSOKOSC) return false;
    if (poczatek.wiersz < 0) return false;
    if (sciana.orientacja == "H") {
        if (poczatek.kolumna >= SZEROKOSC - 1) return false;
        if (poczatek.wiersz <= 0) return false;
    } else {
        if (poczatek.wiersz >= WYSOKOSC - 1) return false;
        if (poczatek.kolumna <= 0) return false;
    }
    // przecięcie z inną ścianą
    Punkt srodek = 
        przesun_punkt(poczatek, sciana.orientacja == "H" ? PRAWO : DOL);
    Punkt koniec = 
        przesun_punkt(srodek, sciana.orientacja == "H" ? PRAWO : DOL);
    return 
        !plansza.sciany[srodek.kolumna][srodek.wiersz][POZIOMA] &&
        !plansza.sciany[srodek.kolumna][srodek.wiersz][PIONOWA] &&
        !plansza.sciany[poczatek.kolumna][poczatek.wiersz][sciana.orientacja == "H" ? POZIOMA : PIONOWA] && 
        (!czy_na_planszy(koniec) || !plansza.sciany[koniec.kolumna][koniec.wiersz][sciana.orientacja == "H" ? POZIOMA : PIONOWA]);
}

// Zakładamy, że ściana jest poprawna
void dodaj_sciane(Plansza& plansza, Sciana sciana) {
    assert(czy_moge_dodac_sciane(plansza, sciana));
    Punkt poczatek = sciana.pozycja;
    Punkt srodek = 
        przesun_punkt(poczatek, sciana.orientacja == "H" ? PRAWO : DOL);
    if(sciana.orientacja == "H") {
        // pozioma ściana
        Punkt pierwszy_punkt = sciana.pozycja;
        Punkt drugi_punkt = przesun_punkt(pierwszy_punkt, PRAWO);
        usun_krawedz(plansza, pierwszy_punkt, GORA);
        usun_krawedz(plansza, drugi_punkt, GORA);
        plansza.sciany[srodek.kolumna][srodek.wiersz][POZIOMA] = 1;                
    } else if(sciana.orientacja == "V") {
        // pionowa ściana
        Punkt pierwszy_punkt = sciana.pozycja;
        Punkt drugi_punkt = przesun_punkt(pierwszy_punkt, DOL);
        usun_krawedz(plansza, pierwszy_punkt, LEWO);
        usun_krawedz(plansza, drugi_punkt, LEWO);
        plansza.sciany[srodek.kolumna][srodek.wiersz][PIONOWA] = 1;
    }
}

void test() {
    assert(czy_na_planszy({0, 0}) == true);
    assert(czy_na_planszy({0, -1}) == false);
    assert(czy_na_planszy({0, WYSOKOSC}) == false);
    assert(czy_na_planszy({-1, 0}) == false);
    assert(czy_na_planszy({SZEROKOSC, 0}) == false);

    Punkt p = przesun_punkt({1, 1}, LEWO);
    assert(p.kolumna == 0 && p.wiersz == 1);

    Plansza testowa_plansza;
    inicjalizuj(testowa_plansza);
    assert(czy_moge_isc(testowa_plansza, {0, 0}, LEWO) == false);
    assert(czy_moge_isc(testowa_plansza, {0, 0}, GORA) == false);
    assert(czy_moge_isc(testowa_plansza, {0, 0}, DOL) == true);
    assert(czy_moge_isc(testowa_plansza, {0, 0}, PRAWO) == true);

    assert(czy_moge_isc(testowa_plansza, {0, 4}, LEWO) == false);
    assert(czy_moge_isc(testowa_plansza, {0, 8}, DOL) == false);

    assert(BFS(testowa_plansza, /*start=*/{0, 0}, 
               /*docelowe_punkty=*/{ {0, 1} }).ruch == DOL);
    assert(BFS(testowa_plansza, /*start=*/{0, 0}, 
               /*docelowe_punkty=*/{ {1, 0} }).ruch == PRAWO);
    assert(BFS(testowa_plansza, /*start=*/{1, 0}, 
               /*docelowe_punkty=*/{ {0, 0} }).ruch == LEWO);
    assert(BFS(testowa_plansza, /*start=*/{0, 1}, 
               /*docelowe_punkty=*/{ {0, 0} }).ruch == GORA);

    assert(czy_moge_dodac_sciane(testowa_plansza, {8, 2, "V"}));
}

int main()
{
    for(int wiersz = 0; wiersz < WYSOKOSC; wiersz++) {
        docelowe_punkty[0].push_back({SZEROKOSC-1, wiersz});
        docelowe_punkty[1].push_back({0, wiersz});
    }
    for(int kolumna = 0; kolumna < SZEROKOSC; kolumna++) {
        docelowe_punkty[2].push_back({kolumna, WYSOKOSC-1});
    }
    cout.tie(0);
    cin.tie(0);
    cerr.tie(0);

    test();

    int _;
    cin >> _ >> _ >> LICZBA_GRACZY >> MOJE_ID;
    PRZECIWNIK_ID = (MOJE_ID == 0 ? 1 : 0);

    int moja_kolumna;
    int licznik_tur = 0;
    if (MOJE_ID == 0){
        moja_kolumna = 0;
    }
    else if (MOJE_ID == 1){
        moja_kolumna = 7;
    }
    else {
        licznik_tur = 10;
    }
    

    while (true) {
        // czyszczenie grafu
        licznik_tur++;
        Plansza plansza;
        inicjalizuj(plansza);
    	
        for (int i = 0; i < LICZBA_GRACZY; i++) {
            cin >> plansza.gracze[i].pozycja.kolumna
                >> plansza.gracze[i].pozycja.wiersz
                >> plansza.gracze[i].liczba_scian;
        }
        int moj_wiersz = 5;
        if (plansza.gracze[MOJE_ID].pozycja.wiersz == 0 || plansza.gracze[MOJE_ID].pozycja.wiersz == 1){
            moj_wiersz = 2;
        }
        else if (plansza.gracze[MOJE_ID].pozycja.wiersz == 7 || plansza.gracze[MOJE_ID].pozycja.wiersz == 8){
            moj_wiersz = 7;
        }
        int liczba_scian;
        cin >> liczba_scian; cin.ignore();
        for (int i = 0; i < liczba_scian; i++) {
            int kolumna, wiersz; // lewy, górny róg ściany
            string orientacja;
            cin >> kolumna >> wiersz >> orientacja; cin.ignore();
            dodaj_sciane(plansza, {{kolumna, wiersz}, orientacja});
        }
        
        bool rzucilem_sciane = false;
        GraczInfo ja_info = BFS(plansza, MOJE_ID);
        GraczInfo przeciwnik_info = BFS(plansza, PRZECIWNIK_ID);
        if (licznik_tur <= 4){
            if (MOJE_ID == 0){
                Sciana sciana {{moja_kolumna, moj_wiersz}, "H"};
                if(czy_moge_dodac_sciane(plansza, sciana)){
                    Plansza nowa_plansza = plansza;
                    dodaj_sciane(nowa_plansza, sciana);
                    GraczInfo nowe_ja_info = BFS(nowa_plansza, MOJE_ID);
                    GraczInfo nowe_przeciwnik_info = BFS(nowa_plansza, PRZECIWNIK_ID);
                    if(nowe_ja_info.odleglosc < INF && nowe_przeciwnik_info.odleglosc < INF){
                        cout << moja_kolumna << " " << moj_wiersz << " " << "H" << " " << "JS" << endl;
                        moja_kolumna += 2;
                        rzucilem_sciane = true;
                    }
                }
            }
            else {
                Sciana sciana {{moja_kolumna, moj_wiersz}, "H"};
                if(czy_moge_dodac_sciane(plansza, sciana)){
                    Plansza nowa_plansza = plansza;
                    dodaj_sciane(nowa_plansza, sciana);
                    GraczInfo nowe_ja_info = BFS(nowa_plansza, MOJE_ID);
                    GraczInfo nowe_przeciwnik_info = BFS(nowa_plansza, PRZECIWNIK_ID);
                    if(nowe_ja_info.odleglosc < INF && nowe_przeciwnik_info.odleglosc < INF){
                        cout << moja_kolumna << " " << moj_wiersz << " " << "H" << " " << "JS" <<  endl;
                        moja_kolumna -= 2;
                        rzucilem_sciane = true;
                    }
                }
            }
        }

        
        if(plansza.gracze[MOJE_ID].liczba_scian > 0 && !rzucilem_sciane) {

            std::vector<pair<int, Sciana>> odleglosci_i_sciany;
            for(int kolumna = 0; kolumna < SZEROKOSC; kolumna++) {
                for(int wiersz = 0; wiersz < WYSOKOSC; wiersz++) {
                    for(int orientacja = 0; orientacja < LICZBA_ORIENTACJI; orientacja++) {
                        Sciana sciana = {{kolumna, wiersz}, orientacja == POZIOMA ? "H" : "V"}; 
                        if(czy_moge_dodac_sciane(plansza, sciana)) {
                            Plansza nowa_plansza = plansza;
                            dodaj_sciane(nowa_plansza, sciana);
                            GraczInfo nowe_ja_info = BFS(nowa_plansza, MOJE_ID);
                            GraczInfo nowe_przeciwnik_info = BFS(nowa_plansza, PRZECIWNIK_ID);
                            if(nowe_ja_info.odleglosc < INF && nowe_przeciwnik_info.odleglosc < INF &&
                               nowe_przeciwnik_info.odleglosc - przeciwnik_info.odleglosc > 0 &&
                               ja_info.odleglosc > przeciwnik_info.odleglosc) {
                                int opoznienie_ja = nowe_ja_info.odleglosc - ja_info.odleglosc;
                                int opoznienie_przeciwnik = nowe_przeciwnik_info.odleglosc - przeciwnik_info.odleglosc;
                                odleglosci_i_sciany.push_back(
                                    {opoznienie_przeciwnik - opoznienie_ja, sciana}
                                );
                            }
                        }
                    }
                }
            }
            if(!odleglosci_i_sciany.empty()) {
                sort(odleglosci_i_sciany.begin(), odleglosci_i_sciany.end());
                Sciana najlepsza_sciana = odleglosci_i_sciany.back().second;
                cout << najlepsza_sciana.pozycja.kolumna << " "
                     << najlepsza_sciana.pozycja.wiersz << " "
                     << najlepsza_sciana.orientacja << endl;
                rzucilem_sciane = true;
            }
        } 
        
        if(!rzucilem_sciane) {
            GraczInfo info = BFS(
                plansza,
                /*start*/ plansza.gracze[MOJE_ID].pozycja,
                /*docelowe kratki*/ docelowe_punkty[MOJE_ID] 
            );
            cout << KIERUNEK_DO_STRINGA[info.ruch] << " " << "JS" <<  endl;
        }
    }
}
