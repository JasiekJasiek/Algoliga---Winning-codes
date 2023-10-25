#include <bits/stdc++.h>

using namespace std;

const int inf = 100123;

const int GORA = 0;
const int PRAWO = 1;
const int DOL = 2;
const int LEWO = 3;

const int JA = 0;
const int PRZECIWNIK = 1;

const float NIEPEWNA_KULKA = 0.5;
const float MALA_KULKA = 1;
const float DUZA_KULKA = 10;

const int LICZBA_KIERUNKOW = 4;

#define x first
#define y second

typedef pair<int, int> Punkt;

const Punkt NIEDOZWOLONY_PUNKT = {-inf, -inf};

const std::vector<string> STANY = {
    "ROCK",
    "PAPER",
    "SCISSORS"
};

const std::vector<Punkt> KIERUNKI = {
    {0, -1}, // GORA,
    {1, 0}, // PRAWO,
    {0, 1}, // DOL,
    {-1, 0} // LEWO,
};

struct Mapa {
    int tura = 0;
    int wysokosc, szerokosc;
    vector<string> plansza;
    std::map<Punkt, float> polozenie_do_wartosci_kulki;
    std::map<Punkt, int> polozenie_do_najblizszego_pacmana_kulki;

    void usun_kulke_jesli_jest(int x, int y) {
        if(polozenie_do_wartosci_kulki.count({x, y})) {
            polozenie_do_wartosci_kulki.erase({x, y});
        }
    }

    void nowa_tura() {
        tura++;
    }
};

const int STAN_ZBIERAJ = 0;
const int STAN_ATAKUJ = 1;
const int STAN_BRON = 2;
const int STAN_BRON_SZYBKO = 3;

struct Pacman {
    int id;
    int x, y;
    string typ; 
    int liczba_szybkich_tur; 
    int czas_do_odnowienia; 
    int ostatnio_widziany;
    int kogo_atakujesz;
    int kto_cie_atakuje;
    int kto_obok;

    int stan = STAN_ZBIERAJ;

    int licznik_ataku = 0;

    Punkt ostatnie_polorzenie = {0, 0};

};

int odleglosci [ 40 ][ 40 ];


struct Gracz {
    int punkty = 0;
    // id -> Pacman
    map<int, Pacman> pacmany;

    void nowa_tura() {
    }
};

Mapa mapa;
vector<Gracz> gracze(2);

int odleglosc(std::pair<int, int> a) {
    return odleglosci[ a.x ][ a.y ];
}



std::vector<Punkt> kierunki = {
    {0, -1}, // góra
    {1, 0}, // prawo
    {0, 1}, // dół
    {-1, 0}, // lewo
};

bool czy_dobry_punkt(const Mapa& mapa, const Punkt& punkt) {
    return punkt.x >= 0 && punkt.y >= 0 &&
           punkt.x < mapa.szerokosc && punkt.y < mapa.wysokosc;
}

std::set<Punkt> daj_widocznosc_pacmana(const Mapa& mapa, const Pacman& pacman) {
    std::set<Punkt> widocznosc_pacmana;
    widocznosc_pacmana.insert({pacman.x, pacman.y});
    for(auto& kierunek : kierunki) {
        int x = pacman.x, y = pacman.y;
        while(true) {
            x += kierunek.x;
            y += kierunek.y;
            if (!czy_dobry_punkt(mapa, {x, y}) || mapa.plansza[y][x] == '#') {
                break;
            }
            widocznosc_pacmana.insert({x, y});
        }
    }
    return widocznosc_pacmana;
}

