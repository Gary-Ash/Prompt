 ## Documentation

[terminal control codes reference](https://invisible-island.net/ncurses/man/terminfo.5.html) 
  
 
Character Name             Number                  Color Name              Number  
  

Seperator                  25b6                    Black                   235
End Cap                    e0b0                    Yellow                  236
Home                       f015                    Orange                  166
Lock                       f023                    Red                     160
Unlock                     f09c                    Magenta                 125
Ellipsis                   2026                    Purple                  61
Git                        e702                    Blue                    33
GitHub                     f09b                    Cyam                    37
GitLab                     f296                    Green                   64
Bit Bucket                 f171                    Gold                    220
Ahead                      f01b                    Soft Grey               253
Behind                     f01a                    Soft Pink               225
Staged                     f055                    White                   231
Unstaged                   f06a
Untracked                  f059
Conflicts                  f057
Stashed                    f01c

#define RGBA(r, g, b, a) ((r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (a & 0xFF))

u8 red = (color & 0xFF000000) >> 24;
u8 green = (color & 0x00FF0000) >> 16;
u8 blue = (color & 0x0000FF00) >> 8;

void extractRGBA(uint32_t color, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a) {
    r = (color >> 24) & 0xFF; // Extract the red component
    g = (color >> 16) & 0xFF; // Extract the green component
    b = (color >> 8) & 0xFF;  // Extract the blue component
    a = color & 0xFF;         // Extract the alpha component
}

38;2;r;g;b	Set text colour to an RGB value (e.g. \x1b[38;2;255;255;0m)
48;2;r;g;b	Set background colour to an RGB value

38;5;n	Set text colour to index n in a 256-colour palette (e.g. \x1b[38;5;34m)
48;5;n	Set background colour to index n in a 256-colour palette
