#include "DrawingWindow.h"

// Simple constructor method
DrawingWindow::DrawingWindow()
{
}

// Complex constructor method
DrawingWindow::DrawingWindow(int w, int h, bool fullscreen)
{
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) !=0) {
    printMessageAndQuit("Could not initialise SDL: ", SDL_GetError());
  }

  width = w;
  height = h;
  pixelBuffer = new uint32_t[width*height];
  clearPixels();

  uint32_t flags = SDL_WINDOW_OPENGL;
  if(fullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  int ANYWHERE = SDL_WINDOWPOS_UNDEFINED;
  window = SDL_CreateWindow("COMS30115", ANYWHERE, ANYWHERE, width, height, flags);
  if(window == 0) printMessageAndQuit("Could not set video mode: ", SDL_GetError());

  flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  renderer = SDL_CreateRenderer(window, -1, flags);
  if(renderer == 0) printMessageAndQuit("Could not create renderer: ", SDL_GetError());

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  SDL_RenderSetLogicalSize(renderer, width, height);

  int PIXELFORMAT = SDL_PIXELFORMAT_ARGB8888;
  texture = SDL_CreateTexture(renderer, PIXELFORMAT, SDL_TEXTUREACCESS_STATIC, width, height);
  if(texture == 0) printMessageAndQuit("Could not allocate texture: ", SDL_GetError());
}

// Deconstructor method
void DrawingWindow::destroy()
{
  delete[] pixelBuffer;
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void DrawingWindow::renderFrame()
{
  SDL_UpdateTexture(texture, NULL, pixelBuffer, width*sizeof(uint32_t));
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

bool DrawingWindow::pollForInputEvents(SDL_Event *event)
{
  if(SDL_PollEvent(event)) {
    if((event->type == SDL_QUIT) || ((event->type == SDL_KEYDOWN) && (event->key.keysym.sym == SDLK_ESCAPE))) {
      destroy();
      printMessageAndQuit("Exiting", NULL);
    }
    return true;
  }
  return false;
}

void DrawingWindow::setPixelColour(int x, int y, uint32_t colour)
{
  if((x<0) || (x>=width) || (y<0) || (y>=height)) {
    std::cout << x << "," <<  y << " not on visible screen area" << std::endl;
  }
  else pixelBuffer[(y*width)+x] = colour;
}

uint32_t DrawingWindow::getPixelColour(int x, int y)
{
  if((x<0) || (x>=width) || (y<0) || (y>=height)) {
    std::cout << x << "," <<  y << " not on visible screen area" << std::endl;
    return -1;
  }
  else return pixelBuffer[(y*width)+x];
}

void DrawingWindow::clearPixels()
{
  memset(pixelBuffer, 0, width * height * sizeof(uint32_t));
}
