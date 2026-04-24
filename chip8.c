#include<stdio.h>
#include <SDL2/SDL.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>

typedef struct{
    SDL_Window *window;
}sdl_t;

typedef struct{
    uint32_t window_width;  //chip-8 width=64 , height=32
    uint32_t window_height;
}config_t;


bool set_config_from_args(config_t *config,const int argc , const char **argv){
    
}

bool init_sdl(sdl_t *sdl, config_t config){
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER)!=0){
        SDL_Log("Unable to initialize SDL:%s\n",SDL_GetError());
        return false;
    }
    sdl->window=SDL_CreateWindow("CHIP-8",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                config.window_width,config.window_height,0);

    if(!sdl->window){
        SDL_Log("Could not create SDL window:%s\n",SDL_GetError());
    }
    return true;
}
void cleanup(sdl_t *window){
    SDL_DestroyWindow(&window);
    SDL_Quit();
}
int main(int argc , char **argv){

    sdl_t sdl={0};
    config_t config={0};
    if(!init_sdl(&sdl,)){ //when !initsdl= true that means fn SDL initialization failed
        exit(EXIT_FAILURE);
    }
    cleanup(&sdl);
    exit(EXIT_SUCCESS);

}