#include "inc/SDL2/SDL.h"
#include "inc/SDL2/SDL_ttf.h"
#include "inc/SDL2/SDL_image.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>
#include <string>
#include <chrono>
#include <Windows.h>
#include "3.h"
using namespace std;
using namespace std::chrono; 
#undef main 

const int _SCREEN_WIDTH2 = 500;
const int _SCREEN_HEIGHT2 = 600;
int ch = 1;
int wintime2 = 0;
bool win2 = false;
bool _quit2 = false, _quit12 = false;
bool gameOver2 = false;
bool button_clicked2 = false;
  
const int FADE_DURATION = 2000;
const int OBSTACLE_WIDTH = 100;
const int OBSTACLE_HEIGHT = 150;
const int NUM_OBSTACLES = 20;

vector<SDL_Texture*> gBackgroundTextures1; 
SDL_Thread* _musicThread2 = NULL;

const double GRAVITY = 0.5;
const double JUMP_SPEED = -10.0; 

const int BULLET_SPEED = 20;
const int BULLET_WIDTH = 25;
const int BULLET_HEIGHT = 15;
const int FALL_SPEED = 13;  
const unsigned int seed = 42;
random_device rd;
mt19937 rng(seed);
uniform_int_distribution<> dis(-9, 9);
uniform_int_distribution<> dis1(1, 9);
SDL_Texture* wintexture2 = NULL;
SDL_Window* window2 = NULL;
SDL_Renderer* renderer2 = NULL;
SDL_Texture* spriteTexture  = NULL;
SDL_Texture* obstacleTexture = NULL;
SDL_Texture* obstacleTexture1 = NULL;
SDL_Texture* _obstacleTexture1 = NULL;   
SDL_Texture* _obstacleTexture  = NULL;
SDL_Texture* _obstacle1Texture  = NULL;
SDL_Texture* obstacle1Texture  = NULL;
SDL_Texture* bulletTexture  = NULL;
SDL_Texture* gExplosionTexture1 = NULL;

struct Bullet 
{
    SDL_Rect rect;
};

