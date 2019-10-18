#include "common.h"
#include "cmath"
#include "OpenSimplexNoise.h"

bool Init();
void CleanUp();
void Run();
void DrawGrid();
void GenLife();
void Rule();
void DrawLife();
void GenCase(int &num);
void Glider(int posx, int posy);
void LWSS(int posx, int posy);


SDL_Window *window;
SDL_GLContext glContext;
SDL_Surface *gScreenSurface = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Rect pos;

int screenWidth = 500;
int screenHeight = 500;
int gridSize = 5;
double genChance = .3;
bool space = false;
int num = 0;
const double PI = 3.1415926;
OpenSimplexNoise *noise1 = nullptr;
double featureSize = 10;

vector<vector<int>> life;

bool Init()
{
    if (SDL_Init(SDL_INIT_NOPARACHUTE & SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialised");
    }

    //Create Window Instance
    window = SDL_CreateWindow(
        "Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,   
        SDL_WINDOW_OPENGL);

    //Check that the window was succesfully created
    if (window == NULL)
    {
        //Print error, if null
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }
    else{
        gScreenSurface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Log("Window Successful Generated");
    }
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);

    return true;
}

int main()
{
    //Error Checking/Initialisation
    if (!Init())
    {
        printf("Failed to Initialize");
        return -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Swap Render Buffers
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Run();

    CleanUp();
    return 0;
}

void CleanUp()
{
    //Free up resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Run()
{
    bool gameLoop = true;
    srand(time(NULL));
    long rand1 = rand() * (RAND_MAX + 1) + rand();
    noise1 = new OpenSimplexNoise{rand1};

    GenLife();
    
    while (gameLoop)
    {   
        if(space){
            num++;
            GenCase(num);
            space = false;
        }
        if(gridSize > 2)
            DrawGrid();
        DrawLife();
        Rule();
        
        SDL_RenderPresent(renderer);
        pos.x = 0;
        pos.y = 0;
        pos.w = screenWidth;
        pos.h = screenHeight;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &pos);
        
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameLoop = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        gameLoop = false;
                        break;
                    default:
                        break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym){
                    case SDLK_SPACE:
                        space = true;
                    default:
                        break;
                }
            }
        }
    }
}

void GenLife(){
    life.clear();
    vector<int> temp;
    for(int x = 0; x < screenWidth / gridSize; x++){
        for(int y = 0; y < screenHeight / gridSize; y++){
            double spot = ((*noise1).eval(x/featureSize, y/featureSize) + 1)/2;
            double num = static_cast<double>(rand()) / RAND_MAX;
            if(spot < genChance){
                if(num < genChance * 2)
                    temp.push_back(1);
                else
                    temp.push_back(0);
            }
            else if(num < genChance)
                    temp.push_back(1);
            else
                temp.push_back(0);
        }
        life.push_back(temp);
        temp.clear();
    }
}

void GenCase(int &num){
    life.clear();
    if(num == 0){
        GenLife();
    }
    else if(num == 1){
        for(int x = 0; x < screenWidth / gridSize; x++){
            vector<int> temp;
            for(int y = 0; y < screenHeight / gridSize; y++){
                temp.push_back(0);
            }
            life.push_back(temp);
            temp.clear();
        }
        int posx = (screenHeight / gridSize)/2;
        int posy = (screenWidth / gridSize)/2;
        Glider(posx, posy);
    }
    else if(num == 2){
        for(int x = 0; x < screenWidth / gridSize; x++){
            vector<int> temp;
            for(int y = 0; y < screenHeight / gridSize; y++){
                temp.push_back(0);
            }
            life.push_back(temp);
            temp.clear();
        }
        int posx = (screenHeight / gridSize)/2;
        int posy = (screenWidth / gridSize)/2;
        LWSS(posx, posy);
    }
    else{
        num = 0;
        GenLife();
    }
}

void Glider(int posx, int posy){
    life[posx][posy] = 0;
    life[posx + 1][posy] = 0;
    life[posx + 2][posy] = 1;
    life[posx][posy + 1] = 1;
    life[posx + 1][posy + 1] = 0;
    life[posx + 2][posy + 1] = 1;
    life[posx][posy + 2] = 0;
    life[posx + 1][posy + 2] = 1;
    life[posx + 2][posy + 2] = 1;
}

void LWSS(int posx, int posy){
    life[posx][posy] = 1;
    life[posx + 1][posy] = 0;
    life[posx + 2][posy] = 0;
    life[posx + 3][posy] = 1;
    life[posx + 4][posy] = 0;
    life[posx][posy + 1] = 0;
    life[posx + 1][posy + 1] = 0;
    life[posx + 2][posy + 1] = 0;
    life[posx + 3][posy + 1] = 0;
    life[posx + 4][posy + 1] = 1;
    life[posx][posy + 2] = 1;
    life[posx + 1][posy + 2] = 0;
    life[posx + 2][posy + 2] = 0;
    life[posx + 3][posy + 2] = 0;
    life[posx + 4][posy + 2] = 1;
    life[posx][posy + 3] = 0;
    life[posx + 1][posy + 3] = 1;
    life[posx + 2][posy + 3] = 1;
    life[posx + 3][posy + 3] = 1;
    life[posx + 4][posy + 3] = 1;
}

void Rule(){
    vector<vector<int>> newLife;
    vector<vector<int>> neighbors;
    vector<int> temp1;
    for(int x = 0; x < life.size(); x++){
        for(int y = 0; y < life[0].size(); y++){
            vector<int> temp2;
            int posx;
            int posy;
            for(int i = -1; i < 2; i++){
                for(int j = -1; j < 2; j++){
                    if(i == 0 && j == 0){}
                    else{
                        if(x + i < 0)
                            posx = life.size() - 1;
                        else if(x + i > life.size() - 1)
                            posx = 0;
                        else
                            posx = x + i;
                        if(y + j < 0)
                            posy = life[0].size() - 1;
                        else if(y + j > life[0].size() - 1)
                            posy = 0;
                        else
                            posy = y + j;
                        temp2.push_back(posx);
                        temp2.push_back(posy);
                        neighbors.push_back(temp2);
                        temp2.clear();
                    }
                }
            }
            int numNeighbors = 0;
            for(int i = 0; i < neighbors.size(); i++){
                posx = neighbors[i][0];
                posy = neighbors[i][1];
                if(life[posx][posy] == 1)
                    numNeighbors++;
            }

            if(numNeighbors < 2 && life[x][y] == 1)
                temp1.push_back(0);
            else if(numNeighbors > 3 && life[x][y] == 1)
                temp1.push_back(0);
            else if(numNeighbors == 3 && life[x][y] == 0)
                temp1.push_back(1);
            else
                temp1.push_back(life[x][y]);
            
            neighbors.clear();
        }
        newLife.push_back(temp1);
        temp1.clear();
    }
    life = newLife;
    newLife.clear();
}

void DrawLife(){
    for(int x = 0; x < life.size(); x++){
        for(int y = 0; y < life[0].size(); y++){
            if(life[x][y] == 1){
                pos.x = x * gridSize;
                pos.y = y * gridSize;
                pos.w = gridSize;
                pos.h = gridSize;
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &pos);
            }
        }
    }
}

void DrawGrid(){
    for(int x = 0; x <= screenWidth; x+=gridSize){
        pos.x = x;
        pos.y = 0;
        pos.w = 1;
        pos.h = screenHeight;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &pos);
    }
    for(int y = 0; y <= screenHeight; y+=gridSize){
        pos.x = 0;
        pos.y = y;
        pos.w = screenWidth;
        pos.h = 1;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &pos);
    }
}