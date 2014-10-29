////////////////////////////////////////////////////////////////////////////
//
// PingPong Screensaver for XBox Media Center
// Copyright (c) 2005 Joakim Eriksson <je@plane9.com>
//
// Thanks goes to Warren for his 'TestXBS' program!
//
////////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "pingpong.h"
#include "timer.h"
#include <time.h>
#include <kodi/xbmc_scr_dll.h>
#include <string.h>

static char gScrName[1024];

CRenderD3D		gRender;
CPingPong		gPingPong;
CTimer*			gTimer = null;
CRGBA			gCol[3];
float ballspeed[2];

extern "C" void Stop();

////////////////////////////////////////////////////////////////////////////
// XBMC has loaded us into memory, we should set our core values
// here and load any settings we may have from our config file
//
ADDON_STATUS ADDON_Create(void* hdl, void* props)
{
  if (!props)
    return ADDON_STATUS_UNKNOWN;

  SCR_PROPS* scrprops = (SCR_PROPS*)props;

  gRender.m_Width = scrprops->width;
  gRender.m_Height = scrprops->height;

  return ADDON_STATUS_NEED_SETTINGS;
}

extern "C" void Start()
{
  srand(time(NULL));

  gPingPong.m_Paddle[0].m_Col = gCol[0];
  gPingPong.m_Paddle[1].m_Col = gCol[1];
  gPingPong.m_Ball.m_Col = gCol[2];
  gPingPong.m_Ball.m_Vel.Set(ballspeed[0]*gRender.m_Width, ballspeed[1]*gRender.m_Height, 0.0);

  gTimer = new CTimer();
  gTimer->Init();
  if (!gPingPong.RestoreDevice(&gRender))
    Stop();
}

////////////////////////////////////////////////////////////////////////////
// XBMC tells us to render a frame of our screensaver. This is called on
// each frame render in XBMC, you should render a single frame only - the DX
// device will already have been cleared.
//
extern "C" void Render()
{
	gTimer->Update();
	gPingPong.Update(gTimer->GetDeltaTime());
	gPingPong.Draw(&gRender);
}

////////////////////////////////////////////////////////////////////////////
// XBMC tells us to stop the screensaver we should free any memory and release
// any resources we have created.
//
extern "C" void ADDON_Stop()
{
	gPingPong.InvalidateDevice(&gRender);
	SAFE_DELETE(gTimer);
}

void ADDON_Destroy()
{
}

ADDON_STATUS ADDON_GetStatus()
{
  return ADDON_STATUS_OK;
}

bool ADDON_HasSettings()
{
  return true;
}

unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet)
{
  return 0;
}

ADDON_STATUS ADDON_SetSetting(const char *strSetting, const void *value)
{
  static const float C[5][3] = {{1.0, 1.0, 1.0},
                                {0.5, 0.5, 0.5},
                                {1.0, 0.0, 0.0},
                                {0.0, 1.0, 0.0},
                                {0.0, 0.0, 1.0}};
  int c = *(int*)value;
  if (strcmp(strSetting,"paddle1") == 0 && c < 5)
    gCol[0].Set(C[c][0], C[c][1], C[c][2], 1.0);
  if (strcmp(strSetting,"paddle2") == 0 && c < 5)
    gCol[1].Set(C[c][0], C[c][1], C[c][2], 1.0);
  if (strcmp(strSetting,"ball") == 0 && c < 5)
    gCol[2].Set(C[c][0], C[c][1], C[c][2], 1.0);

  if (strcmp(strSetting, "ballspeedx") == 0)
    ballspeed[0] = *(float*)value;
  if (strcmp(strSetting, "ballspeedy") == 0)
    ballspeed[1] = *(float*)value;

  return ADDON_STATUS_OK;
}

void ADDON_FreeSettings()
{
}

void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data)
{
}

void GetInfo(SCR_INFO *info)
{
}
