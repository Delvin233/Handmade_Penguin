#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_haptic.h>
#include <SDL2/SDL_joystick.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>

// NOTE: MAP_ANONYMOUS is not defined on Mac OS X and some other UNIX systems.
// On the vast majority of those systems, one can use MAP_ANON instead.
// Huge thanks to Adam Rosenfield for investigating this, and suggesting this
// workaround:
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#define internal static
#define local_persist static
#define global_variable static

#define MAX_CONTROLLERS 4
SDL_GameController *ControllerHandles[MAX_CONTROLLERS];
SDL_Haptic *RumbleHandles[MAX_CONTROLLERS];

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

struct sdl_offscreen_buffer
{
  // NOTE(casey): Pixels are alwasy 32-bits wide, Memory Order BB GG RR XX
  SDL_Texture *Texture;
  void *Memory;
  int Width;
  int Height;
  int Pitch;
};

struct sdl_window_dimension
{
  int Width;
  int Height;
};

global_variable sdl_offscreen_buffer GlobalBackbuffer;

sdl_window_dimension
SDLGetWindowDimension (SDL_Window *Window)
{
  sdl_window_dimension Result;

  SDL_GetWindowSize (Window, &Result.Width, &Result.Height);

  return (Result);
}

internal void
RenderWeirdGradient (sdl_offscreen_buffer Buffer, int BlueOffset,
                     int GreenOffset)
{
  uint8 *Row = (uint8 *)Buffer.Memory;
  for (int Y = 0; Y < Buffer.Height; ++Y)
    {
      uint32 *Pixel = (uint32 *)Row;
      for (int X = 0; X < Buffer.Width; ++X)
        {
          uint8 Blue = (X + BlueOffset);
          uint8 Green = (Y + GreenOffset);

          *Pixel++ = ((Green << 8) | Blue);
        }

      Row += Buffer.Pitch;
    }
}

