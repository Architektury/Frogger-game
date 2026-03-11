#include <iostream>
#include <conio.h>
#include <ctime>
#include <fstream>
#include <thread>
#include <chrono>
using namespace std;


#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 20


const string GREEN = "\033[42m";
const string RED = "\033[41m";
const string BLUE = "\033[44m";
const string RESET = "\033[0m";
const string YELLOW = "\033[43m";
const string MAGENTA = "\033[45m";
struct Frog {
    int x, y;
};

struct Car {
    int x, y;
    int speed;
    int direction;
    int frameCounter;
    bool disappearing;
    bool stop;
};

struct Truck {
    int x, y;
    int speed;
    int direction;
    int frameCounter;
};

struct Obstacle {
    int x, y;
};

struct GameSettings {
    int numCars;
    int numTrucks;
    int numObstacles;
};


void setupGameWindow();
void initializeGame(Frog& frog, Car cars[], Truck trucks[], Obstacle obstacles[], GameSettings& settings);
void handleInput(Frog& frog, bool& isRiding);
void updateGame(Car cars[], int numCars, Frog& frog, Truck trucks[], int numTrucks, Obstacle obstacles[], int numObstacles, bool& gameOver, bool& isRiding, int& ridingTruckIndex);
void drawGameState(const Frog& frog, const Car cars[], int numCars, const Truck trucks[], int numTrucks, const Obstacle obstacles[], int numObstacles, int elapsedTime, int score);
bool checkWin(const Frog& frog);
void endGameMessage(bool win, int score);
void initializeFrog(Frog& frog);
void initializeCars(Car cars[], int numCars);
void initializeTrucks(Truck trucks[], int numTrucks);
void initializeObstacle(Obstacle obstacles[], int numObstacles);
void drawGame(const Frog& frog, const Car cars[], int numCars, const Truck trucks[], int numTrucks, const Obstacle obstacles[], int numObstacles);
void updateCars(Car cars[], int numCars, const Frog& frog, bool& gameOver);
void updateTrucks(Truck trucks[], int numTrucks);
int getRidingTruckIndex(const Frog& frog, const Truck trucks[], int numTrucks);
void RideTruck(Frog& frog, const Truck& truck);

bool checkCollisionObstacle(const Frog& frog, const Obstacle obstacles[], int numObstacles);
void setCursorPosition(int x, int y);
void clearScreen();

int main() {
    Frog frog;
    Car cars[20];
    Truck trucks[10];
    Obstacle obstacles[10];
    GameSettings settings = { 10, 5, 10 
};
    bool isRiding = false, gameOver = false;
    int ridingTruckIndex = -1, timeLimit = 30, elapsedTime = 0, score = 2000;

    initializeGame(frog, cars, trucks, obstacles, settings);

    cout << "Press any key to start!\n";
    _getch();

    auto startTime = chrono::steady_clock::now();

    while (!gameOver) {
        setupGameWindow();

        auto now = chrono::steady_clock::now();
        elapsedTime = chrono::duration_cast<chrono::seconds>(now - startTime).count();
        score = max(0, 2000 - elapsedTime * 50);

        drawGameState(frog, cars, settings.numCars, trucks, settings.numTrucks, obstacles, settings.numObstacles, elapsedTime, score);

        if (elapsedTime >= timeLimit) {
            gameOver = true;
            break;
        
}

        handleInput(frog, isRiding);
        updateGame(cars, settings.numCars, frog, trucks, settings.numTrucks, obstacles, settings.numObstacles, gameOver, isRiding, ridingTruckIndex);

        if (checkWin(frog)) {
            endGameMessage(true, score);
            return 0;
        
}

        this_thread::sleep_for(chrono::milliseconds(100));
    
}

    endGameMessage(false, score);
    return 0;
}

