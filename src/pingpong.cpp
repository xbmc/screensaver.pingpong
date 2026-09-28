/*
 *  Copyright (C) 2005-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2005 Joakim Eriksson <je@plane9.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "main.h"
#include "pingpong.h"

#include <cstddef>
#include <vector>

#define NUMQUADS 3

#define TOPANDBOTTOM    200.0f
#define PADDLEPOSX      20.0f
#define PADDLEMAXSPEED  500.0f

////////////////////////////////////////////////////////////////////////////
//
CPingPong::CPingPong()
{
}

////////////////////////////////////////////////////////////////////////////
//
CPingPong::~CPingPong()
{
  m_renderHelper->m_functions.__imp_glBindBuffer(GL_ARRAY_BUFFER, 0);
  m_renderHelper->m_functions.__imp_glDeleteBuffers(1, &m_vertexVBO);
  m_renderHelper->m_functions.__imp_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  m_renderHelper->m_functions.__imp_glDeleteBuffers(1, &m_indexVBO);
}

bool CPingPong::Init()
{
  m_renderHelper = dynamic_pointer_cast<kodi::gui::CRenderHelper>(kodi::gui::GetRenderHelper());

  std::string fraqShader = kodi::addon::GetAddonPath("resources/shaders/" GL_TYPE_STRING "/frag.glsl");
  std::string vertShader = kodi::addon::GetAddonPath("resources/shaders/" GL_TYPE_STRING "/vert.glsl");
  if (!LoadShaderFiles(vertShader, fraqShader) || !CompileAndLink())
    return false;

  m_renderHelper->m_functions.__imp_glGenBuffers(1, &m_vertexVBO);
  m_renderHelper->m_functions.__imp_glGenBuffers(1, &m_indexVBO);
  return true;
}

////////////////////////////////////////////////////////////////////////////
//
bool CPingPong::RestoreDevice(CRenderD3D* render)
{
  m_Width = render->m_Width;
  m_Height = render->m_Height;
  m_Paddle[0].m_Pos.Set( PADDLEPOSX, render->m_Height/2, 0.0f);
  m_Paddle[1].m_Pos.Set(render->m_Width-PADDLEPOSX, render->m_Height/2, 0.0f);

  m_Ball.m_Pos.Set((render->m_Width-2*PADDLEPOSX)/2.0+PADDLEPOSX, render->m_Height/2, 0.0f);
  topy = 7*render->m_Height/8;
  bottomy = render->m_Height/8;
  
  m_projMat = glm::ortho(0.0f, float(m_Width), float(m_Height), 0.0f);
  return true;
}

////////////////////////////////////////////////////////////////////////////
//
void CPingPong::InvalidateDevice(CRenderD3D* render)
{
  render->Release();
}

////////////////////////////////////////////////////////////////////////////
//
void CPingPong::Update(f32 dt)
{
  // The paddle 'ai'. If you now can call it that
  for (int i = 0; i < 2; i++)
  {
    f32 speed = 0.5f;
    // If the ball is moving toward us then meet up with it (quickly)
    if (DotProduct(m_Ball.m_Vel, m_Paddle[i].m_Pos) > 0.0f)
    {
      speed = 1.0f;
    }

    if (m_Ball.m_Pos.y > m_Paddle[i].m_Pos.y)
      m_Paddle[i].m_Pos.y += PADDLEMAXSPEED*dt*speed;
    else
      m_Paddle[i].m_Pos.y -= PADDLEMAXSPEED*dt*speed;
  }

  // Perform collisions
  if (m_Ball.m_Pos.y > topy)
    m_Ball.m_Vel.y *= -1.0f;
  if (m_Ball.m_Pos.y < bottomy)
    m_Ball.m_Vel.y *= -1.0f;

  if ((m_Ball.m_Pos.x-m_Ball.m_Size.x) < (m_Paddle[0].m_Pos.x+m_Paddle[0].m_Size.x))
    m_Ball.m_Vel.x *= -1.0f;
  if ((m_Ball.m_Pos.x+m_Ball.m_Size.x) > (m_Paddle[1].m_Pos.x-m_Paddle[1].m_Size.x))
    m_Ball.m_Vel.x *= -1.0f;

  m_Ball.m_Pos.x += m_Ball.m_Vel.x*dt;
  m_Ball.m_Pos.y += m_Ball.m_Vel.y*dt;
}

////////////////////////////////////////////////////////////////////////////
//
bool CPingPong::Draw(CRenderD3D* render)
{
  // Fill  in the vertex buffers with the quads
  std::vector<TRenderVertex> vert(4*4);
  TRenderVertex* vert2 = AddQuad(&vert[0], m_Ball.m_Pos, m_Ball.m_Size, m_Ball.m_Col);
  vert2 = AddQuad(vert2, m_Paddle[0].m_Pos, m_Paddle[0].m_Size, m_Paddle[0].m_Col);
  vert2 = AddQuad(vert2, m_Paddle[1].m_Pos, m_Paddle[1].m_Size, m_Paddle[1].m_Col);

  EnableShader();

  GLubyte idx[3*8];
  for (size_t j = 0; j < 4; ++j)
  {
    idx[6*j]   = 4*j;
    idx[6*j+1] = 4*j+1;
    idx[6*j+2] = 4*j+2;
    idx[6*j+3] = 4*j+2;
    idx[6*j+4] = 4*j+3;
    idx[6*j+5] = 4*j;
  }

  m_renderHelper->m_functions.__imp_glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  m_renderHelper->m_functions.__imp_glClear(GL_COLOR_BUFFER_BIT);

  m_renderHelper->m_functions.__imp_glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
  m_renderHelper->m_functions.__imp_glBufferData(GL_ARRAY_BUFFER, sizeof(TRenderVertex)*12, &vert[0], GL_STATIC_DRAW);

  m_renderHelper->m_functions.__imp_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVBO);
  m_renderHelper->m_functions.__imp_glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte)*24, idx, GL_STATIC_DRAW);

  m_renderHelper->m_functions.__imp_glVertexAttribPointer(m_aPosition, 4, GL_FLOAT, 0, sizeof(TRenderVertex), BUFFER_OFFSET(offsetof(TRenderVertex, pos)));
  m_renderHelper->m_functions.__imp_glVertexAttribPointer(m_aColor, 4, GL_FLOAT, 0, sizeof(TRenderVertex), BUFFER_OFFSET(offsetof(TRenderVertex, col)));

  m_renderHelper->m_functions.__imp_glEnableVertexAttribArray(m_aPosition);
  m_renderHelper->m_functions.__imp_glEnableVertexAttribArray(m_aColor);

  m_renderHelper->m_functions.__imp_glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_BYTE, 0);

  m_renderHelper->m_functions.__imp_glDisableVertexAttribArray(m_aPosition);
  m_renderHelper->m_functions.__imp_glDisableVertexAttribArray(m_aColor);

  DisableShader();
  return true;
}

////////////////////////////////////////////////////////////////////////////
// Adds a quad to a vertex buffer
//
TRenderVertex* CPingPong::AddQuad(TRenderVertex* vert, const CVector& pos, const CVector& size, const CRGBA& col)
{
  vert->pos = CVector(pos.x-size.x, pos.y+size.y, 0.0f);  vert->col = col; vert++;
  vert->pos = CVector(pos.x-size.x, pos.y-size.y, 0.0f);  vert->col = col; vert++;
  vert->pos = CVector(pos.x+size.x, pos.y+size.y, 0.0f);  vert->col = col; vert++;
  vert->pos = CVector(pos.x+size.x, pos.y-size.y, 0.0f);  vert->col = col; vert++;
  return vert;
}

void CPingPong::OnCompiledAndLinked()
{
  fprintf(stderr, "-1---------------> %s %p\n", __func__, m_renderHelper->m_functions.__imp_glGetUniformLocation);
  m_uProjMatrix = m_renderHelper->m_functions.__imp_glGetUniformLocation(ProgramHandle(), "u_modelViewProjectionMatrix");
  fprintf(stderr, "-2---------------> %s\n", __func__);
  m_aPosition = m_renderHelper->m_functions.__imp_glGetAttribLocation(ProgramHandle(), "a_position");
  fprintf(stderr, "-3---------------> %s\n", __func__);
  m_aColor = m_renderHelper->m_functions.__imp_glGetAttribLocation(ProgramHandle(), "a_color");
}

bool CPingPong::OnEnabled()
{
  m_renderHelper->m_functions.__imp_glUniformMatrix4fv(m_uProjMatrix, 1, GL_FALSE, glm::value_ptr(m_projMat));
  return true;
}
