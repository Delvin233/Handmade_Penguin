#include <SDL3/SDL.h>
#include <SDL3/SDL_gamepad.h>
#include <stdio.h>
#include <sys/mman.h>

// a workaround since MAP_ANONYMOUS is not defined on some UNIX systems
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#define internal static
#define local_persist static
#define global_variable static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

// global_variable SDL_Texture *Texture;
// global_variable void *BitmapMemory;
// global_variable int BitmapWidth;
// global_variable int BitmapHeight;
// global_variable int BytesPerPixel = 4;

struct sdl_offscreen_buffer
{
  SDL_Texture *Texture;
  void *Memory;
  int Width;
  int Height;
  int BytesPerPixel;
};
global_variable sdl_offscreen_buffer GlobalBackbuffer;

internal void
RenderWeirdGradient (sdl_offscreen_buffer Buffer, int BlueOffset,
                     int GreenOffset)
{
  int Width = Buffer.Width;
  int Height = Buffer.Height;

  int Pitch = Width * Buffer.BytesPerPixel;
  uint8 *Row = (uint8 *)Buffer.Memory;
  for (int Y = 0; Y < Buffer.Height; ++Y)
    {
      uint32 *Pixel = (uint32 *)Row;
      for (int X = 0; X < Buffer.Width; ++X)
        {
          uint8 Blue = (X + BlueOffset);
          uint8 Green = (Y + GreenOffset);

          *Pixel++ = ((0xFF << 24) | (Green << 8) | Blue << 0);
        }
      Row += Pitch;
    }
};

internal void
SDLResizeTexture (sdl_offscreen_buffer *Buffer, SDL_Renderer *Renderer,
                  int Width, int Height)
{

  if (Buffer->Memory)
    {
      munmap (Buffer->Memory,
              Buffer->Width * Buffer->Height * Buffer->BytesPerPixel);
    }
  if (Buffer->Texture)
    {
      SDL_DestroyTexture (Buffer->Texture);
    }
  Buffer->Texture
      = SDL_CreateTexture (Renderer, SDL_PIXELFORMAT_ARGB8888,
                           SDL_TEXTUREACCESS_STREAMING, Width, Height);
  Buffer->Width = Width;
  Buffer->Height = Height;
  Buffer->BytesPerPixel = 4;

  Buffer->Memory
      = mmap (0, Width * Height * Buffer->BytesPerPixel,
              PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
}

internal void
SDLUpdateWindow (SDL_Window *Window, SDL_Renderer *Renderer,
                 sdl_offscreen_buffer Buffer)
{
  SDL_UpdateTexture (Buffer.Texture, 0, Buffer.Memory,
                     Buffer.Width * Buffer.BytesPerPixel);
  SDL_RenderTexture (Renderer, Buffer.Texture, 0, 0);
  SDL_RenderPresent (Renderer);
}

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
        SDLResizeTexture (&GlobalBackbuffer, Renderer, Event->window.data1,
                          Event->window.data2);
      }
      break;
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
      {
        printf ("Window focus gained \n");
      }
      break;
    case SDL_EVENT_WINDOW_EXPOSED:
      {
        printf ("Window exposed \n");
        SDL_Window *Window = SDL_GetWindowFromID (Event->window.windowID);
        SDL_Renderer *Renderer = SDL_GetRenderer (Window);
      }
      break;
    }
  return (ShouldQuit);
};

int
main (int argc, char *argv[])
{
  // Initializing our subsystem
  SDL_Init (SDL_INIT_VIDEO);
  // Create the window
  SDL_Window *Window
      = SDL_CreateWindow ("Handmade Penguin", 640, 480, SDL_WINDOW_RESIZABLE);

  if (Window)
    {
      // Create a renderer for the window
      SDL_Renderer *Renderer = SDL_CreateRenderer (Window, NULL);

      if (Renderer)
        {
          bool Running = true;
          int Width, Height;
          SDL_GetWindowSize (Window, &Width, &Height);
          int XOffset = 0;
          int YOffset = 0;
          while (Running)
            {
              SDL_Event Event;
              while (SDL_PollEvent (&Event))
                {
                  if (HandleEvent (&Event))
                    {
                      Running = false;
                    }
                }
              RenderWeirdGradient (GlobalBackbuffer, XOffset, YOffset);
              SDLUpdateWindow (Window, Renderer, GlobalBackbuffer);

              ++XOffset;
              YOffset += 2;
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
