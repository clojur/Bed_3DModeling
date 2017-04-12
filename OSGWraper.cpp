// MFC_OSG.cpp : implementation of the CDbOSGManager class
//
#include "stdafx.h"
 
#include <osgfx/Scribe>
#include <process.h>
#include <osgutil/Tessellator>
#include <osgUtil/SmoothingVisitor>
#include <osg/PolygonMode>
#include <osg/Viewport>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Quat>
#include <osg/LightModel>
#include <osg/Point>
#include "osg/BlendFunc"
#include <osgGA/StateSetManipulator>
#include "osgGA/guieventadapter"
#include <osg/Multisample>
#include "2Dcameramanipulator.h"
#include "Pickhandler.h"
#include "BoxSelector.h"
#include "MoveObjHandler.h"
#include "osgCopyHandler.h"
#include "osgArrayHandler.h"
#include "osgExtendHandler.h"
#include "boxzoom.h"
#include "osgMirrorHandler.h"
#include "dbmanipulator.h"
#include "DistHandler.h"
#include "SetupScene.h"
#include "osgBaseView.h"





#include "OSGWraper.h"

#include "osg/DisplaySettings"
#include "osgDB/ReadFile"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

 

CDbOSGManager::CDbOSGManager(CView* pView ):CDbOSGManagerInterface( pView)
{ 
	m_pControlData = NULL;
	mViewer = NULL;
 }

CDbOSGManager::~CDbOSGManager()
{
	if (mViewer)
	{
		mViewer->setDone(true);
#ifdef _DEBUG
		//Sleep(1000);zmz
#else
		//Sleep(500);zmz
#endif		
		mViewer->stopThreading();
	}
	delete mViewer;
 
	if (m_pControlData)
	{
		if (CViewControlData::GetGlobalHandler())
		{
			if (CViewControlData::GetGlobalHandler()->m_pOSG == m_pControlData)
			{
				CViewControlData::SetGlobalHandler(NULL);
			} 
		}
	}
	delete m_pControlData;
}
void InitGLExtends(unsigned int contextID)
{
	if (1)
	{
		const char* version = (const char*)glGetString(GL_VERSION);
		if (!version)
		{
			//refactor by wr
			//db_print_info(MT_ERROR, " OpenGL version test failed, requires valid graphics context.");
			return;
		}

		float glVersion = osg::findAsciiToFloat(version);
		float glslLanguageVersion = 0.0f;

		bool shadersBuiltIn = OSG_GLES2_FEATURES || OSG_GL3_FEATURES;

		bool isShaderObjectsSupported = shadersBuiltIn || osg::isGLExtensionSupported(contextID, "GL_ARB_shader_objects");
		bool isVertexShaderSupported = shadersBuiltIn || osg::isGLExtensionSupported(contextID, "GL_ARB_vertex_shader");
		bool isFragmentShaderSupported = shadersBuiltIn || osg::isGLExtensionSupported(contextID, "GL_ARB_fragment_shader");
		bool isLanguage100Supported = shadersBuiltIn || osg::isGLExtensionSupported(contextID, "GL_ARB_shading_language_100");
		bool isGeometryShader4Supported = osg::isGLExtensionSupported(contextID, "GL_EXT_geometry_shader4");
		bool isGpuShader4Supported = osg::isGLExtensionSupported(contextID, "GL_EXT_gpu_shader4");
		bool areTessellationShadersSupported = osg::isGLExtensionSupported(contextID, "GL_ARB_tessellation_shader");
		bool isUniformBufferObjectSupported = osg::isGLExtensionSupported(contextID, "GL_ARB_uniform_buffer_object");
		bool isGetProgramBinarySupported = osg::isGLExtensionSupported(contextID, "GL_ARB_get_program_binary");
		bool isGpuShaderFp64Supported = osg::isGLExtensionSupported(contextID, "GL_ARB_gpu_shader_fp64");
		bool isShaderAtomicCountersSupported = osg::isGLExtensionSupported(contextID, "GL_ARB_shader_atomic_counters");

		bool isRectangleSupported = OSG_GL3_FEATURES ||
			osg::isGLExtensionSupported(contextID, "GL_ARB_texture_rectangle") ||
			osg::isGLExtensionSupported(contextID, "GL_EXT_texture_rectangle") ||
			osg::isGLExtensionSupported(contextID, "GL_NV_texture_rectangle");

		bool isCubeMapSupported = OSG_GLES2_FEATURES || OSG_GL3_FEATURES ||
			osg::isGLExtensionSupported(contextID, "GL_ARB_texture_cube_map") ||
			osg::isGLExtensionSupported(contextID, "GL_EXT_texture_cube_map") ||
			(glVersion >= 1.3f);

		bool isClipControlSupported = osg::isGLExtensionSupported(contextID, "GL_ARB_clip_control") ||
			(glVersion >= 4.5f);


		bool  isGlslSupported = (glVersion >= 2.0f) ||
			(isShaderObjectsSupported &&
			isVertexShaderSupported &&
			isFragmentShaderSupported &&
			isLanguage100Supported);

		if (isGlslSupported)
		{
			// If glGetString raises an error, assume initial release "1.00"
			while (glGetError() != GL_NO_ERROR) {}        // reset error flag

			const char* langVerStr = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
			if ((glGetError() == GL_NO_ERROR) && langVerStr)
			{
				glslLanguageVersion = (osg::findAsciiToFloat(langVerStr));
			}
			else
				glslLanguageVersion = 1.0f;
		}

		std::stringstream oisStr;
		oisStr << "glVersion=" << glVersion << ", "
			<< "isGlslSupported=" << (isGlslSupported ? "YES" : "NO") << ", "
			<< "glslLanguageVersion=" << glslLanguageVersion
			<< std::endl;
		//refacor by wr
		//db_print_info(MT_INFO, oisStr.str().c_str());
	}
}

