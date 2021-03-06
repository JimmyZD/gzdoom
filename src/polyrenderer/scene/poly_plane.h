/*
**  Polygon Doom software renderer
**  Copyright (c) 2016 Magnus Norddahl
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
*/

#pragma once

#include "polyrenderer/drawers/poly_triangle.h"

class PolyDrawSectorPortal;

class PolyPlaneUVTransform
{
public:
	PolyPlaneUVTransform(const FTransform &transform, FTexture *tex);

	TriVertex GetVertex(vertex_t *v1, double height) const
	{
		TriVertex v;
		v.x = (float)v1->fX();
		v.y = (float)v1->fY();
		v.z = (float)height;
		v.w = 1.0f;
		v.u = GetU(v.x, v.y);
		v.v = GetV(v.x, v.y);
		return v;
	}

private:
	float GetU(float x, float y) const { return (xOffs + x * cosine - y * sine) * xscale; }
	float GetV(float x, float y) const { return (yOffs - x * sine - y * cosine) * yscale; }

	float xscale;
	float yscale;
	float cosine;
	float sine;
	float xOffs, yOffs;
};

class RenderPolyPlane
{
public:
	static void RenderPlanes(PolyRenderThread *thread, const TriMatrix &worldToClip, const PolyClipPlane &clipPlane, subsector_t *sub, uint32_t stencilValue, double skyCeilingHeight, double skyFloorHeight, std::vector<std::unique_ptr<PolyDrawSectorPortal>> &sectorPortals);

private:
	void Render(PolyRenderThread *thread, const TriMatrix &worldToClip, const PolyClipPlane &clipPlane, subsector_t *sub, uint32_t stencilValue, bool ceiling, double skyHeight, std::vector<std::unique_ptr<PolyDrawSectorPortal>> &sectorPortals);
	void RenderSkyWalls(PolyRenderThread *thread, PolyDrawArgs &args, subsector_t *sub, sector_t *frontsector, FSectorPortal *portal, PolyDrawSectorPortal *polyportal, bool ceiling, double skyHeight, const PolyPlaneUVTransform &transform);
	void SetDynLights(PolyRenderThread *thread, PolyDrawArgs &args, subsector_t *sub);
};

class Render3DFloorPlane
{
public:
	static void RenderPlanes(PolyRenderThread *thread, const TriMatrix &worldToClip, const PolyClipPlane &clipPlane, subsector_t *sub, uint32_t stencilValue, uint32_t subsectorDepth, std::vector<PolyTranslucentObject *> &translucentObjects);

	void Render(PolyRenderThread *thread, const TriMatrix &worldToClip, const PolyClipPlane &clipPlane);

	subsector_t *sub = nullptr;
	uint32_t stencilValue = 0;
	bool ceiling = false;
	F3DFloor *fakeFloor = nullptr;
	bool Masked = false;
	bool Additive = false;
	double Alpha = 1.0;
};

class PolyTranslucent3DFloorPlane : public PolyTranslucentObject
{
public:
	PolyTranslucent3DFloorPlane(Render3DFloorPlane plane, uint32_t subsectorDepth) : PolyTranslucentObject(subsectorDepth, 1e7), plane(plane) { }

	void Render(PolyRenderThread *thread, const TriMatrix &worldToClip, const PolyClipPlane &portalPlane) override
	{
		plane.Render(thread, worldToClip, portalPlane);
	}

	Render3DFloorPlane plane;
};
