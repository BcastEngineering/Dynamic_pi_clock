#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cmath>
#include <ctime>

const int INITIAL_WINDOW_WIDTH = 1920;
const int INITIAL_WINDOW_HEIGHT = 1080;
const int DOT_RADIUS = 10;  // Radius of each dot
int SDL_ShowCursor(int toggle);

void drawCircle(SDL_Renderer* renderer, int cx, int cy, int radius) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        SDL_RenderDrawPoint(renderer, cx + x, cy - y);
        SDL_RenderDrawPoint(renderer, cx + y, cy - x);
        SDL_RenderDrawPoint(renderer, cx - y, cy - x);
        SDL_RenderDrawPoint(renderer, cx - x, cy - y);
        SDL_RenderDrawPoint(renderer, cx - x, cy + y);
        SDL_RenderDrawPoint(renderer, cx - y, cy + x);
        SDL_RenderDrawPoint(renderer, cx + y, cy + x);
        SDL_RenderDrawPoint(renderer, cx + x, cy + y);

        y += 1;
        err += 1 + 2 * y;
        if (2 * (err - x) + 1 > 0) {
            x -= 1;
            err += 1 - 2 * x;
        }
    }
}

void drawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if (dx * dx + dy * dy <= radius * radius) {
                SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
            }
        }
    }
}

void drawSecondDots(SDL_Renderer* renderer, int cx, int cy, int radius, int currentSecond) {
    const int numDots = 60;  // Number of seconds
    double angleStep = 2 * M_PI / numDots;
    double currentAngle;

    for (int i = 0; i < numDots; ++i) {
        currentAngle = i * angleStep - M_PI / 2;  // Start from the top of the circle
        int x = cx + radius * cos(currentAngle);
        int y = cy + radius * sin(currentAngle);

        // Only light up dots for seconds that have passed
        SDL_Color color = (i <= currentSecond) ? SDL_Color{255, 0, 0, 255} : SDL_Color{0, 0, 0, 0}; // Red for lit dots, transparent for others

        drawFilledCircle(renderer, x, y, DOT_RADIUS, color);
    }
}

void drawHourDots(SDL_Renderer* renderer, int cx, int cy, int radius) {
    // Draw dots for all 12 hours with corresponding positions
    const int numHours = 12;
    double angleStep = 2 * M_PI / numHours;
    double currentAngle;

    for (int i = 0; i < numHours; ++i) {
        currentAngle = i * angleStep - M_PI / 2;  // Start from the top of the circle
        int x = cx + radius * cos(currentAngle);
        int y = cy + radius * sin(currentAngle);

        // Use red color for hour dots
        SDL_Color color = {255, 0, 0, 255};  // Red color for hour dots

        drawFilledCircle(renderer, x, y, DOT_RADIUS, color);
    }
}

void drawDigitalTime(SDL_Renderer* renderer, TTF_Font* font, TTF_Font* fontSmall, int cx, int cy, std::tm* localTime) {
    char timeString[6];  // Enough space for HH:MM and null terminator
    std::strftime(timeString, sizeof(timeString), "%H:%M", localTime);

    SDL_Color color = {255, 0, 0, 255};  // Red color for the digital time

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, timeString, color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {cx - textSurface->w / 2, cy - textSurface->h, textSurface->w, textSurface->h};

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Draw the seconds below the time
    char secondsString[3];
    std::strftime(secondsString, sizeof(secondsString), "%S", localTime);

    SDL_Surface* secondsSurface = TTF_RenderText_Solid(fontSmall, secondsString, color);
    SDL_Texture* secondsTexture = SDL_CreateTextureFromSurface(renderer, secondsSurface);

    SDL_Rect secondsRect = {cx - secondsSurface->w / 2, cy + 30, secondsSurface->w, secondsSurface->h};

    SDL_RenderCopy(renderer, secondsTexture, NULL, &secondsRect);

    SDL_FreeSurface(secondsSurface);
    SDL_DestroyTexture(secondsTexture);
}

void drawClock(SDL_Renderer* renderer, TTF_Font* font, TTF_Font* fontSmall, int windowWidth, int windowHeight, std::tm* localTime) {
    int cx = windowWidth / 2;
    int cy = windowHeight / 2;
    int clockRadius = std::min(windowWidth, windowHeight) / 3;

    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black background
    SDL_RenderClear(renderer);

    // Draw clock face
    drawCircle(renderer, cx, cy, clockRadius);

    // Draw the circular dots for hours (now positioned further out)
    drawHourDots(renderer, cx, cy, clockRadius * 1.15);

    // Draw the circular dots for seconds (now positioned closer to the center)
    drawSecondDots(renderer, cx, cy, clockRadius * 1, localTime->tm_sec);

    // Draw digital time in the center of the clock
    drawDigitalTime(renderer, font, fontSmall, cx, cy *1.1, localTime);

    // Update screen
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }
    //Remove mouse cursor
    SDL_ShowCursor(SDL_DISABLE);


    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create a window with resizable property
    SDL_Window* window = SDL_CreateWindow("Dynamic Pi Clock", 
                                          SDL_WINDOWPOS_CENTERED, 
                                          SDL_WINDOWPOS_CENTERED, 
                                          INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, 
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window == nullptr) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Load fonts
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 110);
    TTF_Font* fontSmall = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 80);
    if (font == nullptr || fontSmall == nullptr) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    bool running = true;
    SDL_Event event;
    int windowWidth = INITIAL_WINDOW_WIDTH;
    int windowHeight = INITIAL_WINDOW_HEIGHT;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                windowWidth = event.window.data1;
                windowHeight = event.window.data2;
            }
        }

        // Get current time
        time_t now = time(0);
        std::tm* localTime = localtime(&now);

        // Draw clock
        drawClock(renderer, font, fontSmall, windowWidth, windowHeight, localTime);

        // Delay to control frame rate
       // SDL_Delay(10 - (localTime->tm_sec % 10)); // Adjust for exact timing every half second
    }

    // Clean up
    TTF_CloseFont(font);
    TTF_CloseFont(fontSmall);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