void playCollisionSound1()
{ 
    PlaySound(TEXT("del.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

void bulletsound()
{ 
    PlaySound(TEXT("del1.wav"), NULL, SND_FILENAME | SND_ASYNC);
}
bool _isInsideButton2(const SDL_Rect& buttonRect, int mouseX, int mouseY) 
{
    return (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
            mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h);
}

int _playMusic2(void* filePath) 
{
    const char* musicFilePath = static_cast<const char*>(filePath);
    SDL_AudioSpec wavSpec;
    Uint8* wavStart;
    Uint32 wavLength;

    if (SDL_LoadWAV(musicFilePath, &wavSpec, &wavStart, &wavLength) == NULL) {
        std::cerr << "Failed to load audio file: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
    if (audioDevice == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        SDL_FreeWAV(wavStart);
        return 1;
    }

    SDL_QueueAudio(audioDevice, wavStart, wavLength);
    SDL_PauseAudioDevice(audioDevice, 0); 

    while (SDL_GetQueuedAudioSize(audioDevice) > 0) 
    {
        SDL_Delay(100); 
    }

    SDL_CloseAudioDevice(audioDevice);
    SDL_FreeWAV(wavStart);
    return 0;
}

SDL_Texture* _renderText2(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) {
        cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << endl;
    }
    return texture;
}

bool _init2(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    *window = SDL_CreateWindow("Damszt - Planet X Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _SCREEN_WIDTH2, _SCREEN_HEIGHT2, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}

// Load a texture from a file
SDL_Texture* _loadTexture2(const std::string& path, SDL_Renderer* renderer) {
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == NULL) {
            std::cerr << "Unable to create texture from " << path << "! SDL_Error: " << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
} 

bool _renderbuttontext2(const string& text, int x, int y) 
{
    SDL_Color textColor = { 0, 0, 0 };
    TTF_Font* gfont = TTF_OpenFont("E:/Mini project/font/danfo.ttf", 24);
    SDL_Surface* textSurface = TTF_RenderText_Solid(gfont, text.c_str(), textColor);
    if (textSurface == NULL) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer2, textSurface);
    if (textTexture == NULL) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_Rect renderQuad = { x, y, textWidth, textHeight };
    
    SDL_RenderCopy(renderer2, textTexture, NULL, &renderQuad);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    return true;
} 

// Check if two rectangles are colliding
bool checkCollision2(SDL_Rect a, SDL_Rect b)
{
    SDL_Rect intersection;
    if (SDL_IntersectRect(&a, &b, &intersection)) 
    {
        if (intersection.y + intersection.h >= a.y && intersection.y <= a.y + a.h) {
            return true;
        }
    }
    return false;
}

void close(SDL_Window* window, SDL_Renderer* renderer) 
{
    if (renderer != NULL) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }

    if (window != NULL) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
 
    for (size_t i = 0; i < gBackgroundTextures1.size(); ++i) 
    {
        if (gBackgroundTextures1[i] != NULL) {
            SDL_DestroyTexture(gBackgroundTextures1[i]);
            gBackgroundTextures1[i] = NULL;
        }
    }
    gBackgroundTextures1.clear(); 
    if (spriteTexture != NULL) {
        SDL_DestroyTexture(spriteTexture);
        spriteTexture = NULL;
    }

    if (obstacleTexture != NULL) {
        SDL_DestroyTexture(obstacleTexture);
        obstacleTexture = NULL;
    }

    if (obstacleTexture1 != NULL) {
        SDL_DestroyTexture(obstacleTexture1);
        obstacleTexture1 = NULL;
    }

    if (_obstacleTexture1 != NULL) {
        SDL_DestroyTexture(_obstacleTexture1);
        _obstacleTexture1 = NULL;
    }

    if (_obstacleTexture != NULL) {
        SDL_DestroyTexture(_obstacleTexture);
        _obstacleTexture = NULL;
    }

    if (_obstacle1Texture != NULL) {
        SDL_DestroyTexture(_obstacle1Texture);
        _obstacle1Texture = NULL;
    }

    if (obstacle1Texture != NULL) {
        SDL_DestroyTexture(obstacle1Texture);
        obstacle1Texture = NULL;
    }

    if (bulletTexture != NULL) {
        SDL_DestroyTexture(bulletTexture);
        bulletTexture = NULL;
    }

    if (gExplosionTexture1 != NULL) {
        SDL_DestroyTexture(gExplosionTexture1);
        gExplosionTexture1 = NULL;
    }

    if (wintexture2 != NULL) {
        SDL_DestroyTexture(wintexture2);
        wintexture2 = NULL;
    } 
    ch = 1;
    wintime2 = 0;
    win2 = false;
    _quit2 = false;
    _quit12 = false;
    gameOver2 = false;
    button_clicked2 = false; 
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}


int level2() 
{ 
     
    if (!(_init2(&window2, &renderer2))) {
        std::cerr << "Failed to initialize!" << std::endl;
        return -1;
    }
    SDL_Surface* iconSurface = IMG_Load("E:/Mini project/images/icon.jpg");
    if (iconSurface == nullptr) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        IMG_Quit();
        SDL_DestroyWindow(window2);
        SDL_Quit();
        return 1;
    }
 
    SDL_SetWindowIcon(window2, iconSurface);  
    SDL_FreeSurface(iconSurface);

    if (TTF_Init() != 0) {
        cerr << "TTF_Init Error: " << TTF_GetError() << endl;
        SDL_Quit();
        return 1;
    }
    TTF_Font* font = TTF_OpenFont("E:/Mini project/font/borel.ttf", 24);
    if (!font) {
        cerr << "TTF_OpenFont Error: " << TTF_GetError() << endl;
        SDL_DestroyRenderer(renderer2);
        SDL_DestroyWindow(window2);
        SDL_Quit();
        return 1;
    }

    gBackgroundTextures1.push_back(_loadTexture2("E:/Mini project/images/cave1.png", renderer2));
    gBackgroundTextures1.push_back(_loadTexture2("E:/Mini project/images/cave2.png", renderer2));
    gBackgroundTextures1.push_back(_loadTexture2("E:/Mini project/images/cave3.png", renderer2)); 
    gBackgroundTextures1.push_back(_loadTexture2("E:/Mini project/images/cave2.png", renderer2));
    gBackgroundTextures1.push_back(_loadTexture2("E:/Mini project/images/cave4.png", renderer2)); 

    for (size_t i = 0; i < gBackgroundTextures1.size(); ++i) {
        if (gBackgroundTextures1[i] == NULL) {
            std::cerr << "Failed to load one of the background textures!" << std::endl;
            return 1;
        }
    }
    spriteTexture = _loadTexture2("E:/Mini project/images/spacecraft.png", renderer2);
    obstacleTexture = _loadTexture2("E:/Mini project/images/rock1.png", renderer2);
    obstacleTexture1 = _loadTexture2("E:/Mini project/images/species12.png", renderer2);  
    _obstacleTexture1 = _loadTexture2("E:/Mini project/images/species1.png", renderer2); 
    _obstacleTexture = _loadTexture2("E:/Mini project/images/rock1up.png", renderer2);
    _obstacle1Texture = _loadTexture2("E:/Mini project/images/rock3up.png", renderer2);  
    obstacle1Texture = _loadTexture2("E:/Mini project/images/rock2.png", renderer2); 
    bulletTexture = _loadTexture2("E:/Mini project/images/medicine.png", renderer2); 
   gExplosionTexture1 = _loadTexture2("E:/Mini project/images/blast.png", renderer2);
    

    if (spriteTexture == NULL || obstacleTexture == NULL || obstacleTexture1 == NULL || bulletTexture == NULL) {
        std::cerr << "Failed to load textures!" << std::endl; 
    }

    _musicThread2 = SDL_CreateThread(_playMusic2, "MusicThread", (void*)"2.wav");
     
    SDL_Event e;

    double bgX1 = 0.0;
    double bgX2 = static_cast<double>(_SCREEN_WIDTH2);
    const double bgSpeed = 0.6;

    double spriteX = 70.0;  
    double spriteY = 580.0;
    double spriteSpeed = 0.0;

    bool hitObstacle = false;
    bool isBlasted = false;
    bool isclicked = true;
    int blastX = 0, blastY = 0;
    int blastFrame = 0;
    Uint32 blastStartTime = 0;
    int obstacleFallY = 0, obstacleFallX = 0, tempX = 0;
    Uint32 enemyStartTime = SDL_GetTicks();

    bool obstaclehit = false; 
    int bulletSpeed = BULLET_SPEED;

    vector<SDL_Rect> obstacles;
    vector<SDL_Rect> obstacles1; 
    for (int i = 0; i < NUM_OBSTACLES; i++) 
    {
        int ypos = 0;                   
        ypos = -45; 
        obstacles.push_back({_SCREEN_WIDTH2 + dis1(rng) * 100 , ypos , OBSTACLE_WIDTH  , OBSTACLE_HEIGHT + dis1(rng)*9}); 
        ypos = _SCREEN_HEIGHT2 - OBSTACLE_HEIGHT + 45; 
        int ter =  _SCREEN_WIDTH2 + dis1(rng) * 100; 
        obstacles1.push_back({_SCREEN_WIDTH2 + dis1(rng) * 100 , ypos - dis1(rng) * 10 + 50, OBSTACLE_WIDTH,  177});             
    }
 
    SDL_Rect secondObstacle = {_SCREEN_WIDTH2 , (_SCREEN_HEIGHT2 / 2 ) + (std::rand()%12 ) * 10 , 80, 110};
    const int secondObstacleSpeed = 5;

    Uint32 startTime = SDL_GetTicks();
    int tempx = 0; 
    int currentBackgroundIndex = 0;
    int nextBackgroundIndex = (currentBackgroundIndex + 1) % gBackgroundTextures1.size();
    Uint32 lastBgSwitchTime = startTime;
    const Uint32 bgSwitchInterval = 7000;
    bool fading = false;
    Uint32 fadeStartTime = 0;
    vector<Bullet> bullets;
    auto start = high_resolution_clock::now();
    SDL_Color color = {255, 255, 255, 255};

    while (!(_quit2)) 
    {
        wintime2 += 10;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                _quit2 = true;
            } else if (e.type == SDL_KEYDOWN ) {
                if (e.key.keysym.sym == SDLK_UP) {
                    spriteSpeed = JUMP_SPEED;
                }
                 else if (e.key.keysym.sym == SDLK_SPACE) 
                 {                     
                    bullets.push_back({ { (int)spriteX + 50, (int)spriteY + 50, BULLET_WIDTH, BULLET_HEIGHT } });
                    break;
                }                
            }
        }

        for (Bullet& bullet : bullets) {
            bullet.rect.x += BULLET_SPEED;
        }

        bullets.erase(remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return b.rect.x > _SCREEN_WIDTH2; }), bullets.end());
        
        Uint32 currentTime = SDL_GetTicks();
        if (!isBlasted)
        {
            spriteSpeed += GRAVITY;
            spriteY += static_cast<int>(spriteSpeed);
 
            if (spriteY < 0) {
                spriteY = 0;
                spriteSpeed = 0;
            }
            if (spriteY + 110 > _SCREEN_HEIGHT2) 
            {  
                spriteY = _SCREEN_HEIGHT2 - 110; 
                spriteSpeed = 0;
            } 
        }
        SDL_Rect spriteRect = {(int)spriteX, (int)spriteY, 75, 110};
        if (currentTime - lastBgSwitchTime >= 3000)
        {
            tempx = 1;
        }
        if (currentTime - lastBgSwitchTime > bgSwitchInterval && !fading) 
        { 
            fading = true;
            fadeStartTime = currentTime;
            nextBackgroundIndex = (currentBackgroundIndex + 1) % gBackgroundTextures1.size();
            if (nextBackgroundIndex == 0) 
            {
                nextBackgroundIndex = 1;
            }
            lastBgSwitchTime = currentTime;
        }
        if (!isBlasted) 
        { 
            bgX1 -= bgSpeed;
            bgX2 -= bgSpeed;

            if (bgX1 <= -_SCREEN_WIDTH2) {
                bgX1 = bgX2 + _SCREEN_WIDTH2;;
            }

            if (bgX2 <= -_SCREEN_WIDTH2) {
                bgX2 = bgX1 + _SCREEN_WIDTH2;
            }
            for (auto& obstacle : obstacles1) 
            {
                if(tempx == 1)
                {
                    obstacle.x -= 3;
                    if (obstacle.x <= -OBSTACLE_WIDTH) 
                    {
                        obstacle.x = _SCREEN_WIDTH2; 
                    }
                    SDL_Rect visibleSpriteRect = {spriteRect.x + 1, spriteRect.y + 30, 75, 55};
                    SDL_Rect visibleobsRect = {obstacle.x + 20, obstacle.y + 10, 70, 95}; 
                    if (checkCollision2(visibleSpriteRect, visibleobsRect) && !win2) 
                    {
                        playCollisionSound1();
                        hitObstacle = true;   
                        isBlasted = true;                 
                        break; 
                    } 
                     
                }
            } 
            for (auto& obstacle : obstacles) 
            { 
                if(tempx == 1)
                {
                    obstacle.x -= 3;
                    if (obstacle.x <= -OBSTACLE_WIDTH) 
                    {
                        obstacle.x = _SCREEN_WIDTH2; 
                    }
                    SDL_Rect visibleSpriteRect = {spriteRect.x + 1, spriteRect.y + 30, 75, 55};
                    SDL_Rect visibleobsRect = {obstacle.x + 20, obstacle.y + 45, 70, 138};  
                    SDL_Rect visibleobs2Rect = {secondObstacle.x + 10, secondObstacle.y + 10, 50, 90};
                    if (checkCollision2(visibleSpriteRect, visibleobsRect) && !win2) 
                    {
                        playCollisionSound1();
                        hitObstacle = true;   
                        isBlasted = true;                 
                        break; 
                    }
                    else if (checkCollision2(visibleSpriteRect, visibleobs2Rect) && !win2) 
                    {
                        playCollisionSound1();
                        hitObstacle = true;    
                        isBlasted = true;                
                        break;  
                    }
                    
                    for (auto& b : bullets)
                    {
                        SDL_Rect visiblebulletRect = {b.rect.x, b.rect.y, 25, 15};
                        if (checkCollision2(visiblebulletRect, visibleobs2Rect) && !win2)
                        {
                            bulletsound();
                            obstacleFallX = secondObstacle.x; 
                            tempX = obstacleFallX;
                            secondObstacle.x = -OBSTACLE_WIDTH;
                            b.rect.x = -BULLET_WIDTH;
                            obstaclehit = true;
                            b.rect.y = -BULLET_HEIGHT;
                            obstacleFallY = secondObstacle.y;  
                            visiblebulletRect = {0,0,0,0};
                        } 
                        visiblebulletRect = {0,0,0,0} ;
                    }
                }
            } 
            if(tempx == 1)
            {
                secondObstacle.x -= secondObstacleSpeed;
                if (secondObstacle.x + OBSTACLE_WIDTH < 0) 
                {
                    secondObstacle.x = _SCREEN_WIDTH2;
                    secondObstacle.y = _SCREEN_HEIGHT2/2 + dis(rng) * 10; 
                } 
            }   
        } 
        if (hitObstacle) 
        { 
            SDL_RenderCopy(renderer2,gExplosionTexture1, NULL, &spriteRect);
            SDL_RenderPresent(renderer2);
            SDL_Delay(1000); 
            gameOver2 = true;
            break; 
        }
        if (obstaclehit)
        {
            obstacleFallY += FALL_SPEED;
            obstacleFallX -= 2;
            secondObstacle.y = obstacleFallY;  
            secondObstacle.x = obstacleFallX;
            
            if (secondObstacle.y > _SCREEN_HEIGHT2) 
            {
                secondObstacle.x = _SCREEN_WIDTH2 ;
                secondObstacle.y = _SCREEN_HEIGHT2/2 + dis(rng) * 10;  
                obstaclehit = false;
                ch = (ch > 1) ? 1 : 2;
            }
        }    
        if (wintime2 >= 30000)
        {  
            win2 = true; 
            spriteX += 3;
            if (spriteX + 110> _SCREEN_WIDTH2) 
                {
                    spriteX = -110;
                    SDL_Delay(10);
                    gameOver2 = true;
                    break;
                }   
        } 
        
        if (gameOver2) 
        {
            break;
        }

        auto now = high_resolution_clock::now();
        auto elapsed = duration_cast<seconds>(now - start).count();
        string timeText = "Time: " + to_string(elapsed) + " sec";

        SDL_Texture* timeTexture = _renderText2(renderer2, font, timeText, color);
        if (!timeTexture) {
            cerr << "Failed to render text" << endl;
            break;
        }

        SDL_RenderClear(renderer2);
 
        SDL_Rect backgroundRect1 = {static_cast<int>(bgX1), 0, _SCREEN_WIDTH2, _SCREEN_HEIGHT2};
        SDL_Rect backgroundRect2 = {static_cast<int>(bgX2), 0, _SCREEN_WIDTH2, _SCREEN_HEIGHT2};
        float alpha;
        if (fading) 
        {
            float elapsedTime = (currentTime - fadeStartTime) / (float)FADE_DURATION; 
            if (elapsedTime < 0.998f)
                { 
                    alpha = 255 * elapsedTime; 
                    SDL_SetTextureAlphaMod(gBackgroundTextures1[currentBackgroundIndex], 255 - alpha); 
                    SDL_RenderCopy(renderer2, gBackgroundTextures1[currentBackgroundIndex], NULL, &backgroundRect1); 
                    SDL_RenderCopy(renderer2, gBackgroundTextures1[currentBackgroundIndex], NULL, &backgroundRect2); 
                    SDL_SetTextureAlphaMod(gBackgroundTextures1[nextBackgroundIndex], alpha);
                    SDL_SetTextureAlphaMod(gBackgroundTextures1[nextBackgroundIndex], alpha); 
                    SDL_RenderCopy(renderer2, gBackgroundTextures1[nextBackgroundIndex], NULL, &backgroundRect1);
                    SDL_RenderCopy(renderer2, gBackgroundTextures1[nextBackgroundIndex], NULL, &backgroundRect2);
                    SDL_SetTextureAlphaMod(gBackgroundTextures1[nextBackgroundIndex], 255);   
                }
            else
            {
                alpha = 255 * elapsedTime; 
                SDL_SetTextureAlphaMod(gBackgroundTextures1[currentBackgroundIndex], 255 - alpha); 
                SDL_RenderCopy(renderer2, gBackgroundTextures1[currentBackgroundIndex], NULL, &backgroundRect1); 
                SDL_RenderCopy(renderer2, gBackgroundTextures1[currentBackgroundIndex], NULL, &backgroundRect2); 
                SDL_SetTextureAlphaMod(gBackgroundTextures1[nextBackgroundIndex], alpha);
                SDL_SetTextureAlphaMod(gBackgroundTextures1[nextBackgroundIndex], alpha); 
                SDL_RenderCopy(renderer2, gBackgroundTextures1[nextBackgroundIndex], NULL, &backgroundRect1);
                SDL_RenderCopy(renderer2, gBackgroundTextures1[nextBackgroundIndex], NULL, &backgroundRect2);
                SDL_SetTextureAlphaMod(gBackgroundTextures1[nextBackgroundIndex], 255);
                fading = false;
                currentBackgroundIndex = nextBackgroundIndex;
            } 
        }

        else 
        {
            SDL_RenderCopy(renderer2, gBackgroundTextures1[currentBackgroundIndex], NULL, &backgroundRect1);
            SDL_RenderCopy(renderer2, gBackgroundTextures1[currentBackgroundIndex], NULL, &backgroundRect2);
        }
    
        SDL_RenderCopy(renderer2, spriteTexture, NULL, &spriteRect);

        for (const auto& obstacle : obstacles) 
        {  
            SDL_RenderCopy(renderer2, obstacleTexture, NULL, &obstacle); 
        }
        for (const auto& obstacle : obstacles1) 
        { 
            SDL_RenderCopy(renderer2, _obstacle1Texture, NULL, &obstacle);  
        }
        if (tempx == 1)
        { 
            if (ch == 1)
                SDL_RenderCopy(renderer2, obstacleTexture1, NULL, &secondObstacle); 
            else if (ch == 2)
                SDL_RenderCopy(renderer2, _obstacleTexture1, NULL, &secondObstacle); 
        }
             
        
        for (const Bullet& bullet : bullets) 
        {
            SDL_RenderCopy(renderer2, bulletTexture, NULL, &bullet.rect);
        }

        int width, height;
        SDL_QueryTexture(timeTexture, NULL, NULL, &width, &height);
        SDL_Rect destRect = {27, 25, width, height};
        SDL_RenderCopy(renderer2, timeTexture, NULL, &destRect);

        SDL_RenderPresent(renderer2);

        Uint32 frameTime = SDL_GetTicks() - startTime; 
        if (frameTime < 10 )
        {    
            SDL_Delay((10) - frameTime);
        }  
        startTime = SDL_GetTicks();   
    }

    SDL_RenderClear(renderer2);
    if (_quit2)
    {
        close(window2, renderer2); 
        return 4;
    }
    else if (gameOver2 && win2) 
    {
        SDL_RenderClear(renderer2);  
        SDL_Rect buttonRect = { _SCREEN_WIDTH2 - 70 , 0 , 75, 45 };
        wintexture2 = _loadTexture2("E:/Mini project/images/level3.png", renderer2);
        while(!_quit12)
        {
            while (SDL_PollEvent(&e) != 0) 
            {
                if (e.type == SDL_QUIT) 
                {
                    _quit12 = true;
                } 
                else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    if (_isInsideButton2(buttonRect, mouseX, mouseY)) 
                    {
                        button_clicked2 = true;
                    }
                }
                else if (e.key.keysym.sym == SDLK_SPACE) 
                {
                    button_clicked2 = true;
                }
            }
            SDL_SetRenderDrawColor(renderer2, 255, 255, 255, 255);
            SDL_RenderClear(renderer2);
            SDL_RenderCopy(renderer2, wintexture2, NULL, NULL); 
            if (button_clicked2) 
            {
                break;
                _quit12 = true;
            } 
            else 
            {
                SDL_SetRenderDrawColor(renderer2, 210, 252, 252, 0);
                SDL_RenderFillRect(renderer2, &buttonRect);
                _renderbuttontext2("Done", _SCREEN_WIDTH2 - 66 , 0);
            }

            SDL_RenderPresent(renderer2);
            
        }
        close(window2, renderer2);  
        return 3;
    } 
    else
    {
        SDL_RenderClear(renderer2);  
        wintexture2 = _loadTexture2("E:/Mini project/images/nowinner.png", renderer2); 
        SDL_Rect buttonRect = { _SCREEN_WIDTH2 - 75 , 0 , 75, 45 };
        while(!_quit12)
        {
            while (SDL_PollEvent(&e) != 0) 
            {
                if (e.type == SDL_QUIT) 
                {
                    _quit12 = true;
                } 
                else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    if (_isInsideButton2(buttonRect, mouseX, mouseY)) 
                    {
                        button_clicked2 = true;
                    }
                }
                else if (e.key.keysym.sym == SDLK_SPACE) 
                {
                    button_clicked2 = true;
                }
            }
            SDL_SetRenderDrawColor(renderer2, 255, 255, 255, 255);
            SDL_RenderClear(renderer2);
            SDL_RenderCopy(renderer2, wintexture2, NULL, NULL); 
            if (button_clicked2) 
            {
                break;
                _quit12 = true;
            } 
            else 
            {
                SDL_SetRenderDrawColor(renderer2, 210, 252, 252, 0);
                SDL_RenderFillRect(renderer2, &buttonRect);
                _renderbuttontext2("Close", _SCREEN_WIDTH2 - 74 , 0);
            }

            SDL_RenderPresent(renderer2);
            
        }
        close(window2, renderer2); 
        return 4;
    } 
    
    SDL_WaitThread(_musicThread2, NULL);  
}