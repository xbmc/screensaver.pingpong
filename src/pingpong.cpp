////////////////////////////////////////////////////////////////////////////
//
// PingPong Screensaver for XBox Media Center
// Copyright (c) 2005 Joakim Eriksson <je@plane9.com>
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

#ifdef HAS_GLES2
#if defined(__APPLE__)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif//__APPLE__
#elif defined(__APPLE__)
#include <OpenGL/gl.h>
#elif !defined(WIN32)
#include <GL/gl.h>
#endif

#include <vector>

#define NUMQUADS	3

#define TOPANDBOTTOM	200.0f
#define PADDLEPOSX	20.0f
#define PADDLEMAXSPEED	500.0f

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

////////////////////////////////////////////////////////////////////////////
//
CPingPong::CPingPong()
{
#ifndef WIN32
  m_shader = new CGUIShader("vert.glsl", "frag.glsl");
  m_shader->CompileAndLink();
  glGenBuffers(1, &m_vertexVBO);
  glGenBuffers(1, &m_indexVBO);
#endif
}

////////////////////////////////////////////////////////////////////////////
//
CPingPong::~CPingPong()
{
#ifndef WIN32
  delete m_shader;
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &m_vertexVBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &m_indexVBO);
#endif
}

////////////////////////////////////////////////////////////////////////////
//
bool	CPingPong::RestoreDevice(CRenderD3D* render)
{
  m_Width = render->m_Width;
  m_Height = render->m_Height;
	m_Paddle[0].m_Pos.Set( PADDLEPOSX, render->m_Height/2, 0.0f);
	m_Paddle[1].m_Pos.Set(render->m_Width-PADDLEPOSX, render->m_Height/2, 0.0f);

	m_Ball.m_Pos.Set((render->m_Width-2*PADDLEPOSX)/2.0+PADDLEPOSX, render->m_Height/2, 0.0f);
  topy = 7*render->m_Height/8;
  bottomy = render->m_Height/8;
	return true;
}

////////////////////////////////////////////////////////////////////////////
//
void	CPingPong::InvalidateDevice(CRenderD3D* render)
{
  render->Release();
}

////////////////////////////////////////////////////////////////////////////
//
void		CPingPong::Update(f32 dt)
{
	// The paddle 'ai'. If you now can call it that
	for (int i=0; i<2; i++)
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
	if (m_Ball.m_Pos.y > topy)		m_Ball.m_Vel.y *= -1.0f;
	if (m_Ball.m_Pos.y < bottomy)		m_Ball.m_Vel.y *= -1.0f;

	if ((m_Ball.m_Pos.x-m_Ball.m_Size.x) < (m_Paddle[0].m_Pos.x+m_Paddle[0].m_Size.x))	m_Ball.m_Vel.x *= -1.0f;
	if ((m_Ball.m_Pos.x+m_Ball.m_Size.x) > (m_Paddle[1].m_Pos.x-m_Paddle[1].m_Size.x))	m_Ball.m_Vel.x *= -1.0f;
	
	m_Ball.m_Pos.x += m_Ball.m_Vel.x*dt; 
        m_Ball.m_Pos.y += m_Ball.m_Vel.y*dt;
}

////////////////////////////////////////////////////////////////////////////
//
bool		CPingPong::Draw(CRenderD3D* render)
{
	// Fill	in the vertex buffers with the quads
        std::vector<TRenderVertex> vert(4*4);
	TRenderVertex* vert2 = AddQuad(&vert[0], m_Ball.m_Pos, m_Ball.m_Size, m_Ball.m_Col);
	vert2 = AddQuad(vert2, m_Paddle[0].m_Pos, m_Paddle[0].m_Size, m_Paddle[0].m_Col);
	vert2 = AddQuad(vert2, m_Paddle[1].m_Pos, m_Paddle[1].m_Size, m_Paddle[1].m_Col);

#ifndef WIN32
  m_shader->PushMatrix();
  m_shader->Enable();

  struct PackedVertex
  {
    GLfloat x, y, z;
    GLfloat r, g, b;
  } vertex[12];

  GLubyte idx[3*8];
  for (size_t j = 0; j < 12; ++j)
  {
    vertex[j].x = -1.0 + vert[j].pos.x * 2.0 / m_Width;
    vertex[j].y = -1.0 + vert[j].pos.y * 2.0 / m_Height;
    vertex[j].z = 0.0;
    vertex[j].r = vert[j].col.r;
    vertex[j].g = vert[j].col.g;
    vertex[j].b = vert[j].col.b;
  }

  for (size_t j = 0; j < 4; ++j)
  {
    idx[6*j]   = 4*j;
    idx[6*j+1] = 4*j+1;
    idx[6*j+2] = 4*j+2;
    idx[6*j+3] = 4*j+2;
    idx[6*j+4] = 4*j+3;
    idx[6*j+5] = 4*j;
  }

  GLint posLoc = m_shader->GetPosLoc();
  GLint colLoc = m_shader->GetColLoc();

  glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(PackedVertex)*12, &vertex[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte)*24, idx, GL_STATIC_DRAW);

  glVertexAttribPointer(posLoc, 3, GL_FLOAT, 0, sizeof(PackedVertex), BUFFER_OFFSET(offsetof(PackedVertex, x)));
  glVertexAttribPointer(colLoc, 3, GL_FLOAT, 0, sizeof(PackedVertex), BUFFER_OFFSET(offsetof(PackedVertex, r)));

  glEnableVertexAttribArray(posLoc);
  glEnableVertexAttribArray(colLoc);

  glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_BYTE, 0);

  glDisableVertexAttribArray(posLoc);
  glDisableVertexAttribArray(colLoc);

  m_shader->Disable();
  m_shader->PopMatrix();
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
TRenderVertex*	CPingPong::AddQuad(TRenderVertex* vert, const CVector& pos, const CVector& size, const CRGBA& col)
{
	vert->pos =	CVector(pos.x-size.x, pos.y+size.y, 0.0f);	vert->col =	col; vert++;
	vert->pos =	CVector(pos.x-size.x, pos.y-size.y, 0.0f);	vert->col =	col; vert++;
	vert->pos =	CVector(pos.x+size.x, pos.y+size.y, 0.0f);	vert->col =	col; vert++;
	vert->pos =	CVector(pos.x+size.x, pos.y-size.y, 0.0f);	vert->col =	col; vert++;
	return vert;
}







