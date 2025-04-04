    #include "inc/SDL2/SDL.h"
    #include "inc/SDL2/SDL_ttf.h"
    #include "inc/SDL2/SDL_image.h"
    #include <iostream>
    #include <vector>
    #include <string>
    #include <cmath>
    #include <Windows.h>
    #include <chrono>
    #include "1.h"
    #include <cstdlib>
    using namespace std;
    using namespace std::chrono;
    #undef main
 
    const int _SCREEN_WIDTH1 = 500;
    const int _SCREEN_HEIGHT1 = 600;
    const int SPRITE_SPEED = 25;
    const double BACKGROUND_SCROLL_SPEED = 2.8;
    int BACKGROUND_CHANGE_INTERVAL = 4700;
    const int FADE_DURATION = 1700; 
    const double ENEMY_SPEED = 5.5;
    int ENEMY_SPAWN_INTERVAL = 500;     
    bool fading = false;
    const int ENEMY_WIDTH = 50;
    const int ENEMY_HEIGHT = 70;
    int wintime = 0;
    SDL_Window* _gWindow1 = NULL;
    TTF_Font* font = NULL, *gfont1 = NULL;
    SDL_Renderer* _gRenderer1 = NULL;
    SDL_Texture* timeTexture = NULL;
    std::vector<SDL_Texture*> gBackgroundTextures;
    SDL_Texture* gSpriteTexture = NULL;
    SDL_Texture* gEnemyTexture = NULL;
    SDL_Texture* gEnemyTexture1 = NULL; 
    SDL_Texture* gEnemyTexture2 = NULL;
    SDL_Texture* enemyTexture = NULL;
    SDL_Texture* gExplosionTexture = NULL; 
    SDL_Texture* wintexture = NULL;    
    Uint32 fadeStartTime = 0;
    Uint32 lastEnemySpawnTime = 0;
    SDL_Thread* _musicThread1 = NULL;
    bool gameOver = false;    
    bool buttonClicked = false;
    bool _quit1 = false, _quit0 = false; 
    int currentBackgroundIndex = 0;
    bool win = false;
    class Enemy
    {
        public:
        int x, y;
        SDL_Texture* texture;
        Enemy(int xPos, int yPos, SDL_Texture* tex) : x(xPos), y(yPos), 
        texture(tex) {}
    };
    vector<Enemy> enemies;
    void playCollisionSound()
    { 
        PlaySound(TEXT("del.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
    
    bool _isInsideButton1(const SDL_Rect& buttonRect, int mouseX, int mouseY) 
    {
        return (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
        mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h);
    }
    int _playMusic1(void* filePath) 
    {
        const char* musicFilePath = static_cast<const char*>(filePath);
        SDL_AudioSpec wavSpec;
        Uint8* wavStart;
        Uint32 wavLength;
        if (SDL_LoadWAV(musicFilePath, &wavSpec, &wavStart, &wavLength) == NULL) {
        std::cerr << "Failed to load audio file: " << SDL_GetError() <<
        std::endl;
        return 1;
        }
        SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
        if (audioDevice == 0) 
        {
        std::cerr << "Failed to open audio device: " << SDL_GetError() <<
        std::endl;
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
    SDL_Texture* _renderText1(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) {
    cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << endl;
    return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
    cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() <<
    endl;
    }
    return texture;
    }
    bool _init1() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) 
    {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() 
    << std::endl;
    return false;
    } 
    _gWindow1 = SDL_CreateWindow("Damszt - Planet X Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _SCREEN_WIDTH1, _SCREEN_HEIGHT1, SDL_WINDOW_SHOWN);
    if (_gWindow1 == NULL) {
    std::cerr << "Window could not be created! SDL_Error: " <<
    SDL_GetError() << std::endl;
    return false;
    }
    _gRenderer1 = SDL_CreateRenderer(_gWindow1, -1, SDL_RENDERER_ACCELERATED);
    if (_gRenderer1 == NULL) {
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
    return true;
    }
    SDL_Texture* _loadTexture1(const std::string& path) {
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
    std::cerr << "Unable to load image " << path << "! SDL_image Error: "
    << IMG_GetError() << std::endl;
    } else {
    newTexture = SDL_CreateTextureFromSurface(_gRenderer1, loadedSurface);
    if (newTexture == NULL) {
    std::cerr << "Unable to create texture from " << path << "! SDL_Error: " << SDL_GetError() << std::endl;
    }
    SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
    }
    bool _renderbuttontext1(const string& text, int x, int y) 
    {
    SDL_Color textColor = { 0, 0, 0 };
    gfont1 = TTF_OpenFont("E:/Mini project/font/danfo.ttf", 24);
    SDL_Surface* textSurface = TTF_RenderText_Solid(gfont1, text.c_str(), textColor);
    if (textSurface == NULL) {
    std::cerr << "Unable to render text surface! SDL_ttf Error: " <<
    TTF_GetError() << std::endl;
    return false;
    }
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(_gRenderer1, 
    textSurface);
    if (textTexture == NULL) {
    std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
    return false;
    }
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_Rect renderQuad = { x, y, textWidth, textHeight };
    SDL_RenderCopy(_gRenderer1, textTexture, NULL, &renderQuad);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    return true;
    }
    void _close1() 
{ 
     
    for (size_t i = 0; i < gBackgroundTextures.size(); ++i) 
    {
        if (gBackgroundTextures[i] != NULL) {
            SDL_DestroyTexture(gBackgroundTextures[i]);
            gBackgroundTextures[i] = NULL;
        }
    }
    gBackgroundTextures.clear(); 
    if (gSpriteTexture != NULL) {
        SDL_DestroyTexture(gSpriteTexture);
        gSpriteTexture = NULL;
    }

    if (gEnemyTexture != NULL) {
        SDL_DestroyTexture(gEnemyTexture);
        gEnemyTexture = NULL;
    }

    if (gEnemyTexture1 != NULL) {
        SDL_DestroyTexture(gEnemyTexture1);
        gEnemyTexture1 = NULL;
    }

    if (gEnemyTexture2 != NULL) {
        SDL_DestroyTexture(gEnemyTexture2);
        gEnemyTexture2 = NULL;
    }

    if (gExplosionTexture != NULL) {
        SDL_DestroyTexture(gExplosionTexture);
        gExplosionTexture = NULL;
    }

    if (timeTexture != NULL) {
        SDL_DestroyTexture(timeTexture);
        timeTexture = NULL;
    }

    if (enemyTexture != NULL) {
        SDL_DestroyTexture(enemyTexture);
        enemyTexture = NULL;
    }
 
    enemies.clear();
    enemies.shrink_to_fit();
 
    if (_gRenderer1 != NULL) {
        SDL_DestroyRenderer(_gRenderer1);
        _gRenderer1 = NULL;
    }

    if (_gWindow1 != NULL) {
        SDL_DestroyWindow(_gWindow1);
        _gWindow1 = NULL;
    }
 
    if (font != NULL) {
        TTF_CloseFont(font);
        font = NULL;
    }

    if (gfont1 != NULL) {
        TTF_CloseFont(gfont1);
        gfont1 = NULL;
    }
 
    fadeStartTime = 0;
    BACKGROUND_CHANGE_INTERVAL = 4700;
    buttonClicked = false;
    ENEMY_SPAWN_INTERVAL = 500;
    gameOver = false;
    _quit1 = false;
    wintime = 0;
    fading = false;
    currentBackgroundIndex = 0;
    _quit0 = false;
    win = false; 
    TTF_Quit();
    IMG_Quit();
    SDL_Quit(); 
}

    
    bool checkCollision(SDL_Rect a, SDL_Rect b) 
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
int level1() 
{  
    if (!(_init1())) {
    std::cerr << "Failed to initialize!" << std::endl;
    return 1;
    }
    SDL_Surface* iconSurface = IMG_Load("E:/Mini project/images/icon.jpg");
    if (iconSurface == nullptr) {
    std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
    IMG_Quit();
    SDL_DestroyWindow(_gWindow1);
    SDL_Quit();
    return 1;
    }
    SDL_SetWindowIcon(_gWindow1, iconSurface); 
    SDL_FreeSurface(iconSurface);
    if (TTF_Init() != 0) {
    cerr << "TTF_Init Error: " << TTF_GetError() << endl;
    SDL_Quit();
    return 1;
    }
    font = TTF_OpenFont("E:/Mini project/font/borel.ttf", 24);
    if (!font) {
    cerr << "TTF_OpenFont Error: " << TTF_GetError() << endl;
    SDL_DestroyRenderer(_gRenderer1);
    SDL_DestroyWindow(_gWindow1);
    SDL_Quit();
    return 1;
    }
    // Load background textures
    gBackgroundTextures.push_back(_loadTexture1("E:/Mini project/images/sky.png"));
    gBackgroundTextures.push_back(_loadTexture1("E:/Mini project/images/space.png"));
    gBackgroundTextures.push_back(_loadTexture1("E:/Mini project/images/facer.png"));
    gBackgroundTextures.push_back(_loadTexture1("E:/Mini project/images/x1.png"));
    gBackgroundTextures.push_back(_loadTexture1("E:/Mini project/images/x2.png"));
    gBackgroundTextures.push_back(_loadTexture1("E:/Mini project/images/x3.png"));
    gBackgroundTextures.push_back(_loadTexture1("E:/Mini project/images/x4.png"));
    for (size_t i = 0; i < gBackgroundTextures.size(); ++i) {
    if (gBackgroundTextures[i] == NULL) {
    std::cerr << "Failed to load one of the background textures!" <<    std::endl;
    return 1;
    }
    }
    gSpriteTexture = _loadTexture1("E:/Mini project/images/rocket.png");
    if (gSpriteTexture == NULL) {
    cout << "Failed to load sprite texture!" << std::endl;
    return 1;
    }
    gEnemyTexture1 = _loadTexture1("E:/Mini project/images/alien1.png"); 
    if (gEnemyTexture1 == NULL) 
    {
    cout << "Failed to load enemy texture 1!" << std::endl;
    return 1;
    }
    gEnemyTexture2 = _loadTexture1("E:/Mini project/images/alien2.png");
    if (gEnemyTexture2 == NULL) {
    cout << "Failed to load enemy texture!" << std::endl;
    return 1;
    }
    gExplosionTexture = _loadTexture1("E:/Mini project/images/blast.png");
    if (gExplosionTexture == NULL) {
    cout << "Failed to load explosion texture!" << std::endl;
    return 1;
    }
    _musicThread1 = SDL_CreateThread(_playMusic1, "MusicThread", (void*)"1.wav");
    SDL_Event e;
    SDL_Rect spriteRect = { _SCREEN_WIDTH1 / 2 - 50, _SCREEN_HEIGHT1 - 100 , 80, 100 };
    double backgroundY1 = 0.0;
    double backgroundY2 = static_cast<double>(-_SCREEN_HEIGHT1);
    Uint32 lastBackgroundChangeTime = SDL_GetTicks();
    int nextBackgroundIndex = (currentBackgroundIndex + 1) % gBackgroundTextures.size();
    lastEnemySpawnTime = SDL_GetTicks();
    auto start = high_resolution_clock::now();
    SDL_Color color = {255, 255, 255, 255};
    Uint32 startTime = SDL_GetTicks();
    while (!(_quit1)) 
    {
        wintime = SDL_GetTicks();
            while (SDL_PollEvent(&e) != 0) 
            {
                if (e.type == SDL_QUIT) 
                { 
                    _quit1 = true; 
                }
            }
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        if (currentKeyStates[SDL_SCANCODE_UP] && spriteRect.y > _SCREEN_HEIGHT1 / 2) 
        {
        spriteRect.y -= SPRITE_SPEED/3;
        }
        if (currentKeyStates[SDL_SCANCODE_DOWN] && spriteRect.y < _SCREEN_HEIGHT1 - spriteRect.h)
        {
        spriteRect.y += SPRITE_SPEED/3;
        }
        if (currentKeyStates[SDL_SCANCODE_LEFT] && spriteRect.x > 0) {
        spriteRect.x -= SPRITE_SPEED/3;
        }
        if (currentKeyStates[SDL_SCANCODE_RIGHT] && spriteRect.x < _SCREEN_WIDTH1 - spriteRect.w) 
        {
        spriteRect.x += SPRITE_SPEED/3;
        } 
        backgroundY1 += BACKGROUND_SCROLL_SPEED;
        backgroundY2 += BACKGROUND_SCROLL_SPEED;
        if (backgroundY1 >= _SCREEN_HEIGHT1) {
        backgroundY1 = backgroundY2 - _SCREEN_HEIGHT1;
        }
        if (backgroundY2 >= _SCREEN_HEIGHT1) {
        backgroundY2 = backgroundY1 - _SCREEN_HEIGHT1;
        }
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastBackgroundChangeTime > BACKGROUND_CHANGE_INTERVAL && !fading) 
        {
        fading = true;
        fadeStartTime = currentTime;
        nextBackgroundIndex = (currentBackgroundIndex + 1) % gBackgroundTextures.size();
        ENEMY_SPAWN_INTERVAL -= 50;
        if (nextBackgroundIndex == 0) 
        {
        nextBackgroundIndex = 5;
        }
        lastBackgroundChangeTime = currentTime;
        }
        if (wintime >= 32000)
        { 
            win = true;
            spriteRect.y -= SPRITE_SPEED / 3;
            if (spriteRect.y + 100 < -150) 
            { 
            SDL_Delay(10);
            gameOver = true;
            break; 
            } 
        }
        if (currentTime - lastEnemySpawnTime > ENEMY_SPAWN_INTERVAL) 
        {
        int enemyX = rand() % (_SCREEN_WIDTH1 - 40); 
        int enemyType = rand()%2;
        if(enemyType == 0)
        { 
        enemyTexture = gEnemyTexture1; 
        enemyType += 1;
        }
        else
        {
        enemyTexture = gEnemyTexture2;
        enemyType -= 1;
        } 
        if (currentBackgroundIndex >= 1)
            enemies.push_back(Enemy(enemyX, -50, enemyTexture));
        lastEnemySpawnTime = currentTime;
        }
        for (Enemy& enemy : enemies) 
        {
            enemy.y += ENEMY_SPEED;
        } 
        for (Enemy& enemy : enemies) 
        {
            SDL_Rect visibleSpriteRect = {spriteRect.x + 30, spriteRect.y + 2,20, 90};
            SDL_Rect visibleSpriteRect1 = {spriteRect.x + 20, spriteRect.y + 60, 40, 15};
            SDL_Rect visibleobsRect = {enemy.x + 12, enemy.y , 45, 45}; 
            if (!win)
            {
                if (checkCollision(visibleobsRect, visibleSpriteRect) || checkCollision(visibleobsRect, visibleSpriteRect1)) 
                { 
                    playCollisionSound();
                    SDL_RenderCopy(_gRenderer1, gExplosionTexture, NULL, &spriteRect);
                    SDL_RenderPresent(_gRenderer1);
                    SDL_Delay(1000); 
                    gameOver = true;
                    visibleobsRect = {0,0,0,0};
                    break;
                }
            }
        }
        if (gameOver || _quit1) 
        {
            break;
        }
        auto now = high_resolution_clock::now();
        auto elapsed = duration_cast<seconds>(now - start).count();
        string timeText = "Time: " + to_string(elapsed) + " sec";
        timeTexture = _renderText1(_gRenderer1, font, timeText,color);
        if (!timeTexture) {
        cerr << "Failed to render text" << endl;
        break;
        }
        SDL_RenderClear(_gRenderer1);
        SDL_Rect backgroundRect1 = { 0, static_cast<int>(backgroundY1), _SCREEN_WIDTH1, _SCREEN_HEIGHT1 };
        SDL_Rect backgroundRect2 = { 0, static_cast<int>(backgroundY2), _SCREEN_WIDTH1, _SCREEN_HEIGHT1 };
        float alpha;
        if (fading) 
        {
        float elapsedTime = (currentTime - fadeStartTime) / (float)FADE_DURATION; 
        if (elapsedTime < 0.998f)
        { 
        alpha = 255 * elapsedTime; 
        SDL_SetTextureAlphaMod(gBackgroundTextures[currentBackgroundIndex], 255 - alpha); 
        SDL_RenderCopy(_gRenderer1, gBackgroundTextures[currentBackgroundIndex], NULL, &backgroundRect1); 
        SDL_RenderCopy(_gRenderer1, gBackgroundTextures[currentBackgroundIndex], NULL, &backgroundRect2); 
        SDL_SetTextureAlphaMod(gBackgroundTextures[nextBackgroundIndex], alpha);
        SDL_SetTextureAlphaMod(gBackgroundTextures[nextBackgroundIndex], alpha);
        SDL_RenderCopy(_gRenderer1, gBackgroundTextures[nextBackgroundIndex], NULL, &backgroundRect1);
        SDL_RenderCopy(_gRenderer1, gBackgroundTextures[nextBackgroundIndex], NULL, &backgroundRect2);
        SDL_SetTextureAlphaMod(gBackgroundTextures[nextBackgroundIndex], 255); 
        }
        else
        {
        alpha = 255 * elapsedTime; 
        SDL_SetTextureAlphaMod(gBackgroundTextures[currentBackgroundIndex], 255 - alpha);
        SDL_RenderCopy(_gRenderer1, gBackgroundTextures[currentBackgroundIndex], NULL, &backgroundRect1); 
        SDL_RenderCopy(_gRenderer1, gBackgroundTextures[currentBackgroundIndex], NULL, &backgroundRect2); 
        SDL_SetTextureAlphaMod(gBackgroundTextures[nextBackgroundIndex], alpha);
        SDL_SetTextureAlphaMod(gBackgroundTextures[nextBackgroundIndex], alpha); 
        SDL_RenderCopy(_gRenderer1, gBackgroundTextures[nextBackgroundIndex], NULL, &backgroundRect1);
        SDL_RenderCopy(_gRenderer1, gBackgroundTextures[nextBackgroundIndex], NULL, &backgroundRect2);
        SDL_SetTextureAlphaMod(gBackgroundTextures[nextBackgroundIndex], 255);
        fading = false;
        currentBackgroundIndex = nextBackgroundIndex;
        } 
        }
        else
        {
        SDL_RenderCopy(_gRenderer1, gBackgroundTextures[currentBackgroundIndex], NULL, &backgroundRect1);
        SDL_RenderCopy(_gRenderer1, gBackgroundTextures[currentBackgroundIndex], NULL, &backgroundRect2);
        }
        SDL_RenderCopy(_gRenderer1, gSpriteTexture, NULL, &spriteRect); 
        for (Enemy& enemy : enemies) 
        {
            SDL_Rect enemyRect = { enemy.x, enemy.y, ENEMY_HEIGHT, ENEMY_WIDTH};
            SDL_RenderCopy(_gRenderer1, enemy.texture, NULL, &enemyRect); 
        } 
        int width, height;
        SDL_QueryTexture(timeTexture, NULL, NULL, &width, &height);
        SDL_Rect destRect = {27, 25, width, height};
        SDL_RenderCopy(_gRenderer1, timeTexture, NULL, &destRect);
        SDL_RenderPresent(_gRenderer1);
        Uint32 frameTime = SDL_GetTicks() - startTime; 
        if (frameTime < 10 )
        { 
        SDL_Delay((10) - frameTime);
        } 
        startTime = SDL_GetTicks();
    }

    SDL_RenderClear(_gRenderer1);
    
    if (_quit1)
    { 
        _close1();  
        return 4;
    }
    else if (gameOver && win) 
    {
        SDL_RenderClear(_gRenderer1); 
        SDL_Rect buttonRect = { _SCREEN_WIDTH1 - 70 , 0 , 75, 45 };
        wintexture = _loadTexture1("E:/Mini project/images/level2.png");
        SDL_Event p;
        while(!(_quit0))
        {
        while (SDL_PollEvent(&p) != 0) 
        {
        if (p.type == SDL_QUIT) 
        {
            _quit0 = true;
        } 
        else if (p.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (_isInsideButton1(buttonRect, mouseX, mouseY)) 
        {
            buttonClicked = true;
        }
        }
        else if (p.key.keysym.sym == SDLK_SPACE) 
        {
            buttonClicked = true;
        }
        }
        SDL_SetRenderDrawColor(_gRenderer1, 255, 255, 255, 255);
        SDL_RenderClear(_gRenderer1);
        SDL_RenderCopy(_gRenderer1, wintexture, NULL, NULL); 
        if (buttonClicked) 
        {
            break;
            _quit0 = true;
        } 
        else
        {
            SDL_SetRenderDrawColor(_gRenderer1, 210, 252, 252, 0);
            SDL_RenderFillRect(_gRenderer1, &buttonRect);
            _renderbuttontext1("Done", _SCREEN_WIDTH1 - 66 , 0);
        }
        SDL_RenderPresent(_gRenderer1);
        }
        _close1(); 
        return 2;
    } 
    else
    {
        SDL_Event t;
        SDL_RenderClear(_gRenderer1); 
        SDL_Rect buttonRect = { _SCREEN_WIDTH1 - 75 , 0 , 75, 45 };
        wintexture = _loadTexture1("E:/Mini project/images/nowinner.png"); 
        while(!(_quit0))
        {
        while (SDL_PollEvent(&t) != 0) 
        {
        if (t.type == SDL_QUIT) 
        {
            _quit0 = true;
        } 
        else if (t.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (_isInsideButton1(buttonRect, mouseX, mouseY)) 
        {
            buttonClicked = true;
        }
        }
        else if (t.key.keysym.sym == SDLK_SPACE) 
        {
            buttonClicked = true;
        }
        }
        SDL_SetRenderDrawColor(_gRenderer1, 255, 255, 255, 255);
        SDL_RenderClear(_gRenderer1);
        SDL_RenderCopy(_gRenderer1, wintexture, NULL, NULL); 
        if (buttonClicked) 
        {
            _quit0 = true;
            break;
        } 
        else
        {
        SDL_SetRenderDrawColor(_gRenderer1, 210, 252, 252, 0);
        SDL_RenderFillRect(_gRenderer1, &buttonRect);
        _renderbuttontext1("Close", _SCREEN_WIDTH1 - 74 , 0);
        }
        SDL_RenderPresent(_gRenderer1);
        }
        _close1(); 
        buttonRect = {0,0,0,0};
        return 4;
    }
    SDL_WaitThread(_musicThread1, NULL); 
}