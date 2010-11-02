/******************************************************************************/
// Bullfrog Engine Emulation Library - for use to remake classic games like
// Syndicate Wars, Magic Carpet or Dungeon Keeper.
/******************************************************************************/
/** @file bflib_mouse.cpp
 *     Mouse related routines.
 * @par Purpose:
 *     Pointer position, movement and cursor support.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     12 Feb 2008 - 26 Oct 2010
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#include "bflib_mouse.h"

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <windows.h>

#include "bflib_basics.h"
#include "globals.h"
#include "bflib_video.h"
#include "bflib_memory.h"
#include "bflib_sprite.h"
#include "bflib_vidraw.h"
#include "bflib_mshandler.hpp"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
/*

struct mouse_buffer mbuffer;
struct mouse_info minfo;
char ptr[4096];
unsigned int redraw_active=0;
unsigned int mouse_initialised=0;
short volatile mouse_mickey_x;
short volatile mouse_mickey_y;
long volatile mouse_dx;
long volatile mouse_dy;
unsigned long mouse_pos_change_saved;
struct DevInput joy;
*/
volatile TbBool lbMouseAutoReset = false;
/******************************************************************************/
TbResult LbMouseChangeSpriteAndHotspot(struct TbSprite *pointerSprite, long hot_x, long hot_y)
{
#if (BFDEBUG_LEVEL > 18)
  if (pointerSprite == NULL)
    SYNCLOG("Setting to %s","NONE");
  else
    SYNCLOG("Setting to %dx%d, data at %p",(int)pointerSprite->SWidth,(int)pointerSprite->SHeight,pointerSprite);
#endif
  if (!lbMouseInstalled)
    return Lb_FAIL;
  if (!pointerHandler.SetMousePointerAndOffset(pointerSprite, hot_x, hot_y))
    return Lb_FAIL;
  return Lb_SUCCESS;
}

TbResult LbMouseSetup(struct TbSprite *pointerSprite)
{
  TbResult ret;
  long x,y;
  if (lbMouseInstalled)
    LbMouseSuspend();
  y = (lbDisplay.MouseWindowHeight + lbDisplay.MouseWindowY) / 2;
  x = (lbDisplay.MouseWindowWidth + lbDisplay.MouseWindowX) / 2;
  pointerHandler.Install();
  lbMouseOffline = true;
  lbMouseInstalled = true;
  LbMouseSetWindow(0,0,LbGraphicsScreenWidth(),LbGraphicsScreenHeight());
  ret = Lb_SUCCESS;
  if (LbMouseSetPosition(x,y) != Lb_SUCCESS)
    ret = Lb_FAIL;
  if (LbMouseChangeSprite(pointerSprite) != Lb_SUCCESS)
    ret = Lb_FAIL;
  lbMouseInstalled = (ret == Lb_SUCCESS);
  lbMouseOffline = false;
  return ret;
}

TbResult LbMouseSetPointerHotspot(long hot_x, long hot_y)
{
  if (!lbMouseInstalled)
    return Lb_FAIL;
  if (!pointerHandler.SetPointerOffset(hot_x, hot_y))
    return Lb_FAIL;
  return Lb_SUCCESS;
}

TbResult LbMouseSetPosition(long x, long y)
{
  if (!lbMouseInstalled)
    return Lb_FAIL;
  if (!pointerHandler.SetMousePosition(x, y))
    return Lb_FAIL;
  return Lb_SUCCESS;
}

TbResult LbMouseChangeSprite(struct TbSprite *pointerSprite)
{
#if (BFDEBUG_LEVEL > 18)
  if (pointerSprite == NULL)
    SYNCLOG("Setting to %s","NONE");
  else
    SYNCLOG("Setting to %dx%d, data at %p",(int)pointerSprite->SWidth,(int)pointerSprite->SHeight,pointerSprite);
#endif
  if (!lbMouseInstalled)
    return Lb_FAIL;
  if (!pointerHandler.SetMousePointer(pointerSprite))
    return Lb_FAIL;
  return Lb_SUCCESS;
}

void GetPointerHotspot(long *hot_x, long *hot_y)
{
  struct TbPoint *hotspot;
  hotspot = pointerHandler.GetPointerOffset();
  if (hotspot == NULL)
    return;
  *hot_x = hotspot->x;
  *hot_y = hotspot->y;
}

TbResult LbMouseIsInstalled(void)
{
  if (!lbMouseInstalled)
    return Lb_FAIL;
  if (!pointerHandler.IsInstalled())
    return Lb_FAIL;
  return Lb_SUCCESS;
}

TbResult LbMouseSetWindow(long x, long y, long width, long height)
{
  if (!lbMouseInstalled)
    return Lb_FAIL;
  if (!pointerHandler.SetMouseWindow(x, y, width, height))
    return Lb_FAIL;
  return Lb_SUCCESS;
}

