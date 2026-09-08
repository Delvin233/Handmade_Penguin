#include <SDL3/SDL.h>
// #include <SDL3/SDL_events.h>
// #include <SDL3/SDL_oldnames.h>
// #include <SDL3/SDL_render.h>
#include <stdio.h>

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
      }
      break;
    case SDL_EVENT_WINDOW_EXPOSED:
      {
        printf ("Window exposed \n");
        SDL_Window *Window = SDL_GetWindowFromID (Event->window.windowID);
        SDL_Renderer *Renderer = SDL_GetRenderer (Window);
        if (Renderer)
          {
            static bool IsWhite = true;
            if (IsWhite == true)
              {
                SDL_SetRenderDrawColor (Renderer, 255, 255, 255, 255);
                IsWhite = false;
              }
            else
              {
                SDL_SetRenderDrawColor (Renderer, 0, 0, 0, 255);
                IsWhite = true;
              }
            SDL_RenderClear (Renderer);
            SDL_RenderPresent (Renderer);
          }
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

  int Width, Height;
  SDL_GetWindowSize (Window, &Width, &Height);

  if (Window)
    {
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
    }
SDL_Quit:
  return 0;
}
