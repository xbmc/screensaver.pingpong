////////////////////////////////////////////////////////////////////////////
//
// Author:
//   Joakim Eriksson
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#ifdef WIN32
#include <d3d11.h>
#endif

/***************************** D E F I N E S *******************************/
/****************************** M A C R O S ********************************/
/***************************** C L A S S E S *******************************/

////////////////////////////////////////////////////////////////////////////
// 
struct TRenderVertex;

class CRenderD3D
{
public:
  void  Init(void* pContext);
  void  DrawQuad(TRenderVertex* verts);
  void  Begin(void);
  void  Release(void);

#ifdef WIN32
  ID3D11DeviceContext* m_pContext;
  ID3D11Buffer*        m_pVBuffer;
  ID3D11PixelShader*   m_pPShader;
#endif

	int			m_Width;
	int			m_Height;

};

/***************************** G L O B A L S *******************************/

void LoadSettings();
void SetDefaults();

/***************************** I N L I N E S *******************************/
