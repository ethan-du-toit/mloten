# MLOTEN

"MLOTEN" is a fantasy console emulator based on RISC-V. (The name was chosen to be meaningless yet memorable.) It was submitted as my final programming project for my Software Development and Design HSC. 

At present the only game that runs on it is a short, unpolished demo, "Life of an Elephant-Headed Man", that uses the conceit that "MLOTEN" was a failed Soviet/post-Soviet console. It uses my own amateurish art and level design.

# Building MLOTEN

"MLOTEN" should compile on Linux, Mac, and Windows, but as I do not have a Windows environment to test it in, only Unix is supported via the given Makefile. 

"MLOTEN" is written in C. To build the emulator itself, the only library that is required is SDL2. Build it using the Makefile in the "bin/" directory in the root of the project. It will build an executable with the name "rv".

To build the demo game, the GNU RISC-V cross-compiling toolchain is required. If you only wish to run the demo game, not build it, a compiled ROM is supplied in the "bin/" directory in the root of the project with the filename "rvgame.bin". 
If you wish to build it, a Makefile is provided in the "gamesrc/" directory. It will build a demo game ROM with the filename "rvgame.bin".

In order to play the demo game, or in fact any game, invoke the "MLOTEN" executable with the path to the game's ROM as an argument from the command line. 
This specific command will work to play the demo game, if everything is built with the provided Makefiles, and it is run from the "bin/" directory in the root of the project:

    ./rv rvgame.bin
