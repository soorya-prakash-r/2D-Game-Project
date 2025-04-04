#include "inc/SDL2/SDL.h"
#include "inc/SDL2/SDL_image.h"
#include "inc/SDL2/SDL_ttf.h"
#include <queue>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "4.h"
#include <Windows.h> 
#include <chrono>

using namespace std;
using namespace std::chrono; 
#undef main

const int _SCREEN_WIDTH3 = 480;
const int _SCREEN_HEIGHT3 = 580;  
const int RECT_WIDTH = 50; 
const int RECT_HEIGHT = 25;
int rectX = (_SCREEN_WIDTH3 - RECT_WIDTH) / 2  ;
int rectY = (_SCREEN_HEIGHT3 - RECT_HEIGHT) / 2 - 48;
const int TILE_SIZE = 32;    
const int SPEED = 2;
int etime[4];
int ckey = 1, key = 1;
bool ismoving[4] = {true, true, true, true}, found = false;
queue<SDL_Point> path[4];
SDL_Thread* _musicThread3 = NULL;
bool f[4] = {true,true,true,true}, f1[4] = {true,true,true,true};
bool win3 = false;
bool _quit3 = false, _quit13 = false;
bool button_clicked = false;
int truetime1 = 0, truetime2 = 0, truetime3 = 0, truetime4 = 0;
auto elapsed = 0;

 
SDL_Window* gWindow3 = NULL;
SDL_Renderer* gRenderer3 = NULL;
SDL_Texture* gWallTexture = NULL;
SDL_Texture* bTexture = NULL;
SDL_Texture* gemTexture = NULL;
SDL_Texture* gEnemyTexture3 = NULL;
SDL_Texture* spriteTexture3 = NULL;
SDL_Texture* gExplosionTexture3;
SDL_Texture* wintexture3 = NULL;

bool follow[4] = {false, false, false, false};
bool gameOver3 = false;

void normalize(float& dx, float& dy) 
{
    float length = sqrt(dx * dx + dy * dy);
    if (length != 0) {
        dx /= length;
        dy /= length;
    }
}

template <typename T>
void clearQueue(std::queue<T>& q) {
    while (!q.empty()) {
        q.pop();
    }
}

