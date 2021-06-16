#include "graphics.h"
#include "trender.h"

IDirect3D7 *trender_dx::d3d;
IDirect3DDevice7 *trender_dx::d3ddev;
	
UINT32 trender_dx::initialize(trendercontext *ctx)
{
	((trendercontext_directx*)ctx)->dd->QueryInterface(IID_IDirect3D7,(void**)&d3d);
	//buildtables();
	return 1;
}
