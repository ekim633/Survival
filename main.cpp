#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <string>
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/*TODO: Music
TODO: UpGrades Such as bullet speed, bullet damage

*/

struct Enemy {
    Vector2 position;
    Vector2 velocity;
    int Health = 40;
};

struct Bullet {
    Vector2 position;
    Vector2 velocity;
};

int main() {
    int windowWidth = 800;
    int windowHeight = 600;
    InitWindow(windowWidth, windowHeight, "Spawner");
    SetTargetFPS(60);

    int BlockSize = 40;
    int BlockSpeed = 1;
    int BulletSpeed = 5;
    int Counter = 0;

    Vector2 middlePosition = {windowWidth/2,windowHeight/2};
    Vector2 TowerPosition= {middlePosition.x - BlockSize/2, middlePosition.y -BlockSize/2};

    Enemy enemies[32];
    uint enemy_count = 0;


    Enemy firstEnemy = {};
    firstEnemy.position = {0,0};
    firstEnemy.velocity.y = 1;
    enemies[enemy_count++] = firstEnemy;

    Bullet bullets[100];
    uint bullet_count = 0;

    int HealthBar = BlockSize;
    int damage = 10;

    bool gameOver = false;

    bool paused = false;
    int UpgradeKills = 10;

    int changesides = 0;

    Font font = GetFontDefault();

    while (!WindowShouldClose()) {
        //Initilization
        if (!(paused || gameOver)) {
            if (GetRandomValue(1, 80) == 1 && enemy_count < ARRAY_SIZE(enemies)) {
                Enemy enemy = {};
                enemy.velocity.y = 1;

                //switch around the x or y for blocks
                if (changesides == 4) {
                    changesides = 0;
                }

                switch(changesides) {
                    case 0:
                        enemy.position = {float(GetRandomValue(0,windowWidth)), 0};
                    break;
                    case 1:
                        enemy.position = {float(GetRandomValue(0,windowWidth)),windowHeight};
                    break;
                    case 2:
                        enemy.position = {0,float(GetRandomValue(0,windowHeight))};
                    break;
                    case 3:
                        enemy.position = {windowWidth,float(GetRandomValue(0,windowHeight))};
                    break;
                }

                Rectangle rect2 = {enemy.position.x,enemy.position.y,BlockSize,BlockSize};
                bool spawn = true;
                //check if the blocks will spawn next to each other
                for (int i = 0; i < enemy_count; i++  ) {
                    Rectangle rect1 = {enemies[i].position.x,enemies[i].position.y,BlockSize,BlockSize};
                    if(CheckCollisionRecs(rect1,rect2)) {
                        spawn = false;
                    }
                }
                if (spawn) {
                    enemies[enemy_count++] = enemy;
                    changesides++;
                }
            }

            //Spawning bullets
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Bullet bullet = {};
                bullet.position = {middlePosition.x,middlePosition.y};

                Vector2 BulletDirection= {GetMousePosition().x - middlePosition.x,GetMousePosition().y - middlePosition.y};
                BulletDirection = Vector2Normalize(BulletDirection);

                Vector2 shoot = {BulletSpeed * BulletDirection.x,BulletSpeed * BulletDirection.y};
                bullet.velocity = Vector2Add(bullet.velocity, shoot);
                bullets[bullet_count++] = bullet;
            }
            //Updating bullet velocity
            for (int i = 0; i < bullet_count; i++) {
                bullets[i].position = Vector2Add(bullets[i].position, bullets[i].velocity);
            }

            //Update Positions
            for (int i = 0; i < enemy_count; i++) {
                //Direction of the Blocks
                Vector2 Direction = {TowerPosition.x- enemies[i].position.x,TowerPosition.y - enemies[i].position.y};
                Direction = Vector2Normalize(Direction);
                Vector2 move = {BlockSpeed * Direction.x, BlockSpeed * Direction.y};
                enemies[i].position = Vector2Add(enemies[i].position,move);

                Rectangle Tower = {TowerPosition.x,TowerPosition.y,BlockSize,BlockSize};
                Rectangle rect1 = {enemies[i].position.x,enemies[i].position.y,BlockSize,BlockSize};

                //Deleting the enemy when they hit the edge of the player
                if (CheckCollisionRecs(Tower,rect1)) {
                    gameOver = true;
                    Enemy temp = enemies[i];
                    enemies[i] = enemies[enemy_count - 1];
                    enemies[enemy_count - 1] = temp;
                    enemy_count -= 1;
                }

                //Checking if bullets hit the enemies
                for (int j = 0; j < bullet_count; j++) {
                    Rectangle bulRec = {bullets[j].position.x,bullets[j].position.y,20,10};
                    if (CheckCollisionRecs(rect1,bulRec) ){
                        enemies[i].Health -= damage;
                        if (enemies[i].Health <= 0 ) {
                            Enemy temp = enemies[i];
                            enemies[i] = enemies[enemy_count - 1];
                            enemies[enemy_count - 1] = temp;
                            enemy_count -= 1;
                            Counter += 1;
                        }
                        Bullet tempBullet = bullets[j];
                        bullets[j] = bullets[bullet_count - 1];
                        bullets[bullet_count - 1] = tempBullet;
                        bullet_count -= 1;
                    }
                }


            }
        }

        //Upgrades



        if ((Counter + 1) % UpgradeKills == 0) {
            paused = true;

            if (IsKeyPressed('1')) {
                damage += 2;
                paused = false;
                Counter += 1;
            }

            if (IsKeyPressed('2')) {
                BulletSpeed += 1;
                paused = false;
                Counter += 1;
            }
        }


        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < enemy_count; i++ ) {
            DrawRectangle(enemies[i].position.x,enemies[i].position.y, BlockSize,BlockSize ,RED);
            DrawRectangle(enemies[i].position.x, enemies[i].position.y - 10, enemies[i].Health, 5, PINK);
        }

        for (int i = 0; i < bullet_count; i++) {
            DrawRectangle(bullets[i].position.x,bullets[i].position.y, 20,10, PURPLE);
        }

        DrawRectangle(TowerPosition.x,TowerPosition.y,BlockSize,BlockSize,BLUE);
        DrawCircleLines(middlePosition.x, middlePosition.y, 25, GREEN);



        std::string Counts = std::to_string(Counter);
        DrawText(Counts.c_str(),10,0,40, BLACK);

        if (paused) {
            DrawRectangle(middlePosition.x - 150,middlePosition.y - 20, 400, 40, WHITE);
            Vector2 TextSize = MeasureTextEx(font, "Upgrade Menu",30, 0);

            DrawText("Upgrade Menu", middlePosition.x - TextSize.x/2 , middlePosition.y - 30, 30 ,BLACK);
            DrawText("Press 1: Bullet Damage",middlePosition.x - TextSize.x/2, middlePosition.y + 0, 20 , BLACK );
            DrawText("Press 2: Bullet Speed",middlePosition.x - TextSize.x/2, middlePosition.y + 20, 20 , BLACK );
        }


        if (gameOver) {
            DrawRectangle(middlePosition.x - 150,middlePosition.y - 20, 400, 40, WHITE);
            DrawText("Game Over", middlePosition.x - 150,middlePosition.y - 20, 50,BLACK);
        }


        EndDrawing();

    }
    return 0;
}
