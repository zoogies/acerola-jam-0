// does not work for whatever reason. only can get film grain to work if
// its copy pasted into each respective file

// #include <math.h>

// #include <yoyoengine/yoyoengine.h>
// #include "film_grain.h"

// SDL_Texture * _tex_noise[20];

// void init_film_grain(){
//     unsigned char buff[640*360*3];
//     int x, y;

//     for(x = 0; x < 20; x++)  // create 20 noise textures to draw over screen
//     {
//         for(y = 0; y < 640*360; y++)
//         {
//             unsigned char c = rand()%255;
//             buff[y*3] = c;    // red
//             buff[y*3+1] = c;    // green
//             buff[y*3+2] = c;    // blue
//         }

//         // create texture  and set its blending properties
//         _tex_noise[x] = SDL_CreateTexture(YE_STATE.runtime.renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STATIC, 640, 360);
//         SDL_UpdateTexture(_tex_noise[x], 0, buff, 640*3);
//         SDL_SetTextureBlendMode(_tex_noise[x], SDL_BLENDMODE_BLEND);
//         SDL_SetTextureAlphaMod(_tex_noise[x], 20); // set strength of texture blend from 0-255
//     }
// }

// void render_film_grain(){
//     printf("rendering\n");
//     SDL_RenderCopy(YE_STATE.runtime.renderer, _tex_noise[rand()%20], 0, 0);
// }

// void shutdown_film_grain(){

// }