void wczytaj_widoczne_kulki(Mapa& mapa) {
    std::map<Punkt, float> polozenie_do_widocznej_wartosci_kulki;
    int liczba_widocznych_kulek;
    cin >> liczba_widocznych_kulek; cin.ignore();
    for (int i = 0; i < liczba_widocznych_kulek; i++) {
        int x, y, wartosc;
        cin >> x >> y >> wartosc; cin.ignore();
        polozenie_do_widocznej_wartosci_kulki[{x, y}] = wartosc;
        mapa.polozenie_do_wartosci_kulki[{x, y}] = wartosc;
    }

    for(const auto& [id, pacman]: gracze[JA].pacmany) {
        std::set<Punkt> punkty_ktore_widzi_pacman = daj_widocznosc_pacmana(mapa, pacman);
        for(Punkt p : punkty_ktore_widzi_pacman) { // kulki, które powinniśmy widzieć
            if(!polozenie_do_widocznej_wartosci_kulki.count(p)) { // tej kulki nie ma?
                mapa.usun_kulke_jesli_jest(p.x, p.y);
            }
        }
    }
}

void oblicz_najblizszych_pacmanow_do_kulek(Mapa& mapa) {
    for(const auto& [punkt_kulki, wartosc] : mapa.polozenie_do_wartosci_kulki) {
        int najblizszy_pacman_id = -1;
        int najblizszy_pacman_odleglosc = -1;
        for(const auto& [id, pacman] : gracze[JA].pacmany) {
            int odleglosc_kulki_do_pacmana = odleglosc(punkt_kulki);
            if(najblizszy_pacman_id == -1 ||
                najblizszy_pacman_odleglosc > odleglosc_kulki_do_pacmana) {
                    najblizszy_pacman_id = pacman.id;
                    najblizszy_pacman_odleglosc = odleglosc_kulki_do_pacmana;
                }
        }
        mapa.polozenie_do_najblizszego_pacmana_kulki[punkt_kulki] = najblizszy_pacman_id;
    }
}

void inicjalizuj_mape() {
    cin >> mapa.szerokosc >> mapa.wysokosc; cin.ignore();

    for (int i = 0; i < mapa.wysokosc; i++) {
        string wiersz;
        getline(cin, wiersz); // one line of the grid: space " " is floor, pound "#" is wall
        mapa.plansza.push_back(wiersz);
    }

    // wypełnij mapę kulkami
    for(int x = 0; x < mapa.szerokosc; x++) {
        for(int y = 0; y < mapa.wysokosc; y++) {
            if(mapa.plansza[y][x] != '#') {
                mapa.polozenie_do_wartosci_kulki[{x, y}] = NIEPEWNA_KULKA;
            }
        }        
    }
}

// Zakłada, że istnieje co najmniej jedna kulka o wartości
// minimalna_wartosc_kulki
Punkt daj_wspolrzedne_najblizszej_kulki(
    const Mapa& mapa, const Pacman& pacman, 
    bool ogranicz_tylko_do_najblizszych) {
    Punkt najblizsza_kulka = NIEDOZWOLONY_PUNKT;
    float wartosc_najblizszej_kulki = 0;
    for(auto [punkt, wartosc] : mapa.polozenie_do_wartosci_kulki) {
        if(ogranicz_tylko_do_najblizszych) {
            if(!mapa.polozenie_do_najblizszego_pacmana_kulki.count(punkt) ||
                mapa.polozenie_do_najblizszego_pacmana_kulki.at(punkt) != pacman.id) {
                continue;
            }
        }
        if(najblizsza_kulka == NIEDOZWOLONY_PUNKT ||
           odleglosc(najblizsza_kulka) / wartosc_najblizszej_kulki > 
           odleglosc(punkt) / wartosc) {
               najblizsza_kulka = punkt;
               wartosc_najblizszej_kulki = wartosc;
           }
    }
    return najblizsza_kulka;
}

bool czy_pierwszy_typ_atakuje_drugi(string typ_a, string typ_b) {
    if(typ_a == "PAPER") {
        return typ_b == "ROCK";
    }
    if(typ_a == "SCISSORS") {
        return typ_b == "PAPER";
    }
    return typ_b == "SCISSORS";
}

int czy_ofiara_w_poblizu(const Mapa& mapa, Pacman& pacman) {
    for(const auto& [id, pacman_przeciwnika] : gracze[PRZECIWNIK].pacmany) {
        if(czy_pierwszy_typ_atakuje_drugi(pacman.typ, pacman_przeciwnika.typ) &&
           pacman_przeciwnika.ostatnio_widziany >= mapa.tura - 1 &&
           odleglosc({pacman_przeciwnika.x, pacman_przeciwnika.y}) < 3) {
            return pacman_przeciwnika.id;
        }
    }
    return -1;
}