void initializeGame(Frog& frog, Car cars[], Truck trucks[], Obstacle obstacles[], GameSettings& settings) {
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, "forg.txt", "r");

    if (err != 0 || !file) {
        cerr << "Could not open forg.txt. Using default settings.\n";
    
}
    else {

        int readCars = 0, readTrucks = 0;
        int readCount = fscanf_s(file, "%d %d", &readCars, &readTrucks);

        if (readCount == 2 && readCars > 0 && readTrucks > 0) {

            settings.numCars = readCars;
            settings.numTrucks = readTrucks;
            cout << "Settings loaded from file: " << settings.numCars << " cars, " << settings.numTrucks << " trucks.\n";
        
}

        fclose(file);
    
}

    srand(static_cast<unsigned int>(time(0)));
    initializeFrog(frog);
    initializeCars(cars, settings.numCars);
    initializeTrucks(trucks, settings.numTrucks);
    initializeObstacle(obstacles, settings.numObstacles);
}

void handleInput(Frog& frog, bool& isRiding) {
    if (_kbhit()) {
        char input = _getch();
        if (input == 'w') frog.y--;
        if (input == 's') frog.y++;
        if (input == 'a') frog.x--;
        if (input == 'd') frog.x++;
        isRiding = false;
    
}
}

void updateGame(Car cars[], int numCars, Frog& frog, Truck trucks[], int numTrucks, Obstacle obstacles[], int numObstacles, bool& gameOver, bool& isRiding, int& ridingTruckIndex) {
    updateCars(cars, numCars, frog, gameOver);
    updateTrucks(trucks, numTrucks);

    if (checkCollisionObstacle(frog, obstacles, numObstacles)) frog.y++;

    int truckIndex = getRidingTruckIndex(frog, trucks, numTrucks);
    if (!isRiding && truckIndex != -1) {
        isRiding = true;
        ridingTruckIndex = truckIndex;
    
}

    if (isRiding && ridingTruckIndex != -1) RideTruck(frog, trucks[ridingTruckIndex]);
}

void drawGameState(const Frog& frog, const Car cars[], int numCars, const Truck trucks[], int numTrucks, const Obstacle obstacles[], int numObstacles, int elapsedTime, int score) {
    drawGame(frog, cars, numCars, trucks, numTrucks, obstacles, numObstacles);
    setCursorPosition(45, 10);
    cout << "Time: " << elapsedTime << "s / 30s    Score: " << score;
}

void endGameMessage(bool win, int score) {
    setCursorPosition(0, SCREEN_HEIGHT + 2);
    if (win) {
        cout << GREEN << "You won the game!\n" << RESET << "Score: " << score << endl;
    
}
    else {
        cout << RED << "Game Over!\n" << RESET;
    
}
}
void setupGameWindow() {
    clearScreen();
    for (int i = 0; i <= SCREEN_HEIGHT; i++) {
        for (int j = 0; j <= SCREEN_WIDTH; j++) {
            if (!(i % 2) && i != 0 && i != SCREEN_HEIGHT && j != 0 && j != SCREEN_WIDTH)
            {
                cout << YELLOW << "-" << RESET;
            
}
            if (i == 0 && j != 0 && j != SCREEN_WIDTH)
            {
                cout << MAGENTA << "#" << RESET;
            
}
            if (i == SCREEN_HEIGHT || j == 0 || j == SCREEN_WIDTH - 1) {
                cout << "#";
            
}
            if (i % 2) {
                cout << " ";
            
}
        
}
        cout << "\n";
    
}
}

void initializeFrog(Frog& frog) {
    frog.x = SCREEN_WIDTH / 2;
    frog.y = SCREEN_HEIGHT - 2;
}

void initializeCars(Car cars[], int numCars) {
    for (int i = 0; i < numCars; i++) {
        cars[i].x = rand() % SCREEN_WIDTH;
        cars[i].y = (rand() % ((SCREEN_HEIGHT - 4) / 2)) * 2 + 3;
        cars[i].speed = rand() % 3 + 1;
        cars[i].direction = (rand() % 2 == 0) ? 1 : -1;
        cars[i].frameCounter = 0;
        cars[i].disappearing = (rand() % 2 == 0);
        cars[i].stop = (rand() % 2 == 0);
    
}
}

void initializeTrucks(Truck trucks[], int numTrucks) {
    for (int i = 0; i < numTrucks; i++) {
        trucks[i].x = rand() % SCREEN_WIDTH;
        trucks[i].y = (rand() % ((SCREEN_HEIGHT - 4) / 2)) * 2 + 3;
        trucks[i].speed = rand() % 3 + 1;
        trucks[i].direction = (rand() % 2 == 0) ? 1 : -1;
        trucks[i].frameCounter = 0;
    
}
}

