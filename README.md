## [Challenge #22 [intermediate]](https://www.reddit.com/r/dailyprogrammer/comments/qr0l2/3102012_challenge_22_intermediate/)

Create a traversable maze, with optional elements.

The game is won when the player reaches a golden tile.

### Build
Only tested on Windows 10.

Requires [SDL2](https://www.libsdl.org/download-2.0.php) and [SDL_image 2.0](https://www.libsdl.org/projects/SDL_image/).

The linker must be able to find SDL2.lib/dll SDL2main.lib, SDL2_image.lib/dll.
If that's the case then you should be able to build with the powershell script vbuild.ps1.

### Usage
Make sure that libpng16-16.dll and zlib1.dll are located in the
execution directory.
Once they are, just run the vrun.ps1 script.

## Credits
Game tiles used: https://opengameart.org/content/lots-of-free-2d-tiles-and-sprites-by-hyptosis

Text: https://opengameart.org/content/platformer-art-replacement-gui-text
