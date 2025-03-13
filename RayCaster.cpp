#include <iostream>
//#include <vector>
#include <cmath>
#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MAP_WIDTH = 24;
const int MAP_HEIGHT = 24;
const double MOVE_SPEED = 0.1;
const double ROTATION_SPEED = 0.05;

// mapa świata (0 - puste, 1-4 - różne typy ścian)
int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
  /*{1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,1,0,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,0,1,1,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,1,0,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,0,1,1,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1},*/
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,1},
    {1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
  /*{1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}*/
};

class Raycaster {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    double posX = 4.0, posY = 5.0;  // początkowa pozycja gracza
    double dirX = -1.0, dirY = 0.0;   // wektor kierunku (środek pola widzenia)
    double planeX = 0.0, planeY = 0.66; // wektor płaszczyzny (szerokość pola widzenia)

    // sprawdzenie kolizji przed ruchem
    bool canMove(double newX, double newY) {
        return worldMap[static_cast<int>(newX)][static_cast<int>(newY)] == 0;
    }

    void drawMiniMap() {
        const int MINIMAP_SCALE = 10;
        const int MINIMAP_OFFSET = 10;

        // rysowanie mapy
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                SDL_Rect tile;
                tile.x = x * MINIMAP_SCALE + MINIMAP_OFFSET;
                tile.y = y * MINIMAP_SCALE + MINIMAP_OFFSET;
                tile.w = MINIMAP_SCALE;
                tile.h = MINIMAP_SCALE;

                if (worldMap[y][x] == 1) {
                    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);  // brązowy
                }
                else if (worldMap[y][x] == 2) {
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);    // zielony
                }
                else if (worldMap[y][x] == 3) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);    // niebieski
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // szary dla pustych
                }

                SDL_RenderFillRect(renderer, &tile);
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150);  // żółty

        // linia reprezentująca kierunek patrzenia
        SDL_RenderDrawLine(
            renderer,
            static_cast<int>(posY * MINIMAP_SCALE + MINIMAP_OFFSET + MINIMAP_SCALE / 2),
            static_cast<int>(posX * MINIMAP_SCALE + MINIMAP_OFFSET + MINIMAP_SCALE / 2),
            static_cast<int>((posY + dirY * 2) * MINIMAP_SCALE + MINIMAP_OFFSET + MINIMAP_SCALE / 2),
            static_cast<int>((posX + dirX * 2) * MINIMAP_SCALE + MINIMAP_OFFSET + MINIMAP_SCALE / 2)
        );

        // oznaczenie pozycji gracza
        SDL_Rect playerPos;
        playerPos.x = static_cast<int>(posY * MINIMAP_SCALE + MINIMAP_OFFSET); 
        playerPos.y = static_cast<int>(posX * MINIMAP_SCALE + MINIMAP_OFFSET);
        playerPos.w = MINIMAP_SCALE;
        playerPos.h = MINIMAP_SCALE;
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Czerwony
        SDL_RenderFillRect(renderer, &playerPos);
    }

