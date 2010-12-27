//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// ViewLights
//
//--------------------------------------------------------------------------------
#ifndef ViewLights_h
#define ViewLights_h
//--------------------------------------------------------------------------------
#include "IRenderView.h"
#include "GeometryDX11.h"
#include "RenderEffectDX11.h"
#include "AppSettings.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
    // Light type
    enum LightType
    {
        Point = 0,
        Spot = 1,
        Directional = 2,

        NumLightTypes
    };

    // Light parameters
    struct Light
    {
        Vector3f Position;
        Vector3f Color;
        Vector3f Direction;
        float Range;
        float SpotInnerAngle;
        float SpotOuterAngle;
        LightType Type;

        Light() : Position( 0.0f, 0.0f, 0.0f ), Color( 1.0f, 1.0f, 1.0f ), Direction( -1.0f, -1.0f, 1.0f ),
                Range( 2.0f ), SpotInnerAngle( 0 ), SpotOuterAngle ( 0 ), Type ( Point ) {}
    };

    class ViewLights : public IRenderView
    {
    public:

        // Maximum number of lights supported
        static const int MaxNumLights = 1000;

    public:
        ViewLights( RendererDX11& Renderer );

        virtual void Update( float fTime );
        virtual void PreDraw( RendererDX11* pRenderer );
        virtual void Draw( PipelineManagerDX11* pPipelineManager, ParameterManagerDX11* pParamManager );

        virtual void SetRenderParams( ParameterManagerDX11* pParamManager );
        virtual void SetUsageParams( ParameterManagerDX11* pParamManager );

        void AddLight( const Light& light );
        void SetTargets( ResourcePtr GBufferTarget, ResourcePtr pRenderTarget,
                            ResourcePtr DepthTarget, int Viewport);
        void SetClipPlanes( float NearClip, float FarClip );

        virtual ~ViewLights();

    protected:

        int					m_iViewport;
        int					m_iDisabledDSState;
        int                 m_iLessThanDSState;
        int                 m_iGreaterThanDSState;
        int                 m_iBlendState;
        int                 m_iBackFaceCullRSState;

        float               m_fNearClip;
        float               m_fFarClip;

        ResourcePtr	            m_pRenderTarget;
        ResourcePtr			    m_pDepthTarget;
        ResourcePtr             m_pGBufferTarget;

        ResourcePtr             m_pVertexBuffer;        

        RenderEffectDX11		m_PointLightEffect[2];
        RenderEffectDX11		m_SpotLightEffect[2];
        RenderEffectDX11		m_DirectionalLightEffect[2];

        int                     m_iPointLightIL;
        int                     m_iSpotLightIL;
        int                     m_iDirectionalLightIL;

        TArray<Light>           m_PointLights;
        TArray<Light>           m_SpotLights;
        TArray<Light>           m_DirectionalLights;
        Matrix4f                m_WorldMatrix;
    };
}
//--------------------------------------------------------------------------------
#endif // ViewLights_h