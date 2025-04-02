#include "include/raylib.h"
#include "vector"
#include <iostream>

// Definizione delle texture utilizzate
Texture2D UP;
Texture2D RIGHT;
Texture2D LEFT;
Texture2D DOWN;
Texture2D UP_LEFT;
Texture2D UP_RIGHT;
Texture2D DOWN_RIGHT;
Texture2D DOWN_LEFT;
Texture2D BULLET;

Texture2D ZOMBIE;

// Definizione dei vettori riguardanti i proiettili
std::vector<Vector2> bullets_pos;
std::vector<Vector2> bullets_dir;
// Definizione dei vettori riguardanti gli zombie
std::vector<Vector2> zombies_pos;
std::vector<int> zombies_health;

void MovePlayer(Vector2& pos, int speed, int width, int height) {
    float vcorrere = (float)speed;
    // Modifica la velocità in quella di corsa se SHIFT è premuto
    if (IsKeyDown(KEY_LEFT_SHIFT)){
        vcorrere = (float)speed * 1.5f;
    }
    // Muove il personaggio in base ai tasti WASD premuti
    // Ne blocca il movimento se ció comporta la fuoriuscita dallo schermo
    if (IsKeyDown(KEY_D) && pos.x + vcorrere < GetScreenWidth() - RIGHT.width){
        pos.x += vcorrere;
    }
    if (IsKeyDown(KEY_A) && pos.x - vcorrere > 0){
        pos.x -= vcorrere;
    }
    if (IsKeyDown(KEY_W) && pos.y - vcorrere > 0){
        pos.y -= vcorrere;
    }
    if(IsKeyDown(KEY_S) && pos.y + vcorrere < GetScreenHeight() - RIGHT.height){
        pos.y += vcorrere;
    }


}

void RotatePlayer(Vector2& dir) {
    // Modifica la direzione del player in base ai tasti freccia premuti
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
        if (IsKeyDown(KEY_UP)) {
            dir.y = -1;
        } else if (IsKeyDown(KEY_DOWN)) {
            dir.y = 1;
        } else {
            dir.y = 0;
        }

        if (IsKeyDown(KEY_LEFT)) {
            dir.x = -1;
        } else if (IsKeyDown(KEY_RIGHT)) {
            dir.x = 1;
        } else {
            dir.x = 0;
        }
    }
}

Texture2D current_texture;
void DrawPlayer(Vector2 dir, Vector2 pos) {
    // Modifica la texture corrente in base alla direzione passata.
    // Nel caso in cui current_texture non sia ancora stata inizializzata, viene inizializzata
    // alla texture rivolta verso l'alto
    if(current_texture.id <= 0) {
        current_texture = UP;
    } else if (dir.y == -1 && dir.x == -1) {
        current_texture = UP_LEFT;
    } else if (dir.y == -1 && dir.x == 1) {
        current_texture = UP_RIGHT;

    } else if (dir.y == 1 && dir.x == -1){
        current_texture = DOWN_LEFT;

    }else if (dir.y == 1 && dir.x == 1){
        current_texture = DOWN_RIGHT;

    }else if (dir.y == -1){
        current_texture = UP;
    }
    else if (dir.x == 1){
        current_texture = RIGHT;

    }
    else if (dir.x == -1){
        current_texture = LEFT;

    }
    else if (dir.y == 1){
        current_texture = DOWN;

    }
    DrawTextureEx(current_texture, pos, 0, 2, WHITE);
}

void PlayerShoot(Vector2 dir, Vector2 pos) {
    // Aggiunge a bullets_pos la posizione centrale del giocatore, e la direzione in cui
    // il proiettile dovrá andare a bullets_dir
    if (IsKeyPressed(KEY_SPACE) && IsKeyUp(KEY_LEFT_SHIFT)) {
        pos.x += (float)BULLET.width;
        pos.y += (float)BULLET.height / 2;

        bullets_pos.push_back(pos);
        bullets_dir.push_back(dir);
    }
}

void RemoveBullet(int i) {
    // Rimuove la posizione e la direzione associate al proiettile posizionato all'indice i
    bullets_pos.erase(bullets_pos.begin() + i);
    bullets_dir.erase(bullets_dir.begin() + i);
}

