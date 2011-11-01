//--------------------------------------------------------------------------------
#include "App.h"
#include "Log.h"

#include <sstream>

#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtChar.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"

#include "ScriptManager.h"

#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"

#include "GeometryLoaderDX11.h"
#include "GeometryGeneratorDX11.h"
#include "MaterialGeneratorDX11.h"
#include "RasterizerStateConfigDX11.h"

#include "IParameterManager.h"

#include "GeometryGeneratorDX11.h"
#include "ShaderResourceParameterWriterDX11.h"
#include "RotationController.h"

using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
App::App()
{
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	return( ConfigureRenderingEngineComponents( 800, 600, D3D_FEATURE_LEVEL_11_0 ) );
}
//--------------------------------------------------------------------------------
void App::ShutdownEngineComponents()
{
	if ( m_pRenderer11 )
	{
		m_pRenderer11->Shutdown();
		delete m_pRenderer11;
	}

	if ( m_pWindow )
	{
		m_pWindow->Shutdown();
		delete m_pWindow;
	}
}
//--------------------------------------------------------------------------------
void App::Initialize()
{

	// Create the light parameters for use with this effect

	Vector4f LightParams = Vector4f( 0.2f, 0.7f, 0.2f, 0.7f );
	m_pLightColor = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightColor" ) );
	m_pLightColor->InitializeParameterData( &LightParams );

	Vector4f LightPosition = Vector4f( -1000.0f, 200.0f, 0.0f, 0.0f );
	m_pLightPosition = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightPositionWS" ) );
	m_pLightPosition->InitializeParameterData( &LightPosition );


	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera->GetNode()->Position() = Vector3f( 0.0f, 50.0f, -20.0f );
	m_pCamera->GetNode()->Rotation().Rotation( Vector3f( 0.7f, 0.0f, 0.0f ) );
	m_pRenderView->SetBackColor( Vector4f( 0.1f, 0.1f, 0.3f, 0.0f ) );


	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of 
	// manually manipulating it.

	m_pNode = new Node3D();


	// Create the displaced skinned actor

	m_pDisplacedActor = new SkinnedActor();
	GeometryPtr pGeometry = GeometryPtr( new GeometryDX11() );
	GeometryGeneratorDX11::GenerateWeightedSkinnedCone( pGeometry, 16, 20, 2.0f, 40.0f, 6, m_pDisplacedActor );
	pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );
	m_pDisplacedActor->GetBody()->SetMaterial( MaterialGeneratorDX11::GenerateSkinnedSolid( *m_pRenderer11 ) );

	RotationController* pRotController1 = new RotationController();
	m_pDisplacedActor->GetNode()->AttachController( pRotController1 );

	
	// Create the skinned actor without displacement

	m_pSkinnedActor = new SkinnedActor();
	GeometryPtr pSkinnedGeometry = GeometryPtr( new GeometryDX11() );
	GeometryGeneratorDX11::GenerateWeightedSkinnedCone( pSkinnedGeometry, 16, 20, 2.0f, 40.0f, 6, m_pSkinnedActor );
	pSkinnedGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_pSkinnedActor->GetBody()->SetMaterial( MaterialGeneratorDX11::GenerateSkinnedTextured( *m_pRenderer11 ) );

	RotationController* pRotController2 = new RotationController();
	m_pSkinnedActor->GetNode()->AttachController( pRotController2 );

	
	// Generate the static mesh, and attach a texture to its entity

	m_pStaticActor = new Actor();
	GeometryPtr pStaticGeometry = GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"box.ms3d" ) );
	pStaticGeometry->LoadToBuffers();
	MaterialPtr pStaticMaterial = MaterialGeneratorDX11::GenerateStaticTextured(*RendererDX11::Get());
	m_pStaticActor->GetBody()->SetGeometry( pStaticGeometry );
	m_pStaticActor->GetBody()->SetMaterial( pStaticMaterial );

	RotationController* pRotController3 = new RotationController();
	m_pStaticActor->GetBody()->AttachController( pRotController3);

	
	ResourcePtr ColorTexture = RendererDX11::Get()->LoadTexture( L"Tiles.png" );
	ShaderResourceParameterWriterDX11* pTextureParameter = new ShaderResourceParameterWriterDX11();
	pTextureParameter->SetRenderParameterRef(
		(RenderParameterDX11*)m_pRenderer11->m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"ColorTexture" ) ) );
	pTextureParameter->SetValue( ColorTexture );
	m_pStaticActor->GetBody()->Parameters.AddRenderParameter( pTextureParameter );


	// Attach the actors to the scene, so that they will be rendered all together.

	m_pNode->AttachChild( m_pDisplacedActor->GetNode() );
	m_pNode->AttachChild( m_pSkinnedActor->GetNode() );
	m_pNode->AttachChild( m_pStaticActor->GetNode() );

	m_pScene->AddEntity( m_pNode );


	// Setup the skinned actors' bind poses and start their animations.

	m_pDisplacedActor->SetBindPose();
	m_pDisplacedActor->SetSkinningMatrices( *m_pRenderer11 );
	m_pDisplacedActor->PlayAllAnimations();

	m_pSkinnedActor->SetBindPose();
	m_pSkinnedActor->SetSkinningMatrices( *m_pRenderer11 );
	m_pSkinnedActor->PlayAllAnimations();

	m_pStaticActor->GetBody()->Position() = Vector3f( -20.0f, 10.0f, 15.0f );
	m_pSkinnedActor->GetNode()->Position() = Vector3f( 0.0f, 0.0f, 20.0f );
	m_pDisplacedActor->GetNode()->Position() = Vector3f( 20.0f, 0.0f, 20.0f );
}
//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();


	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EventManager::Get()->ProcessEvent( new EvtFrameStart( *m_pTimer ) );


	std::wstringstream out;
	out << L"Hieroglyph 3 : Skin and Bones" << std::endl;
	out << L"The static mesh represents rigid geometry, while skinned meshes can be animated." << std::endl;
	out << L"Each node in the skinned meshes displays its coordinate axes." << std::endl;
	out << L"To start the animations press 'A'" << std::endl;

	m_pTextOverlayView->WriteText( out.str(), Matrix4f::Identity(), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	// Update the scene, and then render all cameras within the scene.

	m_pScene->Update( m_pTimer->Elapsed() );

	m_pDisplacedActor->SetSkinningMatrices( *m_pRenderer11 );
	m_pSkinnedActor->SetSkinningMatrices( *m_pRenderer11 );

	m_pScene->Render( m_pRenderer11 );


	// Perform the rendering and presentation for the window.

	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );


	// Save a screenshot if desired.  This is done by pressing the 's' key, which
	// demonstrates how an event is sent and handled by an event listener (which
	// in this case is the application object itself).

	if ( m_bSaveScreenshot  )
	{
		m_bSaveScreenshot = false;
		m_pRenderer11->pImmPipeline->SaveTextureScreenShot( 0, GetName(), D3DX11_IFF_BMP );
	}
}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	SAFE_DELETE( m_pStaticActor );
	SAFE_DELETE( m_pSkinnedActor );
	SAFE_DELETE( m_pDisplacedActor );
	
	SAFE_DELETE( m_pNode );
	SAFE_DELETE( m_pCamera );

	// Print the framerate out for the log before shutting down.

	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write( out.str() );
}
//--------------------------------------------------------------------------------
bool App::HandleEvent( IEvent* pEvent )
{
	eEVENT e = pEvent->GetEventType();

	if ( e == SYSTEM_KEYBOARD_KEYDOWN )
	{
		EvtKeyDown* pKeyDown = (EvtKeyDown*)pEvent;

		unsigned int key = pKeyDown->GetCharacterCode();

		return( true );
	}
	else if ( e == SYSTEM_KEYBOARD_KEYUP )
	{
		EvtKeyUp* pKeyUp = (EvtKeyUp*)pEvent;

		unsigned int key = pKeyUp->GetCharacterCode();

		if ( key == VK_ESCAPE ) // 'Esc' Key - Exit the application
		{
			this->RequestTermination();
			return( true );
		}
		else if ( key == 0x41 ) // 'A' Key - restart animations
		{
			m_pDisplacedActor->PlayAllAnimations();
			m_pSkinnedActor->PlayAllAnimations();
			return( true );
		}
		else if ( key == 0x53 ) // 'S' Key - Save a screen shot for the next frame
		{
			m_bSaveScreenshot = true;
			return( true );
		}
		else
		{
			return( false );
		}
	}

	// Call the parent class's event handler if we haven't handled the event.

	return( RenderApplication::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"SkinAndBones" ) );
}
//--------------------------------------------------------------------------------