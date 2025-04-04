#include "inc/SDL2/SDL.h"
#include "inc/SDL2/SDL_image.h"
#include "inc/SDL2/SDL_ttf.h"
#include <iostream>
#include <Windows.h>
#include <string>
#include "2.h"
#include <cstdlib>
using namespace std;
#undef main

const int SCREEN_WIDTH1 = 500;
const int SCREEN_HEIGHT1 = 600; 
SDL_Window* gWindow1 = NULL;
SDL_Renderer* gRenderer1 = NULL;
TTF_Font* gFont1 = NULL;
SDL_Texture* BackgroundTexture1 = NULL;
SDL_Thread* musicThread1 = NULL; 

int playMusic1(void* filePath) {
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
    SDL_PauseAudioDevice(audioDevice, 0); // Start audio playback

    while (SDL_GetQueuedAudioSize(audioDevice) > 0) {
        SDL_Delay(100); 
    }

    SDL_CloseAudioDevice(audioDevice);
    SDL_FreeWAV(wavStart);
    return 0;
}

bool init1() 
{
if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() 
<< std::endl;
return false;
}
if (TTF_Init() == -1) {
std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " <<
TTF_GetError() << std::endl;
return false;
}
gWindow1 = SDL_CreateWindow("Damszt - Planet X Game", 
SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH1, SCREEN_HEIGHT1, 
SDL_WINDOW_SHOWN);
if (gWindow1 == NULL) {
std::cerr << "Window could not be created! SDL_Error: " <<
SDL_GetError() << std::endl;
return false;
}
gRenderer1 = SDL_CreateRenderer(gWindow1, -1, SDL_RENDERER_ACCELERATED);
if (gRenderer1 == NULL) {
std::cerr << "Renderer could not be created! SDL_Error: " <<
SDL_GetError() << std::endl;
return false;
}
int imgFlags = IMG_INIT_PNG;
if (!(IMG_Init(imgFlags) & imgFlags)) {
std::cerr << "SDL_image could not initialize! SDL_image Error: " <<
IMG_GetError() << std::endl;
return false;
}
gFont1 = TTF_OpenFont("E:/Mini project/font/pixel.ttf", 28);
if (gFont1 == NULL) {
std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() 
<< std::endl;
return false;
}
return true;
}
SDL_Texture* loadTexture1(const std::string& path) {
SDL_Texture* newTexture = NULL;
SDL_Surface* loadedSurface = IMG_Load(path.c_str());
if (loadedSurface == NULL) {
std::cerr << "Unable to load image " << path << "! SDL_image Error: "
<< IMG_GetError() << std::endl;
} else {
newTexture = SDL_CreateTextureFromSurface(gRenderer1, loadedSurface);
if (newTexture == NULL) {
std::cerr << "Unable to create texture from " << path << "!SDL_Error: " << SDL_GetError() << std::endl;
}
SDL_FreeSurface(loadedSurface);
}
return newTexture;
}
void close1() {
SDL_DestroyTexture(BackgroundTexture1);
BackgroundTexture1 = NULL;
SDL_DestroyRenderer(gRenderer1);
SDL_DestroyWindow(gWindow1);
gRenderer1 = NULL;
gWindow1 = NULL;
TTF_CloseFont(gFont1);
gFont1 = NULL;
TTF_Quit();
IMG_Quit();
SDL_Quit();
}
bool renderText1(const std::string& text, int x, int y) {
SDL_Color textColor = { 0, 0, 0 };
SDL_Surface* textSurface = TTF_RenderText_Solid(gFont1, text.c_str(), 
textColor);
if (textSurface == NULL) {
std::cerr << "Unable to render text surface! SDL_ttf Error: " <<
TTF_GetError() << std::endl;
return false;
}
SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer1, textSurface);
if (textTexture == NULL) {
std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
return false;
}
int textWidth = textSurface->w;
int textHeight = textSurface->h;
SDL_Rect renderQuad = { x, y, textWidth, textHeight };
SDL_RenderCopy(gRenderer1, textTexture, NULL, &renderQuad);
SDL_FreeSurface(textSurface);
SDL_DestroyTexture(textTexture);
return true;
}
bool isInsideButton1(const SDL_Rect& buttonRect, int mouseX, int mouseY) {
return (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h);
}
 
int instructions()
{ 
    if (!init1()) {
    std::cerr << "Failed to initialize!" << std::endl;
    return 1;
    }
    SDL_Surface* iconSurface = IMG_Load("E:/Mini project/images/icon.jpg");
    if (iconSurface == nullptr) {
    std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
    IMG_Quit();
    SDL_DestroyWindow(gWindow1);
    SDL_Quit();
    return 1;
    }
    SDL_SetWindowIcon(gWindow1, iconSurface); 
    SDL_FreeSurface(iconSurface);
    BackgroundTexture1 = loadTexture1("E:/Mini project/images/new1.jpg");
    if (BackgroundTexture1 == NULL) {
    std::cerr << "Failed to load background texture!" << std::endl;
    return 1;
    }
    musicThread1 = SDL_CreateThread(playMusic1, "MusicThread", (void*)"S3.wav");
    bool quit = false;
    SDL_Event e;
    SDL_Rect buttonRect = { SCREEN_WIDTH1 / 2 - 110 + 65, 510, 75, 45 };
    bool button_Clicked = false;
    int temp = 0;
    while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) 
    {
    quit = true;
    } 
    else if (e.type == SDL_MOUSEBUTTONDOWN) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    if (isInsideButton1(buttonRect, mouseX, mouseY)) 
    {
    button_Clicked = true;
    }
    }
    }
    SDL_SetRenderDrawColor(gRenderer1, 255, 255, 255, 255);
    SDL_RenderClear(gRenderer1);
    SDL_RenderCopy(gRenderer1, BackgroundTexture1, NULL, NULL);
    if (button_Clicked) 
    {
    quit = true;
    break;
    } 
    else
    {
    SDL_SetRenderDrawColor(gRenderer1, 210, 252, 252, 0);
    SDL_RenderFillRect(gRenderer1, &buttonRect);
    renderText1("Back", SCREEN_WIDTH1 / 2 -100 + 60 , 510);
    }
    SDL_RenderPresent(gRenderer1);
    } 
    close1(); 
    SDL_WaitThread(musicThread1, NULL);
    return 4;
}