void RemoveZombie(int i) {
    // Rimuove la posizione e la vita associate allo zombie posizionato all'indice i
    zombies_pos.erase(zombies_pos.begin() + i);
    zombies_health.erase(zombies_health.begin() + i);
}

void MoveBullets(float speed) {
    // Cambia le posizioni dei proiettili salvate in bullets_pos, nella direzione corrispondente salvata
    // in bullets_dir
    for (int i = 0; i < bullets_dir.size(); i++) {
        Vector2 dir = bullets_dir[i];
        Vector2& pos = bullets_pos[i];

        Vector2 move_vec { dir.x * speed, dir.y * speed};
        pos.x += move_vec.x;
        pos.y += move_vec.y;

        if ((int)pos.x > GetScreenWidth() || (int)pos.x < 0 || (int)pos.y > GetScreenHeight() || (int)pos.y < 0) {
            RemoveBullet(i);
            i--;
        }
    }
}

void DrawBullets() {
    // Disegna tutti i proiettili in base alle posizioni salvate nel vettore bullets_pos
    for (int i = 0; i < bullets_pos.size(); i++) {
        Vector2 pos = bullets_pos[i];
        DrawTexture(BULLET, (int)pos.x, (int)pos.y, WHITE);
    }
}

void MoveZombies(float speed, Vector2 player_pos) {
    // Cambia la posizione di ogni zombie della velocitá specificata in direzione del player
    for (int i = 0; i < zombies_pos.size(); i++) {
        if (zombies_pos[i].x > player_pos.x) {
            zombies_pos[i].x -= speed;
        } else if (zombies_pos[i].x < player_pos.x) {
            zombies_pos[i].x += speed;
        }

        if (zombies_pos[i].y > player_pos.y) {
            zombies_pos[i].y -= speed;
        } else if (zombies_pos[i].y < player_pos.y) {
            zombies_pos[i].y += speed;
        }
    }
}

void AddZombie(Vector2 pos) {
    // Aggiunge dal fondo il vettore passato negli argomenti al vettore delle posizioni degli zombie (zombies_pos):
    // questo permette di disegnarlo su schermo e di controllarne le collisioni.
    // Inoltre aggiunge un valore intero indicante la vita dello zombie al vettore delle vite degli zombie (zombies_health)
    zombies_pos.push_back(pos);
    zombies_health.push_back(6);
}

bool spawned = false;
void ZombiesClock() {
    // Permette di generare uno zombie una sola volta ogni due secondi
    if ((int)GetTime() % 2 != 0) {
        spawned = false;
        return;
    }

    if ((int)GetTime() % 2 == 0 && !spawned) {
        spawned = true;

        // Genera uno zombie casualmente in uno degli angoli dello schermo
        int pick = GetRandomValue(0, 3);
        if (pick == 0) {
            AddZombie(Vector2 {0, 0});
        } else if (pick == 1) {
            AddZombie(Vector2 {0, (float)GetScreenHeight()});
        } else if (pick == 2) {
            AddZombie(Vector2 {(float)GetScreenWidth(), 0});
        } else if (pick == 3) {
            AddZombie(Vector2 {(float)GetScreenWidth(), (float)GetScreenHeight()});
        }
    }
}

void DrawZombies() {
    // Disegna tutti gli zombie in base alle posizioni salvate nel vettore zombies_pos
    for (int i = 0; i < zombies_pos.size(); i++) {
        DrawTextureEx(ZOMBIE, zombies_pos[i], 0, 2, WHITE);
    }
}

bool IsPlayerHitted(Vector2 player_pos) {
    // Controlla per ogni zombie se il rettangolo rappresentante la hit box dello zombie coincida con quello del player
    // Ritorna vero se ció accade, altrimenti ritorna falso
    Rectangle player_rect {player_pos.x, player_pos.y, (float)current_texture.width, (float)current_texture.height};
    for (int i = 0; i < zombies_pos.size(); i++) {
        Rectangle zombie_rect {zombies_pos[i].x, zombies_pos[i].y, (float)ZOMBIE.width, (float)ZOMBIE.height};
        if (CheckCollisionRecs(player_rect, zombie_rect)) {
            return true;
        }
    }
    return false;
}

