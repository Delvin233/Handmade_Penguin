#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <cstdlib>
#include <stdio.h>

#define internal static
#define local_persist static
#define global_variable static

global_variable SDL_Texture *Texture;
global_variable void *Pixels;
global_variable int TextureWidth;

internal void
SDLResizeTexture (SDL_Renderer *Renderer, int Width, int Height)
{

  if (Pixels)
    {
      free (Pixels);
    }
  if (Texture)
    {
      SDL_DestroyTexture (Texture);
    }
  SDL_Texture *Texture
      = SDL_CreateTexture (Renderer, SDL_PIXELFORMAT_ABGR8888,
                           SDL_TEXTUREACCESS_STREAMING, Width, Height);

  TextureWidth = Width;
  void *Pixels = malloc (Width * Height * 4);
}
internal void
SDLUpdateWindow (SDL_Window *Window, SDL_Renderer *Renderer)
{
  SDL_UpdateTexture (Texture, 0, Pixels, TextureWidth * 4);
  SDL_RenderTexture (Renderer, Texture, 0, 0);
  SDL_RenderPresent (Renderer);
}
// SDL_GetWindowSize (Window, &Width, &Height);

bool
HandleEvent (SDL_Event *Event)
{
  bool ShouldQuit = false;
  switch (Event->type)
    {
    case SDL_EVENT_QUIT:
      {
        printf ("SDL_QUIT\n");
        return (ShouldQuit) = true;
      }
      break;
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
      {
        printf ("SDL_WINDOWEVENT_SIZE_CHANGED (%d, %d\n", Event->window.data1,
                Event->window.data2);
        SDL_Window *Window = SDL_GetWindowFromID (Event->window.windowID);
        SDL_Renderer *Renderer = SDL_GetRenderer (Window);
        SDLResizeTexture (Renderer, Event->window.data1, Event->window.data2);
      }
      break;
    case SDL_EVENT_WINDOW_EXPOSED:
      {
        printf ("Window exposed \n");
        SDL_Window *Window = SDL_GetWindowFromID (Event->window.windowID);
        SDL_Renderer *Renderer = SDL_GetRenderer (Window);
        // if (Renderer)
        //   {
        //     static bool IsWhite = true;
        //     if (IsWhite == true)
        //       {
        //         SDL_SetRenderDrawColor (Renderer, 255, 255, 255, 255);
        //         IsWhite = false;
        //       }
        //     else
        //       {
        //         SDL_SetRenderDrawColor (Renderer, 0, 0, 0, 255);
        //         IsWhite = true;
        //       }
        //     SDL_RenderClear (Renderer);
        //     SDL_RenderPresent (Renderer);
        //   }
        SDLUpdateWindow (Window, Renderer);
      }
      break;
    }
  return (ShouldQuit);
};

int
main (int argc, char *argv[])
{
  // Initializing our subsystem
  if (SDL_Init (SDL_INIT_VIDEO) != 0)
    {
    };
  // Create the window
  SDL_Window *Window;
  Window
      = SDL_CreateWindow ("Handmade Penguin", 640, 480, SDL_WINDOW_RESIZABLE);

  if (Window)
    {
      // Create a renderer for the window
      SDL_Renderer *Renderer = SDL_CreateRenderer (Window, NULL);

      if (Renderer)
        {
          for (;;)
            {
              SDL_Event Event;
              SDL_WaitEvent (&Event);
              if (HandleEvent (&Event))
                {
                  break;
                }
            }
        }
      else
        {
          // TODO: logging
        }
    }
  else
    {
      // TODO: logging
    }
SDL_Quit:
  return 0;
}
