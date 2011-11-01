
#include "Application.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"
#include "SpriteRendererDX11.h"
#include "SpriteFontDX11.h"

#include "GeometryDX11.h"
#include "MaterialDX11.h"
#include "Camera.h"
#include "Scene.h"
#include "TArray.h"

#include "ViewDeferredRenderer.h"
#include "ViewTextOverlay.h"
#include "AppSettings.h"

using namespace Glyph3;

class App : public Application
{

public:
	App();

public:
	virtual void Initialize();
	virtual void Update();
	virtual void Shutdown();

	virtual bool ConfigureEngineComponents();
	virtual void ShutdownEngineComponents();

	virtual bool HandleEvent( IEvent* pEvent );
	virtual std::wstring GetName( );

protected:


    void DrawHUD();

	RendererDX11*			m_pRenderer11;
	Win32RenderWindow*		m_pWindow;

	ResourcePtr				m_BackBuffer;

    MaterialPtr		        m_pMaterial;
    RenderEffectDX11*       m_pGBufferEffect[GBufferOptMode::NumSettings];
    int					    m_iGBufferDSState;
    int                     m_iGBufferRSState;

    ResourcePtr             m_DiffuseTexture;
    ResourcePtr             m_NormalMap;

	ViewTextOverlay*		m_pTextOverlayView;
	ViewDeferredRenderer*	m_pDeferredView;
	SpriteRendererDX11		m_SpriteRenderer;
	SpriteFontDX11			m_Font;

	Node3D*					m_pNode;
	Entity3D*				m_pEntity;

	Camera*					m_pCamera;

    int                     m_vpWidth;
    int                     m_vpHeight;

	bool					m_bSaveScreenshot;
};