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
#include <xbmc_scr_dll.h>
#include <string.h>

static char gScrName[1024];

CRenderD3D		gRender;
CPingPong		gPingPong;
CTimer*			gTimer = null;
CRGBA			gCol[3];
float ballspeed[2];

////////////////////////////////////////////////////////////////////////////
// XBMC has loaded us into memory, we should set our core values
// here and load any settings we may have from our config file
//
ADDON_STATUS ADDON_Create(void* hdl, void* props)
{
  if (!props)
    return ADDON_STATUS_UNKNOWN;

  AddonProps_Screensaver* scrprops = (AddonProps_Screensaver*)props;

  gRender.m_Width = scrprops->width;
  gRender.m_Height = scrprops->height;
  gRender.Init(scrprops->device);

  return ADDON_STATUS_NEED_SETTINGS;
}

extern "C" void Stop() 
{
	gPingPong.InvalidateDevice(&gRender);
	SAFE_DELETE(gTimer);
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
	gRender.Begin();
	gTimer->Update();
	gPingPong.Update(gTimer->GetDeltaTime());
	gPingPong.Draw(&gRender);
}

void ADDON_Destroy()
{
}

ADDON_STATUS ADDON_GetStatus()
{
  return ADDON_STATUS_OK;
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

#ifdef WIN32
const BYTE PixelShader[] =
{
     68,  88,  66,  67,  18, 124, 
    182,  35,  30, 142, 196, 211, 
     95, 130,  91, 204,  99,  13, 
    249,   8,   1,   0,   0,   0, 
    124,   1,   0,   0,   4,   0, 
      0,   0,  48,   0,   0,   0, 
    124,   0,   0,   0, 188,   0, 
      0,   0,  72,   1,   0,   0, 
     65, 111, 110,  57,  68,   0, 
      0,   0,  68,   0,   0,   0, 
      0,   2, 255, 255,  32,   0, 
      0,   0,  36,   0,   0,   0, 
      0,   0,  36,   0,   0,   0, 
     36,   0,   0,   0,  36,   0, 
      0,   0,  36,   0,   0,   0, 
     36,   0,   0,   2, 255, 255, 
     31,   0,   0,   2,   0,   0, 
      0, 128,   0,   0,  15, 176, 
      1,   0,   0,   2,   0,   8, 
     15, 128,   0,   0, 228, 176, 
    255, 255,   0,   0,  83,  72, 
     68,  82,  56,   0,   0,   0, 
     64,   0,   0,   0,  14,   0, 
      0,   0,  98,  16,   0,   3, 
    242,  16,  16,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
    242,  32,  16,   0,   0,   0, 
      0,   0,  70,  30,  16,   0, 
      1,   0,   0,   0,  62,   0, 
      0,   1,  73,  83,  71,  78, 
    132,   0,   0,   0,   4,   0, 
      0,   0,   8,   0,   0,   0, 
    104,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
    116,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,  15,   0,   0, 
    122,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,   3,   0,   0,   0, 
    122,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,  12,   0,   0,   0, 
     83,  86,  95,  80,  79,  83, 
     73,  84,  73,  79,  78,   0, 
     67,  79,  76,  79,  82,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0, 171,  79,  83, 
     71,  78,  44,   0,   0,   0, 
      1,   0,   0,   0,   8,   0, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0,  83,  86,  95,  84, 
     65,  82,  71,  69,  84,   0, 
    171, 171
};
#endif

void CRenderD3D::Init(void* pContext)
{
#ifdef WIN32
  m_pContext = reinterpret_cast<ID3D11DeviceContext*>(pContext);
  ID3D11Device* pDevice = nullptr;
  m_pContext->GetDevice(&pDevice);

  CD3D11_BUFFER_DESC vbDesc(sizeof(TRenderVertex) * 5, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
  pDevice->CreateBuffer(&vbDesc, nullptr, &m_pVBuffer);

  pDevice->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &m_pPShader);

  SAFE_RELEASE(pDevice);
#endif
}

void CRenderD3D::Begin(void)
{
#ifdef WIN32
  m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
  UINT strides = sizeof(TRenderVertex), offsets = 0;
  m_pContext->IASetVertexBuffers(0, 1, &m_pVBuffer, &strides, &offsets);
  m_pContext->PSSetShader(m_pPShader, NULL, 0);
#endif
}

void CRenderD3D::DrawQuad(TRenderVertex* verts)
{
#ifdef WIN32
  TRenderVertex buff[] = { verts[0], verts[1], verts[2], verts[3] };
  D3D11_MAPPED_SUBRESOURCE res = {};
  if (SUCCEEDED(m_pContext->Map(m_pVBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
  {
    memcpy(res.pData, buff, sizeof(buff));
    m_pContext->Unmap(m_pVBuffer, 0);
  }
  m_pContext->Draw(4, 0);
#endif
}

void CRenderD3D::Release(void)
{
#ifdef WIN32
  SAFE_RELEASE(m_pPShader);
  SAFE_RELEASE(m_pVBuffer);
#endif
}
