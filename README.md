# Dynamic_pi_clock
# Dynamic Pi Clock

**This project was heavily assisted by ChatGPT**

SDL Dynamic Pi Clock is a graphical clock application for the Raspberry Pi that uses SDL2 and SDL_ttf libraries to display an analog-style clock with digital time and dots representing hours and seconds. This clock is designed to be run on a Raspberry Pi and can be displayed on any screen connected to it.

## Features

- **Analog Clock**: Displays an analog clock face with hour and second markers.
- **Digital Time**: Shows the current time digitally in the center of the clock.
- **Real-time Updates**: Updates every second to show the current time.
- **Resizable Window**: The window can be resized, and the clock will adjust accordingly.

## Prerequisites

To run this project, you need to have the SDL2 and SDL_ttf libraries installed on your Raspberry Pi.

### Install SDL2 and SDL_ttf libraries

    sudo apt-get update
    sudo apt-get install libsdl2-dev libsdl2-ttf-dev

Installation

Clone the repository to your local machine using the following command:

    git clone https://github.com/BcastEngineering/Dynamic_pi_clock.git

Compile the dynamic_pi_clock.cpp file using g++:

    cd Dynamic_pi_clock
    g++ dynamic_pi_clock.cpp -o dynamic_pi_clock -lSDL2 -lSDL2_ttf

Running the Program

    ./dynamic_pi_clock

Contributing

Contributions are welcome! Feel free to open issues or submit pull requests for improvements or new features.

