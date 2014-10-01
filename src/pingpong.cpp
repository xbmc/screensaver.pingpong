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

#include <GL/gl.h>
#include <vector>

#define NUMQUADS	3

#define TOPANDBOTTOM	200.0f
#define PADDLEPOSX	20.0f
#define PADDLEMAXSPEED	500.0f

////////////////////////////////////////////////////////////////////////////
//
CPingPong::CPingPong()
{
}

////////////////////////////////////////////////////////////////////////////
//
CPingPong::~CPingPong()
{
}

////////////////////////////////////////////////////////////////////////////
//
bool	CPingPong::RestoreDevice(CRenderD3D* render)
{
	m_Paddle[0].m_Pos.Set( PADDLEPOSX, render->m_Height/2, 0.0f);
	m_Paddle[1].m_Pos.Set(render->m_Width-PADDLEPOSX, render->m_Height/2, 0.0f);

	m_Ball.m_Pos.Set((render->m_Width-2*PADDLEPOSX)/2.0+PADDLEPOSX, render->m_Height/2, 0.0f);
        topy = 3*render->m_Height/4;
        bottomy = render->m_Height/4;
	return true;
}

////////////////////////////////////////////////////////////////////////////
//
void	CPingPong::InvalidateDevice(CRenderD3D* render)
{
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

        glBegin(GL_QUADS);
        for (size_t j=0;j<12;++j)
        {
          glColor3f(vert[j].col.r, vert[j].col.g, vert[j].col.b);
          glVertex2f(vert[j].pos.x, vert[j].pos.y);
        }
        glEnd();

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