public:
    Raycaster() {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow("Raycaster z poruszaniem",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }

    void moveForward() {
        double newX = posX + dirX * MOVE_SPEED; // dodaje do aktualnej pozycji wektor kierunku pomnozony przez predkosc ruchu
        double newY = posY + dirY * MOVE_SPEED;
        if (canMove(newX, posY)) posX = newX; //sprawdza kolizje oddzielnie dal osi X i Y i aktualizuje tylko jeśli nie ma kolizji na danej osi
        if (canMove(posX, newY)) posY = newY;
    }

    void moveBackward() {
        double newX = posX - dirX * MOVE_SPEED;
        double newY = posY - dirY * MOVE_SPEED;
        if (canMove(newX, posY)) posX = newX;
        if (canMove(posX, newY)) posY = newY;
    }

    void rotateLeft() {
        // obracanie kierunku
        double oldDirX = dirX;
        dirX = dirX * cos(ROTATION_SPEED) - dirY * sin(ROTATION_SPEED);
        dirY = oldDirX * sin(ROTATION_SPEED) + dirY * cos(ROTATION_SPEED);

        // obracanie płaszczyzny kamery
        double oldPlaneX = planeX;
        planeX = planeX * cos(ROTATION_SPEED) - planeY * sin(ROTATION_SPEED);
        planeY = oldPlaneX * sin(ROTATION_SPEED) + planeY * cos(ROTATION_SPEED);
    }

    void rotateRight() {
        // obracanie kierunku
        double oldDirX = dirX;
        dirX = dirX * cos(-ROTATION_SPEED) - dirY * sin(-ROTATION_SPEED);
        dirY = oldDirX * sin(-ROTATION_SPEED) + dirY * cos(-ROTATION_SPEED);

        // obracanie płaszczyzny kamery
        double oldPlaneX = planeX;
        planeX = planeX * cos(-ROTATION_SPEED) - planeY * sin(-ROTATION_SPEED);
        planeY = oldPlaneX * sin(-ROTATION_SPEED) + planeY * cos(-ROTATION_SPEED);
    }

    void renderFrame() {
        SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255); // jasnoniebieskie niebo
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 50, 150, 50, 255); // zielona podłoga
        SDL_Rect floorRect = { 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2 };
        SDL_RenderFillRect(renderer, &floorRect);

        for (int x = 0; x < SCREEN_WIDTH; x++) { //rysowanie lini pionowych od lewej do prawej strony

            double cameraX = 2 * x / (double)SCREEN_WIDTH - 1; //pozycja x wprzestrzeni kamery (od -1 do 1)
            double rayDirX = dirX + planeX * cameraX; //wektor kierunkowy laczacy kierunek patrzenia gracza z plaszczyzna kamery zmodyfikowana przez camerax
            double rayDirY = dirY + planeY * cameraX;

            // pozycja gracza na mapie
            int mapX = static_cast<int>(posX);
            int mapY = static_cast<int>(posY);

            // obliczanie długości promienii potrzebnych do przeciecia jednej jednostki na osi X i Y
            double deltaDistX = std::abs(1 / rayDirX); // odległość jaką trzeba przebyć od jednej pionowej linii do nastepnej
            double deltaDistY = std::abs(1 / rayDirY); // odleglosc jaka trzeba przebyc od jednej poziomej linii do nastepnej
            double perpWallDist; //prostopadla odleglosc od gracza do sciany

          
            int stepX, stepY; // okreslaja kierunkek w ktorym bedziemy przeszukiwac siatke mapy (lewo/prawo i gora/dol)
          //  bool hit = 0; //0 - brak kolizji, 1 - kolizja ze sciana, flaga wyjscia z petli DDA
           // int side; // 0 - promien uderza w sciane od strony X, 1 - od strony Y
            int collisionType = 0;

            if (rayDirX < 0) { // jesli promien idzie w kierunku malejacych x, to przeszukujemy w lewo
                stepX = -1;
            }
            else {
                stepX = 1;
            }
            if (rayDirY < 0) {
                stepY = -1;
            }
            else {
                stepY = 1;
            }

            //pozioma lub pionowa odleglosc, jaka promien musi przebyc od pozycji gracza do pierwszego przeciecia linii z siatka
            double sideDistX = (rayDirX < 0) ?
                (posX - mapX) * deltaDistX : //jesli promien idzie w lewo
                (mapX + 1.0 - posX) * deltaDistX;  
            double sideDistY = (rayDirY < 0) ?
                (posY - mapY) * deltaDistY :
                (mapY + 1.0 - posY) * deltaDistY;

            // DDA (Digital Differential Analysis)
            while (collisionType == 0) { //iteracja dopoki promien nie uderzy w sciane
                if (sideDistX < sideDistY) {  //jesli promieniowi jest blizej do pionowej linii siatki
                    sideDistX += deltaDistX; // dodajemy do odleglosci do najblizszej lini siatki odleglosc rowna odleglosci pomiedzy pionowymi liniami i otrzymujemy odleglosc od gracza do nastepnej pionowej lini
                    mapX += stepX; //aktualizacja pozycji na mapie
                    if (worldMap[mapX][mapY] > 0) collisionType = 1; // promien uderzyl w sciane pinowa
                }
                else {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    if (worldMap[mapX][mapY] > 0) collisionType = 2;
                }

                //if (worldMap[mapX][mapY] > 0) hit = 1; //jesli jest sciana, konczy petle
            }

            // obliczenie odległości do ściany
            if (collisionType==1)
                perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;// prostopadla odleglosc od kamery do sciany (zawsze mniejsza lub rowna prawdziwej odleglosci)
            else
                perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

            // obliczenie wysokości linii do narysowania
            int lineHeight = static_cast<int>(SCREEN_HEIGHT / perpWallDist);// im dalej sciana, tym nizsza linia
            int drawStart = std::max(0, -lineHeight / 2 + SCREEN_HEIGHT / 2); //gorny punkt lini, jesli obliczona wartosc jest ujemna to zostanie uzyte 0
            int drawEnd = std::min(SCREEN_HEIGHT - 1, lineHeight / 2 + SCREEN_HEIGHT / 2);// dolny punkt, jesli obliczona wartosc przekracza wysokosc ekranu, zostaje uzyte SCREEN_HEIGHT-1 czyli dolna krawedz ekranu

            // kolory scian
            SDL_Color wallColor;
            switch (worldMap[mapX][mapY]) {
            case 1: wallColor = { 139, 69, 19, 255 }; break;  // braz
            case 2: wallColor = { 0, 255, 0, 255 }; break;    // zielony
            case 3: wallColor = { 0, 0, 255, 255 }; break;    // niebieski
            default: wallColor = { 255, 255, 255, 255 };      // bialy
            }

            // przyciemnianie scian
            if (collisionType == 2) { //przyciemnia sciany rownolegle do osi X (na polnoc i poludnie)
                wallColor.r /= 2;
                wallColor.g /= 2;
                wallColor.b /= 2;
            }

            //ustawienie koloru i rysowanie linii
            SDL_SetRenderDrawColor(renderer, wallColor.r, wallColor.g, wallColor.b, wallColor.a);
            SDL_RenderDrawLine(renderer, x, drawStart, x, drawEnd);
        }

        drawMiniMap();

        SDL_RenderPresent(renderer); // wyswietlenie wszystkiego co zostalo do tej pory narysowane
    }

    void gameLoop() {
        SDL_Event event;
        bool quit = false;

        while (!quit) {
            while (SDL_PollEvent(&event)) { // sprawdza oczekujace zdarzenia
                if (event.type == SDL_QUIT) //jesli uzytkownik zamknal okno
                    quit = true;

                if (event.type == SDL_KEYDOWN) { //sprawdza ktory klawisz nacisnieto
                    switch (event.key.keysym.sym) {
                    case SDLK_UP:
                    case SDLK_w: //w lub strzalka w gore
                        moveForward();
                        break;
                    case SDLK_DOWN:
                    case SDLK_s:
                        moveBackward();
                        break;
                    case SDLK_LEFT:
                    case SDLK_a:
                        rotateLeft();
                        break;
                    case SDLK_RIGHT:
                    case SDLK_d:
                        rotateRight();
                        break;
                    case SDLK_ESCAPE:// escape zamyka okno
                        quit = true;
                        break;
                    }
                }
            }
            renderFrame(); 
            SDL_Delay(16); // około 60 FPS
        }
    }

    ~Raycaster() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

#ifdef _WIN32
#undef main
#endif

int main() {
#ifdef _WIN32
#undef main
#endif
    Raycaster raycaster;
    raycaster.gameLoop();
    return 0;
}