void CheckForZombies() {
    // Controlla per ogni zombie se un qualsiasi proiettile collide con la sua hit box
    // In caso ció accade, rimuove uno di vita nell'array zombies_health, al suo stesso indice
    for (int i = 0; i < zombies_pos.size(); i++) {
        Rectangle zombie_rect = {zombies_pos[i].x, zombies_pos[i].y, (float)ZOMBIE.width * 2, (float)ZOMBIE.height * 2};

        for (int k = 0; k < bullets_pos.size(); k++) {
            Rectangle bullet_rect = {bullets_pos[k].x, bullets_pos[k].y, (float)BULLET.width, (float)BULLET.height};

            if (CheckCollisionRecs(bullet_rect, zombie_rect)) {
                RemoveBullet(k);
                zombies_health[i]--;
                k--;
            }
        }
    }
}

void KillZombies() {
    // Rimuove qualsiasi zombie abbia 0 o meno di vita
    for (int i = 0; i < zombies_health.size(); i++) {
        if (zombies_health[i] <= 0) {
            RemoveZombie(i);
            i--;
        }
    }
}

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    Vector2 player_pos;
    player_pos.x = 400;
    player_pos.y = 400;
    Vector2 player_dir;
    player_dir.y = -1;
    player_dir.x = 0;

    const int speed = 2;

    InitWindow(screenWidth, screenHeight, "Diller");

    UP = LoadTexture("../Assets/su.png");
    DOWN = LoadTexture("../Assets/giu.png");
    LEFT = LoadTexture("../Assets/sinistra.png");
    RIGHT = LoadTexture("../Assets/destra.png");
    UP_LEFT = LoadTexture("../Assets/susinistra.png");
    UP_RIGHT = LoadTexture("../Assets/sudestra.png");
    DOWN_LEFT = LoadTexture("../Assets/giuleft.png");
    DOWN_RIGHT = LoadTexture("../Assets/giudestra.png");
    BULLET = LoadTexture("../Assets/bullet.png");

    ZOMBIE = LoadTexture("../Assets/zombie.png");

    // Carica le teture per la schermata di morte e per lo sfondo di gioco
    const Texture2D WASTED = LoadTexture("../Assets/wasted.png");
    const Texture2D BKG = LoadTexture("../Assets/map.png");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------d
    // Main game loop
    bool is_player_alive = true;
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (is_player_alive) {
            // Logica Player
            MovePlayer(player_pos, speed, GetScreenWidth(), GetScreenHeight());
            RotatePlayer(player_dir);
            PlayerShoot(player_dir, player_pos);

            // Spawna gli zombie
            ZombiesClock();

            // Movimento zombie verso il player
            MoveZombies(speed * 0.5, player_pos);

            // Movimento proiettili
            MoveBullets(speed * 2.5);

            // Controlla se gli zombie collidono con un proiettile, e in caso gli toglie vita
            CheckForZombies();
            // Uccide gli zombie con 0 o meno di vita
            KillZombies();

            // Uccisione player
            if (IsPlayerHitted(player_pos)) {
                is_player_alive = false;
            }
        } else {
            // Se R é premuto, resetta il gioco
            if (IsKeyPressed(KEY_R)) {
                player_pos = {(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2};
                bullets_pos.clear();
                bullets_dir.clear();
                zombies_pos.clear();
                zombies_health.clear();
                is_player_alive = true;
            }
        }

        BeginDrawing();
        // Disegna lo sfondo
        DrawTextureEx(BKG, Vector2 {0,0 }, 0, 1.5, WHITE);
        // Disegna proiettili, player e zombie
        DrawBullets();
        DrawPlayer(player_dir, player_pos);
        DrawZombies();

        // Disegna schermata di morte se il Player e' morto
        if (!is_player_alive) {
            DrawText("Premi R per ricominciare", 625, 650, 50, RED);
            DrawTexture(WASTED, 0, 0, WHITE);
        }

        ClearBackground(GRAY);
        EndDrawing();
        //----------------------------------------------------------------------------------

    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}