int czy_agresor_w_poblizu(const Mapa& mapa, Pacman& pacman) {
    for(const auto& [id, pacman_przeciwnika] : gracze[PRZECIWNIK].pacmany) {
        if(czy_pierwszy_typ_atakuje_drugi(pacman_przeciwnika.typ, pacman.typ) &&
           pacman_przeciwnika.ostatnio_widziany >= mapa.tura - 1 &&
           odleglosc({pacman_przeciwnika.x, pacman_przeciwnika.y}) < 3) {
            return pacman_przeciwnika.id;
        }
    }
    return -1;
}

bool czy_ten_sam_typ (string a, string b){
    return a == b;
}

int czy_przeciwnik_blisko (const Mapa& mapa, Pacman& pacman){
    for(const auto& [id, pacman_przeciwnika] : gracze[PRZECIWNIK].pacmany) {
        if(czy_ten_sam_typ(pacman_przeciwnika.typ, pacman.typ) &&
           pacman_przeciwnika.ostatnio_widziany >= mapa.tura - 1 &&
           odleglosc({pacman_przeciwnika.x, pacman_przeciwnika.y}) <= 1) {
            return pacman_przeciwnika.id;
        }
    }
    return -1;
}



void zmien_stan_gdy_bron_szybko (const Mapa& mapa, Pacman& pacman){
    {
        int id_przeciwnika_w_poblizu = czy_ofiara_w_poblizu(mapa, pacman);
        if (id_przeciwnika_w_poblizu != -1 && pacman.czas_do_odnowienia == 0) {
            pacman.stan = STAN_ATAKUJ;
            pacman.kogo_atakujesz = id_przeciwnika_w_poblizu;
            //cerr << pacman.kogo_atakujesz << '\n';
            return;
        }
    }
    {
        int id_przeciwnika_w_poblizu = czy_agresor_w_poblizu(mapa, pacman);
        if (id_przeciwnika_w_poblizu != -1 && pacman.czas_do_odnowienia == 0) {
            pacman.stan = STAN_BRON;
            pacman.kto_cie_atakuje = id_przeciwnika_w_poblizu;
            return;
        }
    }
     if (czy_ofiara_w_poblizu(mapa, pacman) == -1 || pacman.czas_do_odnowienia != 0) {
        pacman.stan = STAN_ZBIERAJ;
        pacman.kogo_atakujesz = -1;
    }
}

void bron_szybko (const Mapa& mapa, Pacman& pacman){
    string typ_przeciwnika =  gracze[PRZECIWNIK].pacmany[pacman.kto_obok].typ;
    string nowy_typ;
    if (typ_przeciwnika == "ROCK"){
        cout << "SWITCH " << pacman.id << " PAPER" << " " << "Jasiek" << " | ";
        return;
    }
    else if (typ_przeciwnika == "PAPER"){
        cout << "SWITCH " << pacman.id << " SCISSORS" << " " << "Jasiek" << " | ";
        return;
    }
    else if (typ_przeciwnika == "SCISSORS"){
        cout << "SWITCH " << pacman.id << " ROCK" << " " << "Jasiek" << " | ";
        return;
    }


}

void zmien_stan_gdy_zbieraj(const Mapa& mapa, Pacman& pacman) {
    {
        int id_przeciwnika_w_poblizu = czy_przeciwnik_blisko(mapa, pacman);
        if (pacman.czas_do_odnowienia == 0 && id_przeciwnika_w_poblizu != -1){
            pacman.stan = STAN_BRON_SZYBKO;
            pacman.kto_obok = id_przeciwnika_w_poblizu;
        }
    }
    {
        int id_przeciwnika_w_poblizu = czy_ofiara_w_poblizu(mapa, pacman);
        if (id_przeciwnika_w_poblizu != -1 && pacman.czas_do_odnowienia == 0) {
            pacman.stan = STAN_ATAKUJ;
            pacman.kogo_atakujesz = id_przeciwnika_w_poblizu;
            //cerr << pacman.kogo_atakujesz << '\n';
            return;
        }
    }
    {
        int id_przeciwnika_w_poblizu = czy_agresor_w_poblizu(mapa, pacman);
        //cerr << id_przeciwnika_w_poblizu << '\n';
        if (id_przeciwnika_w_poblizu != -1 && pacman.czas_do_odnowienia == 0) {
            pacman.stan = STAN_BRON;
            pacman.kto_cie_atakuje = id_przeciwnika_w_poblizu;
            return;
        }
    }
}

