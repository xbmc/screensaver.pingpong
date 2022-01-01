/*
 *  Copyright (C) 2005-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2005 Joakim Eriksson <je@plane9.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "main.h"
#include "pingpong.h"
#include "timer.h"

#include <kodi/addon-instance/Screensaver.h>
#include <time.h>
#include <string.h>

class ATTR_DLL_LOCAL CScreensaverPingPong
  : public kodi::addon::CAddonBase,
    public kodi::addon::CInstanceScreensaver
{
public:
  CScreensaverPingPong();

  virtual bool Start() override;
  virtual void Stop() override;
  virtual void Render() override;

private:
  CPingPong m_pingPong;
  CTimer* m_timer;
  CRenderD3D m_render;
  CRGBA m_col[3];
  float m_ballspeed[2];
};

////////////////////////////////////////////////////////////////////////////
// Kodi has loaded us into memory, we should set our core values
// here and load any settings we may have from our config file
//
CScreensaverPingPong::CScreensaverPingPong()
  : m_timer(nullptr)
{
  static const float C[5][3] = {{1.0, 1.0, 1.0},
                                {0.5, 0.5, 0.5},
                                {1.0, 0.0, 0.0},
                                {0.0, 1.0, 0.0},
                                {0.0, 0.0, 1.0}};
  int c = kodi::addon::GetSettingInt("paddle1");
  if (c < 5)
    m_col[0].Set(C[c][0], C[c][1], C[c][2], 1.0);
  c = kodi::addon::GetSettingInt("paddle2");
  if (c < 5)
    m_col[1].Set(C[c][0], C[c][1], C[c][2], 1.0);
  c = kodi::addon::GetSettingInt("ball");
  if (c < 5)
    m_col[2].Set(C[c][0], C[c][1], C[c][2], 1.0);

  m_ballspeed[0] = kodi::addon::GetSettingFloat("ballspeedx");
  m_ballspeed[1] = kodi::addon::GetSettingFloat("ballspeedy");

  m_render.m_Width = Width();
  m_render.m_Height = Height();
  m_render.Init(Device());
}

////////////////////////////////////////////////////////////////////////////
// Kodi tells us we should get ready to start rendering. This function
// is called once when the screensaver is activated by Kodi.
//
bool CScreensaverPingPong::Start()
{
  srand(time(NULL));

  m_pingPong.m_Paddle[0].m_Col = m_col[0];
  m_pingPong.m_Paddle[1].m_Col = m_col[1];
  m_pingPong.m_Ball.m_Col = m_col[2];
  m_pingPong.m_Ball.m_Vel.Set(m_ballspeed[0]*m_render.m_Width, m_ballspeed[1]*m_render.m_Height, 0.0);

  m_timer = new CTimer();
  m_timer->Init();
  if (!m_pingPong.RestoreDevice(&m_render))
  {
    Stop();
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////
// Kodi tells us to stop the screensaver we should free any memory and release
// any resources we have created.
//
void CScreensaverPingPong::Stop()
{
  m_pingPong.InvalidateDevice(&m_render);
  SAFE_DELETE(m_timer);
}

////////////////////////////////////////////////////////////////////////////
// Kodi tells us to render a frame of our screensaver. This is called on
// each frame render in Kodi, you should render a single frame only - the DX
// device will already have been cleared.
//
void CScreensaverPingPong::Render()
{
  m_render.Begin();
  m_timer->Update();
  m_pingPong.Update(m_timer->GetDeltaTime());
  m_pingPong.Draw(&m_render);
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

ADDONCREATOR(CScreensaverPingPong);