void CDbOSGManager::InitOSG(CViewControlData* pControlData)
{
 	//创建单视图
	InitViews();

	//数据指针赋值
	m_pControlData = pControlData;
	if(m_pControlData)
	{
		m_pControlData->mView = mViewer;
		m_pControlData->mViewer = mViewer;
		m_pControlData->m_Osg   = this;
	 
		//建立图形环境，启动仿真循环
		SetupWindow();

 		//初始化场景图数据
		pControlData->InitSceneGraph();
		if (1)
		{
			//初始化交互对象
			pControlData->InitPickManipulator();	 

			// Set the Scene Data
			m_pControlData->mRoot->setName("mRoot");
	
			m_pControlData->setUpdateCallback(new CUpdateCallback(m_pControlData));

			//创建最终的场景
			m_pControlData->CreateSreen(); 
		}
		
		//osg::Multisample* pms = new osg::Multisample;
		//pms->setSampleCoverage(1, true);//  属性设置对了吗？
		//m_pControlData->mRoot->getOrCreateStateSet()->setAttributeAndModes(pms, osg::StateAttribute::ON);

		mViewer->setSceneData(m_pControlData->mRoot.get());

		// Realize the Viewer
		mViewer->realize();  
		float verson = m_pControlData->GetGLVersion();
		if (m_pControlData->GetGLVersion() < 3.0)
		{
			//refactor by wr
			//db_print_info(MT_WARNING, "显卡opengl驱动版本太低(小于3.0)，请更新显卡驱动程序!");
		}
	}

	//使用自定义光标
	osgViewer::GraphicsWindow::MouseCursor cursor =  osgViewer::GraphicsWindow::MouseCursor(0);

	osgViewer::ViewerBase::Windows windows;
	mViewer->getWindows(windows);
	if(windows.size() > 0 )
	{
		windows[0]->setCursor(cursor);	
	}	
}


void CDbOSGManager::InitViews(void)
{
	mViewer = new osgViewer::Viewer();
	//osg::DisplaySettings::instance()->setNumMultiSamples(4);

	//mViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);	
}