void zbieraj(const Mapa& mapa, Pacman& pacman) {
    Punkt idz_do = NIEDOZWOLONY_PUNKT;
    int min_odleglosc = 1e6;
    for(const auto& [id, pacman_zly]: gracze[PRZECIWNIK].pacmany){
        int odleglosc_aktualnego_pacmana = odleglosc({pacman_zly.x, pacman_zly.y});
        if (odleglosc_aktualnego_pacmana < min_odleglosc){
            min_odleglosc = odleglosc_aktualnego_pacmana;
        }
    }


    /*if (pacman.czas_do_odnowienia == 0 && min_odleglosc > 15){
        cout << "SPEED " << pacman.id << " | ";
        return;
    }*/

    for(bool ogranicz_tylko_do_najblizszych : {true, false}) {
        if(idz_do != NIEDOZWOLONY_PUNKT) {
            break;
        }
        idz_do = daj_wspolrzedne_najblizszej_kulki(
            mapa, pacman, ogranicz_tylko_do_najblizszych);
    }

    // Wypisywanie
    cout << "MOVE " << pacman.id 
                    << " " << idz_do.x 
                    << " " << idz_do.y << " " << "Jasiek" << " | ";

    //pacman.ostatnie_polorzenie = idz_do;


}

void zmien_stan_gdy_atakuj(const Mapa& mapa, Pacman& pacman) {
    if (czy_ofiara_w_poblizu(mapa, pacman) == -1) {
        pacman.stan = STAN_ZBIERAJ;
        pacman.kogo_atakujesz = -1;
    }
    int id_przeciwnika_w_poblizu = czy_agresor_w_poblizu(mapa, pacman);
    if (id_przeciwnika_w_poblizu != -1 && pacman.czas_do_odnowienia == 0) {
        pacman.stan = STAN_BRON;
        pacman.kto_cie_atakuje = id_przeciwnika_w_poblizu;
        return;
    }
}

void atakuj(const Mapa& mapa, Pacman& pacman) {
    //cerr << pacman.kto_cie_atakuje << '\n';
    Punkt idz_do = {
        gracze[PRZECIWNIK].pacmany[pacman.kogo_atakujesz].x,
        gracze[PRZECIWNIK].pacmany[pacman.kogo_atakujesz].y,
    };
    string stan_przeciwnik = gracze[PRZECIWNIK].pacmany[pacman.kogo_atakujesz].typ;
    //cerr << stan_przeciwnik << '\n';
    if (odleglosc(idz_do) <= 2 &&
        gracze[PRZECIWNIK].pacmany[pacman.kogo_atakujesz].czas_do_odnowienia == 0){
            cout << "SWITCH " << pacman.id << " " << stan_przeciwnik << " " << "Jasiek" << " | " ;
            return;
    }


    cout << "MOVE " << pacman.id 
                    << " " << idz_do.x 
                    << " " << idz_do.y << " " << "Jasiek" << " | ";

    //pacman.ostatnie_polorzenie = idz_do;
    
}

