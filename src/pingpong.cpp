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
#ifndef WIN32
  std::string fraqShader = kodi::GetAddonPath("resources/shaders/" GL_TYPE_STRING "/frag.glsl");
  std::string vertShader = kodi::GetAddonPath("resources/shaders/" GL_TYPE_STRING "/vert.glsl");
  if (!LoadShaderFiles(vertShader, fraqShader) || !CompileAndLink())
    return;
  
  glGenBuffers(1, &m_vertexVBO);
  glGenBuffers(1, &m_indexVBO);
#endif
}

////////////////////////////////////////////////////////////////////////////
//
CPingPong::~CPingPong()
{
#ifndef WIN32
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &m_vertexVBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &m_indexVBO);
#endif
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
  
#ifndef WIN32
  m_projMat = glm::ortho(0.0f, float(m_Width), float(m_Height), 0.0f);
#endif
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

#ifndef WIN32
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

  glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(TRenderVertex)*12, &vert[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte)*24, idx, GL_STATIC_DRAW);

  glVertexAttribPointer(m_aPosition, 4, GL_FLOAT, 0, sizeof(TRenderVertex), BUFFER_OFFSET(offsetof(TRenderVertex, pos)));
  glVertexAttribPointer(m_aColor, 4, GL_FLOAT, 0, sizeof(TRenderVertex), BUFFER_OFFSET(offsetof(TRenderVertex, col)));

  glEnableVertexAttribArray(m_aPosition);
  glEnableVertexAttribArray(m_aColor);

  glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_BYTE, 0);

  glDisableVertexAttribArray(m_aPosition);
  glDisableVertexAttribArray(m_aColor);

  DisableShader();
#else
  render->DrawQuad(&vert[0]);
  render->DrawQuad(&vert[4]);
  render->DrawQuad(&vert[8]);
#endif
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


#ifndef WIN32
void CPingPong::OnCompiledAndLinked()
{
  m_uProjMatrix = glGetUniformLocation(ProgramHandle(), "u_modelViewProjectionMatrix");
  m_aPosition = glGetAttribLocation(ProgramHandle(), "a_position");
  m_aColor = glGetAttribLocation(ProgramHandle(), "a_color");
}

bool CPingPong::OnEnabled()
{
  glUniformMatrix4fv(m_uProjMatrix, 1, GL_FALSE, glm::value_ptr(m_projMat));
  return true;
}
#endif
