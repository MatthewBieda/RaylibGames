#include <raylib.h>
#include <iostream>
#include <deque>
#include <raymath.h>
#include <algorithm>

Color green {173, 204, 96, 255};
Color darkGreen {43, 51, 24, 255};

const int cellSize = 30;
const int cellCount = 25;
const int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(const Vector2& element, const std::deque<Vector2>& deque)
{
    return std::find(deque.begin(), deque.end(), element) != deque.end();
}

bool EventTriggered(double interval) 
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) 
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

struct Snake 
{
    std::deque<Vector2> body {{6, 9}, {5, 9}, {4, 9}};
    Vector2 direction {1, 0};
    bool addSegment = false;

    void Draw() 
    {
        for (const auto& [x, y]: body) 
        {
            Rectangle segment = Rectangle{offset + x * cellSize, offset + y * cellSize, cellSize, cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void Update() 
    {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment) 
        {
            addSegment = false;
        } else {
            body.pop_back();
        }
    }

    void Reset() 
    {
        body = {{6, 9}, {5, 9}, {4, 9}};
        direction = {1, 0};
    }
};

struct Food 
{
    Vector2 position {5, 6};
    Texture2D texture;

    Food(std::deque<Vector2> snakeBody) 
    {
        Image image = LoadImage("food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }

    void Draw() 
    {
        DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
    }

    Vector2 GenerateRandomCell() 
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return {x,y};
    }

    Vector2 GenerateRandomPos(std::deque<Vector2> snakeBody) 
    {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody)) 
        {
            position = GenerateRandomCell();
        }
        return position;
    }
};

struct Game 
{
    Snake snake;
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    Game() 
    {
        InitAudioDevice();
        eatSound = LoadSound("eat.mp3");
        wallSound = LoadSound("wall.mp3");
    }

    ~Game() 
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw() 
    {
        food.Draw();
        snake.Draw();
    }

    void Update() 
    {
        if (running) 
        {
            snake.Update();
            checkCollisionWithFood();
            checkCollisionWithEdges();
            checkCollisionWithTail();
        }
    }

    void checkCollisionWithFood() 
    {
        if (Vector2Equals(snake.body[0], food.position)) 
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            ++score;
            PlaySound(eatSound);
        }
    }

    void checkCollisionWithEdges() 
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1) 
        {
            GameOver();
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1) 
        {
            GameOver();
        }
    }

    void checkCollisionWithTail() 
    {
        std::deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();

        if (ElementInDeque(snake.body[0], headlessBody)) 
        {
            GameOver();
        }
    }

    void GameOver() 
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
    }
};

int main() 
{
    std::cout << "Testing" << std::endl; 
    InitWindow(2*offset + cellSize * cellCount, 2*offset + cellSize * cellCount, "Snake");
    SetTargetFPS(60);

    Game game;

    while (!WindowShouldClose()) 
    {
        BeginDrawing(); 

        if (EventTriggered(0.2)) 
        {
            game.Update();
        }

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) 
        {
            game.snake.direction = {0, -1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.snake.direction = {0, 1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) 
        {
            game.snake.direction = {-1, 0};
            game.running = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) 
        {
            game.snake.direction = {1, 0};
            game.running = true;
        }

        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{offset - 5, offset - 5, cellCount*cellSize + 10, cellCount*cellSize + 10},5, darkGreen);
        DrawText("Retro Snake",offset - 5, 20, 40, darkGreen);
        DrawText(TextFormat("%i", game.score), offset - 5, offset + cellCount*cellSize + 10, 40, darkGreen);
        game.Draw();

        EndDrawing();
    }

    UnloadTexture(game.food.texture);
    CloseWindow();

    return 0;
}
