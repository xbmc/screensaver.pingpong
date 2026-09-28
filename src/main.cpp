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
  fprintf(stderr, "----------------> %s\n", __func__);
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
  fprintf(stderr, "----------------> %s\n", __func__);
  srand(time(NULL));

  m_pingPong.Init();
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
  fprintf(stderr, "----------------> %s\n", __func__);
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
  //fprintf(stderr, "----------------> %s\n", __func__);
  m_render.Begin();
  m_timer->Update();
  m_pingPong.Update(m_timer->GetDeltaTime());
  m_pingPong.Draw(&m_render);
}

void CRenderD3D::Init(void* pContext)
{
}

void CRenderD3D::Begin(void)
{
}

void CRenderD3D::ClearColor()
{
}

void CRenderD3D::DrawQuad(TRenderVertex* verts)
{
}

void CRenderD3D::Release(void)
{
}

ADDONCREATOR(CScreensaverPingPong);