void zmien_stan_gdy_bron(const Mapa& mapa, Pacman& pacman) {
    {
        if (pacman.czas_do_odnowienia != 0){
            pacman.stan = STAN_ZBIERAJ;
        }
    }
    {
        int id_przeciwnika_w_poblizu = czy_agresor_w_poblizu(mapa, pacman);
        if (id_przeciwnika_w_poblizu == -1) {
            //cerr << pacman.id << " -> ZBIERAJ" << endl;
            pacman.stan = STAN_ZBIERAJ;
            pacman.kto_cie_atakuje = -1;
        }
    }
    {
        int id_przeciwnika_w_poblizu = czy_ofiara_w_poblizu(mapa, pacman);
        if (id_przeciwnika_w_poblizu != -1 && pacman.czas_do_odnowienia == 0) {
            //cerr << pacman.id << " -> ZBIERAJ" << endl;
            pacman.stan = STAN_ATAKUJ;
            pacman.kogo_atakujesz = id_przeciwnika_w_poblizu;
            return;
        }
    }
}

void bron(const Mapa& mapa, Pacman& pacman) {
    const Pacman& agresor = gracze[PRZECIWNIK].pacmany[pacman.kto_cie_atakuje];
    string nowy_stan;
    string stan_przeciwnik = gracze[PRZECIWNIK].pacmany[pacman.kto_cie_atakuje].typ;
    Punkt a = {
        gracze[PRZECIWNIK].pacmany[pacman.kto_cie_atakuje].x,
        gracze[PRZECIWNIK].pacmany[pacman.kto_cie_atakuje].y
    };
    //cerr << stan_przeciwnik << '\n';
    if (odleglosc(a) <= 1 &&
        gracze[PRZECIWNIK].pacmany[pacman.kto_cie_atakuje].czas_do_odnowienia == 0){
            cout << "SWITCH " << pacman.id << " " << stan_przeciwnik << " " << "Jasiek" << " | ";
            return;
    }

    for(string stan : STANY) {
        if(czy_pierwszy_typ_atakuje_drugi(stan, agresor.typ)) {
            nowy_stan = stan;
            break;
        }
    }

    cout << "SWITCH " << pacman.id << " " << nowy_stan << " " << "Jasiek" << " | ";
}

Punkt przesun_punkt(Punkt punkt, int kierunek) {
    Punkt przesuniecie = KIERUNKI[kierunek];
    punkt.x += przesuniecie.x;
    if ( punkt.x >= mapa.szerokosc){
        punkt.x = 0;
    }
    else if (punkt.x < 0){
        punkt.x = mapa.szerokosc - 1;
    }
    return {
        punkt.x,
        punkt.y + przesuniecie.y
    };
}

bool czy_moge_isc (Punkt punkt, int kierunek){
    Punkt p = przesun_punkt(punkt, kierunek);
    if (mapa.plansza[ p.y ][ p.x ] == '#'){
        return false;
    } 
    else{
        return true;
    }
}


void bfs(Punkt a) {
    queue< Punkt > q;
    odleglosci[ a.x ][ a.y ] = 0;

    q.push(a);
  
     while(q.size()) {
        Punkt punkt = q.front(); q.pop();
        int odleglosc_bfs = odleglosci[punkt.x][punkt.y];
        for(int kierunek = 0; kierunek < LICZBA_KIERUNKOW; kierunek++) {
            if(!czy_moge_isc(punkt, kierunek)) {
                continue;
            }
            Punkt nowy_punkt = przesun_punkt(punkt, kierunek);
            if(odleglosci[nowy_punkt.x][nowy_punkt.y] < inf) {
                continue;
            }
            odleglosci[nowy_punkt.x][nowy_punkt.y] = odleglosc_bfs + 1;
            q.push(nowy_punkt);
        }
    }   
 
}