void playCollisionSound3()
{ 
    PlaySound(TEXT("del.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

void playwinSound()
{ 
    PlaySound(TEXT("del1.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

void move(SDL_Rect& a, SDL_Rect& b, int x, float speed = 1.8f)
{
    if (!path[x].empty()) 
    {
        SDL_Point nextPoint = path[x].front();
        float dx = nextPoint.x - b.x;
        float dy = nextPoint.y - b.y; 
        normalize(dx, dy); 
        b.x += static_cast<int>(dx * speed);
        b.y += static_cast<int>(dy * speed);
 
        if ( abs(b.x - nextPoint.x) < speed &&  abs(b.y - nextPoint.y) < speed) 
        {
            path[x].pop(); 
        }
    }
}

bool _checkcollision3(SDL_Rect a, SDL_Rect b) 
{
    SDL_Rect intersection;
    if (SDL_IntersectRect(&a, &b, &intersection)) 
    {        
        if (intersection.y + intersection.h >= a.y && intersection.y <= a.y + a.h) 
        {
            return true;
        }
    }
    return false;
}

int _playMusic3(void* filePath) 
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

bool _renderbuttontext3(const string& text, int x, int y) 
{
    SDL_Color textColor = { 0, 0, 0 };
    TTF_Font* gfont = TTF_OpenFont("E:/Mini project/font/danfo.ttf", 24);
    SDL_Surface* textSurface = TTF_RenderText_Solid(gfont, text.c_str(), textColor);
    if (textSurface == NULL) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer3, textSurface);
    if (textTexture == NULL) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_Rect renderQuad = { x, y, textWidth, textHeight };
    
    SDL_RenderCopy(gRenderer3, textTexture, NULL, &renderQuad);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    return true;
}  

SDL_Texture* _renderText3(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color) {
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

bool _init3() 
{ 
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gWindow3 = SDL_CreateWindow("Damszt - Planet X Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _SCREEN_WIDTH3, _SCREEN_HEIGHT3, SDL_WINDOW_SHOWN);
    if (gWindow3 == NULL) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gRenderer3 = SDL_CreateRenderer(gWindow3, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer3 == NULL) {
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

SDL_Texture* _loadTexture3(const std::string& path) {
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
    } else {
        newTexture = SDL_CreateTextureFromSurface(gRenderer3, loadedSurface);
        if (newTexture == NULL) {
            std::cerr << "Unable to create texture from " << path << "! SDL_Error: " << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

bool _isInsideButton3(const SDL_Rect& buttonRect, int mouseX, int mouseY) 
{
    return (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
            mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h);
}

void _close3() 
{ 
    if (gRenderer3 != NULL) {
        SDL_DestroyRenderer(gRenderer3);
        gRenderer3 = NULL;
    }

    if (gWindow3 != NULL) {
        SDL_DestroyWindow(gWindow3);
        gWindow3 = NULL;
    }

    // Destroy textures
    if (gWallTexture != NULL) {
        SDL_DestroyTexture(gWallTexture);
        gWallTexture = NULL;
    }

    if (bTexture != NULL) {
        SDL_DestroyTexture(bTexture);
        bTexture = NULL;
    }

    if (gemTexture != NULL) {
        SDL_DestroyTexture(gemTexture);
        gemTexture = NULL;
    }

    if (gEnemyTexture3 != NULL) {
        SDL_DestroyTexture(gEnemyTexture3);
        gEnemyTexture3 = NULL;
    }

    if (spriteTexture3 != NULL) {
        SDL_DestroyTexture(spriteTexture3);
        spriteTexture3 = NULL;
    }

    if (gExplosionTexture3 != NULL) {
        SDL_DestroyTexture(gExplosionTexture3);
        gExplosionTexture3 = NULL;
    }

    if (wintexture3 != NULL) {
        SDL_DestroyTexture(wintexture3);
        wintexture3 = NULL;
    }

    for (int i=0;i<4;i++)
    {
        clearQueue(path[i]);
        follow[i] = false;
        f[i] = true;
        f1[i] = true;
        ismoving[i] = true;
        etime[i] = 0;
    }
 
    ckey = 1; key = 1;
    found = false; 
    win3 = false;
    _quit3 = false; _quit13 = false;
    button_clicked = false;
    truetime1 = 0; truetime2 = 0; truetime3 = 0; truetime4 = 0;
    gameOver3 = false;
    elapsed = 0;

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void renderTexture(SDL_Texture* texture, int x, int y, int width, int height) {
    SDL_Rect renderQuad = { x, y, width, height };
    SDL_RenderCopy(gRenderer3, texture, NULL, &renderQuad);
}

int level3() 
{
    if (TTF_Init() != 0) {
        cerr << "TTF_Init Error: " << TTF_GetError() << endl;
        SDL_Quit();
        return 1;
    } 

    TTF_Font* font = TTF_OpenFont("E:/Mini project/font/borel.ttf", 24);
    if (!font) {
        cerr << "TTF_OpenFont Error: " << TTF_GetError() << endl;
        SDL_DestroyRenderer(gRenderer3);
        SDL_DestroyWindow(gWindow3);
        SDL_Quit();
        return 1;
    }

    Uint32 startTime = SDL_GetTicks();

    if (!(_init3())) {
        std::cerr << "Failed to initialize!" << std::endl;
        return 1;
    }

    SDL_Surface* iconSurface = IMG_Load("E:/Mini project/images/icon.jpg");
    if (iconSurface == nullptr) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        IMG_Quit();
        SDL_DestroyWindow(gWindow3);
        SDL_Quit();
        return 1;
    }
 
    SDL_SetWindowIcon(gWindow3, iconSurface);  
    SDL_FreeSurface(iconSurface); 

    gWallTexture = _loadTexture3("E:/Mini project/images/wall1.png");
    bTexture = _loadTexture3("E:/Mini project/images/button.png");
    gemTexture = _loadTexture3("E:/Mini project/images/gem.png");
    gEnemyTexture3 = _loadTexture3("E:/Mini project/images/species3.png");
    spriteTexture3 = _loadTexture3("E:/Mini project/images/disc.png");
    gExplosionTexture3 = _loadTexture3("E:/Mini project/images/blast.png");

    SDL_Rect treasure = {rectX, rectY, RECT_WIDTH, RECT_HEIGHT };
    SDL_Rect ruby = {rectX + 6, rectY, RECT_WIDTH - 10, RECT_HEIGHT };
    SDL_Rect e1 = { rectX + 10, rectY - 38, TILE_SIZE, TILE_SIZE};
    SDL_Rect e2 = { rectX + 10, rectY + 30, TILE_SIZE, TILE_SIZE};
    SDL_Rect e3 = { rectX - 35, rectY - 4, TILE_SIZE, TILE_SIZE};
    SDL_Rect e4 = { rectX + 48, rectY - 4, TILE_SIZE, TILE_SIZE};
    SDL_Rect sprite = {_SCREEN_WIDTH3 / 2 + 17, _SCREEN_HEIGHT3 / 2 + 255, TILE_SIZE , TILE_SIZE };

    if (gWallTexture == NULL || gemTexture == NULL || bTexture == NULL || gEnemyTexture3 == NULL || spriteTexture3 == NULL || gExplosionTexture3 == NULL) 
    {        std::cerr << "Failed to load texture!" << std::endl;
        _close3();
        return 1;
    }
 
    const int mazeWidth = _SCREEN_WIDTH3 / 32 ;
    const int mazeHeight = _SCREEN_HEIGHT3 / 32;
 
    int maze[20][21] = 
    {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
        {1,1,0,1,1,1,1,0,1,1,1,0,1,0,1},
        {1,1,0,1,1,1,1,0,1,1,1,0,1,0,1},
        {1,1,0,1,1,1,1,0,1,1,1,0,0,0,1},
        {1,1,0,1,1,1,1,0,1,1,1,0,1,0,1},
        {1,1,0,1,1,1,1,0,1,1,1,0,1,0,1},
        {1,1,0,0,0,0,0,0,0,0,0,0,1,0,1},
        {1,1,0,1,1,1,1,0,1,0,1,0,1,0,1},
        {1,1,0,0,1,0,0,0,0,0,1,0,1,0,1},
        {1,0,0,1,1,1,1,0,0,1,1,0,1,0,1},
        {1,0,0,1,1,1,1,0,1,1,1,0,1,0,1},
        {1,1,0,0,0,0,0,0,0,1,1,0,1,0,1},
        {1,1,0,1,1,1,1,1,1,0,1,0,1,0,1},
        {1,1,0,0,0,0,0,0,0,0,0,0,1,0,1},
        {1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
        {1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,0,1,1,1,1,1,1}
    };
    
    _musicThread3 = SDL_CreateThread(_playMusic3, "MusicThread", (void*)"3.wav");
    SDL_Event e;
    auto start = high_resolution_clock::now();
    SDL_Color color = {255, 255, 255, 255};

    while (!(_quit3) && elapsed <= 120) 
    { 
        while (SDL_PollEvent(&e) != 0) 
        {
            if (e.type == SDL_QUIT) {
                _quit3 = true;
            }
            const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        if (currentKeyStates[SDL_SCANCODE_UP] && sprite.y  > 0) 
        { 
            key = 1;
        }
        else if (currentKeyStates[SDL_SCANCODE_DOWN] && sprite.y < _SCREEN_HEIGHT3 - sprite.h) 
        {
            key = 2;
        }
        else if (currentKeyStates[SDL_SCANCODE_LEFT] && sprite.x > 0) 
        {
            key = 3;
        }
        else if (currentKeyStates[SDL_SCANCODE_RIGHT] && sprite.x < _SCREEN_WIDTH3 - sprite.w) 
        {
            key = 4;
        } 
        if (follow[0])
        {
            path[0].push({sprite.x, sprite.y});
        }
        if (follow[1])
        {
            path[1].push({sprite.x, sprite.y});
        }
        if (follow[2])
        {
            path[2].push({sprite.x, sprite.y});
        }
        if (follow[3])
        {
            path[3].push({sprite.x, sprite.y});
        }
        }   

        SDL_Rect e1rect = {e1.x + 5, e1.y + 5, TILE_SIZE - 12, TILE_SIZE - 12};
        SDL_Rect e2rect = {e2.x + 5, e2.y + 5, TILE_SIZE - 12, TILE_SIZE - 12};
        SDL_Rect e3rect = {e3.x + 5, e3.y + 5, TILE_SIZE - 12, TILE_SIZE - 12};
        SDL_Rect e4rect = {e4.x + 5, e4.y + 5, TILE_SIZE - 12, TILE_SIZE - 12};

        SDL_Rect _e1 = {e1.x + 5, e1.y - 150, TILE_SIZE - 10, TILE_SIZE + 150};
        SDL_Rect _e2 = {e2.x + 5, e2.y + 5, TILE_SIZE - 12, TILE_SIZE + 110};
        SDL_Rect _e3 = {e3.x - 110 , e3.y + 5, TILE_SIZE + 110, TILE_SIZE - 12};
        SDL_Rect _e4 = {e4.x + 5, e4.y + 5, TILE_SIZE + 88, TILE_SIZE - 12};
        SDL_Rect _r = {ruby.x + 5, ruby.y + 3, TILE_SIZE , TILE_SIZE - 14};
        
        SDL_Rect visibleSpriteRect = {sprite.x + 1, sprite.y + 4, TILE_SIZE - 3, TILE_SIZE - 9};

        if (_checkcollision3(_r, visibleSpriteRect))
        { 
            ruby.x -= 5;
            SDL_RenderCopy(gRenderer3, gExplosionTexture3, NULL, &sprite);
            SDL_RenderPresent(gRenderer3);
            ruby = {0,0,0,0};
            SDL_RenderCopy(gRenderer3, gemTexture, NULL, &ruby);
            SDL_RenderPresent(gRenderer3);
            playwinSound();
            SDL_Delay(1000); 
            gameOver3 = true;  
            win3 = true;
            break;
        }

        if (e1.x == rectX + 10 && e1.y == rectY - 38)
        {
            if(_checkcollision3(_e1, visibleSpriteRect))
            {
                follow[0] = true; 
            }
        }
        if (e2.x == rectX + 10 && e2.y == rectY + 30)
        {
            if(_checkcollision3(_e2, visibleSpriteRect))
                follow[1] = true;
        }
        if (e3.x == rectX - 35 && e3.y == rectY - 4)
        {
            if(_checkcollision3(_e3, visibleSpriteRect))
                follow[2] = true;
        }
        if (e4.x == rectX + 48 && e4.y == rectY - 4)
        {
            if(_checkcollision3(_e4, visibleSpriteRect))
                follow[3] = true;
        }

        if (_checkcollision3(e1rect, visibleSpriteRect) || _checkcollision3(e2rect, visibleSpriteRect) || _checkcollision3(e3rect, visibleSpriteRect) || _checkcollision3(e4rect, visibleSpriteRect)) 
        {              
            playCollisionSound3();  
            SDL_RenderCopy(gRenderer3, gExplosionTexture3, NULL, &sprite);
            SDL_RenderPresent(gRenderer3);
            SDL_Delay(1000); 
            gameOver3 = true;  
            break;
        } 

        auto now = high_resolution_clock::now();
        elapsed = duration_cast<seconds>(now - start).count();
        string timeText = "Time: " + to_string(elapsed) + " sec";

        SDL_Texture* timeTexture = _renderText3(gRenderer3, font, timeText, color);
        if (!timeTexture) {
            cerr << "Failed to render text" << endl;
            break;
        }

        SDL_SetRenderDrawColor(gRenderer3, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(gRenderer3);

        if (follow[0])
        { 
            etime[0] = SDL_GetTicks();
            if (e1.y >= 30 && f1[0])
            {
                e1.y -= 2;
                e1.x = rectX + 10;  
            }
            else
            {
                f1[0] = false;
            }
            if (f[0])
            {
                truetime1 = SDL_GetTicks();  
                f[0] = false;
            }
            if (!f1[0])
            {
                if (etime[0] <= truetime1 + 6000)
                    move(sprite, e1, 0);  
                else
            {
                move(sprite, e1, 1);
                move(sprite, e1, 1);
                e1.x = -_SCREEN_WIDTH3;
                e1.y = -_SCREEN_HEIGHT3;
                SDL_RenderCopy(gRenderer3, gEnemyTexture3, NULL, &e1);
                e1.x = rectX + 10;
                e1.y = rectY - 38;
                f[0] = true;
                f1[0] = true;
                follow[0] = false;
                clearQueue(path[0]);
            }
            }             
        }
        if (follow[1])
        {
            etime[1] = SDL_GetTicks(); 
            if (e2.y <= 390 && f1[1])
            {
                e2.y += 2;
                e2.x = rectX + 10;  
            }
            else
            {
                f1[1] = false;
            }
            if (f[1])
            {
                truetime2 = SDL_GetTicks();  
                f[1] = false;
            }
            if (!f1[1])
            {
                if (etime[1] <= truetime2 + 6000)
                {
                    move(sprite, e2, 1);  
                } 
                else
                {
                    move(sprite, e2, 1);
                    move(sprite, e2, 1);
                    e2.x = -_SCREEN_WIDTH3;
                    e2.y = -_SCREEN_HEIGHT3;
                    SDL_RenderCopy(gRenderer3, gEnemyTexture3, NULL, &e2);
                    e2.x = rectX + 10;
                    e2.y = rectY + 30;
                    f[1] = true;
                    f1[1] = true;
                    follow[1] = false;
                    clearQueue(path[1]);
                }
            }
        }
        if (follow[2])
        {
            etime[2] = SDL_GetTicks(); 
            if (e3.x >= 58 && f1[2])
            {
                e3.x -= 2;
                e3.y = rectY - 4;  
            }
            else
            {
                f1[2] = false;
            }
            if (f[2])
            {
                truetime3 = SDL_GetTicks();  
                f[2] = false;
            }
            if (!f1[2])
            {
                if (etime[2] <= truetime3 + 6000)
                {
                    move(sprite, e3, 2);  
                } 
                else
                {
                    move(sprite, e3, 2);
                    move(sprite, e3, 2);
                    e3.x = -_SCREEN_WIDTH3;
                    e3.y = -_SCREEN_HEIGHT3;
                    SDL_RenderCopy(gRenderer3, gEnemyTexture3, NULL, &e3);
                    e3.x = rectX - 35;
                    e3.y = rectY - 4;
                    f[2] = true;
                    f1[2] = true;
                    follow[2] = false;
                    clearQueue(path[2]);
                }
            }
        }
        if (follow[3])
        {
            etime[3] = SDL_GetTicks(); 
            if (e4.x <= 350 && f1[3])
            {
                e4.x += 2;
                e4.y = rectY - 4;  
            }
            else
            {
                f1[3] = false;
            }
            if (f[3])
            {
                truetime4 = SDL_GetTicks();  
                f[3] = false;
            }
            if (!f1[3])
            {
                if (etime[3] <= truetime4 + 9000)
                {
                    move(sprite, e4, 3, 2.5);  
                } 
                else
                {
                    move(sprite, e4, 3);
                    move(sprite, e4, 3);
                    e4.x = -_SCREEN_WIDTH3;
                    e4.y = -_SCREEN_HEIGHT3;
                    SDL_RenderCopy(gRenderer3, gEnemyTexture3, NULL, &e4);
                    e4.x = rectX + 48;
                    e4.y = rectY - 4;
                    f[3] = true;
                    f1[3] = true;
                    follow[3] = false;
                    clearQueue(path[3]);
                }
            }
        }

        SDL_Rect enemyRect = {0,0,0,0}; 

        for (int y = 0; y < mazeHeight; ++y) 
        {
            for (int x = 0; x < mazeWidth; ++x) 
            {
                if (maze[y][x] == 1) 
                {
                    renderTexture(gWallTexture, x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE); 
                }
            }
        } 

        for (int y = 0; y < mazeHeight && !found; ++y) 
        {
            for (int x = 0; x < mazeWidth; ++x) 
            {
                if (maze[y][x] == 1) 
                { 
                    enemyRect = {x * TILE_SIZE + 3, y * TILE_SIZE + 4 , TILE_SIZE -7, TILE_SIZE - 11};
                    
                    if (_checkcollision3(visibleSpriteRect, enemyRect))
                    {
                        ismoving[key-1] = false;
                        if (key == 1) sprite.y += 4;
                        else if (key == 2) sprite.y -= 4;
                        else if (key == 3) sprite.x += 4;
                        else if (key == 4) sprite.x -= 4;
                        found = true;
                        ckey = key;
                        break;
                    }   
                } 
            }
        }   

        if (key == 1  && sprite.y  > 0 && ismoving[0]) { sprite.y -= SPEED; ismoving[ckey-1] = true; found = false; }
        else if (key == 2 && sprite.y < _SCREEN_HEIGHT3 - sprite.h && ismoving[1]) { sprite.y += SPEED; ismoving[ckey-1] = true; found = false;}
        else if (key == 3 && sprite.x > 0 && ismoving[2]) { sprite.x -= SPEED; ismoving[ckey-1] = true; found = false;}
        else if (key == 4 && sprite.x < _SCREEN_WIDTH3 - sprite.w && ismoving[3]) { sprite.x += SPEED; ismoving[ckey-1] = true; found = false;}

        SDL_RenderCopy(gRenderer3, bTexture, NULL, &treasure);
        SDL_RenderCopy(gRenderer3, gemTexture, NULL, &ruby);
        SDL_RenderCopy(gRenderer3, gEnemyTexture3, NULL, &e1);
        SDL_RenderCopy(gRenderer3, gEnemyTexture3, NULL, &e2);
        SDL_RenderCopy(gRenderer3, gEnemyTexture3, NULL, &e3);
        SDL_RenderCopy(gRenderer3, gEnemyTexture3, NULL, &e4);
        SDL_RenderCopy(gRenderer3, spriteTexture3, NULL, &sprite);   
        
        int width, height;
        SDL_QueryTexture(timeTexture, NULL, NULL, &width, &height);
        SDL_Rect destRect = {27, 25, width, height};
        SDL_RenderCopy(gRenderer3, timeTexture, NULL, &destRect);
        
        SDL_RenderPresent(gRenderer3);
        
        Uint32 frameTime = SDL_GetTicks() - startTime; 
        if (frameTime < 10 )
        {    
            SDL_Delay((10) - frameTime);
        }  
        startTime = SDL_GetTicks();
    }  
    SDL_RenderClear(gRenderer3);
    
    SDL_DestroyTexture(gWallTexture);
    TTF_CloseFont(font);
    SDL_DestroyTexture(gExplosionTexture3);
    SDL_DestroyTexture(gEnemyTexture3);
    SDL_DestroyTexture(bTexture);
    SDL_DestroyTexture(spriteTexture3);
    if (_quit3)
    {
        _close3();
        sprite = {0,0,0,0};
        return 4;
    }
    else if (win3)
    {
        SDL_RenderClear(gRenderer3);  
        SDL_Rect buttonRect = { _SCREEN_WIDTH3 - 75 , 0 , 75, 45 };
        wintexture3 = _loadTexture3("E:/Mini project/images/winner.png");
        while(!(_quit13))
        {
            while (SDL_PollEvent(&e) != 0) 
            {
                if (e.type == SDL_QUIT) 
                {
                    _quit13 = true;
                } 
                else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    if (_isInsideButton3(buttonRect, mouseX, mouseY)) 
                    {
                        button_clicked = true;
                    }
                }
                else if (e.key.keysym.sym == SDLK_SPACE) 
                {
                    button_clicked = true;
                }
            }
            SDL_SetRenderDrawColor(gRenderer3, 255, 255, 255, 255);
            SDL_RenderClear(gRenderer3);
            SDL_RenderCopy(gRenderer3, wintexture3, NULL, NULL); 
            if (button_clicked) 
            {
                break;
                _quit13 = true;
            } 
            else 
            {
                SDL_SetRenderDrawColor(gRenderer3, 210, 252, 252, 0);
                SDL_RenderFillRect(gRenderer3, &buttonRect);
                _renderbuttontext3("Close", _SCREEN_WIDTH3 - 74 , 0);
            }
            SDL_RenderPresent(gRenderer3);
            
        } 
        _close3();
        sprite = {0,0,0,0};
        return 4;
    }
    else
    {
        SDL_RenderClear(gRenderer3);  
        wintexture3 = _loadTexture3("E:/Mini project/images/nowinner.png"); 
        SDL_Rect buttonRect = { _SCREEN_WIDTH3 - 75 , 0 , 75, 45 }; 
        while(!(_quit13))
        {
            while (SDL_PollEvent(&e) != 0) 
            {
                if (e.type == SDL_QUIT) 
                {
                    _quit13 = true;
                } 
                else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    if (_isInsideButton3(buttonRect, mouseX, mouseY)) 
                    {
                        button_clicked = true;
                    }
                }
                else if (e.key.keysym.sym == SDLK_SPACE) 
                {
                    button_clicked = true;
                }
            }
            SDL_SetRenderDrawColor(gRenderer3, 255, 255, 255, 255);
            SDL_RenderClear(gRenderer3);
            SDL_RenderCopy(gRenderer3, wintexture3, NULL, NULL); 
            if (button_clicked) 
            {
                break;
                _quit13 = true;
            } 
            else 
            {
                SDL_SetRenderDrawColor(gRenderer3, 210, 252, 252, 0);
                SDL_RenderFillRect(gRenderer3, &buttonRect);
                _renderbuttontext3("Close", _SCREEN_WIDTH3 - 74 , 0);
            }
            SDL_RenderPresent(gRenderer3);
            
        } 
        _close3();
        sprite = {0,0,0,0};
        return 4;
    }
    SDL_WaitThread(_musicThread3, NULL);
}
