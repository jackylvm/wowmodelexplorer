#pragma once

#include "core.h"
#include "SMaterial.h"
#include <algorithm>
#include "ILostResetCallback.h"

struct SBufferParam;
class IRenderTarget;
class IMaterialRenderer;
class IVertexBuffer;
class IIndexBuffer;
class ISceneStateServices;
class IMaterialRenderServices;
class IShaderServices;

enum E_RENDER_MODE
{
	ERM_NONE = 0,
	ERM_2D,
	ERM_3D,
	ERM_STENCIL_FILL,

	ERM_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

struct SDriverSetting
{
	bool vsync;
	u8	antialias;
	u8	quality;
	bool fullscreen;
};

struct SDisplayMode
{
	bool operator==(const SDisplayMode& mode) const
	{
		return 0 == memcmp(this, &mode, sizeof(SDisplayMode));
	}
	u16 width;
	u16 height;
	u8 refreshrate;
};

class CAdapterInfo
{
private:
	DISALLOW_COPY_AND_ASSIGN(CAdapterInfo);

public:
	CAdapterInfo() : index(0), vendorID(0)
	{
		::memset(description, 0, sizeof(description));
		::memset(name, 0, sizeof(name));
		::memset(vendorName, 0, DEFAULT_SIZE);
	}

public:
	u32		index;
	u32		vendorID;
	c8		description[512];
	c8		name[32];
	c8		vendorName[DEFAULT_SIZE];
	
	typedef std::list<SDisplayMode, qzone_allocator<SDisplayMode>>	T_DisplayModeList;
	T_DisplayModeList displayModes;

	void addDisplayMode(const SDisplayMode& mode)
	{
		if(displayModes.end() == std::find(displayModes.begin(), displayModes.end(), mode))
		{
			displayModes.push_back(mode);
		}
	}

	SDisplayMode getCloseMatchDisplayMode(u32 width, u32 height) const
	{
		SDisplayMode ret;
		//select first
		T_DisplayModeList::const_iterator itr = displayModes.begin();
		u32 diff = abs(((s32)itr->width - (s32)width)) + abs(((s32)itr->height - (s32)height));
		ret = (*itr);

		++itr;
		for (; itr != displayModes.end(); ++itr)
		{
			u32 d = abs(((s32)itr->width - (s32)width)) + abs(((s32)itr->height - (s32)height));
			if ( d < diff)
			{
				diff = d;
				ret = (*itr);
			}
		}

		return ret;
	}
};

class IVideoDriver
{
public:
	virtual ~IVideoDriver() { }

public:
	virtual E_DRIVER_TYPE getDriverType() const = 0;
	virtual u32 getAdapterCount() const = 0;

	virtual bool beginScene() = 0;
	virtual bool endScene() = 0;
	virtual bool clear(bool renderTarget, bool zBuffer, bool stencil, SColor color) = 0;

	virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const =0;

	virtual void setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat ) = 0;
	virtual const matrix4& getTransform( E_TRANSFORMATION_STATE state ) const = 0;

	virtual void setMaterial(const SMaterial& material) = 0;
	virtual const SMaterial& getMaterial() const = 0;

	virtual SOverrideMaterial& getOverrideMaterial() = 0;
	virtual void setTexture(u32 stage, ITexture* texture) = 0;
	virtual ITexture* getTexture(u32 index) const= 0;

	virtual bool setRenderTarget( IRenderTarget* texture ) = 0;

	virtual void setViewPort( recti area ) = 0;
	virtual const recti& getViewPort() const = 0;

	virtual void registerLostReset( ILostResetCallback* callback ) = 0;
	virtual void removeLostReset( ILostResetCallback* callback ) = 0;

	virtual void setDisplayMode(const dimension2du& size) = 0;
	virtual dimension2du getDisplayMode() const = 0;
	virtual bool setDriverSetting(const SDriverSetting& setting) = 0;
	virtual const SDriverSetting& getDriverSetting() const = 0;

	virtual ISceneStateServices* getSceneStateServices() const = 0;
	virtual IMaterialRenderServices* getMaterialRenderServices() const = 0;
	virtual IShaderServices*	getShaderServices() const = 0;

	//draw
	virtual void draw3DMode(const SBufferParam& bufferParam, 
		E_PRIMITIVE_TYPE primType,
		u32 primCount, 
		const SDrawParam& drawParam) = 0;

	virtual void draw2DMode(const SBufferParam& bufferParam, 
		E_PRIMITIVE_TYPE primType,
		u32 primCount, 
		const SDrawParam& drawParam,
		const S2DBlendParam& blendParam) = 0;
	
	virtual void drawDebugInfo(const c8* strMsg) = 0;
};