void initializeObstacle(Obstacle obstacles[], int numObstacles) {
    for (int i = 0; i < numObstacles; i++) {
        obstacles[i].x = rand() % SCREEN_WIDTH;
        obstacles[i].y = (rand() % ((SCREEN_HEIGHT - 4) / 2)) * 2 + 3;
    
}
}

void drawGame(const Frog& frog, const Car cars[], int numCars, const Truck trucks[], int numTrucks, const Obstacle obstacles[], int numObstacles) {
    setCursorPosition(frog.x, frog.y);
    cout << GREEN << "F" << RESET;

    for (int i = 0; i < numCars; i++) {
        setCursorPosition(cars[i].x, cars[i].y);
        cout << RED << "C" << RESET;
    
}

    for (int i = 0; i < numTrucks; i++) {
        setCursorPosition(trucks[i].x, trucks[i].y);
        cout << BLUE << "<" << RESET;
    
}
    for (int i = 0; i < numObstacles; i++) {
        setCursorPosition(obstacles[i].x, obstacles[i].y);
        cout << GREEN << "#" << RESET;
    
}
}

void updateCars(Car cars[], int numCars, const Frog& frog, bool& gameOver) {
    for (int i = 0; i < numCars; i++) {

        if (cars[i].x == -2) {
            cars[i].frameCounter = cars[i].frameCounter - 4;
            if (cars[i].frameCounter <= 0) {

                cars[i].x = 0;
                cars[i].speed = rand() % 3 + 1;
                cars[i].direction = (rand() % 2 == 0) ? 1 : -1;
            
}
            continue;
        
}




        int proximityRange = 2;
        if (cars[i].stop)
        {
            bool closeToFrog = (abs(cars[i].x - frog.x) <= proximityRange && frog.y == cars[i].y);
            if (closeToFrog) {
                continue;
            
}
        
}



        if (rand() % 50 == 0) {
            cars[i].speed = rand() % 3 + 1;
        
}


        cars[i].frameCounter++;
        if (cars[i].frameCounter >= 4 - cars[i].speed) {
            cars[i].frameCounter = 0;
            cars[i].x += cars[i].direction;


            if (cars[i].x >= SCREEN_WIDTH || cars[i].x < 0) {
                if (cars[i].disappearing) {
                    cars[i].x = -2;
                    cars[i].frameCounter = rand() % 50 + 20;
                
}
                else {

                    cars[i].x = (cars[i].x < 0) ? SCREEN_WIDTH - 1 : 0;
                
}
            
}


            if (frog.x == cars[i].x && frog.y == cars[i].y) {
                gameOver = true;
                return;
            
}
        
}
    
}
}


void updateTrucks(Truck trucks[], int numTrucks) {
    for (int i = 0; i < numTrucks; i++) {
        trucks[i].x += trucks[i].direction;
        if (trucks[i].x >= SCREEN_WIDTH) trucks[i].x = 0;
        if (trucks[i].x < 0) trucks[i].x = SCREEN_WIDTH - 1;
    
}
}



int getRidingTruckIndex(const Frog& frog, const Truck trucks[], int numTrucks) {
    for (int i = 0; i < numTrucks; i++) {
        if (frog.x == trucks[i].x && frog.y == trucks[i].y) return i;
    
}
    return -1;
}

void RideTruck(Frog& frog, const Truck& truck) {
    frog.x = truck.x;
    frog.y = truck.y;
}

bool checkWin(const Frog& frog) {
    return frog.y == 0;
}

bool checkCollisionObstacle(const Frog& frog, const Obstacle obstacles[], int numObstacles) {
    for (int i = 0; i < numObstacles; i++) {
        if (frog.x == obstacles[i].x && frog.y == obstacles[i].y) return true;
    
}
    return false;
}

void setCursorPosition(int x, int y) {
    cout << "\033[" << y + 1 << ";" << x + 1 << "H";
}

void clearScreen() {
    cout << "\033[2J\033[1;1H";
}