void CDbOSGManager::SetupWindow()
{
	RECT rect;
	::GetWindowRect(m_hWnd, &rect);

	// Init the GraphicsContext Traits
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

	// Init the Windata Variable that holds the handle for the Window to display OSG in.
	osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

	// Setup the traits parameters
	traits->x = 0;
	traits->y = 0;
	traits->width = rect.right - rect.left;
	traits->height = rect.bottom - rect.top;

	if (traits->width < 10)
		traits->width = 800;
	if (traits->height < 10)
		traits->height = 600;

	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->setInheritedWindowPixelFormat = true;
	traits->inheritedWindowData = windata;
	if (busedMulsample)
		traits->samples = 4;
	else
		traits->samples = 0;


	traits->useCursor=true;

	width = traits->width;
	height = traits->height;

	// Create the Graphics Context

	osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());

	//

	//gc->getState()->setUseModelViewAndProjectionUniforms(true);
	//gc->getState()->setUseVertexAttributeAliasing(true);


	traits->x = 0;
	traits->y = 0;

	// Init a new Camera (Master for this View)
	osg::Camera* camera = mViewer->getCamera();

	// Assign Graphics Context to the Camera
	camera->setGraphicsContext(gc);

	// Set the viewport for the Camera
	camera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));

	// Set projection matrix and camera attribtues
	camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	camera->setClearColor(backColor);
	camera->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.4f, 1.0f));
	//test
	if (0)
		camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	else
	{
		camera->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES);
		camera->setNearFarRatio(0.00001f);
	}


	double ratio = 1.0*width / height;
	double width1 = width*1.5;
	double height1 = width1 / ratio;

	if (!m_pControlData->bParalled)
	{
	//	camera->setProjectionMatrixAsPerspective(35.0f, static_cast<double>(width / 2) / static_cast<double>(height / 2), 0.1, 1000.0);
		camera->setProjectionMatrixAsFrustum(-width1, width1, -height1, height1, 1.0, 10000.0);
	}
	else
	{
		camera->setProjectionMatrixAsOrtho(-width1, width1, -height1, height1, 1.0, 10000.0);
	}

	mViewer->setCamera(camera);

	// Add the Camera Manipulator to the Viewer
	mViewer->setCameraManipulator(new CDb3DManipulator(mViewer->getCamera()  ,m_pControlData) );

	mViewer->setKeyEventSetsDone(false);

	/*去掉小对象过滤功能
	*/
	osg::CullStack::CullingMode cullingMode = camera->getCullingMode();
	cullingMode &= ~(osg::CullStack::SMALL_FEATURE_CULLING);
	camera->setCullingMode(cullingMode);
//	camera->setSmallFeatureCullingPixelSize(1);
	
	/*灯光设置 ，应该默认光照效果不佳？
	*/
	if (0)
	{
		//设置系统的全局灯光
		mViewer->setLightingMode(osgViewer::View::HEADLIGHT);
		osg::Light* pLight = mViewer->getLight();
		pLight->setAmbient(osg::Vec4( 0.2f,   0.2f, 0.2f,1.0f));
		pLight->setDiffuse( osg::Vec4(0.8f,   0.8f, 0.8f, 1.0f));
		pLight->setSpecular(osg::Vec4(1.0f,   1.0f, 1.0f, 1.0f));	 

		//设置系统全局环境光
		osg::StateSet* globalStateset = camera->getOrCreateStateSet();
		if(globalStateset)
		{
			osg::LightModel* pLightModel =  (osg::LightModel*)globalStateset->getAttribute(osg::StateAttribute::LIGHTMODEL);
			if(pLightModel)
			{
				pLightModel->setTwoSided(TRUE);
				pLightModel->setAmbientIntensity(osg::Vec4(0.3f,0.3f,0.3f,1.0f));
				pLightModel->setLocalViewer(false);
			}	
		}

	

	} 
}




bool CDbOSGManager::OnCommand(const CString& strCommand)
{
	if(m_pControlData)
	{
		return m_pControlData->OnCommand(strCommand);
	}
	return false;
}

void CDbOSGManager::OnCommandUpdate(const CString& strCommand, bool& enable, bool& checked)
{
	if (m_pControlData)
	{
		m_pControlData->OnCommandUpdate(strCommand,enable,checked);
	}
}