TbResult LbMouseOnMove(struct TbPoint shift)
{
  if ((!lbMouseInstalled) || (lbMouseOffline))
    return Lb_FAIL;
  if (!pointerHandler.SetMousePosition(lbDisplay.MMouseX+shift.x, lbDisplay.MMouseY+shift.y))
    return Lb_FAIL;
  return Lb_SUCCESS;
}

/**
 * Converts mouse coordinates into relative shift coordinates.
 */
void MouseToScreen(struct TbPoint *pos)
{
  static long mx = 0;
  static long my = 0;
  struct TbRect clip;
  struct TbPoint orig;
  if ( lbMouseAutoReset )
  {
      if (!pointerHandler.GetMouseWindow(&clip))
          return;
      orig.x = pos->x;
      orig.y = pos->y;
      pos->x -= mx;
      pos->y -= my;
      mx = orig.x;
      my = orig.y;
      if ((mx < clip.left + 50) || (mx > clip.right - 50)
       || (my < clip.top + 50) || (my > clip.bottom - 50))
      {
        mx = (clip.right-clip.left)/2 + clip.left;
        my = (clip.bottom-clip.top)/2 + clip.top;
        SDL_WarpMouse(mx, my);
      }
  } else
  {
    pos->x -= lbDisplay.MMouseX;
    pos->y -= lbDisplay.MMouseY;
  }
}

TbResult LbMouseSuspend(void)
{
  if (!lbMouseInstalled)
    return Lb_FAIL;
  if (!pointerHandler.Release())
    return Lb_FAIL;
  return Lb_SUCCESS;
}

TbResult LbMouseOnBeginSwap(void)
{
  if (!pointerHandler.PointerBeginSwap())
    return Lb_FAIL;
  return Lb_SUCCESS;
}

void LbMouseOnEndSwap(void)
{
  pointerHandler.PointerEndSwap();
}

void mouseControl(unsigned int action, struct TbPoint *pos)
{
  struct TbPoint dstPos;
  dstPos.x = pos->x;
  dstPos.y = pos->y;
  switch ( action )
  {
  case MActn_MOUSEMOVE:
      MouseToScreen(&dstPos);
      LbMouseOnMove(dstPos);
      break;
  case MActn_LBUTTONDOWN:
      lbDisplay.MLeftButton = 1;
      if ( !lbDisplay.LeftButton )
      {
        MouseToScreen(&dstPos);
        LbMouseOnMove(dstPos);
        lbDisplay.MouseX = lbDisplay.MMouseX;
        lbDisplay.MouseY = lbDisplay.MMouseY;
        lbDisplay.RLeftButton = 0;
        lbDisplay.LeftButton = 1;
      }
      break;
  case MActn_LBUTTONUP:
      lbDisplay.MLeftButton = 0;
      if ( !lbDisplay.RLeftButton )
      {
        MouseToScreen(&dstPos);
        LbMouseOnMove(dstPos);
        lbDisplay.RMouseX = lbDisplay.MMouseX;
        lbDisplay.RMouseY = lbDisplay.MMouseY;
        lbDisplay.RLeftButton = 1;
      }
      break;
  case MActn_RBUTTONDOWN:
      lbDisplay.MRightButton = 1;
      if ( !lbDisplay.RightButton )
      {
        MouseToScreen(&dstPos);
        LbMouseOnMove(dstPos);
        lbDisplay.MouseX = lbDisplay.MMouseX;
        lbDisplay.MouseY = lbDisplay.MMouseY;
        lbDisplay.RRightButton = 0;
        lbDisplay.RightButton = 1;
      }
      break;
  case MActn_RBUTTONUP:
      lbDisplay.MRightButton = 0;
      if ( !lbDisplay.RRightButton )
      {
        MouseToScreen(&dstPos);
        LbMouseOnMove(dstPos);
        lbDisplay.RMouseX = lbDisplay.MMouseX;
        lbDisplay.RMouseY = lbDisplay.MMouseY;
        lbDisplay.RRightButton = 1;
      }
      break;
  case MActn_MBUTTONDOWN:
      lbDisplay.MMiddleButton = 1;
      if ( !lbDisplay.MiddleButton )
      {
        MouseToScreen(&dstPos);
        LbMouseOnMove(dstPos);
        lbDisplay.MouseX = lbDisplay.MMouseX;
        lbDisplay.MouseY = lbDisplay.MMouseY;
        lbDisplay.MiddleButton = 1;
        lbDisplay.RMiddleButton = 0;
      }
      break;
  case MActn_MBUTTONUP:
      lbDisplay.MMiddleButton = 0;
      if ( !lbDisplay.RMiddleButton )
      {
        MouseToScreen(&dstPos);
        LbMouseOnMove(dstPos);
        lbDisplay.RMouseX = lbDisplay.MMouseX;
        lbDisplay.RMouseY = lbDisplay.MMouseY;
        lbDisplay.RMiddleButton = 1;
      }
      break;
  case MActn_WHEELMOVEUP:
  case MActn_WHEELMOVEDOWN:
      break;
    default:
      break;
  }
}