int main()
{
    inicjalizuj_mape();

    Punkt test = {34, 11};

    Punkt test_wynik = {0, 11};

    assert(przesun_punkt(test, PRAWO) == test_wynik);

    // game loop
    while(true) {
        gracze[JA].nowa_tura();
        gracze[PRZECIWNIK].nowa_tura();
        mapa.nowa_tura();

        cin >> gracze[JA].punkty 
            >> gracze[PRZECIWNIK].punkty; cin.ignore();

        int liczba_widocznych_pacmanow;
        cin >> liczba_widocznych_pacmanow; cin.ignore();
        for (int gracz : {JA, PRZECIWNIK}) {
            for (auto& [id, pacman] : gracze[gracz].pacmany) {
                pacman.typ = "DEAD";
            }
        }
        for (int i = 0; i < liczba_widocznych_pacmanow; i++) {
            int id;
            bool czy_moj;
            cin >> id >> czy_moj;
            int gracz_id = czy_moj ? JA : PRZECIWNIK;
            Pacman& pacman = gracze[gracz_id].pacmany[id]; // ważne: bierzemy oryginał z tablicy!
            pacman.id = id;
            
            cin >> pacman.x >> pacman.y 
                >> pacman.typ 
                >> pacman.liczba_szybkich_tur 
                >> pacman.czas_do_odnowienia; 

            pacman.ostatnio_widziany = mapa.tura;
            cin.ignore();
        }
        for (int gracz : {JA, PRZECIWNIK}) {
            std::vector<int> umarle_pacmany;
            for (auto& [id, pacman] : gracze[gracz].pacmany) {
                if(pacman.typ == "DEAD") {
                    umarle_pacmany.push_back(id);
                }
            }
            for(int umarly : umarle_pacmany) {
                gracze[gracz].pacmany.erase(umarly);
            }
        }


        wczytaj_widoczne_kulki(mapa);
        oblicz_najblizszych_pacmanow_do_kulek(mapa);

        bool jeden_stoi = false;
       

        // ruszaj się pacmanami
        for (auto& [id, pacman] : gracze[JA].pacmany) {
            bool used = false;   
            //cerr << pacman.id << '\n';

            for (int i = 0; i < 40; ++i){
                for (int j = 0; j < 40; ++j){
                    odleglosci[ i ][ j ] = inf;
                }
            }

            Punkt p = {pacman.x, pacman.y};

            bfs(p);

            if (pacman.czas_do_odnowienia == 0 && !jeden_stoi){
                //cerr << "tu" << '\n';
                if (pacman.x == pacman.ostatnie_polorzenie.x && 
                    pacman.y == pacman.ostatnie_polorzenie.y){
                        //cerr << "tu" << '\n';
                        cout << "MOVE " << pacman.id << " " << pacman.x << " " << pacman.y << " " << "Jasiek" << " | ";
                        pacman.ostatnie_polorzenie = {0, 0};
                        used = true;
                        jeden_stoi = true;
                        continue; 
                }
            }
            if (!used){
                pacman.ostatnie_polorzenie = {pacman.x, pacman.y};
            }
            else{
                pacman.ostatnie_polorzenie = {0, 0};
            }

            //cerr << pacman.ostatnie_polorzenie.x << " " << pacman.ostatnie_polorzenie.y << '\n';

            if(pacman.stan == STAN_ZBIERAJ) {
                zmien_stan_gdy_zbieraj(mapa, pacman);
            }
            else if(pacman.stan == STAN_ATAKUJ) {
                pacman.licznik_ataku++;
                if (pacman.licznik_ataku > 4){
                    pacman.stan = STAN_ZBIERAJ;
                    zbieraj(mapa, pacman);
                    pacman.licznik_ataku = 0;
                    continue;
                }
                zmien_stan_gdy_atakuj(mapa, pacman);
            }
            else if(pacman.stan == STAN_BRON) {
                zmien_stan_gdy_bron(mapa, pacman);
            }
            else if (pacman.stan == STAN_BRON_SZYBKO){
                zmien_stan_gdy_bron_szybko(mapa, pacman);
            }

            if(pacman.stan == STAN_ZBIERAJ) {
                zbieraj(mapa, pacman);
                //cerr << "zbieranie" << '\n';
            }
            else if(pacman.stan == STAN_ATAKUJ) {
                atakuj(mapa, pacman);
                //cerr << "atak" << '\n';
            }
            else if(pacman.stan == STAN_BRON) {
                bron(mapa, pacman);
               //cerr << "obrona" << '\n';
            }
            else if (pacman.stan == STAN_BRON_SZYBKO){
                bron_szybko(mapa, pacman);
                //cerr << "szybko_bron" << '\n';
            }

        }

        //cout << "Jasiek" << '\n';

        cout << endl;

        


    }
}


