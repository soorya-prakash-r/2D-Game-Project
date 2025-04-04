#include "inc/SDL2/SDL.h"
#include "inc/SDL2/SDL_image.h"
#include "inc/SDL2/SDL_ttf.h"
#include <iostream>
#include <string>
#include <Windows.h> 
#include "0.h"
#include <cstdlib>
using namespace std;
#undef main

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 600; 

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;
SDL_Texture* gBackgroundTexture = NULL;
SDL_Texture* gBackgroundTexture1 = NULL;
SDL_Texture* gBackgroundTexture2 = NULL;
bool quit1 = false;
bool click = false, _click = false, spacebarpressed = false;

SDL_Thread* musicThread = NULL;

int playMusic(void* filePath) {
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

bool renderbuttontext(const string& text, int x, int y) 
{
    SDL_Color textColor = { 0, 0, 0 };
    TTF_Font* gfont = TTF_OpenFont("E:/Mini project/font/danfo.ttf", 24);
    SDL_Surface* textSurface = TTF_RenderText_Solid(gfont, text.c_str(), textColor);
    if (textSurface == NULL) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (textTexture == NULL) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_Rect renderQuad = { x, y, textWidth, textHeight };
    
    SDL_RenderCopy(gRenderer, textTexture, NULL, &renderQuad);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    return true;
}


bool init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    gWindow = SDL_CreateWindow("Damszt - Planet X Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == NULL) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    gFont = TTF_OpenFont("E:/Mini project/font/pixel.ttf", 28);
    if (gFont == NULL) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}

SDL_Texture* loadTexture(const std::string& path) {
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
    } else {
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL) {
            std::cerr << "Unable to create texture from " << path << "! SDL_Error: " << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

void close() 
{ 
    if (gBackgroundTexture != NULL) {
        SDL_DestroyTexture(gBackgroundTexture);
        gBackgroundTexture = NULL;
    }

    if (gBackgroundTexture1 != NULL) {
        SDL_DestroyTexture(gBackgroundTexture1);
        gBackgroundTexture1 = NULL;
    }

    if (gBackgroundTexture2 != NULL) {
        SDL_DestroyTexture(gBackgroundTexture2);
        gBackgroundTexture2 = NULL;
    } 
    if (gRenderer != NULL) {
        SDL_DestroyRenderer(gRenderer);
        gRenderer = NULL;
    } 
    if (gWindow != NULL) {
        SDL_DestroyWindow(gWindow);
        gWindow = NULL;
    }

    TTF_CloseFont(gFont);
    gFont = NULL;    
    quit1 = false;
    click = false;
    spacebarpressed = false;

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

bool renderText(const std::string& text, int x, int y) {
    SDL_Color textColor = { 0, 0, 0 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, text.c_str(), textColor);
    if (textSurface == NULL) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (textTexture == NULL) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_Rect renderQuad = { x, y, textWidth, textHeight };
    
    SDL_RenderCopy(gRenderer, textTexture, NULL, &renderQuad);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    return true;
}

bool isInsideButton(const SDL_Rect& buttonRect, int mouseX, int mouseY) {
    return (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
            mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h);
} 

int menu()
{  
    if (!init()) {
        std::cerr << "Failed to initialize!" << std::endl;
        return 1;
    }
    SDL_Surface* iconSurface = IMG_Load("E:/Mini project/images/icon.jpg");
    if (iconSurface == nullptr) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        IMG_Quit();
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        return 1;
    }
 
    SDL_SetWindowIcon(gWindow, iconSurface);  
    SDL_FreeSurface(iconSurface);

    gBackgroundTexture = loadTexture("E:/Mini project/images/new.jpg");     
    if (gBackgroundTexture == NULL) {
        std::cerr << "Failed to load background texture!" << std::endl;
        return 1;
    }

    // Start playing background music in a separate thread
    musicThread = SDL_CreateThread(playMusic, "MusicThread", (void*)"0.wav");

    bool quit = false;
    SDL_Event e1;

    SDL_Rect buttonRect = { SCREEN_WIDTH / 2 - 77, 390, 140, 45 };
    SDL_Rect buttonRect1 = { SCREEN_WIDTH / 2 - 110, 450, 200, 45 };
    SDL_Rect buttonRect2 = { SCREEN_WIDTH / 2 - 110 + 65, 510, 75, 45 };

    bool buttonClicked0 = false;
    bool buttonClicked1 = false;
    bool buttonClicked2 = false;
    int temp = 0;

    while (!quit) 
    {
        while (SDL_PollEvent(&e1) != 0) {
            if (e1.type == SDL_QUIT) 
            { 
                quit = true; 
                temp = 2;
            }
             else if (e1.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                if (isInsideButton(buttonRect, mouseX, mouseY)) 
                {
                    buttonClicked0 = true;
                }
                else if (isInsideButton(buttonRect1, mouseX, mouseY)) 
                {
                    buttonClicked1 = true;
                }
                else if (isInsideButton(buttonRect2, mouseX, mouseY)) 
                {
                    buttonClicked2 = true;
                }
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
        SDL_RenderClear(gRenderer);

        SDL_RenderCopy(gRenderer, gBackgroundTexture, NULL, NULL);     
        
        if (buttonClicked0) 
        {
            break; 
        } 
        else if (buttonClicked1)
        {
            temp += 1;
            break; 
        }
        else if (buttonClicked2)
        {
            temp += 2;
            break; 
        }
        else 
        {
            SDL_SetRenderDrawColor(gRenderer, 210, 252, 252, 0);
            SDL_RenderFillRect(gRenderer, &buttonRect);
            renderText("Play Game", SCREEN_WIDTH / 2 - 72 , 390);
            SDL_RenderFillRect(gRenderer, &buttonRect1);
            renderText("Instructions", SCREEN_WIDTH / 2 - 100 , 450);
            SDL_RenderFillRect(gRenderer, &buttonRect2);
            renderText("Exit", SCREEN_WIDTH / 2 -100 + 65 , 510);
        }

        SDL_RenderPresent(gRenderer);
    }

    if (temp == 2)   
    {
        close();
        return 8;
    }
    else if (temp == 1)   
    {
        close(); 
        return 7;
    }
    else
    {
        gBackgroundTexture1 = loadTexture("E:/Mini project/images/intro.png");  
        gBackgroundTexture2 = loadTexture("E:/Mini project/images/level1.png");   
        SDL_RenderClear(gRenderer);  
        SDL_Rect buttonRectx = { SCREEN_WIDTH - 70 , 0 , 75, 45 }; 
        while(!quit1)
        {
            while (SDL_PollEvent(&e1) != 0) 
            {
                if (e1.type == SDL_QUIT) 
                {
                    quit1 = true;
                } 
                else if (e1.type == SDL_KEYDOWN) 
                {
                    if (e1.key.keysym.sym == SDLK_SPACE) 
                    {
                        spacebarpressed = true;
                    }
                } 
                else if (e1.type == SDL_KEYUP) 
                {
                    if (e1.key.keysym.sym == SDLK_SPACE && spacebarpressed) 
                    { 
                        click = true;
                        spacebarpressed = false;
                    }
                }
                else if (e1.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    if (isInsideButton(buttonRectx, mouseX, mouseY)) 
                    {
                        click = true;
                    }
                } 
            }
            SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
            SDL_RenderClear(gRenderer);
            SDL_RenderCopy(gRenderer, gBackgroundTexture1, NULL, NULL); 
            if (click) 
            {
                break;
                quit1 = true;
            } 
            else 
            {
                SDL_SetRenderDrawColor(gRenderer, 210, 252, 252, 0);
                SDL_RenderFillRect(gRenderer, &buttonRectx);
                renderbuttontext("Done", SCREEN_WIDTH - 66 , 0);
            }

            SDL_RenderPresent(gRenderer);
            
        }
        _click = false;
        SDL_RenderClear(gRenderer);
        while(!quit1)
        {
            while (SDL_PollEvent(&e1) != 0) 
            {
                if (e1.type == SDL_QUIT) 
                {
                    quit1 = true;
                } 
                else if (e1.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    if (isInsideButton(buttonRectx, mouseX, mouseY)) 
                    {
                        _click = true;
                    }
                }
                else if (e1.type == SDL_KEYDOWN) 
                {
                    if (e1.key.keysym.sym == SDLK_SPACE) 
                    {
                        spacebarpressed = true;
                    }
                } 
                else if (e1.type == SDL_KEYUP) 
                {
                    if (e1.key.keysym.sym == SDLK_SPACE && spacebarpressed) 
                    { 
                        _click = true;
                        spacebarpressed = false;
                    }
                }
            }
            SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
            SDL_RenderClear(gRenderer);
            SDL_RenderCopy(gRenderer, gBackgroundTexture2, NULL, NULL); 
            if (_click) 
            {
                break;
                quit1 = true;
            } 
            else 
            {
                SDL_SetRenderDrawColor(gRenderer, 210, 252, 252, 0);
                SDL_RenderFillRect(gRenderer, &buttonRectx);
                renderbuttontext("Done", SCREEN_WIDTH - 66 , 0);
            }

            SDL_RenderPresent(gRenderer);
            
        }
        close();
        return 6;
    }

    SDL_WaitThread(musicThread, NULL);
}