/*
int __fastcall LbMouseChangeMoveRatio(int x, int y)
{
  if ( !lbMouseInstalled )
    return -1;
  if ( (x<1) || (x>63) )
    return -1;
  if ( (y<1) || (y>63) )
    return -1;
  minfo.XMoveRatio = x;
  minfo.YMoveRatio = y;
  return 1;
}

void __fastcall LbProcessMouseClick(SDL_MouseButtonEvent *button)
{
  switch (button->state)
  {
  case SDL_PRESSED:
    switch (button->button)
    {
    case SDL_BUTTON_LEFT:
        lbDisplay.MLeftButton=1;
        if (lbDisplay.LeftButton==0)
        {
          lbDisplay.LeftButton=1;
          lbDisplay.MouseX=lbDisplay.MMouseX;
          lbDisplay.MouseY=lbDisplay.MMouseY;
          lbDisplay.RLeftButton=0;
        }
       break;
    case SDL_BUTTON_MIDDLE:
        lbDisplay.MMiddleButton=1;
        if (lbDisplay.MiddleButton==0)
        {
          lbDisplay.MiddleButton=1;
          lbDisplay.MouseX=lbDisplay.MMouseX;
          lbDisplay.MouseY=lbDisplay.MMouseY;
          lbDisplay.RMiddleButton=0;
        }
       break;
    case SDL_BUTTON_RIGHT:
        lbDisplay.MRightButton=1;
        if (lbDisplay.RightButton==0)
        {
          lbDisplay.RightButton=1;
          lbDisplay.MouseX=lbDisplay.MMouseX;
          lbDisplay.MouseY=lbDisplay.MMouseY;
          lbDisplay.RRightButton=0;
        }
       break;
    };break;
  case SDL_RELEASED:
    switch (button->button)
    {
    case SDL_BUTTON_LEFT:
        lbDisplay.MLeftButton=0;
        if (lbDisplay.RLeftButton==0)
        {
          lbDisplay.RLeftButton=1;
          lbDisplay.RMouseX=lbDisplay.MMouseX;
          lbDisplay.RMouseY=lbDisplay.MMouseY;
        }
       break;
    case SDL_BUTTON_MIDDLE:
        lbDisplay.MMiddleButton=0;
        if (lbDisplay.RMiddleButton==0)
        {
          lbDisplay.RMiddleButton=1;
          lbDisplay.RMouseX=lbDisplay.MMouseX;
          lbDisplay.RMouseY=lbDisplay.MMouseY;
        }
       break;
    case SDL_BUTTON_RIGHT:
        lbDisplay.MRightButton=0;
        if (lbDisplay.RRightButton==0)
        {
          lbDisplay.RRightButton=1;
          lbDisplay.RMouseX=lbDisplay.MMouseX;
          lbDisplay.RMouseY=lbDisplay.MMouseY;
        }
       break;
    };break;
  }
  //MouseHandlerMove(button->x,button->y);
}


//Adjusts point coordinates; returns true if the coordinates have changed.
bool __fastcall adjust_point(long *x, long *y)
{
  bool result = false;
  if ( *x >= lbDisplay.MouseWindowX )
  {
    if ( lbDisplay.MouseWindowX + lbDisplay.MouseWindowWidth <= *x )
    {
      *x = lbDisplay.MouseWindowX + lbDisplay.MouseWindowWidth - 1;
      result = true;
    }
  } else
  {
    *x = lbDisplay.MouseWindowX;
    result = true;
  }
  if ( *y >= lbDisplay.MouseWindowY )
  {
    if ( lbDisplay.MouseWindowY + lbDisplay.MouseWindowHeight <= *y )
    {
      *y = lbDisplay.MouseWindowY + lbDisplay.MouseWindowHeight - 1;
      result = true;
    }
  } else
  {
    *y = lbDisplay.MouseWindowY;
    result = true;
  }
  return result;
}

//Returns if the current mouse position is inside of given rectangle
char __fastcall mouse_in_rect(short x1, short x2, short y1, short y2)
{
  return (x1<=lbDisplay.MMouseX) && (x2>lbDisplay.MMouseX) &&
         (y1<=lbDisplay.MMouseY) && (y2>lbDisplay.MMouseY);
}
*/
/******************************************************************************/
#ifdef __cplusplus
}
#endif