internal void
SDLResizeTexture (sdl_offscreen_buffer *Buffer, SDL_Renderer *Renderer,
                  int Width, int Height)
{
  int BytesPerPixel = 4;
  if (Buffer->Memory)
    {
      munmap (Buffer->Memory, Buffer->Width * Buffer->Height * BytesPerPixel);
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
  Buffer->Pitch = Width * BytesPerPixel;
  Buffer->Memory
      = mmap (0, Width * Height * BytesPerPixel, PROT_READ | PROT_WRITE,
              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

internal void
SDLUpdateWindow (SDL_Window *Window, SDL_Renderer *Renderer,
                 sdl_offscreen_buffer Buffer)
{
  SDL_UpdateTexture (Buffer.Texture, 0, Buffer.Memory, Buffer.Pitch);

  SDL_RenderCopy (Renderer, Buffer.Texture, 0, 0);

  SDL_RenderPresent (Renderer);
}

bool
HandleEvent (SDL_Event *Event)
{
  bool ShouldQuit = false;

  switch (Event->type)
    {
    case SDL_QUIT:
      {
        printf ("SDL_QUIT\n");
        ShouldQuit = true;
      }
      break;

    case SDL_WINDOWEVENT:
      {
        switch (Event->window.event)
          {
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            {
              SDL_Window *Window
                  = SDL_GetWindowFromID (Event->window.windowID);
              SDL_Renderer *Renderer = SDL_GetRenderer (Window);
              printf ("SDL_WINDOWEVENT_SIZE_CHANGED (%d, %d)\n",
                      Event->window.data1, Event->window.data2);
            }
            break;

          case SDL_WINDOWEVENT_FOCUS_GAINED:
            {
              printf ("SDL_WINDOWEVENT_FOCUS_GAINED\n");
            }
            break;

          case SDL_WINDOWEVENT_EXPOSED:
            {
              SDL_Window *Window
                  = SDL_GetWindowFromID (Event->window.windowID);
              SDL_Renderer *Renderer = SDL_GetRenderer (Window);
              SDLUpdateWindow (Window, Renderer, GlobalBackbuffer);
            }
            break;
          }
      }
      break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      {
        SDL_Keycode KeyCode = Event->key.keysym.sym;
        bool IsDown = (Event->key.state == SDL_PRESSED);
        bool WasDown = false;
        if (Event->key.state == SDL_RELEASED)
          {
            WasDown = true;
          }
        else if (Event->key.repeat != 0)
          {
            WasDown = true;
          }

        // keybinds for keyboard
        if (KeyCode == SDLK_w)
          {
            // printf ("W \n");
          }
        else if (KeyCode == SDLK_a)
          {
            // printf ("W \n");
          }
        else if (KeyCode == SDLK_s)
          {
            // printf ("W \n");
          }
        else if (KeyCode == SDLK_d)
          {
            // printf ("W \n");
          }
        else if (KeyCode == SDLK_q)
          {
            // printf ("W \n");
          }
        else if (KeyCode == SDLK_e)
          {
            // printf ("W \n");
          }
        else if (KeyCode == SDLK_UP)
          {
            // printf ("W \n");
          }
        else if (KeyCode == SDLK_UP)
          {
            // printf ("W \n");
          }
        else if (KeyCode == SDLK_LEFT)
          {
            // printf ("W \n");
          }
        else if (KeyCode == SDLK_RIGHT)
          {
            // printf ("W \n");
          }
        else if (KeyCode == SDLK_ESCAPE)
          {
            printf ("ESCAPE: ");
            // quite smart though :)... try it and see
            if (IsDown)
              {
                printf ("IsDown");
              }
            if (WasDown)
              {
                printf ("WasDown");
              }
            printf ("\n");
          }
        else if (KeyCode == SDLK_SPACE)
          {
          }
      }
      break;
    }

  return (ShouldQuit);
}

internal void
SDLOpenGameControllers ()
{

  int MaxJoysticks = SDL_NumJoysticks ();
  int ControllerIndex = 0;
  for (int JoystickIndex = 0; JoystickIndex < MaxJoysticks; ++JoystickIndex)
    {
      if (!SDL_IsGameController (JoystickIndex))
        {
          continue;
        }
      if (ControllerIndex >= MAX_CONTROLLERS)
        {
          break;
        }
      ControllerHandles[ControllerIndex]
          = SDL_GameControllerOpen (JoystickIndex);
      // SDL_Joystick *JoystickHandle
      //     = SDL_GameControllerGetJoystick
      //     (ControllerHandles[ControllerIndex]);
      RumbleHandles[ControllerIndex] = SDL_HapticOpen (JoystickIndex);
      if (SDL_HapticRumbleInit (RumbleHandles[ControllerIndex]) != 0)
        {
          SDL_HapticClose (RumbleHandles[ControllerIndex]);
          RumbleHandles[ControllerIndex] = 0;
        }
      ControllerIndex++;
    }
}

internal void
SDLCloseGameControllers ()
{

  for (int ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS;
       ++ControllerIndex)
    {
      if (RumbleHandles[ControllerIndex])
        {
          SDL_HapticClose (RumbleHandles[ControllerIndex]);
        }
      SDL_GameControllerClose (ControllerHandles[ControllerIndex]);
    }
}

int
main (int argc, char *argv[])
{
  // as to why i added this comment?
  // Initialze out subsystem(s)
  SDL_Init (SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);

  // Create our window.
  SDL_Window *Window = SDL_CreateWindow (
      "Handmade Penguuuu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      640, 480, SDL_WINDOW_RESIZABLE);

  SDLOpenGameControllers ();

  if (Window)
    {
      // Create a "Renderer" for our window.
      SDL_Renderer *Renderer = SDL_CreateRenderer (Window, -1, 0);
      if (Renderer)
        {
          bool Running = true;
          sdl_window_dimension Dimension = SDLGetWindowDimension (Window);
          SDLResizeTexture (&GlobalBackbuffer, Renderer, Dimension.Width,
                            Dimension.Height);
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
              // YOffset += 2;

              // Button mapping
              for (int ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS;
                   ++ControllerIndex)
                {
                  if (ControllerHandles[ControllerIndex] != 0
                      && SDL_GameControllerGetAttached (
                          ControllerHandles[ControllerIndex]))
                    {
                      bool Up = SDL_GameControllerGetButton (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_BUTTON_DPAD_UP);
                      bool Down = SDL_GameControllerGetButton (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_BUTTON_DPAD_DOWN);

                      bool Left = SDL_GameControllerGetButton (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_BUTTON_DPAD_LEFT);

                      bool Right = SDL_GameControllerGetButton (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

                      bool Start = SDL_GameControllerGetButton (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_BUTTON_START);

                      bool Back_or_Select = SDL_GameControllerGetButton (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_BUTTON_BACK);

                      bool LeftShoulder = SDL_GameControllerGetButton (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_BUTTON_LEFTSHOULDER);

                      bool RightShoulder = SDL_GameControllerGetButton (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);

                      int16 LeftTrigger = SDL_GameControllerGetAxis (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_AXIS_TRIGGERLEFT);

                      int16 RightTrigger = SDL_GameControllerGetAxis (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

                      bool AButton = SDL_GameControllerGetButton (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_BUTTON_A);

                      bool YButton = SDL_GameControllerGetButton (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_BUTTON_Y);

                      bool XButton = SDL_GameControllerGetButton (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_BUTTON_X);

                      bool BButton = SDL_GameControllerGetButton (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_BUTTON_B);

                      int16 StickX = SDL_GameControllerGetAxis (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_AXIS_LEFTX);

                      int16 StickY = SDL_GameControllerGetAxis (
                          ControllerHandles[ControllerIndex],
                          SDL_CONTROLLER_AXIS_LEFTY);

                      // Button uses and checks
                      if (AButton)
                        {
                          YOffset += 2;
                          printf ("A is pressed \n");
                        }
                      if (BButton)
                        {
                          if (RumbleHandles[ControllerIndex])
                            {
                              SDL_HapticRumblePlay (
                                  RumbleHandles[ControllerIndex], 0.5f, 2000);
                              printf ("B is pressed \n");
                            }
                        }
                    }

                  else
                    {
                      // TODO: When not pluggen in
                    }
                }
            }
        }
      else
        {
          // TODO(casey): Logging
        }
    }
  else
    {
      // TODO(casey): Logging
    }

  SDLCloseGameControllers ();
  SDL_Quit ();
  return (0);
}
