/****************************************************************************
Copyright (c) 2011-2013,WebJet Business Division,CYOU

http://www.genesis-3d.com.cn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#ifndef RENDERPIPELINEMANAGER_H_
#define RENDERPIPELINEMANAGER_H_
#include "graphicsystem/Camera/RenderPipeline/RenderData.h"
#include "graphicsystem/Camera/RenderPipeline/VisibleNode.h"
#include "graphicsystem/Camera/RenderPipeline/ActiveLight.h"

#include "foundation/core/refcounted.h"
#include "foundation/math/bbox.h"
namespace Graphic
{
	class RenderPipelineManager;
	class Camera;
	class Light;
	class ForwardShadingRenderPipeline;
	class DeferredLightingRenderPipeline;
	typedef Util::Array<RenderObject*> RenderCallBacks;

	enum RenderingPiplineType
	{
		eForwordShaderPipeline = 0,
		eLightPrePassShaderPipeline,
		eFixedFunctionPipeline,
		eCustomPipeline,
		eRenderPipelineCount
	};


	class PipelineParamters 
	{
	public:
		RenderConfig    m_renderConfig;
		Camera*			m_camera;
		RenderPipelineManager* m_renderPipelineManager;

		RenderCallBacks m_callBacks;
		VisibleNodeCollection m_visibleNodes;
		RenderDataManager m_renderDatas;
		ActiveLightManager m_activeLights;
	};

	class RenderPipeline;

	class RenderPipelineManager : public Core::RefCounted
	{
		__DeclareClass(RenderPipelineManager)

	public:
		RenderPipelineManager();
		virtual ~RenderPipelineManager();

		void Open();
		void PreRender(Camera* camera);
		void AfterRender();
		void Close();
		void SetupDeferred();
		static void AssignEffectiveLight(PipelineParamters& params);
		static void AssignVisibleNodes(PipelineParamters& params);
		static void AssignRenderDatas(PipelineParamters& params);

		static void OnRenderPipeline(Camera* camera);

		void SetCustomPipeline(const GPtr<RenderPipeline>& customPipeline);
	protected:
		static void renderPipeline(GPtr<RenderPipelineManager>& renderpipemanager);
		static void renderShadowMap(const ActiveLightInfo* aLight);
	public:
		PipelineParamters					m_pipelineContext;
		GPtr<ForwardShadingRenderPipeline> m_forwardPipeline;
		GPtr<DeferredLightingRenderPipeline> m_deferredPipeline;
		GPtr<RenderPipeline>       m_customPipeline;

	};
}
#endif//RENDERPIPELINEMANAGER_H_
