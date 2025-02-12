#include <raylib.h>

Color green {38, 185, 154, 255};
Color darkGreen {20, 160, 133, 255};
Color lightGreen {129, 204, 184, 255};
Color yellow {243, 213, 91, 255};

int playerScore = 0;
int cpuScore = 0;

struct Ball 
{
    float x;
    float y;
    int speedX;
    int speedY;
    int radius;

    void Draw() const
    {
        DrawCircle(x, y, radius, yellow);
    }

    void Update() 
    {
        x += speedX;
        y += speedY;

        if (y + radius >= GetScreenHeight() || y - radius <= 0) 
        {
            speedY *= -1;
        }
        if (x + radius >= GetScreenWidth()) 
        {
            ++cpuScore;
            ResetBall();
        }
        if (x - radius <= 0) 
        {
            ++playerScore;
            ResetBall();
        }
    }

    void ResetBall() 
    {
        speedX = 9;
        speedY = 9;

        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speedChoices[2] = {-1,1};
        speedX *= speedChoices[GetRandomValue(0, 1)];
        speedY *= speedChoices[GetRandomValue(0, 1)];
    }
};

struct Paddle 
{
    float x;
    float y;
    float width;
    float height;
    int speed;

    void Draw() 
    {
        DrawRectangleRounded(Rectangle {x, y, width, height}, 0.8, 0, WHITE);
    }

    void Update() 
    {
        if (IsKeyDown(KEY_UP)) 
        {
            y -= speed;
        } 
        if (IsKeyDown(KEY_DOWN)) 
        {
            y += speed;;
        }
        limitMovement();
    }

    void limitMovement() 
    {
        if (y <= 0) 
        {
            y = 0;
        } 
        if (y + height >= GetScreenHeight()) 
        {
            y = GetScreenHeight() - height;
        }
    }
};

struct CpuPaddle: public Paddle
{
    void Update(int ballY) 
    {
        if (y + height / 2 > ballY) {
            y -= speed;
        }
        if (y + height / 2 <= ballY) 
        {
            y += speed;
        }
        limitMovement();
    }
};

Ball ball;
Paddle player;
CpuPaddle cpu;

int main() {

    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Pong");

    InitAudioDevice();
    Sound bloop = LoadSound("sound.wav");

    SetTargetFPS(60);

    ball.x = screenWidth / 2;
    ball.y = screenHeight / 2;
    ball.speedX = 7;
    ball.speedY = 7;
    ball.radius = 20;

    player.width = 25;
    player.height = 120;
    player.x = screenWidth - player.width - 10;
    player.y = screenHeight / 2 - player.height / 2;
    player.speed = 6;

    cpu.width = 25;
    cpu.height = 120;
    cpu.x = 10;
    cpu.y = screenHeight / 2 - cpu.height / 2;
    cpu.speed = 6;

    while (!WindowShouldClose()) 
    {
        BeginDrawing();
        int fps = GetFPS();
        DrawText(TextFormat("FPS: %i", fps), 10, 10, 20, WHITE);

        // Updating
        ball.Update();
        player.Update();
        cpu.Update(ball.y);

        // Check for a win condition
        if (playerScore >= 10 || cpuScore >= 10) 
        {
            const char* winner = (playerScore >= 10) ? "Player Wins!" : "CPU Wins!";
            ClearBackground(darkGreen);
            DrawText(TextFormat("%s", winner), screenWidth / 2 - 150, screenHeight / 2 - 40, 60, WHITE);
            DrawText("Press R to restart", screenWidth / 2 - 150, screenHeight / 2 + 40, 30, WHITE);

            // Wait for 'R' to reset the game
            if (IsKeyPressed(KEY_R)) 
            {
                playerScore = 0;
                cpuScore = 0;
                ball.ResetBall();
            }

            EndDrawing();
            continue;  // Skip the rest of the loop if the game is over
        }

        // Collision Checking
        if (CheckCollisionCircleRec(Vector2 {ball.x, ball.y}, ball.radius, Rectangle {player.x, player.y, player.width, player.height})) 
        {
            ball.x = player.x - ball.radius - 1;
            PlaySound(bloop);

            ball.speedX = (ball.speedX > 0) ? ball.speedX + 1 : ball.speedX - 1;
            ball.speedX *= -1;
        }
        if (CheckCollisionCircleRec(Vector2 {ball.x, ball.y}, ball.radius, Rectangle {cpu.x, cpu.y, cpu.width, cpu.height})) 
        {
            ball.x = cpu.x + cpu.width + ball.radius + 1;
            PlaySound(bloop);

            ball.speedX = (ball.speedX > 0) ? ball.speedX + 1 : ball.speedX - 1;
            ball.speedX *= -1;
        }

        // Drawing
        ClearBackground(darkGreen);
        DrawRectangle(screenWidth / 2, 0, screenWidth / 2, screenHeight, green);
        DrawCircle(screenWidth / 2, screenHeight / 2, 150, lightGreen);
        DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, WHITE);
        ball.Draw();
        cpu.Draw();
        player.Draw();
        DrawText(TextFormat("%i", cpuScore), screenWidth / 4 - 20, 20, 80, WHITE);
        DrawText(TextFormat("%i", playerScore), 3 * screenWidth / 4 - 20, 20, 80, WHITE);

        EndDrawing();
    }

    UnloadSound(bloop);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
