#include "stdafx.h"

 
#include <osg/PointSprite>
#include <osg/BlendFunc>
#include <osg/StateAttribute>
#include <osg/Point>
#include <osg/AutoTransform>
#include "osgmanipulator/Dragger"
#include "osgmanipulator/Projector"
#include "osg/linewidth"
#include <osg/LineStipple>
#include "osg/PolygonMode"
#include "osg/PolygonOffset"
#include "osg/Material"
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Quat>
#include <osg/LightModel>
#include <osg/Point>
#include "osg/BlendFunc"
#include <osgutil/Tessellator>
#include "osgutil/SmoothingVisitor"
#include <osgUtil/DelaunayTriangulator>
#include <osg/ShadeModel>

#include <osg/ComputeBoundsVisitor>
#include <osg/Texture>
#include <osgDB/ReadFile>
#include <osg/ShadeModel>
#include <osg/LogicOp>
#include <osg/AlphaFunc>
 #include "osg/CullFace"
#include "osg/Image"

 
#include "osgwraper.h"
#include "jacobi.h"
#include "PickHandler.h"
#include "BoxSelector.h"
 #include "OSGTools.h"
 #include "OSGBaseView.h"
#include "osgDB/FileUtils"
#include "string_tools.h"
#include "compare_tools.h"
#include "IDbPropertyInterface.h"

 
 #ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




class CDB4Text : public osgText::Text
{
public:
	CDB4Text()
	{

	}
	CDB4Text(const CDB4Text& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
	{

	}
	META_Object(osgText, CDB4Text);


	void CDB4Text::drawHelp_drawForegroundText(osg::State& state, const GlyphQuads& glyphquad, const osg::Vec4& colorMultiplier) const
	{
		unsigned int contextID = state.getContextID();

		const GlyphQuads::Coords3& transformedCoords = glyphquad._transformedCoords[contextID];
		if (transformedCoords.valid() && !transformedCoords->empty())
		{
			state.setVertexPointer(transformedCoords.get());
			state.setTexCoordPointer(0, glyphquad._texcoords.get());

			if (_colorGradientMode == SOLID)
			{
				state.disableColorPointer();
				state.Color(colorMultiplier.r()*_color.r(), colorMultiplier.g()*_color.g(), colorMultiplier.b()*_color.b(), colorMultiplier.a()*_color.a()); 
			}
			else
			{
				state.setColorPointer(glyphquad._colorCoords.get());
			}

			glyphquad._quadIndices->draw(state, _useVertexBufferObjects);
		}
	}

	void CDB4Text::drawHelp_drawTextWithBackdrop(osg::State& state, const osg::Vec4& colorMultiplier) const
	{
		unsigned int contextID = state.getContextID();

		for (TextureGlyphQuadMap::iterator titr = _textureGlyphQuadMap.begin();
			titr != _textureGlyphQuadMap.end();
			++titr)
		{
			// need to set the texture here...
			state.applyTextureAttribute(0, titr->first.get());
			const GlyphQuads& glyphquad = titr->second;

			if (_backdropType != NONE)
			{
				unsigned int backdrop_index;
				unsigned int max_backdrop_index;
				if (_backdropType == OUTLINE)
				{
					backdrop_index = 0;
					max_backdrop_index = 8;
				}
				else
				{
					backdrop_index = _backdropType;
					max_backdrop_index = _backdropType + 1;
				}

				state.setTexCoordPointer(0, glyphquad._texcoords.get());
				state.disableColorPointer();
				state.Color(_backdropColor.r(), _backdropColor.g(), _backdropColor.b(), _backdropColor.a());

				for (; backdrop_index < max_backdrop_index; backdrop_index++)
				{
					const GlyphQuads::Coords3& transformedBackdropCoords = glyphquad._transformedBackdropCoords[backdrop_index][contextID];
					if (transformedBackdropCoords.valid() && !transformedBackdropCoords->empty())
					{
						state.setVertexPointer(transformedBackdropCoords.get());
						glyphquad._quadIndices->draw(state, _useVertexBufferObjects);
					}
				}
			}

			drawHelp_drawForegroundText(state, glyphquad, colorMultiplier);
		}
	}

	void CDB4Text::drawHelp_renderWithDelayedDepthWrites(osg::State& state, const osg::Vec4& colorMultiplier) const
	{
		// If depth testing is disabled, then just render text as normal
		if (!state.getLastAppliedMode(GL_DEPTH_TEST) )
		{
			drawHelp_drawTextWithBackdrop(state, colorMultiplier);
			return;
		}

		//glPushAttrib( _enableDepthWrites ? (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) : GL_DEPTH_BUFFER_BIT);
		// Render to color buffer without writing to depth buffer.
		glDepthMask(GL_FALSE);
		drawHelp_drawTextWithBackdrop(state, colorMultiplier);

		// Render to depth buffer if depth writes requested.
		if (_enableDepthWrites)
		{
			glDepthMask(GL_TRUE);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			drawHelp_drawTextWithBackdrop(state, colorMultiplier);
		}

		state.haveAppliedAttribute(osg::StateAttribute::DEPTH);
		state.haveAppliedAttribute(osg::StateAttribute::COLORMASK);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		//glPopAttrib();
	}

	void CDB4Text::drawHelp_drawImplementation(osg::State& state, const osg::Vec4& colorMultiplier) const
	{ 
  		unsigned int contextID = state.getContextID();

		state.applyMode(GL_BLEND, true);
#if defined(OSG_GL_FIXED_FUNCTION_AVAILABLE)
		state.applyTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
		state.applyTextureAttribute(0, getActiveFont()->getTexEnv());
#endif
		if (_characterSizeMode != OBJECT_COORDS || _autoRotateToScreen)
		{
			unsigned int frameNumber = state.getFrameStamp() ? state.getFrameStamp()->getFrameNumber() : 0;
			AutoTransformCache& atc = _autoTransformCache[contextID];
			const osg::Matrix& modelview = state.getModelViewMatrix();
			const osg::Matrix& projection = state.getProjectionMatrix();

			osg::Vec3 newTransformedPosition = _position*modelview;

			int width = atc._width;
			int height = atc._height;

			const osg::Viewport* viewport = state.getCurrentViewport();
			if (viewport)
			{
				width  = static_cast<int>(viewport->width());
				height = static_cast<int>(viewport->height());
			}

			bool doUpdate = atc._traversalNumber == -1;
			if (atc._traversalNumber >= 0)
			{
				if (atc._modelview != modelview)
				{
					doUpdate = true;
				}
				else if (width != atc._width || height != atc._height)
				{
					doUpdate = true;
				}
				else if (atc._projection != projection)
				{
					doUpdate = true;
				}
			}

			atc._traversalNumber = frameNumber;
			atc._width = width;
			atc._height = height;

			if (doUpdate)
			{
				atc._transformedPosition = newTransformedPosition;
				atc._projection = projection;
				atc._modelview = modelview;

				computePositions(contextID);
			}

		}


		// Ensure that the glyph coordinates have been transformed for
		// this context id.

		if (!_textureGlyphQuadMap.empty())
		{
			const GlyphQuads& glyphquad = (_textureGlyphQuadMap.begin())->second;
			if (!glyphquad._transformedCoords[contextID].valid() || glyphquad._transformedCoords[contextID]->empty())
			{
				computePositions(contextID);
			}
		}

		osg::GLBeginEndAdapter& gl = (state.getGLBeginEndAdapter());

		state.Normal(_normal.x(), _normal.y(), _normal.z());

		if (_drawMode & FILLEDBOUNDINGBOX)
		{
			if (_textBB.valid())
			{
#if !defined(OSG_GLES1_AVAILABLE) && !defined(OSG_GLES2_AVAILABLE) && !defined(OSG_GL3_AVAILABLE)
				state.applyTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OFF);

				const osg::Matrix& matrix = _autoTransformCache[contextID]._matrix;

				osg::Vec3 c00(osg::Vec3(_textBB.xMin(), _textBB.yMin(), _textBB.zMin())*matrix);
				osg::Vec3 c10(osg::Vec3(_textBB.xMax(), _textBB.yMin(), _textBB.zMin())*matrix);
				osg::Vec3 c11(osg::Vec3(_textBB.xMax(), _textBB.yMax(), _textBB.zMin())*matrix);
				osg::Vec3 c01(osg::Vec3(_textBB.xMin(), _textBB.yMax(), _textBB.zMin())*matrix);

				switch (_backdropImplementation)
				{
				case NO_DEPTH_BUFFER:
					// Do nothing.  The bounding box will be rendered before the text and that's all that matters.
					break;
				case DEPTH_RANGE:
					glPushAttrib(GL_DEPTH_BUFFER_BIT);
					//unsigned int backdrop_index = 0;
					//unsigned int max_backdrop_index = 8;
					//const double offset = double(max_backdrop_index - backdrop_index) * 0.003;
					glDepthRange(0.001, 1.001);
					break;
					/*case STENCIL_BUFFER:
					break;*/
				default:
					glPushAttrib(GL_POLYGON_OFFSET_FILL);
					glEnable(GL_POLYGON_OFFSET_FILL);
					glPolygonOffset(0.1f * osg::PolygonOffset::getFactorMultiplier(), 10.0f * osg::PolygonOffset::getUnitsMultiplier());
				}

				gl.Color4f(colorMultiplier.r()*_textBBColor.r(), colorMultiplier.g()*_textBBColor.g(), colorMultiplier.b()*_textBBColor.b(), colorMultiplier.a()*_textBBColor.a());
				gl.Begin(GL_QUADS);
				gl.Vertex3fv(c00.ptr());
				gl.Vertex3fv(c10.ptr());
				gl.Vertex3fv(c11.ptr());
				gl.Vertex3fv(c01.ptr());
				gl.End();

				switch (_backdropImplementation)
				{
				case NO_DEPTH_BUFFER:
					// Do nothing.
					break;
				case DEPTH_RANGE:
					glDepthRange(0.0, 1.0);
					glPopAttrib();
					break;
					/*case STENCIL_BUFFER:
					break;*/
				default:
					glDisable(GL_POLYGON_OFFSET_FILL);
					glPopAttrib();
				}
#else
				OSG_NOTICE << "Warning: Text::drawImplementation() fillMode FILLEDBOUNDINGBOX not supported" << std::endl;
#endif
			}
		}

#if defined(OSG_GL_FIXED_FUNCTION_AVAILABLE)
		state.applyTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
		state.applyTextureAttribute(0, getActiveFont()->getTexEnv());
#endif

		if (_drawMode & TEXT)
		{

			state.disableAllVertexArrays();

			// Okay, since ATI's cards/drivers are not working correctly,
			// we need alternative solutions to glPolygonOffset.
			// So this is a pick your poison approach. Each alternative
			// backend has trade-offs associated with it, but with luck,
			// the user may find that works for them.
			if (_backdropType != NONE && _backdropImplementation != DELAYED_DEPTH_WRITES)
			{
				switch (_backdropImplementation)
				{
				case POLYGON_OFFSET:
					renderWithPolygonOffset(state, colorMultiplier);
					break;
				case NO_DEPTH_BUFFER:
					renderWithNoDepthBuffer(state, colorMultiplier);
					break;
				case DEPTH_RANGE:
					renderWithDepthRange(state, colorMultiplier);
					break;
				case STENCIL_BUFFER:
					renderWithStencilBuffer(state, colorMultiplier);
					break;
				default:
					renderWithPolygonOffset(state, colorMultiplier);
				}
			}
			else
			{
				drawHelp_renderWithDelayedDepthWrites(state, colorMultiplier);
			}

			// unbind buffers if necessary
			state.unbindVertexBufferObject();
			state.unbindElementBufferObject();
		}

		if (_drawMode & BOUNDINGBOX)
		{

			if (_textBB.valid())
			{
				state.applyTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OFF);

				const osg::Matrix& matrix = _autoTransformCache[contextID]._matrix;

				osg::Vec3 c00(osg::Vec3(_textBB.xMin(), _textBB.yMin(), _textBB.zMin())*matrix);
				osg::Vec3 c10(osg::Vec3(_textBB.xMax(), _textBB.yMin(), _textBB.zMin())*matrix);
				osg::Vec3 c11(osg::Vec3(_textBB.xMax(), _textBB.yMax(), _textBB.zMin())*matrix);
				osg::Vec3 c01(osg::Vec3(_textBB.xMin(), _textBB.yMax(), _textBB.zMin())*matrix);


				gl.Color4f(colorMultiplier.r()*_textBBColor.r(), colorMultiplier.g()*_textBBColor.g(), colorMultiplier.b()*_textBBColor.b(), colorMultiplier.a()*_textBBColor.a());
				gl.Begin(GL_LINE_LOOP);
				gl.Vertex3fv(c00.ptr());
				gl.Vertex3fv(c10.ptr());
				gl.Vertex3fv(c11.ptr());
				gl.Vertex3fv(c01.ptr());
				gl.End();
			}
		}

		if (_drawMode & ALIGNMENT)
		{
			gl.Color4fv(colorMultiplier.ptr());

			float cursorsize = _characterHeight*0.5f;

			const osg::Matrix& matrix = _autoTransformCache[contextID]._matrix;

			osg::Vec3 hl(osg::Vec3(_offset.x() - cursorsize, _offset.y(), _offset.z())*matrix);
			osg::Vec3 hr(osg::Vec3(_offset.x() + cursorsize, _offset.y(), _offset.z())*matrix);
			osg::Vec3 vt(osg::Vec3(_offset.x(), _offset.y() - cursorsize, _offset.z())*matrix);
			osg::Vec3 vb(osg::Vec3(_offset.x(), _offset.y() + cursorsize, _offset.z())*matrix);

			state.applyTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OFF);

			gl.Begin(GL_LINES);
			gl.Vertex3fv(hl.ptr());
			gl.Vertex3fv(hr.ptr());
			gl.Vertex3fv(vt.ptr());
			gl.Vertex3fv(vb.ptr());
			gl.End();

		}
	}
	virtual void drawImplementation(osg::RenderInfo& renderInfo) const
	{
		osgText::Text::drawImplementation(*renderInfo.getState(), osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
};

int GetFontResolutionByCharsize(int tSize)
{
	if (tSize <= 20)
		return 20;
	if (tSize <= 30)
		return 25;
	else if (tSize <= 40)
		return 30;
	else if (tSize <= 50)
		return 35;
	else if (tSize <= 60)
		return 40;
	else if (tSize <= 70)
		return 45;
	else
		return 60;
}

//
void MakeObjectUnSelected(std::vector<osg::Node*>& allObj, CViewControlData* m_pOSG)
{
	if (m_pOSG)
	{
		for (int i = 0; i < allObj.size(); i++)
		{
			CString strObj = allObj[i]->getName().c_str();

			std::map<CString, osg::ref_ptr<osg::Node>>::iterator it = m_pOSG->m_pSelector->selectetSet.find(strObj);
			if (it != m_pOSG->m_pSelector->selectetSet.end())
			{
				m_pOSG->clearBoundingBoxObj(strObj);
				m_pOSG->removeHightLigntEffect(allObj[i]);
				m_pOSG->m_pSelector->selectetSet.erase(it);
			}

			it = m_pOSG->m_pSelector->selectetSetBak.find(strObj);
			if (it != m_pOSG->m_pSelector->selectetSetBak.end())
			{			 
				m_pOSG->m_pSelector->selectetSetBak.erase(it);
			}
 		}
	} 
}

void MakeObjectSelected(std::vector<osg::Node*>& allObj ,CViewControlData* m_pOSG)
{
	if(m_pOSG)
	{
		if(m_pOSG->m_pPickModelHandler->bclearPre)
		{
			m_pOSG->m_pSelector->clearSelectSet();
		}
		for(int i=0; i< allObj.size(); i++)
		{
			CString strObj = allObj[i]->getName().c_str();	

			bool flag = m_pOSG->m_pSelector->addToSet(strObj, allObj[i]);

			m_pOSG->removeHightLigntEffect(allObj[i]);
			m_pOSG->addBoundingBoxObj(strObj);	
		} 
		DisPlayProperty(m_pOSG, m_pOSG->m_pSelector->GetAllSelectNode() );
	}
}

bool IsLButtonDown(const osgGA::GUIEventAdapter& ea)
{
	if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH)
	{
		if (ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			return true;
		}
	}
	return false;
}
bool IsRButtonDown(const osgGA::GUIEventAdapter& ea)
{
	if(ea.getEventType() ==osgGA::GUIEventAdapter::PUSH)
	{ 
		if(ea.getButtonMask() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
		{
			return true;
		}
	}
	return false;
}


//是否escape被按下
bool IsEscaped(const osgGA::GUIEventAdapter& ea)
{
	if(ea.getEventType() ==osgGA::GUIEventAdapter::KEYDOWN)
	{   
		if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Escape)
		{
			return true;
		}
	}
	return false;
}

bool IsReturn(const osgGA::GUIEventAdapter& ea)
{
 	static double lastTime = -10;
	if ( abs(ea.getTime() - lastTime) < .2)//********
		return false;

	if(ea.getEventType() ==osgGA::GUIEventAdapter::KEYDOWN)
	{   
		if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Return 
			|| ea.getKey() == osgGA::GUIEventAdapter::KEY_Space)
		{
			lastTime = ea.getTime();
			return true;
		}
	}
	return false;
}

//轮廓线加粗效果
void SetLunKuoEffect2(osg::Node* pNode, int width,osg::Vec4 color )
{
 
}


BOOL IsTempObject(osg::Node* pNode)
{
	int nCount = pNode->getNumParents();
	if(nCount >0 )
	{
		osg::Group* pParent = pNode->getParent(0);
		if(pParent->getName() == "temp Objects")
		{
			return TRUE;
		}
		else
		{
			return IsTempObject(pParent);
		}
	}
	return FALSE;
}

bool InitEnter(const osgGA::GUIEventAdapter& ea , int& status)
{
	//if( IsReturn(ea) && status ==0)
	//{
	//	return true;
	//}
	return false;
}


bool GetObjWorldPos(osg::Node* pNode ,  osg::Node* pRoot , osg::Vec3& pt1 , osg::Vec3& pt2)
{
	osg::Matrix  worldMatrix;

	osg::MatrixList terrain_worldMatrices = pNode->getWorldMatrices(pRoot );

	if (terrain_worldMatrices.empty()) worldMatrix.makeIdentity();
	else if (terrain_worldMatrices.size()==1) worldMatrix = terrain_worldMatrices.front();
	else
	{
		osg::notify(osg::NOTICE)<<"IntersectionUpdateCallback: warning cannot interestect with multiple terrain instances, just uses first one."<<std::endl;
		worldMatrix = terrain_worldMatrices.front();
	}

	/////////////
	int numberDrawable = pNode->asGeode()->getNumDrawables();
	if(numberDrawable > 0 )
	{					 
		osg::Drawable* pdrawable =  pNode->asGeode()->getDrawable(0);
		osg::Geometry*  pGeometry = pdrawable->asGeometry();
		if(pGeometry)
		{
			osg::Vec3Array *pVertices =(osg::Vec3Array*)pGeometry->getVertexArray();
			if(pVertices->size() ==2)
			{
				pt1 = pVertices->at(0)*worldMatrix;
				pt2 = pVertices->at(1)*worldMatrix;
				return true;
			}
		}
	}

	return false;
}


osg::Vec3 GetObjWorldPos(osg::Node* pNode , osg::Node* pRoot)
{
	osg::BoundingSphere bs = pNode->getBound();
	osg::Vec3 wordPos;

	osg::Matrix  worldMatrix;
	worldMatrix.makeIdentity();

	osg::MatrixList terrain_worldMatrices = pNode->getWorldMatrices(pRoot );
	if (terrain_worldMatrices.empty()) worldMatrix.makeIdentity();
	else if (terrain_worldMatrices.size()==1) worldMatrix = terrain_worldMatrices.front();
	else
	{
		osg::notify(osg::NOTICE)<<"IntersectionUpdateCallback: warning cannot interestect with multiple terrain instances, just uses first one."<<std::endl;
		worldMatrix = terrain_worldMatrices.front();
	}

		
	wordPos = bs.center()*worldMatrix;
	return wordPos;
}

//轮廓线加粗效果
void SetLunKuoEffect(osg::Node* pNode, int width,osg::Vec4 color )
{
	if(pNode)
	{	
		osg::StateSet* stateset = pNode->getOrCreateStateSet();		

		osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;			
		{
			polyoffset->setFactor( 1);
			polyoffset->setUnits(  1);
		}

		osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
		polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);

		stateset->setAttributeAndModes(polyoffset.get(),osg::StateAttribute::ON);
		stateset->setAttributeAndModes(polymode.get(),osg::StateAttribute::ON);
		if(1)
		{
			osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth();
			linewidth->setWidth(width);
			stateset->setAttributeAndModes(linewidth.get() ,osg::StateAttribute::ON);
		}
		SetNodeColorExtend(pNode,color,color,FALSE);

	}
}

void CreateNode3DModel3(int type, osg::Texture2D * texture, osg::Vec3 lPT, osg::Node* pNode,osg::Vec4 color, int radius)
{
	osg::Geode* pGeode = pNode->asGeode();
	if (pGeode)
	{
		pGeode->removeDrawables(0, pGeode->getNumDrawables());		
		{		 
				osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry();

				osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
				pGeometry->setVertexArray(pyramidVertices.get());

				osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
				pGeometry->setColorArray(colors.get());
				pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);		 
			
				double factor = 0.1;

				if (type == 6)///对齐点
				{
 					colors->push_back(osg::Vec4(1,1,0,1)); //索引0 红色 

					int index = 0;

					osg::Vec3 insertPt = lPT + osg::Vec3(20 * factor, 60 * factor, 0);
					pyramidVertices->push_back(insertPt);

					insertPt = lPT + osg::Vec3(60 * factor, 60 * factor, 0);
					pyramidVertices->push_back(insertPt);

					insertPt = lPT + osg::Vec3(60 * factor, 20 * factor, 0);
					pyramidVertices->push_back(insertPt);

					osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP);
					for (int i = index; i < index+3; i++)
						pyramidBase->push_back(i);
					pGeometry->addPrimitiveSet(pyramidBase.get());

					index += 3;
					insertPt = lPT + osg::Vec3(20 * factor, -60 * factor, 0);
					pyramidVertices->push_back(insertPt);
					insertPt = lPT + osg::Vec3(60 * factor, -60 * factor, 0);
					pyramidVertices->push_back(insertPt);
					insertPt = lPT + osg::Vec3(60 * factor, -20 * factor, 0);
					pyramidVertices->push_back(insertPt);

					pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP);
					for (int i = index; i < index + 3; i++)
						pyramidBase->push_back(i);
					pGeometry->addPrimitiveSet(pyramidBase.get());

					index += 3;
					insertPt = lPT + osg::Vec3(-20 * factor, -60 * factor, 0);
					pyramidVertices->push_back(insertPt);
					insertPt = lPT + osg::Vec3(-60 * factor, -60 * factor, 0);
					pyramidVertices->push_back(insertPt);
					insertPt = lPT + osg::Vec3(-60 * factor, -20 * factor, 0);
					pyramidVertices->push_back(insertPt);

					pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP);
					for (int i = index; i < index + 3; i++)
						pyramidBase->push_back(i);
					pGeometry->addPrimitiveSet(pyramidBase.get());

					index += 3;
					insertPt = lPT + osg::Vec3(-20 * factor, 60 * factor, 0);
					pyramidVertices->push_back(insertPt);
					insertPt = lPT + osg::Vec3(-60 * factor, 60 * factor, 0);
					pyramidVertices->push_back(insertPt);
					insertPt = lPT + osg::Vec3(-60 * factor, 20 * factor, 0);
					pyramidVertices->push_back(insertPt);

					pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP);
					for (int i = index; i < index + 3; i++)
						pyramidBase->push_back(i);
					pGeometry->addPrimitiveSet(pyramidBase.get());

					index += 3;
					insertPt = lPT + osg::Vec3(0, 100 * factor, 0);
					pyramidVertices->push_back(insertPt);
					insertPt = lPT + osg::Vec3(0, -100 * factor, 0);
					pyramidVertices->push_back(insertPt);
					insertPt = lPT + osg::Vec3(-100 * factor, 0, 0);
					pyramidVertices->push_back(insertPt);
					insertPt = lPT + osg::Vec3(100 * factor, 0, 0);
					pyramidVertices->push_back(insertPt);
					pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES);
					for (int i = index; i < index + 4; i++)
						pyramidBase->push_back(i);
					pGeometry->addPrimitiveSet(pyramidBase.get());
				}
				else if (type == 0)
				{
					radius *= 0.8;
					colors->push_back(redColor); //索引0 红色 
					int size = 36;
					double alfa = M_PI * 2 / size;
					for (int i = 0; i < size; i++)
					{
						osg::Vec3 pt = lPT + osg::Vec3(radius*cos(i*alfa), radius*sin(i*alfa), 0);
						pyramidVertices->push_back(pt);
					}
					osg::Vec3 cpt = lPT + osg::Vec3(0, 2 * radius, 0);
					pyramidVertices->push_back(cpt);
					osg::Vec3 cpb = lPT + osg::Vec3(0, -2 * radius, 0);
					pyramidVertices->push_back(cpb);

					osg::Vec3 lpc = lPT + osg::Vec3(-2 * radius, 0, 0);
					pyramidVertices->push_back(lpc);
					osg::Vec3 rpc = lPT + osg::Vec3(2 * radius, 0, 0);
					pyramidVertices->push_back(rpc);

					osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
					for (int i = 0; i < size; i++)
						pyramidBase->push_back(i);
					pGeometry->addPrimitiveSet(pyramidBase.get());

					osg::ref_ptr<osg::DrawElementsUInt> pyramidBase2 = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES);
					pyramidBase2->push_back(size);
					pyramidBase2->push_back(size + 1);
					pyramidBase2->push_back(size + 2);
					pyramidBase2->push_back(size + 3);
					pGeometry->addPrimitiveSet(pyramidBase2.get());
				}
				else if (type == 1)///支座位置
				{
					colors->push_back(whiteColor); //索引0 红色 

					osg::Vec3 insertPt = lPT + osg::Vec3(-100 * factor, -160 * factor, 0);
					pyramidVertices->push_back(insertPt);
					insertPt = lPT + osg::Vec3(100 * factor, -160 * factor, 0);
					pyramidVertices->push_back(insertPt);  
					osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES);
					for (int i = 0; i < 2; i++)
						pyramidBase->push_back(i);
					pGeometry->addPrimitiveSet(pyramidBase.get());


					insertPt = lPT + osg::Vec3(-60 * factor, -160 * factor, 0);
					pyramidVertices->push_back(insertPt);
					insertPt = lPT + osg::Vec3(0, -0, 0);
					pyramidVertices->push_back(insertPt);
					insertPt = lPT + osg::Vec3(60 * factor, -160 * factor, 0);
					pyramidVertices->push_back(insertPt);
					pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP);
					for (int i = 2; i < 5; i++)
						pyramidBase->push_back(i);
					pGeometry->addPrimitiveSet(pyramidBase.get());
				}
				else if (type == 2)///拉索锚固点
				{
					colors->push_back(osg::Vec4(1,0,1,1)); //紫色

					int size = 36;
					double alfa = M_PI * 2 / size;
					for (int i = 0; i < size; i++)
					{
						osg::Vec3 pt = lPT + osg::Vec3(40 * factor*cos(i*alfa), 40 * factor*sin(i*alfa), 0);
						pyramidVertices->push_back(pt);
					}
					osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
					for (int i = 0; i < size; i++)
						pyramidBase->push_back(i);
					pGeometry->addPrimitiveSet(pyramidBase.get());


					osg::Vec3 insertPt = lPT + osg::Vec3(-60*factor , 0, 0);
					pyramidVertices->push_back(insertPt);
					insertPt = lPT + osg::Vec3(60 * factor, 0, 0);
					pyramidVertices->push_back(insertPt);

					insertPt = lPT + osg::Vec3(0, -60 * factor, 0);
					pyramidVertices->push_back(insertPt);
					insertPt = lPT + osg::Vec3(0, 60 * factor, 0);
					pyramidVertices->push_back(insertPt);

					pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES);
					for (int i = size; i < size+4; i++)
						pyramidBase->push_back(i);
					pGeometry->addPrimitiveSet(pyramidBase.get());

					size += 4;

					insertPt = lPT + osg::Vec3(-75 * factor, -75 * factor, 0);
					pyramidVertices->push_back(insertPt);
					insertPt = lPT + osg::Vec3(75 * factor, -75 * factor, 0);
					pyramidVertices->push_back(insertPt);

					insertPt = lPT + osg::Vec3(75 * factor, 75 * factor,0);
					pyramidVertices->push_back(insertPt);

					insertPt = lPT + osg::Vec3(-75 * factor, 75 * factor, 0);
					pyramidVertices->push_back(insertPt);

					osg::ref_ptr<osg::DrawElementsUInt> pyramidBase2 = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
					pyramidBase2->push_back(size);
					pyramidBase2->push_back(size + 1);
					pyramidBase2->push_back(size + 2);
					pyramidBase2->push_back(size + 3);
					pGeometry->addPrimitiveSet(pyramidBase2.get());
				}
				else if (type >= 30 && type <40)///刚束位
				{
					colors->push_back(osg::Vec4(1,0,1,1)); //索引0 红色 

					if (type == 31 || type ==33)
					{
						osg::Vec3 insertPt = lPT + osg::Vec3(-20 * factor, 0, 0);
						pyramidVertices->push_back(insertPt);
						insertPt = lPT + osg::Vec3(-60 * factor, 0, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(-60 * factor, 20 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(-140 * factor, 0, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(-60 * factor, -20 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(-60 * factor, 0, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(-20 * factor, 0, 0);
						pyramidVertices->push_back(insertPt);

						osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP);
						for (int i = 0; i < 7; i++)
							pyramidBase->push_back(i);
						pGeometry->addPrimitiveSet(pyramidBase.get());

						insertPt = lPT + osg::Vec3(20 * factor, 0, 0);
						pyramidVertices->push_back(insertPt);
						insertPt = lPT + osg::Vec3(60 * factor, 0, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(60 * factor, 20 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(140 * factor, 0, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(60 * factor, -20 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(60 * factor, 0, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(20 * factor, 0, 0);
						pyramidVertices->push_back(insertPt);

						pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP);
						for (int i = 7; i < 14; i++)
							pyramidBase->push_back(i);
						pGeometry->addPrimitiveSet(pyramidBase.get());

						insertPt = lPT + osg::Vec3(0, -200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(0, 200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						if (type == 31)
						{
							pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP);
							for (int i = 14; i < 16; i++)
								pyramidBase->push_back(i);
							pGeometry->addPrimitiveSet(pyramidBase.get());

						}
					}
					
					if (type >=32)
					{
						osg::Vec3 insertPt = lPT + osg::Vec3(0,-20 * factor,  0);
						pyramidVertices->push_back(insertPt);
						insertPt = lPT + osg::Vec3(0,-60 * factor,  0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(20 * factor, -60 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3( 0,-140 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3( -20 * factor,-60 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(0,-60 * factor,  0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(0, -20 * factor, 0);
						pyramidVertices->push_back(insertPt);

						int index = 0;
						if (type == 33)
							index = 14;
						osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP);
						for (int i = index; i < index+7; i++)
							pyramidBase->push_back(i);
						pGeometry->addPrimitiveSet(pyramidBase.get());

						index += 7;

						insertPt = lPT + osg::Vec3(0, 20 * factor, 0);
						pyramidVertices->push_back(insertPt);
						insertPt = lPT + osg::Vec3(0, 60 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(20 * factor,60 * factor,  0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(0, 140 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(-20 * factor,60 * factor,  0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(0,60 * factor,  0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3( 0,20 * factor, 0);
						pyramidVertices->push_back(insertPt);

						pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP);
						for (int i = index; i < index + 7; i++)
							pyramidBase->push_back(i);
						pGeometry->addPrimitiveSet(pyramidBase.get());

						if (type == 32)
						{
							insertPt = lPT + osg::Vec3(-200 * factor, 0, 0);
							pyramidVertices->push_back(insertPt);

							insertPt = lPT + osg::Vec3(200 * factor, 0, 0);
							pyramidVertices->push_back(insertPt);

							pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP);
							for (int i = 14; i < 16; i++)
								pyramidBase->push_back(i);
							pGeometry->addPrimitiveSet(pyramidBase.get());
						}					
					}
			 
				}
				else if (type == 4)///检查点
				{
					colors->push_back(redColor); //索引0 红色 

					if (0) //五星，废弃
					{
						double r = 1.5*radius;
						for (int i = 0; i < 5; i++)
						{
							double x = r*cos(M_PI*0.4*i + 0.1*M_PI);
							double y = r*sin(M_PI*0.4*i + 0.1*M_PI);
							osg::Vec3 cpt = lPT + osg::Vec3(x, y, 0);
							pyramidVertices->push_back(cpt);
						}
						r = r*sin(0.1* M_PI) / sin(0.2*M_PI);
						for (int i = 0; i < 5; i++)
						{
							double x = r*cos(0.4*M_PI*i + 0.3*M_PI);
							double y = r*sin(0.4*M_PI*i + 0.3*M_PI);
							osg::Vec3 cpt = lPT + osg::Vec3(x, y, 0);
							pyramidVertices->push_back(cpt);
						}
						osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
						for (int i = 0; i < 5; i++)
						{
							pyramidBase->push_back(i);
							pyramidBase->push_back(5 + i);
						}
						pGeometry->addPrimitiveSet(pyramidBase.get());
					}
					else
					{
						osg::Vec3 insertPt = lPT + osg::Vec3(-70 * factor, -70 * factor, 0);
						pyramidVertices->push_back(insertPt);
						insertPt = lPT + osg::Vec3(70 * factor, -70 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(70 * factor, 70 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(-70 * factor, 70 * factor, 0);
						pyramidVertices->push_back(insertPt);

						osg::ref_ptr<osg::DrawElementsUInt> pyramidBase2 = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
						pyramidBase2->push_back(0);
						pyramidBase2->push_back(1);
						pyramidBase2->push_back(2);
						pyramidBase2->push_back(3);
						pGeometry->addPrimitiveSet(pyramidBase2.get());

						pyramidBase2 = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES);
						pyramidBase2->push_back(0);
						pyramidBase2->push_back(2);
						pyramidBase2->push_back(1);
						pyramidBase2->push_back(3);
						pGeometry->addPrimitiveSet(pyramidBase2.get());
					}
					
				}
				else
				{
					colors->push_back(yellowColor); //索引0 红色 
					if (type == 51)
					{
						osg::Vec3 insertPt = lPT + osg::Vec3(0,200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(0,-200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						osg::ref_ptr<osg::DrawElementsUInt> pyramidBase2 = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES);
						pyramidBase2->push_back(0);
						pyramidBase2->push_back(1);			 
						pGeometry->addPrimitiveSet(pyramidBase2.get());
					}
					if (type == 52)
					{
						osg::Vec3 insertPt = lPT + osg::Vec3(-60*factor, 200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(-30*factor, 160 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(-30 * factor, -160 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(-60 * factor, -200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						osg::ref_ptr<osg::DrawElementsUInt> pyramidBase2 = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP);
						pyramidBase2->push_back(0);
						pyramidBase2->push_back(1);
						pyramidBase2->push_back(2);
						pyramidBase2->push_back(3);
						pGeometry->addPrimitiveSet(pyramidBase2.get());


						insertPt = lPT + osg::Vec3(60 * factor, 200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(30 * factor, 160 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(30 * factor, -160 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(60 * factor, -200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						pyramidBase2 = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP);
						pyramidBase2->push_back(4);
						pyramidBase2->push_back(5);
						pyramidBase2->push_back(6);
						pyramidBase2->push_back(7);
						pGeometry->addPrimitiveSet(pyramidBase2.get());
					}
					if (type == 53)
					{
						osg::Vec3 insertPt = lPT + osg::Vec3(-40 * factor, 200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(-20 * factor, 200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(-20 * factor, -200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(-40 * factor, -200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						osg::ref_ptr<osg::DrawElementsUInt> pyramidBase2 = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP);
						pyramidBase2->push_back(0);
						pyramidBase2->push_back(1);
						pyramidBase2->push_back(2);
						pyramidBase2->push_back(3);
						pGeometry->addPrimitiveSet(pyramidBase2.get());

						insertPt = lPT + osg::Vec3(40 * factor, 200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(20 * factor, 200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(20 * factor, -200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						insertPt = lPT + osg::Vec3(40 * factor, -200 * factor, 0);
						pyramidVertices->push_back(insertPt);

						pyramidBase2 = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP);
						pyramidBase2->push_back(4);
						pyramidBase2->push_back(5);
						pyramidBase2->push_back(6);
						pyramidBase2->push_back(7);
						pGeometry->addPrimitiveSet(pyramidBase2.get());
					}
		
				}
		
				pGeometry->setUseVertexBufferObjects(true);
				pGeometry->setDataVariance(osg::Object::DYNAMIC);
				pGeode->addDrawable(pGeometry); 
			}
	
	}
}
//轮廓线中心点划线效果
void SetLunKuoCenterEffect(osg::Node* pNode ,osg::Vec4 color)
{
 	if(pNode)
	{	
		osg::StateSet* stateset = pNode->getOrCreateStateSet();		

		osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;
		{
			polyoffset->setFactor(  1.0f);
			polyoffset->setUnits(   1.0f);
		}

		osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
		polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);

		stateset->setAttributeAndModes(polyoffset.get(),osg::StateAttribute::ON);
		stateset->setAttributeAndModes(polymode.get(),osg::StateAttribute::ON);
		if(1)
		{
			osg::ref_ptr<osg::LineStipple> linewidth = new osg::LineStipple();
			linewidth->setFactor(1);
			linewidth->setPattern(0xc0c0);
			stateset->setAttributeAndModes(linewidth.get(),osg::StateAttribute::ON);
		}
		SetNodeColorExtend(pNode,color,color,FALSE);

		osg::ref_ptr<osg::LogicOp>   logicOp = new osg::LogicOp(osg::LogicOp::AND_INVERTED);
		stateset->setAttributeAndModes(logicOp.get(),osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
	}
}
void SetBlanking(osg::Node* pNode  )
{
 	if (pNode)
	{
		osg::StateSet* stateset = pNode->getOrCreateStateSet();

		osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;
		polyoffset->setFactor(1);
		polyoffset->setUnits( 1);

		osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
		polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
		stateset->setAttributeAndModes(polyoffset.get(), osg::StateAttribute::ON);
		stateset->setAttributeAndModes(polymode.get(),   osg::StateAttribute::ON);
		if (1)
		{
			osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth();
			linewidth->setWidth(1.5);
			stateset->setAttributeAndModes(linewidth.get(), osg::StateAttribute::ON);
 		}
	}
}

void SetNodeColorExtend(osg::Node* pNode ,osg::Vec4 color1,osg::Vec4 color2,BOOL bUseGLColor)
{ 
 	osg::StateSet*  stateset = pNode->getOrCreateStateSet();
	osg::Material* material = dynamic_cast<osg::Material*>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if (! material)
	{
		material = new osg::Material;
		material->setDataVariance(osg::Object::DYNAMIC);
		stateset->setAttributeAndModes(material,osg::StateAttribute::ON);
	}
	if(bUseGLColor)
	{
		material->setColorMode(osg::Material::DIFFUSE);
		material->setAmbient(  osg::Material::FRONT_AND_BACK, color1);
		material->setSpecular( osg::Material::FRONT_AND_BACK, color2);
	}
	else
	{
		material->setColorMode(osg::Material::OFF); // switch glColor usage off
		material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0f,0.0f,1.0f));
		material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0f,0.0f,1.0f));
		material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0f,0.0f,1.0f));

		material->setEmission(osg::Material::FRONT_AND_BACK, color1);
	}

 	stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

}
 osg::Geometry* createDPLineGeode(std::vector<osg::Vec3d>& allPTs, osg::Vec4 color, BOOL bClose, float nWidth )
{
	std::vector<osg::Vec3> fPoints;
	for (INT i = 0; i < allPTs.size(); i++)
		fPoints.push_back(allPTs[i]);

	return createPLineGeode(fPoints, color, bClose, nWidth);
}

osg::Geometry* createPLineGeode(std::vector<osg::Vec3>& allPTs ,osg::Vec4 color,BOOL bClose,float nWidth)
{
	int nCount = allPTs.size();

	osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry();

	osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
	pGeometry->setVertexArray( pyramidVertices.get()  ); 
	for(int i=0; i< allPTs.size(); i++)
	{
 		pyramidVertices->push_back(allPTs[i]);	 
	}

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(color); 
	pGeometry->setColorArray(colors.get() ); 
	pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL); 


	osg::ref_ptr<osg::DrawArrays> pyramidBase = NULL;
	if(bClose)
	{
		pyramidBase=new osg::DrawArrays( osg::PrimitiveSet::LINE_LOOP, 0, allPTs.size() ); 
	}
	else
	{
		pyramidBase=new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0, allPTs.size()); 
	}

	pGeometry->addPrimitiveSet(pyramidBase.get() ); 

	osgUtil::SmoothingVisitor::smooth(*pGeometry );

	pGeometry->setUseVertexBufferObjects(false);

	{
		osg::StateSet* stateset = pGeometry->getOrCreateStateSet();
		osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth();
		linewidth->setWidth(nWidth);
		stateset->setAttributeAndModes(linewidth.get() , osg::StateAttribute::ON); 
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	}

	return pGeometry.release();
}
osg::Geometry* createMultiLineGeode(std::vector< std::vector<osg::Vec3> >& allPTs, osg::Vec4 color, BOOL bClose)
{
	osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry();
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(color); 
	pGeometry->setColorArray(colors.get() ); 
	pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL); 

	osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
	pGeometry->setVertexArray( pyramidVertices.get()  ); 

	int start = 0;
	for(int i=0; i< allPTs.size(); i++ )
	{
		std::vector<osg::Vec3>& oneSection = allPTs[i];

		for(int j=0; j< oneSection.size(); j++)
		{
			pyramidVertices->push_back( oneSection[j] ); 
		}
		osg::ref_ptr<osg::DrawArrays> pyramidBase = NULL;
		if(bClose)
			pyramidBase=new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, start, oneSection.size());
		else
			pyramidBase=new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, start, oneSection.size());

	 
		start +=oneSection.size();

		pGeometry->addPrimitiveSet(pyramidBase.get()); 
	}
	osgUtil::SmoothingVisitor::smooth(*pGeometry );
	pGeometry->setUseVertexBufferObjects(false);
	return pGeometry.release();
}

//创建箭头//创建箭头 ,pt1 基点 dSize 箭头长大小 ， dAngle 箭头方向, dAngle2 箭头（tan（箭头高/箭头长)角度）

osg::Geometry* createArror(osg::Vec3 pt1 , double dSize,   double dAngle , osg::Vec4 color,double dAngle2)
{
	double dl = dSize/(cos(dAngle2*0.5));
	double pt2x = dl*cos( dAngle+ dAngle2*0.5);
	double pt2y = dl*sin(  dAngle+ dAngle2*0.5);
	double pt3x = dl*cos( dAngle- dAngle2*0.5);
	double pt3y = dl*sin(  dAngle- dAngle2*0.5);

	osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry();

	osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
	pGeometry->setVertexArray( pyramidVertices.get() ); 
	pyramidVertices->push_back( pt1 ); 
	pyramidVertices->push_back(  pt1+osg::Vec3(pt3x, pt3y,0) ); 
	pyramidVertices->push_back(  pt1+osg::Vec3(pt2x, pt2y,0) ); 

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(color); 
	pGeometry->setColorArray(colors.get() ); 
	pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL); 


	osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = NULL;
	pyramidBase=new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON); 
	pyramidBase->push_back( 0);
	pyramidBase->push_back( 1);
	pyramidBase->push_back( 2);

	pGeometry->addPrimitiveSet(pyramidBase.get() ); 

	osgUtil::SmoothingVisitor::smooth(*pGeometry );


	pGeometry->setUseVertexBufferObjects(false);

	return pGeometry.release();
}



osg::Geometry* createArror(const osg::Vec3 &basePoint, double length, double arrow_length, const osg::Vec3 &basevec, const osg::Vec3 &basepern_vec, const osg::Vec4 &color, double arrow_angle /*= M_PI / 6*/, int nArrowType, bool bStart, bool bDoubleArrow)
{
	osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry();

	double l1 = length;                  //箭柄长度
	double l2 = arrow_length;            //箭头长度
	double b = l2 * tan(arrow_angle);   //箭头底宽
	//vec;  //箭头向量
	//pern_vec;  //箭头垂直向量
	//pt0;   //箭柄末端点

	osg::Vec3 vec = basevec;
	osg::Vec3 pern_vec = basepern_vec;
	osg::Vec3 pt0 = basePoint;

	if (!bStart)
	{
		if (bDoubleArrow)
		{
			pt0 = basePoint - basevec * (l1 + 2*l2);
		}
		else
		{
			pt0 = basePoint - basevec * (l1 + l2);
		}
	}

	osg::Vec3 pt1 = pt0 + vec * l1;   //箭头底端点
	osg::Vec3 pt2 = pt0 + vec * (l1 + l2);   //箭头尖端点
	osg::Vec3 pt_b1 = pt1 + pern_vec * b;//箭头底点1
	osg::Vec3 pt_b2 = pt1 - pern_vec * b;//箭头底点2

	osg::Vec3 pt3 = pt2 + vec * l2;   //箭头尖端点
	osg::Vec3 pt_b3 = pt2 + pern_vec * b;//箭头底点1
	osg::Vec3 pt_b4 = pt2 - pern_vec * b;//箭头底点2

	osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
	pGeometry->setVertexArray(pyramidVertices.get() );
	pyramidVertices->push_back(pt0);
	pyramidVertices->push_back(pt1);
	pyramidVertices->push_back(pt2);
	pyramidVertices->push_back(pt_b1);
	pyramidVertices->push_back(pt_b2);
	pyramidVertices->push_back(pt3);
	pyramidVertices->push_back(pt_b3);
	pyramidVertices->push_back(pt_b4);

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(color);
	pGeometry->setColorArray(colors.get() );
	pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = NULL;
	if (0 == nArrowType)
	{
		pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON);
		pyramidBase->push_back(2);
		pyramidBase->push_back(3);
		pyramidBase->push_back(4);
		if (bDoubleArrow)
		{
			osg::ref_ptr<osg::DrawElementsUInt>pyramidBase2 = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON);
			pyramidBase2->push_back(5);
			pyramidBase2->push_back(6);
			pyramidBase2->push_back(7);
			pGeometry->addPrimitiveSet(pyramidBase2.get() );
		}
	}
	else if(1 == nArrowType)
	{
		pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
		pyramidBase->push_back(2);
		pyramidBase->push_back(3);
		pyramidBase->push_back(4);
		if (bDoubleArrow)
		{
			osg::ref_ptr<osg::DrawElementsUInt>pyramidBase2 = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
			pyramidBase2->push_back(5);
			pyramidBase2->push_back(6);
			pyramidBase2->push_back(7);
			pGeometry->addPrimitiveSet(pyramidBase2.get() );
		}
	}
	else if (2 == nArrowType)
	{
		pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES);
		pyramidBase->push_back(1);
		pyramidBase->push_back(2);
		pyramidBase->push_back(2);
		pyramidBase->push_back(3);
		pyramidBase->push_back(2);
		pyramidBase->push_back(4);
		if (bDoubleArrow)
		{
			pyramidBase->push_back(2);
			pyramidBase->push_back(5);
			pyramidBase->push_back(5);
			pyramidBase->push_back(6);
			pyramidBase->push_back(5);
			pyramidBase->push_back(7);
		}
	}

	pGeometry->addPrimitiveSet(pyramidBase);

	osg::ref_ptr<osg::DrawElementsUInt> pyramidBase_Line = NULL;
	pyramidBase_Line = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES);
	pyramidBase_Line->push_back(0);
	pyramidBase_Line->push_back(1);
	pGeometry->addPrimitiveSet(pyramidBase_Line.get() );

	osgUtil::SmoothingVisitor::smooth(*pGeometry);
	pGeometry->setUseVertexBufferObjects(false);

	return pGeometry.release();
}

//创建箭头
osg::Geometry* createArror2(osg::Vec3 pt1, double dSize, double dAngle, osg::Vec4 color, double dAngle2)
{
	double dl = dSize / (cos(dAngle2*0.5));
	double pt2x = dl*cos(dAngle + dAngle2*0.5);
	double pt2y = dl*sin(dAngle + dAngle2*0.5);
	double pt3x = dl*cos(dAngle - dAngle2*0.5);
	double pt3y = dl*sin(dAngle - dAngle2*0.5);

	osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry();

	osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
	pGeometry->setVertexArray(pyramidVertices.get() );
	pyramidVertices->push_back(pt1);
	pyramidVertices->push_back(pt1 + osg::Vec3(pt3x, 0,  pt3y ));
	pyramidVertices->push_back(pt1 + osg::Vec3(pt2x, 0 , pt2y ));

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(color);
	pGeometry->setColorArray(colors.get() );
	pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);


	osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = NULL;
	pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON);
	pyramidBase->push_back(0);
	pyramidBase->push_back(1);
	pyramidBase->push_back(2);

	pGeometry->addPrimitiveSet(pyramidBase.get() );

	osgUtil::SmoothingVisitor::smooth(*pGeometry);


	pGeometry->setUseVertexBufferObjects(false);

	return pGeometry.release() ;
}


//两点静态创建直线
osg::Drawable* createLineGeodeByRadius(osg::Vec3 pt1, osg::Vec3 pt2, double dRadisu, osg::Vec4 color)
{
	osg::Vec3 center=(pt1+pt2)*0.5;
	osg::Vec3 dir = pt2 - pt1;
	double lineLength = dir.length();
	dir.normalize();

	osg::ref_ptr<osg::Cylinder> unitCyl = new osg::Cylinder(center, dRadisu,lineLength);

	osg::Quat rotation;
	rotation.makeRotate(osg::Z_AXIS, dir);

	unitCyl->setRotation(rotation);

	osg::ref_ptr<osg::ShapeDrawable> geometry = new osg::ShapeDrawable(unitCyl.get());
	geometry->setColor(color);

//	if(dRadisu <0.1)
	{
		osg::Vec3 v1 = center-osg::Vec3(lineLength,lineLength,lineLength);
		osg::Vec3 v2 = center+osg::Vec3(lineLength,lineLength,lineLength);
		geometry->setInitialBound(osg::BoundingBox(v1, v2));
	}

 	return geometry.release();
}


osg::Geometry* createLineGeode(osg::Vec3 pt1 , osg::Vec3 pt2 ,osg::Vec4 color)
{
	osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry(); 

	osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
	pGeometry->setVertexArray(pyramidVertices.get());
	pyramidVertices->push_back( pt1 ); 
	pyramidVertices->push_back( pt2); 

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array; 
	colors->push_back(color); 
	pGeometry->setColorArray(colors.get());
	pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL); 


	osg::ref_ptr<osg::DrawElementsUInt> pyramidBase=new osg::DrawElementsUInt(osg::PrimitiveSet::LINES); 
	pyramidBase->push_back( 0);
	pyramidBase->push_back( 1);

	pGeometry->addPrimitiveSet(pyramidBase.get() ); 

	osgUtil::SmoothingVisitor::smooth(*pGeometry.get());

	pGeometry->setUseVertexBufferObjects(false);
	pGeometry->setDataVariance(osg::Object::DYNAMIC);

	return pGeometry.release();
}


osg::Geometry* createDynamicLineGeode(osg::Vec2 pt1, osg::Vec2 pt2, osg::Vec4 color)
{
	osg::Vec3 p31(pt1, 0);
	osg::Vec3 p32(pt2, 0);

	osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry();

	osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
	pGeometry->setVertexArray(pyramidVertices.get());
	pyramidVertices->push_back(p31);
	pyramidVertices->push_back(p32);

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(color);
	pGeometry->setColorArray(colors.get() );
	pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);


	osg::ref_ptr<osg::DrawElementsUInt> pyramidBase =  new osg::DrawElementsUInt(osg::PrimitiveSet::LINES);
	pyramidBase->push_back(0);
	pyramidBase->push_back(1);

	pGeometry->addPrimitiveSet(pyramidBase.get() );

	osgUtil::SmoothingVisitor::smooth(*pGeometry.get() );

	pGeometry->setUseVertexBufferObjects(true);
	pGeometry->setDataVariance(osg::Object::DYNAMIC);

	return pGeometry.release();
}
 

osg::Geometry* createCircle(osg::Vec3 pt1 , double radius ,osg::Vec4 color,int size)
{
	{
		osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry();

		osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
		pGeometry->setVertexArray(pyramidVertices.get());


		if (size > 36)
			size = 36;
		if (size < 4)
			size = 4;
		double alfa = M_PI * 2 / size;
		for (int i = 0; i < size; i++)
		{
			osg::Vec3 pt = pt1 + osg::Vec3(radius*cos(i*alfa), radius*sin(i*alfa), 0);
			pyramidVertices->push_back(pt);
		}	

		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(color); //索引0 红色   //zmz
		pGeometry->setColorArray(colors.get() );
		pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

		osg::ref_ptr<osg::DrawElementsUInt> pyramidBase =  new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
		for (int i = 0; i < size; i++)
			pyramidBase->push_back(i); 
		pGeometry->addPrimitiveSet(pyramidBase.get() );

		pGeometry->setUseVertexBufferObjects(true);
		pGeometry->setDataVariance(osg::Object::DYNAMIC);

		return pGeometry.release();
	}
}

 


osg::Geometry* createEllipse(osg::Vec3 pt1, double a, double b, osg::Vec4 color, int size)
{
	{
		osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry();

		osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
		pGeometry->setVertexArray(pyramidVertices.get() );


		if (size > 36)
			size = 36;
		if (size < 4)
			size = 4;
		double alfa = M_PI * 2 / size;
		for (int i = 0; i < size; i++)
		{
			osg::Vec3 pt = pt1 + osg::Vec3(a*cos(i*alfa), b*sin(i*alfa), 0);
			pyramidVertices->push_back(pt);
		}

		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(redColor); //索引0 红色 
		pGeometry->setColorArray(colors.get() );
		pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

		osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = NULL;
		pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
		for (int i = 0; i < size; i++)
			pyramidBase->push_back(i);
		pGeometry->addPrimitiveSet(pyramidBase.get() );

		pGeometry->setUseVertexBufferObjects(true);
		pGeometry->setDataVariance(osg::Object::DYNAMIC);

		return pGeometry.release();
	}
}

std::vector<osg::Drawable *> createLineDim(osgText::Font* pCnFont, double textSize, double halfH, double arrorSize, osg::Vec3 p1, osg::Vec3 p2, const std::wstring& text, osg::Vec4 color,osgText::Text::AlignmentType alignment)
{
	double alfa = 0;
	if (abs(p2.x() - p1.x()) > 0.01)//竖直标注
	{
		alfa = atan( ( p2.z() - p1.z() ) / (p2.x() - p1.x() ) );
 	} 
	else
	{
		if (p1.z() < p2.z())
			alfa = M_PI*0.5;
		else
			alfa = M_PI*1.5;
	}
	alfa += M_PI*0.5;

	std::vector<osg::Drawable*> allGeometrys;
 	osg::Geometry*  pGeometry = createLineGeode(p1, p2, color);
	allGeometrys.push_back(pGeometry);
 
	osg::Vec3 p11 = p1;
	p11[0]  += halfH*cos(alfa);
	p11[2]  += halfH*sin(alfa);

	osg::Vec3 p12 = p1;
 	p12[0] += halfH*cos(alfa+M_PI)/2.4;
	p12[2] += halfH*sin(alfa+M_PI)/2.4;

	pGeometry = createLineGeode(p11, p12, color);
	allGeometrys.push_back(pGeometry);

	osg::Vec3 p21 = p2;
 	p21[0] += halfH*cos(alfa);
	p21[2] += halfH*sin(alfa);

	osg::Vec3 p22 = p2;
 	p22[0] += halfH*cos(alfa + M_PI)/2.4;
	p22[2] += halfH*sin(alfa + M_PI)/2.4;

	pGeometry = createLineGeode(p21, p22, color);
	allGeometrys.push_back(pGeometry);

	double angle2 = atan(0.715);
	pGeometry = createArror2(p1, arrorSize, alfa - M_PI*0.5, color, angle2);
	allGeometrys.push_back(pGeometry);

	pGeometry = createArror2(p2, arrorSize, alfa + M_PI*0.5, color, angle2);
	allGeometrys.push_back(pGeometry);


	allGeometrys.push_back(create3DWTextNoScale(pCnFont, (p1 + p2)*0.5, textSize, text, color, alignment));

	return allGeometrys;
}

std::vector<osg::Drawable *> createLineDim2(double textbellow, osgText::Font* pCnFont, double textSize, double halfH, double arrorSize, osg::Vec3 p1, osg::Vec3 p2, const CString& text, osg::Vec4 color, osgText::Text::AlignmentType alignment, BOOL bAutoAScale)
{
	bool bVert = false;
	double alfa = 0;
	if (abs(p2.x() - p1.x()) > 0.01)
	{
		alfa = atan((p2.z() - p1.z()) / (p2.x() - p1.x()));
	}
	else//竖直标注
	{
		bVert = true;
		if (p1.z() < p2.z())
			alfa = M_PI*0.5;
		else
			alfa = M_PI*1.5;
	}
	alfa += M_PI*0.5;

	std::vector<osg::Drawable*> allGeometrys;
	osg::Geometry*  pGeometry = createLineGeode(p1, p2, color);
	allGeometrys.push_back(pGeometry);

	osg::Vec3 p11 = p1;
	p11[0] += halfH*cos(alfa);
	p11[2] += halfH*sin(alfa);

	osg::Vec3 p12 = p1;
	p12[0] += halfH*cos(alfa + M_PI) / 2.4;
	p12[2] += halfH*sin(alfa + M_PI) / 2.4;

	pGeometry = createLineGeode(p11, p12, color);
	allGeometrys.push_back(pGeometry);

	osg::Vec3 p21 = p2;
	p21[0] += halfH*cos(alfa);
	p21[2] += halfH*sin(alfa);

	osg::Vec3 p22 = p2;
	p22[0] += halfH*cos(alfa + M_PI) / 2.4;
	p22[2] += halfH*sin(alfa + M_PI) / 2.4;

	pGeometry = createLineGeode(p21, p22, color);
	allGeometrys.push_back(pGeometry);

	double angle2 = atan(0.3);
	pGeometry = createArror2(p1, arrorSize, alfa - M_PI*0.5, color, angle2);
	allGeometrys.push_back(pGeometry);

	pGeometry = createArror2(p2, arrorSize, alfa + M_PI*0.5, color, angle2);
	allGeometrys.push_back(pGeometry);

	osg::Vec3 texPos = (p1 + p2)*0.5;
	if (bVert)
		texPos[0] += textbellow;
	else
		texPos[2] += textbellow;


	osgText::Text* pText = create3DText(pCnFont, texPos, textSize, text, color, alignment);
	if (bVert)
		pText->setLayout(osgText::TextBase::VERTICAL);

	allGeometrys.push_back(pText);

	return allGeometrys;
}
// 2016.8.10 hzz 
std::vector<osg::Drawable *> createSingleArrowLineDim3(double textbellow, osgText::Font* pCnFont, double textSize, double halfH, double arrorSize, std::vector<osg::Vec3>&vecPnts,const CString& text, osg::Vec4 color, osgText::Text::AlignmentType alignment, BOOL bAutoAScale)
{
	std::vector<osg::Drawable*> allGeometrys;
	if (vecPnts.size() < 2)
	{
		return allGeometrys;
	}
	osg::Vec3& p1 = vecPnts[0] ;
	osg::Vec3& p2 = vecPnts[1];
	int  bVert = 0;
	double alfa = 0;
	if (abs(p2.x() - p1.x()) > 0.01)
	{
		alfa = atan((p2.y() - p1.y()) / (p2.x() - p1.x()));
	}
	else//竖直标注
	{
		bVert = 1;
		if (p1.y() < p2.y())
		{
			alfa = M_PI*0.5;

		}
		else
		{
			alfa = M_PI*1.5;
			bVert = 2;
		}
	}
	alfa += M_PI*0.5;
	// 存储几何图元
	// 多直线
	osg::Geometry*  pGeometry = NULL;
	for (register size_t i = 0; i < vecPnts.size(); i++)
	{
		if (i == vecPnts.size() - 1)
		{
			continue;
		}
		allGeometrys.push_back(createLineGeode(vecPnts[i], vecPnts[(i + 1) % vecPnts.size()], color));
	}
	// 起点边界线
	/*
	osg::Vec3 p11 = p1;
	p11[0] += halfH*cos(alfa);
	p11[1] += halfH*sin(alfa);

	osg::Vec3 p12 = p1;
	p12[0] += halfH*cos(alfa + M_PI) / 2.4;
	p12[1] += halfH*sin(alfa + M_PI) / 2.4;

	pGeometry = createLineGeode(p11, p12, color);
	allGeometrys.push_back(pGeometry);
	*/
	// 终点边界线
	/*
	osg::Vec3 p21 = p2;
	p21[0] += halfH*cos(alfa);
	p21[1] += halfH*sin(alfa);

	osg::Vec3 p22 = p2;
	p22[0] += halfH*cos(alfa + M_PI) / 2.4;
	p22[1] += halfH*sin(alfa + M_PI) / 2.4;

	pGeometry = createLineGeode(p21, p22, color);
	allGeometrys.push_back(pGeometry);
	*/
	// 起点的箭头
	osg::Vec3 center = (p1 + p2)*0.5;
	double angle2 = atan(0.3);
	int  flag1 = 1;
	if (bVert == 0)
		flag1 = (center - p1).x() > 0 ? 1 : -1;
	pGeometry = createArror(p1, arrorSize, alfa - flag1* M_PI*0.5, color, angle2);
	allGeometrys.push_back(pGeometry);
	// 终点箭头
	/*
	int flag2 = -1;
	if (bVert == 0)
		flag2 = (center - p2).x() > 0 ? 1 : -1;
	pGeometry = createArror(p2, arrorSize, alfa - flag2*M_PI*0.5, color, angle2);
	allGeometrys.push_back(pGeometry);
	*/
	// 标注字体
	// 标注字体的位置
	osg::Vec3 texPos = (vecPnts[vecPnts.size() - 2] + vecPnts[vecPnts.size() - 1])*0.5;
	if (bVert == 1)
	{
		texPos[0] -= textbellow;
	}
	else if (bVert == 2)
		texPos[0] += textbellow;
	else
		texPos[1] += textbellow;
	osgText::Text* pText = create3DText(pCnFont, texPos, textSize, text, color, alignment);
	pText->setAxisAlignment(osgText::Text::XY_PLANE);
	allGeometrys.push_back(pText);
	return allGeometrys;
}
// 2016.8.10 hzz 
std::vector<osg::Drawable *> createSingleArrowLine(double arrorSize, std::vector<osg::Vec3>&vecPnts,osg::Vec4 color)
{
	std::vector<osg::Drawable*> allGeometrys;
	if (vecPnts.size() < 2)
	{
		return allGeometrys;
	}
	osg::Vec3& p1 = vecPnts[vecPnts.size() - 2];
	osg::Vec3& p2 = vecPnts[vecPnts.size() - 1];
	int  bVert = 0;
	double alfa = 0;
	if (abs(p2.x() - p1.x()) > 0.01)
	{
		alfa = atan((p2.y() - p1.y()) / (p2.x() - p1.x()));
	}
	else//竖直标注
	{
		bVert = 1;
		if (p1.y() < p2.y())
		{
			alfa = M_PI*0.5;

		}
		else
		{
			alfa = M_PI*1.5;
			bVert = 2;
		}
	}
	alfa += M_PI*0.5;
	osg::Geometry*  pGeometry = NULL;
	for (register size_t i = 0; i < vecPnts.size(); i++)
	{
		if (i == vecPnts.size() - 1)
		{
			continue;
		}
		allGeometrys.push_back(createLineGeode(vecPnts[i], vecPnts[(i + 1) % vecPnts.size()], color));
	}
	// 起点的箭头
	osg::Vec3 center = (p1 + p2)*0.5;
	double angle2 = atan(0.3);
	/*
	int  flag1 = 1;
	if (bVert == 0)
		flag1 = (center - p1).x() > 0 ? 1 : -1;
	pGeometry = createArror(p1, arrorSize, alfa - flag1* M_PI*0.5, color, angle2);
	allGeometrys.push_back(pGeometry);
	*/
	// 终点箭头
	//
	int flag2 = -1;
	if (bVert == 0)
	flag2 = (center - p2).x() > 0 ? 1 : -1;
	pGeometry = createArror(p2, arrorSize, alfa - flag2*M_PI*0.5, color, angle2);
	allGeometrys.push_back(pGeometry);
	//
	return allGeometrys;
}
std::vector<osg::Drawable *> createLineDim3(double textbellow, osgText::Font* pCnFont, double textSize, double halfH, double arrorSize, osg::Vec3 p1, osg::Vec3 p2, const CString& text, osg::Vec4 color, osgText::Text::AlignmentType alignment, BOOL bAutoAScale)
{
	int  bVert = 0;
	double alfa = 0;
	if (abs(p2.x() - p1.x()) > 0.01)
	{
		alfa = atan((p2.y() - p1.y()) / (p2.x() - p1.x()));
	}
	else//竖直标注
	{
		bVert = 1;
		if (p1.y() < p2.y())
		{
			alfa = M_PI*0.5;

		}
		else
		{
			alfa = M_PI*1.5;
			bVert = 2;
		}
	}
	alfa += M_PI*0.5;

	std::vector<osg::Drawable*> allGeometrys;
	osg::Geometry*  pGeometry = createLineGeode(p1, p2, color);
	allGeometrys.push_back(pGeometry);

	osg::Vec3 p11 = p1;
	p11[0] += halfH*cos(alfa);
	p11[1] += halfH*sin(alfa);

	osg::Vec3 p12 = p1;
	p12[0] += halfH*cos(alfa + M_PI) / 2.4;
	p12[1] += halfH*sin(alfa + M_PI) / 2.4;

	pGeometry = createLineGeode(p11, p12, color);
	allGeometrys.push_back(pGeometry);

	osg::Vec3 p21 = p2;
	p21[0] += halfH*cos(alfa);
	p21[1] += halfH*sin(alfa);

	osg::Vec3 p22 = p2;
	p22[0] += halfH*cos(alfa + M_PI) / 2.4;
	p22[1] += halfH*sin(alfa + M_PI) / 2.4;

	pGeometry = createLineGeode(p21, p22, color);
	allGeometrys.push_back(pGeometry);

	osg::Vec3 center = (p1 + p2)*0.5;

	double angle2 = atan(0.3);

	int  flag1 = 1;
	if (bVert==0)
		flag1=(center - p1).x() > 0 ? 1 : -1;

	pGeometry = createArror(p1, arrorSize, alfa - flag1* M_PI*0.5, color, angle2);
	allGeometrys.push_back(pGeometry);
	
	int flag2 =  -1;
	if (bVert==0)
		flag2 = (center - p2).x() > 0 ? 1 : -1;

	pGeometry = createArror(p2, arrorSize, alfa - flag2*M_PI*0.5, color, angle2);
	allGeometrys.push_back(pGeometry);

	osg::Vec3 texPos = (p1 + p2)*0.5;
	if (bVert == 1)
	{
		texPos[0] -= textbellow;
	}
	else if (bVert ==2)
		texPos[0] += textbellow;
	else
		texPos[1] += textbellow;


	osgText::Text* pText = create3DText(pCnFont, texPos, textSize, text, color, alignment);

	pText->setAxisAlignment(osgText::Text::XY_PLANE);

	//if (bVert)
	//	pText->setLayout(osgText::TextBase::VERTICAL);

	allGeometrys.push_back(pText);

	return allGeometrys;
}



std::vector<osg::Drawable *> createLineDim4( double textbellow, osgText::Font* pCnFont, double textSize, double halfH, double arrorSize, std::vector<osg::Vec3>dimPos , const CString& text, osg::Vec4 color, osgText::Text::AlignmentType alignment, BOOL bAutoAScale)
{
	std::vector<osg::Drawable*> allGeometrys;

	int dimPosCount = dimPos.size();

	if (dimPosCount < 2)
		return allGeometrys;

	osg::Vec3  p1 = dimPos[0];
	osg::Vec3  p0 = dimPos[1];


	osg::Vec3  p3 = dimPos[dimPosCount - 2];
	osg::Vec3  p2 = dimPos[dimPosCount - 1];

 
	bool bVert1 = false;
	double alfa1 = 0;
	if (abs(p0.x() - p1.x()) > 0.01)
	{
		alfa1 = atan((p0.z() - p1.z()) / (p0.x() - p1.x()));
	}
	else//竖直标注
	{
		bVert1 = true;
		if (p1.z() < p0.z())
			alfa1 = M_PI*0.5;
		else
			alfa1 = M_PI*1.5;
	}
	alfa1 += M_PI*0.5;


	bool bVert2 = false;
	double alfa2 = 0;
	if (abs(p2.x() - p3.x()) > 0.01)
	{
		alfa2 = atan((p2.z() - p3.z()) / (p2.x() - p3.x()));
	}
	else//竖直标注
	{
		bVert1 = true;
		if (p3.z() < p2.z())
			alfa2 = M_PI*0.5;
		else
			alfa2 = M_PI*1.5;
	}
	alfa2 += M_PI*0.5;




	osg::Geometry*  pGeometry = NULL;
	for (int i = 0; i < dimPosCount-1; i++)
	{
		pGeometry = createLineGeode(dimPos[i], dimPos[i+1], color);
		allGeometrys.push_back(pGeometry);
	}

	osg::Vec3 p11 = p1;
	p11[0] += halfH*cos(alfa1);
	p11[2] += halfH*sin(alfa1);

	osg::Vec3 p12 = p1;
	p12[0] += halfH*cos(alfa1 + M_PI) / 2.4;
	p12[2] += halfH*sin(alfa1 + M_PI) / 2.4;
	pGeometry = createLineGeode(p11, p12, color);
	allGeometrys.push_back(pGeometry);

 
	osg::Vec3 p21 = p2;
	p21[0] += halfH*cos(alfa2);
	p21[2] += halfH*sin(alfa2);

	osg::Vec3 p22 = p2;
	p22[0] += halfH*cos(alfa2 + M_PI) / 2.4;
	p22[2] += halfH*sin(alfa2 + M_PI) / 2.4;

	pGeometry = createLineGeode(p21, p22, color);
	allGeometrys.push_back(pGeometry);

	double angle2 = atan(0.3);
	pGeometry = createArror2(p1, arrorSize, alfa1 - M_PI*0.5, color, angle2);
	allGeometrys.push_back(pGeometry);

	pGeometry = createArror2(p2, arrorSize, alfa2 + M_PI*0.5, color, angle2);
	allGeometrys.push_back(pGeometry);

	osg::Vec3 texPos = (p1 + p2)*0.5;
	if (dimPosCount % 2 == 1)
		texPos = dimPos[dimPosCount*0.5];
	else
	{
		texPos = (dimPos[dimPosCount*0.5 - 1] + dimPos[dimPosCount*0.5])*0.5;
	}

	if (bVert1)
		texPos[0] += textbellow;
	else
		texPos[2] += textbellow;

	if (bAutoAScale)
	{
		allGeometrys.push_back(create3DTextNoScale(pCnFont, texPos, textSize, text, color, alignment));
	}
	else
	{
		osgText::Text* pText = create3DText(pCnFont, texPos, textSize, text, color, alignment);
		//if (bVert1)
		//	pText->setLayout(osgText::TextBase::VERTICAL);
		allGeometrys.push_back(pText);
	}

	return allGeometrys;
}

osg::Geometry* createLinesGeode(osg::Vec3Array *points, osg::Vec4 color)
{
	osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry();

	pGeometry->setVertexArray( points ); 

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(color); 
	pGeometry->setColorArray(colors.get() ); 
	pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL); 

	pGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, points->size())); 

	osgUtil::SmoothingVisitor::smooth(*pGeometry );

	return pGeometry.release();
}

osg::Node* createHudLine(CViewControlData* pOSG,  osg::Vec2 p1 , osg::Vec2 p2, osg::Geometry*& pGeometry )
{
	osg::ref_ptr<osg::Geode> pGeode = new osg::Geode();
	pGeode->setName("hudline");
	pGeometry =createDynamicLineGeode(p1,p2,redColor );
	pGeode->addDrawable(pGeometry );

	osg::Camera*  pCamera =  createHUDObj(pGeode.get() ,TRUE , pOSG);

	pCamera->setName("hudLine");
	osg::StateSet* stateset = pCamera->getOrCreateStateSet();
 	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

	return pCamera;
}

osg::Geometry* createBoxGeode(osg::Vec3 pt1 , osg::Vec3 pt2)
{
	osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry();

	osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
	pGeometry->setVertexArray( pyramidVertices.get() ); 
	pyramidVertices->push_back( pt1 ); 
	pyramidVertices->push_back( osg::Vec3(pt1.x() , pt2.y(), pt1.z() ) ); 
	pyramidVertices->push_back( pt2); 
	pyramidVertices->push_back( osg::Vec3(pt2.x() , pt1.y(), pt1.z() ) ); 

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(redColor ); //索引0 红色 
	pGeometry->setColorArray(colors.get()); 
	pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL); 

	osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = NULL;
	pyramidBase=new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP); 
	pyramidBase->push_back( 0);
	pyramidBase->push_back( 1);
	pyramidBase->push_back( 2);
	pyramidBase->push_back( 3);

	pGeometry->addPrimitiveSet(pyramidBase.get() ); 	

	pGeometry->setUseVertexBufferObjects(true);
	pGeometry->setDataVariance(osg::Object::DYNAMIC);

	return pGeometry.release();
}

  


osg::Node	* GetChildGroupByName(const CString& name , osg::Node* pNode)
{
	if(pNode)
	{	
		osg::Group* pParent =  pNode->asGroup();
		if(pParent)
		{
			int childCount = pParent->getNumChildren();
			for(int i=0; i< childCount; i++)
			{
				osg::Node* pChild = pParent->getChild(i);
				if (pChild->asGroup() != NULL && (pChild->getName().c_str() == name))
				{
					return pChild;
				}
			}	
			for(int i=0; i< childCount; i++)
			{
				osg::Node* pChild = pParent->getChild(i);
				osg::Group* pGroup =  pChild->asGroup();
				if(pGroup )
				{
					osg::Node* pFind = GetChildGroupByName(name, pGroup);
					if(pFind )
						return pFind;
				}		
			}	
		}

	}
	return NULL;
}

 
osgText::Text* CreateTextImplement(osgText::Font* pFont, const osg::Vec3& pos, float characterSize,  const osg::Vec4& color, osgText::Text::AlignmentType Alignment, BOOL bEnChart)
{
	bool useVBOs = false;

	osg::ref_ptr<osgText::Text> text = new osgText::Text;
	text->setUseVertexBufferObjects(useVBOs);
	text->setFont(pFont);
	text->setColor(color);
	text->setCharacterSize(characterSize);
	text->setPosition(pos);

 	text->setAlignment(Alignment);

	text->setFontResolution(64, 64); // even smoother but again higher texture memory usage.
 	
	text->setAxisAlignment(osgText::Text::SCREEN);
	text->setCharacterSizeMode(osgText::TextBase::SCREEN_COORDS);

	return text.release();
}

osgText::Text* create3DTextNoScale(osgText::Font* pFont,  const osg::Vec3& pos, float characterSize, const CString& message, const osg::Vec4& color, osgText::Text::AlignmentType Alignment,BOOL bEnChart)
{
	osgText::Text* text = CreateTextImplement(pFont, pos, characterSize, color, Alignment, bEnChart); 
	if (bEnChart)
		text->setText(cdb_string_tools::CString_to_string(message));
	else
	{
		WCHAR szwChar[512];
		int nChar = MultiByteToWideChar(CP_ACP, 0, message, -1, szwChar, 512);
		text->setText(szwChar);
	}
 
	return text;
}


osgText::Text* create3DWTextNoScale(osgText::Font* pCnFont, const osg::Vec3& pos, float characterSize, const std::wstring& message,const osg::Vec4& color, osgText::Text::AlignmentType Alignment,BOOL bEnChart)
{
	osgText::Text* text = CreateTextImplement(pCnFont, pos, characterSize, color, Alignment, bEnChart);
	text->setText(message.c_str());
 
	return text;
}
 
osgText::Text* create3DText(osgText::Font* pEnFont, const osg::Vec3& pos, float characterSize, const CString& message, const osg::Vec4& color, osgText::Text::AlignmentType Alignment)
{
	bool useVBOs = false;

	osg::ref_ptr<CDB4Text> text = new CDB4Text;
	text->setUseVertexBufferObjects(useVBOs);
 	text->setFont(pEnFont);
	text->setColor(color);
	text->setCharacterSize(characterSize);
	text->setPosition(pos);

	text->setAlignment(Alignment);	
 
	text->setFontResolution(16, 16); // even smoother but again higher texture memory usage.
	text->setAxisAlignment(osgText::Text::SCREEN); 
	text->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

	text->setText( cdb_string_tools::CString_to_wstring(message).c_str() ); 
  	return text.release();
}

osg::Vec3 compute_pnt( std::vector<osg::Vec3>& evecs,double  * coff )
{
	osg::Vec3  pnt;
	pnt += osg::Vec3( evecs[ 0 ]*coff[0]);
	pnt += osg::Vec3( evecs[ 1 ]*coff[1]);
	pnt += osg::Vec3( evecs[ 2 ]*coff[2]);
	return  pnt;
}
void CalcOBB(std::vector<osg::Vec3>& arr, 	osg::Vec3  & base_pnt, osg::Vec3  & side_0, osg::Vec3  & side_1, osg::Vec3  & side_2 )
{
		//double  boxpt[3], boxvecs[3][3];
		double  oon;
		double  mCoVars[3][3], mXForm[3][3];
		//double  ptAvg[3], ptAvgSq[3], ptAvgXS[3];
		osg::Vec3  ptAvg, ptAvgSq, ptAvgXS;
		double  EigenVals[3];
		double  XfMin[3], XfMax[3];
		double  XfTemp;
		//int  i;

		if(arr.size() == 0 )
			return;


		oon = 1.0 / (double)arr.size();

		/* Make averages for covariance matrix. */

		/* Intitialize averages. */
		ptAvg.set(0,0,0);
		ptAvgSq.set(0,0,0);
		ptAvgXS.set(0,0,0);

		/* Sum all point data. */
		for  ( int  i = 0; i < arr.size(); i++ )
		{
			ptAvg = ptAvg+ arr[ i ] ;

			ptAvgSq = ptAvgSq+ osg::Vec3( arr[i].x() * arr[i].x(), arr[i].y() * arr[i].y() ,arr[i].z() * arr[i].z() );
			ptAvgXS = ptAvgXS+ osg::Vec3( arr[i].x() * arr[i].y(), arr[i].x() * arr[i].z() ,arr[i].y() * arr[i].z() )	;	
		}

		/* Divide sums to make average. */
		ptAvg*= oon;
		ptAvgSq*= oon ;
		ptAvgXS*= oon;

		/* Compose covariance matrix. */
		mCoVars[0][0] = ptAvgSq.x() - ptAvg.x()*ptAvg.x();
		mCoVars[0][1] = ptAvgXS.x() - ptAvg.x()*ptAvg.y();
		mCoVars[0][2] = ptAvgXS.y() - ptAvg.x()*ptAvg.z();

		mCoVars[1][0] = ptAvgXS.x() - ptAvg.y()*ptAvg.x();
		mCoVars[1][1] = ptAvgSq.y() - ptAvg.y()*ptAvg.y();
		mCoVars[1][2] = ptAvgXS.z() - ptAvg.y()*ptAvg.z();

		mCoVars[2][0] = ptAvgXS.y() - ptAvg.z()*ptAvg.x();
		mCoVars[2][1] = ptAvgXS.z() - ptAvg.z()*ptAvg.y();
		mCoVars[2][2] = ptAvgSq.z() - ptAvg.z()*ptAvg.z();
		/* Find eigenvectors for the covariance matrix. */

		/* put eigenvectors into mXForm */
		//EigenSymmetric3(mCoVars, EigenVals, mXForm);
		Jacobi_EigenSymmetric3( mCoVars, EigenVals, mXForm );

		/* Normalize the eigenvectors to make unit axes. */
		std::vector<osg::Vec3>  evec;
		evec.resize(3);
		for  ( int  i = 0; i < 3; i++ )
		{	
			evec[ i ] = osg::Vec3( mXForm[i][0], mXForm[i][1],mXForm[i][2] );	
		}

		/* find minimum/maximum of points in new coord
		system defined by the eigenvectors */

		/* initialize min & max with first point's values */
		XfMax[ 0 ] = XfMin[ 0 ] = evec[ 0 ]* arr[ 0 ] ;
		XfMax[ 1 ] = XfMin[ 1 ] = evec[ 1 ]* arr[ 0 ];
		XfMax[ 2 ] = XfMin[ 2 ] = evec[ 2 ]* arr[ 0 ] ;

		/* check subsequent points */
		for  ( int ind = 1; ind < arr.size(); ind++)
		{
			XfTemp = evec[ 0 ]* arr[ ind ];
			XfMin[ 0 ] = min( XfMin[ 0 ], XfTemp );
			XfMax[ 0 ] = max( XfMax[ 0 ], XfTemp );

			XfTemp = evec[ 1 ]* arr[ ind ] ;
			XfMin[ 1 ] = min( XfMin[ 1 ], XfTemp );
			XfMax[ 1 ] = max( XfMax[ 1 ], XfTemp );

			XfTemp = evec[ 2 ]* arr[ ind ] ;
			XfMin[ 2 ] = min( XfMin[ 2 ], XfTemp );
			XfMax[ 2 ] = max( XfMax[ 2 ], XfTemp );
		}

		/* Now the orthogonal bounding box in transformed
		coordinates is in XfMin and XfMax. */
		base_pnt = compute_pnt( evec, XfMin );

		side_0 = evec[ 0 ];
		side_0*=( XfMax[ 0 ] - XfMin[ 0 ] );

		side_1 = evec[ 1 ];
		side_1*=( XfMax[ 1 ] - XfMin[ 1 ] );

		side_2 = evec[ 2 ];
		side_2*=( XfMax[ 2 ] - XfMin[ 2 ] );    

}
std::vector<CString> GetStrPosition(const CString& strValue)
{
	std::vector<CString> vecPos;

	CString x;
	CString y;
	CString z;

	CString strPos(strValue);
	strPos.Replace("，", ","); 
	int pos = strValue.Find("：");
	if (pos != -1)
		strPos = strValue.Mid(pos + 2);

	pos = strPos.Find(",");
	if (pos != -1)
	{
		x= strPos.Left(pos);
		CString right = strPos.Mid(pos + 1);
		pos = right.Find(",");
		if (pos != -1)
		{
			y = right.Left(pos);
			z =right.Mid(pos + 1);
		}
		else
		{
			y = right;
		}

	}
	else
	{
		x = strPos;
	}
	vecPos.push_back( (LPSTR)(LPCTSTR)x);
	vecPos.push_back((LPSTR)(LPCTSTR)y);
	vecPos.push_back((LPSTR)(LPCTSTR)z);

	return vecPos;
}
 
osg::Vec3 GetPosition(const CString& strValue)
{
	double x=0;
	double y = 0;
	double z = 0;

	CString strPos(strValue);
	strPos.Replace("，", ",");
 	int pos = strValue.Find("：");
	if(pos != -1)
		strPos = strValue.Mid(pos+2);

	pos = strPos.Find(",");
	if(pos != -1)
	{
		x = atof( strPos.Left(pos));
		CString right = strPos.Mid(pos+1);
		pos = right.Find(",");
		if(pos != -1)
		{
			y = atof(right.Left(pos));
			z = atof(right.Mid(pos + 1));
		}
		else
		{
			y = atof(right);
		}

	}
	else
	{
		x = atof(strPos);
	}
	return osg::Vec3(x,y,z);
}

void analyzeStringFormatOfCreateNode(const CString& strValue, osg::Vec3& start,osg::Vec3& delta , int& nCount)
{
	CString strPos(strValue); 
	int pos = strValue.Find("：");
	if(pos != -1)
		strPos = strValue.Mid(pos+2); //去掉命令提示
	strPos.Replace("；",";"); //替换大写的“；”

	CString strStart,strDelta, strCount;
	pos = strPos.Find(";");
	if(pos != -1)         //起点
	{
		strStart = strPos.Left(pos);

		strPos = strPos.Mid(pos+1);
		pos = strPos.Find(";");
		if( pos != -1 )
		{
			strDelta = strPos.Left(pos);
			delta = GetPosition(strDelta);
			strCount= strPos.Mid(pos+1);
			nCount = atoi(strCount);
		}
		else
		{
			delta = GetPosition(strPos);
			nCount=2;
		}
	}
	else
	{
		start = GetPosition(strPos);
		nCount = 1;
		return;
	}
}

osg::Node* createHudSelectBOX(CViewControlData* pOSG,  osg::Vec2 p1 , osg::Vec2 p2 , osg::Geometry*& pGeometry)
{	
	osg::Vec3 p31(p1, 0);
	osg::Vec3 p32(p2, 0);
	osg::ref_ptr<osg::Geode> pGeode = new osg::Geode();
	pGeode->setName("hudBox");
	pGeometry = createBoxGeode(p31, p32);
	pGeode->addDrawable(pGeometry );	

	osg::Camera*  pCamera =  createHUDObj(pGeode ,TRUE , pOSG);

	pCamera->setName("hudBox");
	osg::StateSet* stateset = pCamera->getOrCreateStateSet();
	{
 		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	}
	return pCamera;
}

//两点创建抬头显示选择框，窗口坐标
 osg::Node* createHudSelectCircle(CViewControlData* pOSG, osg::Vec2 p1, osg::Vec2 p2, osg::Geometry*& pGeometry)
{
	double r = (p2 - p1).length();
 
	double alfa = 2 * M_PI / 36;

	osg::Vec3 p31(p1, 0);

	osg::ref_ptr<osg::Geode> pGeode = new osg::Geode();
	pGeode->setName("hudCircle");
	pGeometry = createCircle(p31, r, redColor, 36);

	pGeode->addDrawable(pGeometry);

	osg::Camera*  pCamera = createHUDObj(pGeode.get(), TRUE, pOSG);

	pCamera->setName("hudCircle");
	osg::StateSet* stateset = pCamera->getOrCreateStateSet();
	{
 		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	}
	return pCamera;
}
//两点创建抬头显示选择框，窗口坐标
 osg::Node* createHudSelectEllipse(CViewControlData* pOSG, osg::Vec2 p1, osg::Vec2 p2, osg::Vec2 p3, osg::Geometry*& pGeometry)
 {
	 osg::Vec3 p31(p1, 0);
	 osg::Vec3 p32(p2, 0);
	 osg::ref_ptr<osg::Geode> pGeode = new osg::Geode();
	 pGeode->setName("hudBox");
	 pGeometry = createEllipse(p31, (p2-p1).length() ,(p3-p1).length() ,redColor,36);
	 pGeode->addDrawable(pGeometry);

	 osg::Camera*  pCamera = createHUDObj(pGeode.get(), TRUE, pOSG);

	 pCamera->setName("hudBox");
	 osg::StateSet* stateset = pCamera->getOrCreateStateSet();
	 {
		 stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	 }
	 return pCamera;
 }
//两点创建抬头显示选择框，窗口坐标
 osg::Node* createHudSelectTorus(CViewControlData* pOSG, osg::Vec2 p1, osg::Vec2 p2, osg::Vec2 p3, osg::Geometry*& pGeometry)
  {
	  osg::Vec3 p31(p1, 0);
	  osg::Vec3 p32(p2, 0);
	  osg::ref_ptr<osg::Geode> pGeode = new osg::Geode();
	  pGeode->setName("hudBox");
	  pGeometry = createBoxGeode(p31, p32);
	  pGeode->addDrawable(pGeometry);

	  osg::Camera*  pCamera = createHUDObj(pGeode.get(), TRUE, pOSG);

	  pCamera->setName("hudBox");
	  osg::StateSet* stateset = pCamera->getOrCreateStateSet();
	  {
		  stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	  }
	  return pCamera;
  }


osg::Camera* createOffsetHUDCamera(BOOL bUsedWinSize,CViewControlData* pOSG)
{
	if(pOSG == NULL)
		return NULL;

	double dx,dy;
	pOSG->GetOffsetWindowSize(dx,dy);	

	osg::ref_ptr<osg::Camera> camera = new osg::Camera;

	if(!bUsedWinSize )
		camera->setProjectionMatrix(osg::Matrix::ortho2D(0,1280, 0, 1.0*1280*dy/dx ) );
	else
	{
		camera->setProjectionMatrix(osg::Matrix::ortho2D(0,dx,0,dy));
	}

	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());

	camera->setClearMask(GL_DEPTH_BUFFER_BIT);

	camera->setRenderOrder(osg::Camera::POST_RENDER);

	camera->setAllowEventFocus(false);
	return camera.release() ;
}

osg::Camera* createHUDCamera(BOOL bUsedWinSize,CViewControlData* pOSG)
{
	if(pOSG == NULL)
		return NULL;

	double dx,dy;
	pOSG->GetWindowSize(dx,dy);	

	osg::ref_ptr<osg::Camera> camera = new osg::Camera;

	if(!bUsedWinSize )
		camera->setProjectionMatrix(osg::Matrix::ortho2D(0,1280, 0, 1.0*1280*dy/dx ) );
	else
	{
		camera->setProjectionMatrix(osg::Matrix::ortho2D(0,dx,0,dy));
	}

	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());

	camera->setClearMask(GL_DEPTH_BUFFER_BIT);

	camera->setRenderOrder(osg::Camera::POST_RENDER);

	camera->setAllowEventFocus(false);
	return camera.release();
}


//创建一个HUD对象
osg::Camera* createHUDObj( osg::Geode* pChild,BOOL bUsedWinSize,CViewControlData* pOSG )
{
	osg::Camera* camera = createHUDCamera(bUsedWinSize , pOSG);
	camera->addChild(pChild);
	return camera;
}

//显示梁的梁高, axis截面的x轴方向，lx，左截面对其点和最高点的差值，永远》=0；
void GetElementOutLine(BOOL bVertSection, double factor, osg::Vec3 axis, double lx, double rx, osg::Vec3& lpt, osg::Vec3&rpt, double& dLSectionH, double& dRSectionH, osg::Vec3& lT, osg::Vec3&lB, osg::Vec3&rT, osg::Vec3& rB)
{
	///////////////////////////////////////
	osg::Vec3d worldN = rpt - lpt;
	worldN.normalize();

	osg::Vec3d localXN;  //截面的局部坐标系的X轴平行全局坐标系的X0Y平面；
	osg::Vec3d localYN;

	
	localXN = osg::Vec3d(0, 0, 1) ^ worldN;
	if (localXN.length2() < 1.0e-6 || bVertSection == 2)//柱截面
		localXN = axis; 
	localXN.normalize();


	localYN = worldN^localXN;
	if (bVertSection == 1)//竖直截面
		localYN = osg::Vec3d(0, 0, 1);
		
	localYN.normalize();
 
	lT = lpt + localYN*lx / factor;
	rT = rpt + localYN*rx / factor;
	lB = lT - localYN*dLSectionH / factor;
	rB = rT - localYN*dRSectionH / factor;
}


void GetElementOutLineNew(BOOL bVertSection, double factor, osg::Vec3 leftAxis, osg::Vec3 rightAxis, osg::Vec3 leftTangent, osg::Vec3 rightTangent, double lx, double rx, osg::Vec3& lpt, osg::Vec3&rpt, double& dLSectionH, double& dRSectionH, osg::Vec3& lT, osg::Vec3&lB, osg::Vec3&rT, osg::Vec3& rB)
{
	leftTangent.normalize();

	osg::Vec3d localXN;  //截面的局部坐标系的X轴平行全局坐标系的X0Y平面；
	osg::Vec3d localYN;


	localXN = leftAxis;
	localXN.normalize();

	localYN = leftTangent^localXN;
	if (bVertSection == 1)//竖直截面
		localYN = osg::Vec3(0, 0, 1);

	localYN.normalize();
	lT = lpt + localYN*lx / factor;
	lB = lT - localYN*dLSectionH / factor;
  
 	rightTangent.normalize();
	localXN  = rightAxis;
	localXN.normalize();

	localYN = rightTangent^localXN;
	if (bVertSection == 1)//竖直截面
		localYN = osg::Vec3(0, 0, 1);

	localYN.normalize();

 	rT = rpt + localYN*rx / factor;
 	rB = rT - localYN*dRSectionH / factor;
}

void GetElementOutLineConsiderBrokenLine(double angle, BOOL bVertSection, double factor, osg::Vec3 leftAxis, osg::Vec3 rightAxis, osg::Vec3 leftTangent, osg::Vec3 rightTangent, double lx, double rx, osg::Vec3& lpt, osg::Vec3&rpt, double& dLSectionH, double& dRSectionH, BOOL bBrokenLineL, BOOL bBrokenLineR, osg::Vec3& lT, osg::Vec3&lB, osg::Vec3&rT, osg::Vec3& rB)
{
	osg::Vec3d localLXN;  //截面的局部坐标系的X轴平行全局坐标系的X0Y平面；
	osg::Vec3d localLYN;
	osg::Vec3d localRXN;  //截面的局部坐标系的X轴平行全局坐标系的X0Y平面；
	osg::Vec3d localRYN;

	GetSectionMainAxis(angle,bVertSection,leftAxis,rightAxis,leftTangent,rightTangent,factor,lpt,rpt,bBrokenLineL,bBrokenLineR,localLXN,localLYN,localRXN,localRYN);
	  
	lT = lpt + localLYN*lx / (factor);
	lB = lT - localLYN*dLSectionH / (factor);  

	rT = rpt + localRYN*rx / (factor);
	rB = rT - localRYN*dRSectionH / (factor); 	 
}

std::vector<osg::Geometry*> CreateElementImplementSimple1(const std::vector<std::vector<osg::Vec3>> &leftSecs, const std::vector<std::vector<osg::Vec3>> &rightSecs, std::vector<bool> leftSolids, std::vector<bool> rightSolids, const std::vector<osg::Vec4> &leftColors, const std::vector<osg::Vec4> &rightColors)
{
	std::vector<osg::Geometry*> allGeometry;
	{
		if (leftSolids.size() <1)
			return allGeometry;
		if (leftSolids.size() != rightSolids.size())
			return allGeometry;
		if (leftSolids.size() != rightSecs.size())
			return allGeometry;
		if (leftSolids.size() != leftSecs.size())
			return allGeometry;


		std::vector<std::vector<osg::Vec3>> leftSecs2;
		std::vector<std::vector<osg::Vec3>> rightSecs2;
		for (int i = 0; i< leftSecs.size(); i++)
		{
			if (leftSolids[i])
			{
				leftSecs2.push_back(leftSecs[i]);
			}
		}
		for (int i = 0; i< rightSecs.size(); i++)
		{
			if (rightSolids[i])
			{
				rightSecs2.push_back(rightSecs[i]);
			}
		}
		for (int i = 0; i< leftSecs.size(); i++)
		{
			if (!leftSolids[i])
			{
				leftSecs2.push_back(leftSecs[i]);
			}
		}
		for (int i = 0; i< rightSecs.size(); i++)
		{
			if (!rightSolids[i])
			{
				rightSecs2.push_back(rightSecs[i]);
			}
		}

		for (int i = 0; i< leftSecs2.size(); i++)
		{
			osg::ref_ptr<osg::Geometry> pyramidGeometry = new osg::Geometry();

			pyramidGeometry->setName("main");

			osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
			pyramidGeometry->setVertexArray(pyramidVertices.get() );

			osg::ref_ptr<osg::Vec4Array>  colors = new osg::Vec4Array;
			colors->push_back(leftColors[i]); //索引0 红色 
			pyramidGeometry->setColorArray(colors.get() );
			pyramidGeometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);


			std::vector<osg::Vec3> leftSec = leftSecs2[i];
			std::vector<osg::Vec3> rightSec = rightSecs2[i];

			int nMinCount = min(leftSec.size(), rightSec.size());

			int start = 0;
			for (int j = 0; j< leftSec.size(); j++)
			{
				int index1 = j%nMinCount;;
				int index2 = (j + 1) % nMinCount;

				pyramidVertices->push_back(leftSec[index1]);
				pyramidVertices->push_back(leftSec[index2]);
				pyramidVertices->push_back(rightSec[index2]);
				pyramidVertices->push_back(rightSec[index1]);

				osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
				pyramidBase->push_back(start);
				pyramidBase->push_back(start + 1);
				pyramidBase->push_back(start + 2);
				pyramidBase->push_back(start + 3);

				colors->push_back(leftColors[i]);

				pyramidGeometry->addPrimitiveSet(pyramidBase.get() );
 
				start += 4;
			}

			allGeometry.push_back(pyramidGeometry.release());
		}
		//left
		if (1)
		{
			osg::ref_ptr<osg::Geometry>gtess = new osg::Geometry;

			osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
			gtess->setVertexArray(coords.get() );

			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
 			gtess->setColorArray(colors.get() );
 			gtess->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

			int nstart = 0;
			for (int j = 0; j< leftSecs2.size(); j++)
			{
				std::vector<osg::Vec3> leftSec = leftSecs2[j];
				if (leftSec.size() > 0)
				{
					int count = leftSec.size();
					for (int k = count - 1; k >= 0; k--)
					{
						coords->push_back(leftSec[k]);
					}
					colors->push_back(leftColors[j]);
					{
						osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
						for (int k = 0; k < count; k++)
							pyramidBase->push_back(nstart + k);

						gtess->addPrimitiveSet(pyramidBase.get() );
					}

					nstart += count;
				}
			}
			allGeometry.push_back(gtess.release() );
		}
		//right
		if (1)
		{
			osg::ref_ptr<osg::Geometry> gtess = new osg::Geometry;
			osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
			gtess->setVertexArray(coords.get() );


			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
 			gtess->setColorArray(colors.get() );
			gtess->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);

			int nstart = 0;
			for (int j = 0; j< rightSecs2.size(); j++)
			{
				std::vector<osg::Vec3> rightSec = rightSecs2[j];
				if (rightSec.size() > 0)
				{
					int count = rightSec.size();
					for (int k = 0; k < count; k++)
					{
						coords->push_back(rightSec[k]);
					}
			
					colors->push_back(rightColors[j]);
					{
						osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
						for (int k = 0; k < count; k++)
							pyramidBase->push_back(nstart + k);
						gtess->addPrimitiveSet(pyramidBase.get() );
					}
					nstart += count;
				}
			}
			allGeometry.push_back(gtess.release() );
		}
	}
	return allGeometry;
}

std::vector<osg::Geometry*> CreateElementImplementSimple(const osg::Vec4& color, const std::vector<std::vector<osg::Vec3>> &leftSecs, const std::vector<std::vector<osg::Vec3>> &rightSecs, std::vector<bool> leftSolids, std::vector<bool> rightSolids, BOOL bdrawLeft, BOOL bDrawRight)
{
	std::vector<osg::Geometry*> allGeometry;	
	{
		if(leftSolids.size() <1)
			return allGeometry;
		if( leftSolids.size() != rightSolids.size() )
			return allGeometry;
		if(leftSolids.size() != rightSecs.size())
			return allGeometry;
		if(leftSolids.size() != leftSecs.size() )
			return allGeometry;


		std::vector<std::vector<osg::Vec3>> leftSecs2;
		std::vector<std::vector<osg::Vec3>> rightSecs2;
		for(int i=0; i< leftSecs.size(); i++)
		{
			if(leftSolids[i])
			{
				leftSecs2.push_back(leftSecs[i]);
			}
		}
		for(int i=0; i< rightSecs.size(); i++)
		{
			if(rightSolids[i])
			{
				rightSecs2.push_back(rightSecs[i]);
			}
		}
		for(int i=0; i< leftSecs.size(); i++)
		{
			if(!leftSolids[i])
			{
				leftSecs2.push_back(leftSecs[i]);
			}
		}
		for(int i=0; i< rightSecs.size(); i++)
		{
			if(!rightSolids[i])
			{
				rightSecs2.push_back(rightSecs[i]);
			}
		}

		for(int i=0; i< leftSecs2.size(); i++)
		{			
			osg::ref_ptr<osg::Geometry> pyramidGeometry = new osg::Geometry();

			pyramidGeometry->setName("main");

			osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
			pyramidGeometry->setVertexArray( pyramidVertices.get()  ); 

			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
			colors->push_back(color ); //索引0 红色 
			pyramidGeometry->setColorArray(colors.get() ); 
			pyramidGeometry->setColorBinding(osg::Geometry::BIND_OVERALL); 


			std::vector<osg::Vec3> leftSec   = leftSecs2[i];
			std::vector<osg::Vec3> rightSec  = rightSecs2[i];	

			int nMinCount = min(leftSec.size(),rightSec.size());

			int start = 0;
			for(int j=0; j< leftSec.size(); j++)
			{
				int index1 = j%nMinCount;;
				int index2 = (j+1)%nMinCount;

				pyramidVertices->push_back( leftSec[index2] ); 
				pyramidVertices->push_back( rightSec[index2]);
				pyramidVertices->push_back( rightSec[index1]);
				pyramidVertices->push_back( leftSec[index1] ); 

				osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES);
				pyramidBase->push_back(start);
				pyramidBase->push_back( start+1);
				pyramidBase->push_back( start+2);
				pyramidBase->push_back( start+3);

				pyramidGeometry->addPrimitiveSet(pyramidBase); 
				start +=4;
			}

			allGeometry.push_back(pyramidGeometry.release());
		}	

	
		//left
	 
		//1 为选择状态下面的显示，0非现在状态下面的不现实 3 选择状态下面的显示 4非选择状态下面的不现实
		if (bdrawLeft != 4)
		{
			osg::ref_ptr<osg::Geometry>gtess = new osg::Geometry;
			if (!bdrawLeft)
				gtess->setName("left");
			osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
			gtess->setVertexArray(coords.get() );

			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
			colors->push_back(color ); //索引0 红色 
			gtess->setColorArray(colors.get() ); 
			gtess->setColorBinding(osg::Geometry::BIND_OVERALL); 

			int nstart=0;
			for(int j=0; j< leftSecs2.size(); j++)
			{	
				 	std::vector<osg::Vec3> leftSec=leftSecs2[j];
					if (leftSec.size() > 0)
					{
						int count = leftSec.size();
						for (int k = count - 1; k >= 0; k--)
						{
							coords->push_back(leftSec[k]);
						}
						{
							osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
							for (int k = 0; k < count; k++)
								pyramidBase->push_back(nstart + k);
							gtess->addPrimitiveSet(pyramidBase.get() );
 						}
						nstart += count;
					}	
			}
			allGeometry.push_back( gtess.release());
		}
		//right
		if (bDrawRight != 4)
		{
			osg::ref_ptr<osg::Geometry>gtess = new osg::Geometry;
			if (!bDrawRight)
				gtess->setName("right");
			osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
			gtess->setVertexArray(coords.get() );

			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
			colors->push_back(color ); //索引0 红色 
			gtess->setColorArray(colors.get() ); 
			gtess->setColorBinding(osg::Geometry::BIND_OVERALL); 

			int nstart=0;
			for(int j=0; j< rightSecs2.size(); j++)
			{		
				std::vector<osg::Vec3> rightSec = rightSecs2[j];
				if (rightSec.size() > 0)
				{
					int count = rightSec.size();
					for (int k = 0; k < count; k++)
					{
						coords->push_back(rightSec[k]);
					}		

					{
						osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
						for (int k = 0; k < count; k++)
							pyramidBase->push_back(nstart + k);
						gtess->addPrimitiveSet(pyramidBase.get() );
					}
					nstart += count;
				}	
			}
			allGeometry.push_back( gtess.release() );
		}
	}
	return allGeometry;
}

void Sectionhelp(std::vector<osg::Vec3>& input , std::vector<osg::Vec3>& outPut ,double z)
{
	//std::vector<CDbPoint2DEx> srcList;
	//std::vector<CDbPoint2D> desList;

	//for(int i=0; i< input.size(); i++ )
	//{
	//	CDbPoint2DEx ptEx( input[i].x() , input[i].y(),input[i].z() );
	//	srcList.push_back( ptEx); 
	//}

	//ASSERT(TRUE);

	//DataAlgorithm::GetPoint2DArray(srcList, desList, 1.0);

	//for(int i=0; i< desList.size(); i++ )
	//{
	//	outPut.push_back( osg::Vec3( desList[i].x , desList[i].y , z ) ); 
	//}
}

osg::Geometry* GetSectionGeometry2(const std::vector<std::vector<osg::Vec3>>& leftSecs2, std::vector<std::vector<osg::Vec4>>& leftColors)
{
	osg::ref_ptr<osg::Geometry>gtess = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
	gtess->setVertexArray(coords.get() );

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	gtess->setColorArray(colors.get());
	gtess->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	//osg::ref_ptr<osg::Vec3Array> normal = new osg::Vec3Array;
	//gtess->setNormalArray(normal.get());
	//gtess->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);

	int nstart=0;
	for(int j=0; j< leftSecs2.size() ; j++)
	{	
		std::vector<osg::Vec3> leftSec   = leftSecs2[j];
		std::vector<osg::Vec4> leftColor= leftColors[j];

		int count = 0;	
		count = leftSec.size();
		for (int k = count - 1; k >= 0; k--)
		{
			coords->push_back(leftSec[k]);
			colors->push_back(leftColor[k]);
		}
		
		{
			osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON);
			for (int k = 0; k < count; k++)
				pyramidBase->push_back(nstart + k);
			gtess->addPrimitiveSet(pyramidBase.get() );
 		} 

		nstart +=count;
	}	
	
	osgUtil::Tessellator tessellator;
	tessellator.setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
	tessellator.setBoundaryOnly(false);
	tessellator.setWindingType( osgUtil::Tessellator::TESS_WINDING_ODD); // so that first change in wind type makes the commonest tessellation - ODD.
	tessellator.retessellatePolygons(*gtess);

	return gtess.get() ;
}
osg::Geometry* GetSectionGeometry1(const std::vector<std::vector<osg::Vec3>> &leftSecs2, const std::vector<osg::Vec4>& leftColors, BOOL bLeft)
{
	osg::ref_ptr<osg::Geometry> gtess = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
	gtess->setVertexArray(coords.get() );

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	gtess->setColorArray(colors.get());
	gtess->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);

	//osg::ref_ptr<osg::Vec3Array> normal = new osg::Vec3Array;
	//gtess->setNormalArray(normal.get());
	//gtess->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);

	int nstart = 0;
	for (int j = 0; j < leftSecs2.size(); j++)
	{
		std::vector<osg::Vec3> leftSec = leftSecs2[j];
		if (leftSec.size() == 0)
			continue;

		int count = 0;
		count = leftSec.size();
		if (bLeft)
		{
			for (int k = 0; k < count; k++)
			{
				coords->push_back(leftSec[k]);
			}
		}
		else
		{
			for (int k = count - 1; k >= 0; k--)
			{
				coords->push_back(leftSec[k]);
			}	
		}

		{
			colors->push_back(leftColors[j]);

			osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON);
			for (int k = 0; k < count; k++)
				pyramidBase->push_back(nstart + k);
			gtess->addPrimitiveSet(pyramidBase.get() );
 		}

	 
		nstart += count;
	}


	{
		osgUtil::Tessellator tessellator;
		tessellator.setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
		tessellator.setBoundaryOnly(false);
		tessellator.setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD); // so that first change in wind type makes the commonest tessellation - ODD.
		tessellator.retessellatePolygons(*gtess);
	}
	return gtess.get() ;
}

osg::Geometry* GetSectionGeometry(const std::vector<std::vector<osg::Vec3>> &leftSecs2,	osg::Vec4 color,BOOL bLeft)
{
	osg::ref_ptr<osg::Geometry>gtess = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
	gtess->setVertexArray(coords.get() );


	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	gtess->setColorArray(colors.get());
 	gtess->setColorBinding(osg::Geometry::BIND_PER_VERTEX); 

	int nstart=0;
	for(int j=0; j< leftSecs2.size() ; j++)
	{	
		std::vector<osg::Vec3> leftSec=leftSecs2[j];
		if( leftSec.size() == 0 )
			continue;

		int count = 0;	
		count = leftSec.size();
		if (bLeft)
		{
			for (int k = 0; k < count; k++)
			{
				coords->push_back(leftSec[k]);
				colors->push_back(color);
			}
		}
		else
		{
			for (int k = count - 1; k >= 0; k--)
			{
				coords->push_back(leftSec[k]);
				colors->push_back(color);
			}			
		}

		osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON);
		for (int k = 0; k < count; k++)
			pyramidBase->push_back(nstart+k);
		gtess->addPrimitiveSet(pyramidBase.get() );
 
		nstart +=count;	
	}
	///////////////
	{
		osgUtil::Tessellator tessellator;
		tessellator.setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
		tessellator.setBoundaryOnly(false);
		tessellator.setWindingType( osgUtil::Tessellator::TESS_WINDING_ODD); // so that first change in wind type makes the commonest tessellation - ODD.
		tessellator.retessellatePolygons(*gtess);
	}	
	return gtess.release() ;
}


std::vector<osg::Geometry*> CreateElementImplement2(
	const osg::Vec4& leftColor,
	const osg::Vec4& rightColor,
	const std::vector<std::vector<osg::Vec3>> &leftSecs,
	const std::vector<std::vector<osg::Vec3>> &rightSecs,
	std::vector<bool> leftSolids,
	std::vector<bool> rightSolids)
{
	std::vector<osg::Geometry*> allGeometry;
	{
		if (leftSolids.size() <1)
			return allGeometry;
		if (leftSolids.size() != rightSolids.size())
			return allGeometry;
		if (leftSolids.size() != rightSecs.size())
			return allGeometry;
		if (leftSolids.size() != leftSecs.size())
			return allGeometry;

		std::vector<std::vector<osg::Vec3>> leftSecs2;
		std::vector<std::vector<osg::Vec3>> rightSecs2;
 
		for (int i = 0; i< leftSecs.size(); i++)
		{
			if (leftSolids[i])
			{
				leftSecs2.push_back(leftSecs[i]);
			}
		}
		for (int i = 0; i< rightSecs.size(); i++)
		{
			if (rightSolids[i])
			{
				rightSecs2.push_back(rightSecs[i]);
			}
		}
		for (int i = 0; i< leftSecs.size(); i++)
		{
			if (!leftSolids[i])
			{
				leftSecs2.push_back(leftSecs[i]);
			}
		}
		for (int i = 0; i< rightSecs.size(); i++)
		{
			if (!rightSolids[i])
			{
				rightSecs2.push_back(rightSecs[i]);
			}
		}


		//侧面
		for (int i = 0; i< leftSecs2.size(); i++)
		{
			osg::ref_ptr<osg::Geometry> pMainGeometry = new osg::Geometry();
			pMainGeometry->setName("main");

			osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
			pMainGeometry->setVertexArray(pyramidVertices.get() );

			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
			pMainGeometry->setColorArray(colors.get() );
			pMainGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

			std::vector<osg::Vec3> leftSec = leftSecs2[i];
			std::vector<osg::Vec3> rightSec = rightSecs2[i];

			int nMinCount = min(leftSec.size(), rightSec.size());

			int start = 0;
			for (int j = 0; j< leftSec.size(); j++)
			{
				int index1 = j%nMinCount;;
				int index2 = (j + 1) % nMinCount;

				pyramidVertices->push_back(leftSec[index1]);
				colors->push_back(leftColor);

				pyramidVertices->push_back(leftSec[index2]);
				colors->push_back(leftColor);

				pyramidVertices->push_back(rightSec[index2]);
				colors->push_back(rightColor);

				pyramidVertices->push_back(rightSec[index1]);
				colors->push_back(rightColor);


				osg::ref_ptr<osg::DrawElementsUInt>pyramidBase =new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS);
				pyramidBase->push_back(start);
				pyramidBase->push_back(start + 1);
				pyramidBase->push_back(start + 2);
				pyramidBase->push_back(start + 3);			


				pMainGeometry->addPrimitiveSet(pyramidBase.get() );

	 
				start += 4;
			}

			allGeometry.push_back(pMainGeometry.release());
		}

		//left	
		{
			osg::Geometry* leftGeomery = GetSectionGeometry(leftSecs2, leftColor, TRUE);
			if (leftGeomery)
			{
				allGeometry.push_back(leftGeomery);
			}
		}
		//right	
		{
			osg::Geometry* rightGeomery = GetSectionGeometry(rightSecs2, rightColor, FALSE);
			if (rightGeomery)
			{
				allGeometry.push_back(rightGeomery);
			}
		}
	}
	return allGeometry;
}


std::vector<osg::Geometry*> CreateElementImplement(const osg::Vec4& color,
												   const std::vector<std::vector<osg::Vec3>> &leftSecs, 
												   const std::vector<std::vector<osg::Vec3>> &rightSecs, 
												   std::vector<bool> leftSolids,
												   std::vector<bool> rightSolids,
												   const std::vector<std::vector<osg::Vec4>> &leftColors, 
												   const std::vector<std::vector<osg::Vec4>> &rightColors )
{
	return CreateElementImplement4(color, leftSecs, rightSecs, leftSolids, rightSolids, leftColors, rightColors, TRUE, TRUE);
}
//生成单元实体模型
std::vector<osg::Geometry*> CreateElementImplement4(const osg::Vec4& color, const std::vector<std::vector<osg::Vec3>> &leftSecs, const std::vector<std::vector<osg::Vec3>> &rightSecs, std::vector<bool> leftSolids, std::vector<bool> rightSolids, const std::vector<std::vector<osg::Vec4>> &leftColors, const std::vector<std::vector<osg::Vec4>> &rightColors, BOOL bDrawLeft, BOOL bDrawRight)
{
	std::vector<osg::Geometry*> allGeometry;
	{
		if (leftSolids.size() <1)
			return allGeometry;
		if (leftSolids.size() != rightSolids.size())
			return allGeometry;
		if (leftSolids.size() != rightSecs.size())
			return allGeometry;
		if (leftSolids.size() != leftSecs.size())
			return allGeometry;

		std::vector<std::vector<osg::Vec3>> leftSecs2;
		std::vector<std::vector<osg::Vec3>> rightSecs2;

		std::vector<std::vector<osg::Vec4>> leftColors2;
		std::vector<std::vector<osg::Vec4>> rightColors2;

		for (int i = 0; i< leftSecs.size(); i++)
		{
			if (leftSolids[i])
			{
				leftSecs2.push_back(leftSecs[i]);
			}
		}
		for (int i = 0; i< rightSecs.size(); i++)
		{
			if (rightSolids[i])
			{
				rightSecs2.push_back(rightSecs[i]);
			}
		}
		for (int i = 0; i< leftSecs.size(); i++)
		{
			if (!leftSolids[i])
			{
				leftSecs2.push_back(leftSecs[i]);
			}
		}
		for (int i = 0; i< rightSecs.size(); i++)
		{
			if (!rightSolids[i])
			{
				rightSecs2.push_back(rightSecs[i]);
			}
		}


		//侧面
		for (int i = 0; i< leftSecs2.size(); i++)
		{
			osg::ref_ptr<osg::Geometry> pMainGeometry = new osg::Geometry();
			pMainGeometry->setName("main");

			osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
			pMainGeometry->setVertexArray(pyramidVertices.get() );

			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
			pMainGeometry->setColorArray(colors.get() );
			pMainGeometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET); 

			std::vector<osg::Vec3> leftSec = leftSecs2[i];
			std::vector<osg::Vec3> rightSec = rightSecs2[i];

			std::vector<osg::Vec4> leftColor;
			std::vector<osg::Vec4> rightColor;

			int nMinCount = min(leftSec.size(), rightSec.size());
			int start = 0;

			osg::Vec3 center;
			for (int j = 0; j < nMinCount; j++)
			{
				center += leftSec[j];
				center += rightSec[j];
			}
			center /= nMinCount;
			center *= 0.5;

			for (int j = 0; j< leftSec.size(); j++)
			{
				int index1 = j%nMinCount;;
				int index2 = (j + 1) % nMinCount;

				pyramidVertices->push_back(leftSec[index1]);
				pyramidVertices->push_back(rightSec[index1]);
				pyramidVertices->push_back(rightSec[index2]);
				pyramidVertices->push_back(leftSec[index2]);

				osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS);
				pyramidBase->push_back(start);
				pyramidBase->push_back(start + 1);
				pyramidBase->push_back(start + 2);
				pyramidBase->push_back(start + 3);

				colors->push_back(color);

				pMainGeometry->addPrimitiveSet(pyramidBase.get() );

				start += 4;
			}

			allGeometry.push_back(pMainGeometry.release());
		}

		//left	
		if (bDrawLeft)
		{
			osg::Geometry* leftGeomery = GetSectionGeometry(leftSecs2, color, TRUE);
			if (leftGeomery)
			{
				allGeometry.push_back(leftGeomery);
			}
		}
		//right	
		if (bDrawRight)
		{
			osg::Geometry* rightGeomery = GetSectionGeometry(rightSecs2, color, FALSE);
			if (rightGeomery)
			{
				allGeometry.push_back(rightGeomery);
			}
		}
	}
	return allGeometry;
}





std::vector<osg::Geometry*> CreateElementImplement1(
	const std::vector<std::vector<osg::Vec3>> &leftSecs,
	const std::vector<std::vector<osg::Vec3>> &rightSecs,
	std::vector<bool> leftSolids,
	std::vector<bool> rightSolids,
	const std::vector<osg::Vec4> &leftColors,
	const std::vector<osg::Vec4> &rightColors)
{
	std::vector<osg::Geometry*> allGeometry;
	{
		if (leftSolids.size() <1)
			return allGeometry;
		if (leftSolids.size() != rightSolids.size())
			return allGeometry;
		if (leftSolids.size() != rightSecs.size())
			return allGeometry;
		if (leftSolids.size() != leftSecs.size())
			return allGeometry;

		std::vector<std::vector<osg::Vec3>> leftSecs2;
		std::vector<std::vector<osg::Vec3>> rightSecs2;

 
		for (int i = 0; i< leftSecs.size(); i++)
		{
			if (leftSolids[i])
			{
				leftSecs2.push_back(leftSecs[i]);
			}
		}
		for (int i = 0; i< rightSecs.size(); i++)
		{
			if (rightSolids[i])
			{
				rightSecs2.push_back(rightSecs[i]);
			}
		}
		for (int i = 0; i< leftSecs.size(); i++)
		{
			if (!leftSolids[i])
			{
				leftSecs2.push_back(leftSecs[i]);
			}
		}
		for (int i = 0; i< rightSecs.size(); i++)
		{
			if (!rightSolids[i])
			{
				rightSecs2.push_back(rightSecs[i]);
			}
		}


		//侧面
		for (int i = 0; i< leftSecs2.size(); i++)
		{
			osg::ref_ptr<osg::Geometry> pMainGeometry = new osg::Geometry();
			pMainGeometry->setName("main");

			osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
			pMainGeometry->setVertexArray(pyramidVertices.get() );

			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
			pMainGeometry->setColorArray(colors.get() );
			pMainGeometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET); 


			std::vector<osg::Vec3> leftSec = leftSecs2[i];
			std::vector<osg::Vec3> rightSec = rightSecs2[i];

			int nMinCount = min(leftSec.size(), rightSec.size());

			int start = 0;
			for (int j = 0; j< leftSec.size(); j++)
			{
				int index1 = j%nMinCount;;
				int index2 = (j + 1) % nMinCount;

				pyramidVertices->push_back(leftSec[index1]);
				pyramidVertices->push_back(leftSec[index2]);
				pyramidVertices->push_back(rightSec[index2]);
				pyramidVertices->push_back(rightSec[index1]);

				osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS);
				pyramidBase->push_back(start);
				pyramidBase->push_back(start + 1);
				pyramidBase->push_back(start + 2);
				pyramidBase->push_back(start + 3);			

				colors->push_back(leftColors[i]);

				pMainGeometry->addPrimitiveSet(pyramidBase.get() );
 
				start += 4;
			}

			allGeometry.push_back(pMainGeometry.release());
		}

		//left	
	
		{
			osg::Geometry* leftGeomery = GetSectionGeometry1(leftSecs2, leftColors, TRUE);
			if (leftGeomery)
			{
				allGeometry.push_back(leftGeomery);
			}
		}
		//right
		
		{
			osg::Geometry* rightGeomery = GetSectionGeometry1(rightSecs2, rightColors, FALSE);
			if (rightGeomery)
			{
				allGeometry.push_back(rightGeomery);
			}
		}
	}
	return allGeometry;
}
void CreateHotHandler(osg::Vec3 lpt , osg::Vec4 color,osg::Geode* pGeode , int size)
{	
	osg::ref_ptr<osg::Geometry> pointsGeom = new osg::Geometry();

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	vertices->push_back(lpt+ osg::Vec3(0,0,0) );
	pointsGeom->setVertexArray(vertices.get() );

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back( color );		
	pointsGeom->setColorArray(colors.get() );
	pointsGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

	pointsGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,1));

	pGeode->addDrawable(pointsGeom.get() );

	osg::StateSet* set = 	pGeode->getOrCreateStateSet();
	osg::ref_ptr<osg::Point> p = new osg::Point;
	p->setSize(size);
	set->setAttributeAndModes(p.get() , osg::StateAttribute::ON );		
}


BOOL PointIsInRect(osg::Vec2 point , osg::Vec2 p1 , osg::Vec2 p2 )
{
	double minX  = min(p1.x() , p2.x());
	double maxX = max(p1.x() , p2.x());
	double minY  = min(p1.y() , p2.y());
	double maxY = max(p1.y() , p2.y());

//////////////////////////
	if( point.x() < maxX && point.x() >minX && point.y() <maxY && point.y() >minY)
		return TRUE;
	return false;
} 



//显示属性表
void  DisPlayProperty(CViewControlData* pOSG , std::vector<osg::Node*>& allobjs)
{
	//CString strInfo;
	//strInfo.Format("选择了对象%s", allobjs[0]->getName());
	//::AfxMessageBox(strInfo);



 	if(allobjs.size() > 0  && pOSG->m_command  == 0 )
	{
		if (allobjs.size() > 1)
		{
			IDbPropertyInterface* pGrid = GetDbProperty();
			if (pGrid)
			{
				//*******内存泄漏
 				pOSG->multiObject.objCount = allobjs.size();
				pOSG->multiObject.mulObjIDs.clear();
				pOSG->multiObject.bSame=true;
				pOSG->multiObject.pOSG = pOSG;

				CString strType;

				for(int i=0; i< allobjs.size(); i++)
				{
					CDbObjInterface* p3DData = pOSG->FindObj(allobjs[i]->getName().c_str(), GetObjType(allobjs[i]));

					if(!p3DData)
						continue;

					if(strType.IsEmpty() )
						strType = p3DData->GetRTType();
					else
					{
						if(pOSG->multiObject.bSame)
						{
							CString newType = p3DData->GetRTType();
							if (newType != strType)
								pOSG->multiObject.bSame = false;
						}
						strType = p3DData->GetRTType();
					}
					//
					if(p3DData)
						pOSG->multiObject.mulObjIDs.push_back( p3DData->GetID() );
				}

				pGrid->populate(&(pOSG->multiObject));
			}
		}
		else
		{
			osg::Node* pFirst = allobjs[0];
			CDbObjInterface* p3DData = pOSG->FindObj(pFirst->getName().c_str(), GetObjType(pFirst));
			if (p3DData)
			{ 
				IDbPropertyInterface* pGrid = GetDbProperty();
				if (pGrid)
				{
					pGrid->populate(p3DData);
				}
			}
		}		
	}
}


//创建光源
osg::ref_ptr<osg::Group> createLight(osg::Vec3& direction, osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<osg::Group>  lightRoot = new osg::Group;

	osg::StateSet* lightStateSet = lightRoot->getOrCreateStateSet();
	lightStateSet->setMode( GL_LIGHTING, osg::StateAttribute::ON);
	lightStateSet->setMode( GL_LIGHT0,     osg::StateAttribute::ON);

	node->computeBound();
	osg::BoundingSphere bs = node->getBound();

	// create a  light.
	osg::ref_ptr<osg::Light> myLight1 = new osg::Light;
	myLight1->setLightNum(0);

	myLight1->setPosition( osg::Vec4(bs.center().x() - bs.radius()*direction.x() ,
		bs.center().y() - bs.radius()*direction.y() ,
		bs.center().z() - bs.radius()*direction.z() ,1.0f) );

	myLight1->setDirection( direction);


	myLight1->setAmbient(whiteColor);
	myLight1->setDiffuse(  whiteColor);

	myLight1->setConstantAttenuation(1.0f);
	myLight1->setLinearAttenuation(0.0f);
	myLight1->setQuadraticAttenuation(0.0f);

	osg::ref_ptr<osg::LightSource> lightS1 = new osg::LightSource;
	lightS1->setLight(myLight1.get() );

	lightRoot->addChild( lightS1.get() );

	return lightRoot.release();
}

//创建光源
DB_3DMODELING_API void modifyLight( osg::Vec3& direction,osg::ref_ptr<osg::Node> node, osg::Light*  pLight)
{
	node->computeBound();
	osg::BoundingSphere bs = node->getBound();

	pLight->setPosition( osg::Vec4(bs.center().x() - bs.radius()*direction.x() ,
		bs.center().y() - bs.radius()*direction.y() ,
		bs.center().z() - bs.radius()*direction.z() ,1.0f) );

	pLight->setDirection( direction);
}


osg::Geometry* CreateOBBGeomery(const osg::Vec4& color, std::vector<osg::Vec3>& vertexArray )
{
	osg::Vec3 base_pnt =  vertexArray[0];
	osg::Vec3 pt1 = vertexArray[1];
	osg::Vec3 pt2 = vertexArray[2];
	osg::Vec3 pt3 = vertexArray[3];
	osg::Vec3 pt4 = vertexArray[4];
	osg::Vec3 pt5 = vertexArray[5];
	osg::Vec3 pt6 = vertexArray[6];
	osg::Vec3 pt7 = vertexArray[7];

	osg::ref_ptr<osg::Geometry> BoundGeometry = new osg::Geometry();
	{
		osg::ref_ptr<osg::Vec4Array> color_white = new osg::Vec4Array;
		color_white->push_back(color );

		osg::ref_ptr<osg::Vec3Array> GridVertices = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec3Array> boundVertices = new osg::Vec3Array;
		/*1*/
		GridVertices->push_back( base_pnt ); 
		GridVertices->push_back( pt1 ); 

		GridVertices->push_back( pt1 ); 
		GridVertices->push_back( pt4 ); 


		GridVertices->push_back( pt4 ); 
		GridVertices->push_back( pt3 ); 

		GridVertices->push_back( pt3 ); 
		GridVertices->push_back( base_pnt ); 


		GridVertices->push_back( pt2 ); 
		GridVertices->push_back( pt6 ); 

		GridVertices->push_back( pt6 ); 
		GridVertices->push_back( pt7 ); 

		GridVertices->push_back( pt7 ); 
		GridVertices->push_back( pt5 ); 

		GridVertices->push_back( pt5 ); 
		GridVertices->push_back( pt2 ); 


		GridVertices->push_back( base_pnt ); 
		GridVertices->push_back( pt2 ); 
		GridVertices->push_back( pt1 ); 
		GridVertices->push_back( pt6 ); 
		GridVertices->push_back( pt4 ); 
		GridVertices->push_back( pt7 ); 
		GridVertices->push_back( pt3 ); 
		GridVertices->push_back( pt5 ); 


		BoundGeometry->setVertexArray( GridVertices.get()  );

		osg::ref_ptr<osg::DrawElementsUInt> GridUnits = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
		for (int i=0; i<24; i++ )
		{
			GridUnits->push_back(i);
		}
		BoundGeometry->addPrimitiveSet( GridUnits.get() );		

		BoundGeometry->setColorArray(color_white.get() );
		BoundGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);


	}
	return BoundGeometry.release() ;
}
//OBB
osg::Node*  GetObjOBB(osg::Node* selected_node)
{
	if(!selected_node)
	{
		return NULL;
	}
	std::vector<osg::Vec3> arr;

	osg::Vec3  base_pnt;
	osg::Vec3  side_0;
	osg::Vec3  side_1;
	osg::Vec3  side_2;

	int numberDrawable = selected_node->asGeode()->getNumDrawables();
	for(int i=0; i< numberDrawable; i++)
	{
		osg::Drawable* pdrawable =  selected_node->asGeode()->getDrawable(i);
		if(pdrawable)
		{
			osg::Geometry*  pGeometry = pdrawable->asGeometry();
			if(pGeometry&& pGeometry->getName() == "main")
			{
				osg::Vec3Array *pVertices =(osg::Vec3Array*)pGeometry->getVertexArray();
				if(pVertices)
				{	
					int nCount = pVertices->size();
					for(int i=0; i< nCount; i++)
					{
						if( std::find( arr.begin() , arr.end(), pVertices->at(i) )  == arr.end() )
							arr.push_back( pVertices->at(i));
					}
				}
			}
		}
	}

	if(arr.size() > 1 )
	{
		CalcOBB( arr, 	 base_pnt,  side_0,  side_1,  side_2 );
		if(side_0.length2() <90000)
		{
			side_0.normalize();
			base_pnt -=side_0*150;
			side_0*=300;
		}
		if(side_1.length2() <90000)
		{
			side_1.normalize();
			base_pnt -=side_1*150;
			side_1*=300;
		}
		if(side_2.length2() <90000)
		{
			side_2.normalize();
			base_pnt -=side_2*150;
			side_2*=300;
		}


		osg::Vec3 pt1 = base_pnt+side_0;
		osg::Vec3 pt2 = base_pnt+side_1;
		osg::Vec3 pt3 = base_pnt+side_2;
		osg::Vec3 pt4 = pt1+side_2;
		osg::Vec3 pt5 = pt3+side_1;
		osg::Vec3 pt6 = pt1+side_1;
		osg::Vec3 pt7 = pt4+side_1;			

		std::vector<osg::Vec3> vertexArray;

		vertexArray.push_back( base_pnt );
		vertexArray.push_back( pt1 );
		vertexArray.push_back( pt2 );
		vertexArray.push_back( pt3 );
		vertexArray.push_back( pt4 );
		vertexArray.push_back(pt5 );
		vertexArray.push_back( pt6 );
		vertexArray.push_back( pt7 );

		osg::Geometry* BoundGeometry = CreateOBBGeomery(redColor, vertexArray);

		osg::ref_ptr<osg::Geode> boundary = new osg::Geode();
		boundary->setName( selected_node->getName() );
		boundary->addDrawable(BoundGeometry); 	

		osg::StateSet* stateset = boundary->getOrCreateStateSet();
		BoundGeometry->setStateSet(stateset);
 		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

		osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth();
		linewidth->setWidth(2.0f);
		stateset->setAttributeAndModes(linewidth.get(),osg::StateAttribute::ON);

		return boundary.release();		
	}

	return NULL;
}



osg::Node*  GetObjOBB2(osg::Node* selected_node)
{
	if(!selected_node)
	{
		return NULL;
	}

	osg::ref_ptr<osg::Geode> boundary = new osg::Geode();
	boundary->setName( selected_node->getName() );

	std::vector<osg::Vec3> arr;
	osg::Vec3  base_pnt;
	osg::Vec3  side_0;
	osg::Vec3  side_1;
	osg::Vec3  side_2;			
	int numberDrawable = selected_node->asGeode()->getNumDrawables();
	for(int i=0; i< numberDrawable; i++)
	{
		osg::Drawable* pdrawable =  selected_node->asGeode()->getDrawable(i);
		if(pdrawable)
		{
			osg::Geometry*  pGeometry = pdrawable->asGeometry();
			if(pGeometry && pGeometry->getName() == "main")
			{
				osg::Vec3Array *pVertices =(osg::Vec3Array*)pGeometry->getVertexArray();
				if(pVertices)
				{						
					int nCount = pVertices->size();
					for(int i=0; i< nCount; i++)
					{
						if( std::find( arr.begin() , arr.end(), pVertices->at(i) )  == arr.end() )
							arr.push_back( pVertices->at(i));
					}
				}
			}
		}
	}

	if(arr.size() > 1 )
	{
		CalcOBB( arr, 	 base_pnt,  side_0,  side_1,  side_2 );
		if(side_0.length2() <90000)
		{
			side_0.normalize();
			base_pnt -=side_0*150;
			side_0*=300;
		}
		if(side_1.length2() <90000)
		{
			side_1.normalize();
			base_pnt -=side_1*150;
			side_1*=300;
		}
		if(side_2.length2() <90000)
		{
			side_2.normalize();
			base_pnt -=side_2*150;
			side_2*=300;
		}


		osg::Vec3 pt1 = base_pnt+side_0;
		osg::Vec3 pt2 = base_pnt+side_1;
		osg::Vec3 pt3 = base_pnt+side_2;
		osg::Vec3 pt4 = pt1+side_2;
		osg::Vec3 pt5 = pt3+side_1;
		osg::Vec3 pt6 = pt1+side_1;
		osg::Vec3 pt7 = pt4+side_1;			

		std::vector<osg::Vec3> vertexArray;

		vertexArray.push_back( base_pnt );
		vertexArray.push_back( pt1 );
		vertexArray.push_back( pt2 );
		vertexArray.push_back( pt3 );
		vertexArray.push_back( pt4 );
		vertexArray.push_back(pt5 );
		vertexArray.push_back( pt6 );
		vertexArray.push_back( pt7 );

		osg::Geometry* BoundGeometry = CreateOBBGeomery(redColor, vertexArray);

		boundary->addDrawable(BoundGeometry); 	

		osg::StateSet* stateset = boundary->getOrCreateStateSet();
		BoundGeometry->setStateSet(stateset);
 		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

		osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth();
		linewidth->setWidth(2.0f);
		stateset->setAttributeAndModes(linewidth.get(),osg::StateAttribute::ON);

	}
	return boundary.release();
}


//创建多边形直线
//refacor by wr
//osg::Geometry* AddGroupLine(const std::map<CString,CDbCurve3D>& curves,COLORREF color)
//{
//	std::vector< std::vector<osg::Vec3> > allPTs;
//
//	std::map<CString,CDbCurve3D>::const_iterator it = curves.begin();
//	for(; it != curves.end() ; it++)
//	{
//		std::vector<osg::Vec3> oneCurve;
//		for(int j=0; j< it->second.m_PointList.size(); j++)
//		{
//			CDbPoint3D point = it->second.m_PointList[j];
//
//			oneCurve.push_back( osg::Vec3(point.x, point.y, point.z ) );
//		}
//		allPTs.push_back( oneCurve);
//	}
//	return createMultiLineGeode(allPTs , osg::Vec4( GetRValue(color)/255.0 ,GetGValue(color)/255.0 ,GetBValue(color)/255.0 ,1 ) ,false);
//}


osg::Drawable* GetDrawable(osg::Geode* pGeode , const CString& strName)
{
	if(pGeode)
	{
		int nCount = pGeode->getNumDrawables();
		for(int i=0; i< nCount; i++)
		{
			osg::Drawable* pDrawable = pGeode->getDrawable(i);

			if (pDrawable->getName().c_str() == strName)
			{
				return pDrawable;
			}
		}
	}
	return NULL;
}




 
 
 
DB_3DMODELING_API void SetVisible( osg::Node* pNode, bool bVisible /*= true*/ )
{
	if (!pNode)
	{
		return;
	}

	if (bVisible)
	{
		pNode->setNodeMask(0xffffffff);
	}
	else
	{
		pNode->setNodeMask(0x0);
	}
}


void LoadShaderSource(osg::Shader* shader, const CString& fileName)
{
	std::string fqFileName = osgDB::findDataFile(cdb_string_tools::CString_to_string(fileName));
	if (fqFileName.length() != 0)
	{
		shader->loadShaderSourceFromFile(fqFileName.c_str());
	}
}

void CreateLightShader(osg::StateSet* pSet, const osg::Vec4& lightPosition)
{
	CString fileName("shader\\specular.vs");

	CString strFile = cdb_string_tools::GetApplicationSelfPath() + fileName;

	osg::ref_ptr<osg::Shader> pFragObj = new osg::Shader(osg::Shader::VERTEX);
	LoadShaderSource(pFragObj.get(), (LPSTR)(LPCTSTR)strFile);

	osg::ref_ptr< osg::Program > program = new osg::Program();
	program->addShader(pFragObj); 

	osg::ref_ptr<osg::Uniform> lightPos = new osg::Uniform("lightPos", lightPosition);
	pSet->addUniform(lightPos.get() );

	pSet->setAttribute(program.get(), osg::StateAttribute::ON);
	pSet->setDataVariance(osg::Object::DYNAMIC);
}

class WindowSizeUniformVarying : public osg::Uniform::Callback
{
	virtual void operator () (osg::Uniform* uniform, osg::NodeVisitor* nv)
	{
		if (m_pOSG)
		{
			double width  = m_pOSG->mView->getCamera()->getViewport()->width();
			double height = m_pOSG->mView->getCamera()->getViewport()->height();
			uniform->set(osg::Vec2(width, height));
		}
	}
public:
	CViewControlData* m_pOSG;
};
void CreateOutLineShader(CViewControlData* pOSG, osg::StateSet* pSet, int width, int height)
{
	if (!pOSG->ShaderIsSupport())
	{
		return;
	}

	CString strFile = cdb_string_tools::GetApplicationSelfPath() + "shader\\sharpen-new.fs";

	osg::ref_ptr<osg::Shader> pFragObj = new osg::Shader(osg::Shader::FRAGMENT);
	LoadShaderSource(pFragObj.get() , (LPSTR)(LPCTSTR)strFile);

	osg::ref_ptr< osg::Program > program = new osg::Program();
	program->addShader(pFragObj.get() );

	int textureWidth = width;
	int textureHeight = height;

	{
		GLfloat texCoordOffsets[18];
		GLfloat  xInc = 1.0f / (GLfloat)textureWidth;
		GLfloat  yInc = 1.0f / (GLfloat)textureHeight;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				texCoordOffsets[(((i * 3) + j) * 2) + 0] = (-1.0f * xInc) + ((GLfloat)i * xInc);
				texCoordOffsets[(((i * 3) + j) * 2) + 1] = (-1.0f * yInc) + ((GLfloat)j * yInc);
			}
		}
		osg::ref_ptr<osg::Uniform> texSize = new osg::Uniform("texSize", osg::Vec2(textureWidth, textureHeight));

		WindowSizeUniformVarying* pUniformCalllBack = new WindowSizeUniformVarying;
		pUniformCalllBack->m_pOSG = pOSG;

		texSize->setUpdateCallback(pUniformCalllBack);
		pSet->addUniform(texSize.get() );
	}

	pSet->setAttribute(program.get(), osg::StateAttribute::ON);

	pSet->setDataVariance(osg::Object::DYNAMIC);
}

osg::Node* createOutLineEffect(CViewControlData* pOSG, osg::Node* subgraph, const osg::Vec4& clearColour, double tex_width, double tex_height)
{
	osg::ref_ptr<osg::Group> parent = new osg::Group;

	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setTextureSize(tex_width, tex_height);
	texture->setInternalFormat(GL_RGBA);
	texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
	texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);


	//shder
	{
		osg::ref_ptr<osg::Group> pRealGroup = new osg::Group;
		pRealGroup->addChild(subgraph);
		osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
		stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

		pRealGroup->setStateSet(stateset.get() );

		CreateOutLineShader(pOSG ,  stateset.get(), tex_width, tex_height);

		parent->addChild(pRealGroup.get() );
	}

	if (1)
	{
		osg::ref_ptr<osg::Camera> camera = new osg::Camera;

		// set clear the color and depth buffer
		camera->setClearColor(clearColour);
		camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// just inherit the main cameras view
		camera->setReferenceFrame(osg::Transform::RELATIVE_RF);
		camera->setProjectionMatrix(osg::Matrixd::identity());
		camera->setViewMatrix(osg::Matrixd::identity());

		// set viewport
		camera->setViewport(0, 0, tex_width, tex_height);

		// set the camera to render before the main camera.
		camera->setRenderOrder(osg::Camera::PRE_RENDER);

		// tell the camera to use OpenGL frame buffer object where supported.
		camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);

		// attach the texture and use it as the color buffer.
		camera->attach(osg::Camera::COLOR_BUFFER, texture);

		// add subgraph to render
		camera->addChild(subgraph);

		parent->addChild(camera.get() );
	}
	return parent.release() ;
}


//将截面上的点转换为空间上的点
/*
axis 为截面的x轴方向
worldN 梁轴线方向
basePt  截面安装点
cx 截面对齐点X坐标
cy 截面对齐点Y坐标
dx 所求截面点的x坐标
dy 所求截面点的y坐标
返回所求截面点空间坐标
axis 截面X轴方向
worldN 梁轴线方向

*/
osg::Vec3 GetPointFromSectionToSpace(int type,osg::Vec3 axis, osg::Vec3 worldN, osg::Vec3 basePT, double cx, double cy, double dx, double dy,double alfa)
{
	worldN.normalize();

	osg::Vec3d localXN;  //截面的局部坐标系的X轴平行全局坐标系的X0Y平面；
	osg::Vec3d localYN;

 
	localXN = osg::Vec3(0, 0, 1) ^ worldN;

	if (type == 2 || localXN.length2() < 0.00001)  //为垂直单元
	{
		localXN = osg::Vec3d(0, 1, 0);
		if (alfa > 0)
		{
			osg::Quat rotation;
			rotation.makeRotate(alfa*M_PI / 180, osg::Vec3(0, 0, 1));
			osg::Matrixd mat1;
			mat1.makeRotate(rotation);
			localXN = localXN*mat1;
		}
	}

	localXN.normalize();
 
	if (type == 1)
	{
		localYN = osg::Vec3d(0, 0, 1);
		osg::Vec3d temp = osg::Vec3d(0, 0, 1) ^ worldN;
		osg::Vec3d axisV = temp^osg::Vec3d(0, 0, 1);

		osg::Quat rotation; rotation.makeRotate(alfa*M_PI / 180, axisV);
		osg::Matrixd mat1;
		mat1.makeRotate(rotation);
		localYN = localYN*mat1;
		localXN = localYN^axisV;

		localXN.normalize();
		localYN.normalize();
	}
	else
	{
		if (type == 2)
			localYN = osg::Vec3d(0, 0, 1) ^ localXN;
		else
			localYN = worldN^localXN;
		localYN.normalize();
	}

	double ddx = dx - cx;
	double ddy = dy - cy;
	osg::Vec3d PPX = localXN*ddx;
	osg::Vec3d PPY = localYN*ddy;
	osg::Vec3d lastPT = basePT - PPX + PPY;
	return lastPT;
} 
 


void GetAllChildNode(std::vector<osg::Node*>& nodes, osg::Node* pParent)
{
	if (pParent)
	{
		if (pParent->getNodeMask() != 0)
		{
			nodes.push_back(pParent);
			osg::Group* pGroup = pParent->asGroup();
			if (pGroup)
			{
				if (!pGroup->asGeode())
				{
					for (int i = 0; i < pGroup->getNumChildren(); i++)
					{
						GetAllChildNode(nodes, pGroup->getChild(i));
					}
				}
				
			}
		}

	}
}


//AUTOCAD的颜色索引和RGB值对照表
static const COLORREF ColorMap[] =
{
	RGB(0, 0, 0), RGB(255, 0, 0), RGB(255, 255, 0), RGB(0, 255, 0), RGB(0, 255, 255),
	RGB(0, 0, 255), RGB(255, 0, 255), RGB(255, 255, 255), RGB(128, 128, 128), RGB(192, 192, 192),
	RGB(255, 0, 0), RGB(255, 127, 127), RGB(165, 0, 0), RGB(165, 82, 82), RGB(127, 0, 0),
	RGB(127, 63, 63), RGB(76, 0, 0), RGB(76, 38, 38), RGB(38, 0, 0), RGB(38, 19, 19),
	RGB(255, 63, 0), RGB(255, 159, 127), RGB(165, 41, 0), RGB(165, 103, 82), RGB(127, 31, 0),
	RGB(127, 79, 63), RGB(76, 19, 0), RGB(76, 47, 38), RGB(38, 9, 0), RGB(38, 23, 19),
	RGB(255, 127, 0), RGB(255, 191, 127), RGB(165, 82, 0), RGB(165, 124, 82), RGB(127, 63, 0),
	RGB(127, 95, 63), RGB(76, 38, 0), RGB(76, 57, 38), RGB(38, 19, 0), RGB(38, 28, 19),
	RGB(255, 191, 0), RGB(255, 223, 127), RGB(165, 124, 0), RGB(165, 145, 82), RGB(127, 95, 0),
	RGB(127, 111, 63), RGB(76, 57, 0), RGB(76, 66, 38), RGB(38, 28, 0), RGB(38, 33, 19),
	RGB(255, 255, 0), RGB(255, 255, 127), RGB(165, 165, 0), RGB(165, 165, 82), RGB(127, 127, 0),
	RGB(127, 127, 63), RGB(76, 76, 0), RGB(76, 76, 38), RGB(38, 38, 0), RGB(38, 38, 19),
	RGB(191, 255, 0), RGB(223, 255, 127), RGB(124, 165, 0), RGB(145, 165, 82), RGB(95, 127, 0),
	RGB(111, 127, 63), RGB(57, 76, 0), RGB(66, 76, 38), RGB(28, 38, 0), RGB(33, 38, 19),
	RGB(127, 255, 0), RGB(191, 255, 127), RGB(82, 165, 0), RGB(124, 165, 82), RGB(63, 127, 0),
	RGB(95, 127, 63), RGB(38, 76, 0), RGB(57, 76, 38), RGB(19, 38, 0), RGB(28, 38, 19),
	RGB(63, 255, 0), RGB(159, 255, 127),
	RGB(41, 165, 0), RGB(103, 165, 82), RGB(31, 127, 0), RGB(79, 127, 63), RGB(19, 76, 0),
	RGB(47, 76, 38), RGB(9, 38, 0), RGB(23, 38, 19), RGB(0, 255, 0), RGB(127, 255, 127),
	RGB(0, 165, 0), RGB(82, 165, 82), RGB(0, 127, 0), RGB(63, 127, 63), RGB(0, 76, 0),
	RGB(38, 76, 38), RGB(0, 38, 0), RGB(19, 38, 19), RGB(0, 255, 63), RGB(127, 255, 159),
	RGB(0, 165, 41), RGB(82, 165, 103), RGB(0, 127, 31), RGB(63, 127, 79), RGB(0, 76, 19),
	RGB(38, 76, 47), RGB(0, 38, 9), RGB(19, 38, 23), RGB(0, 255, 127), RGB(127, 255, 191),
	RGB(0, 165, 82), RGB(82, 165, 124), RGB(0, 127, 63), RGB(63, 127, 95), RGB(0, 76, 38),
	RGB(38, 76, 57), RGB(0, 38, 19), RGB(19, 38, 28), RGB(0, 255, 191), RGB(127, 255, 223),
	RGB(0, 165, 124), RGB(82, 165, 145), RGB(0, 127, 95), RGB(63, 127, 111), RGB(0, 76, 57),
	RGB(38, 76, 66), RGB(0, 38, 28), RGB(19, 38, 33), RGB(0, 255, 255), RGB(127, 255, 255),
	RGB(0, 165, 165), RGB(82, 165, 165), RGB(0, 127, 127), RGB(63, 127, 127), RGB(0, 76, 76),
	RGB(38, 76, 76), RGB(0, 38, 38), RGB(19, 38, 38), RGB(0, 191, 255), RGB(127, 223, 255),
	RGB(0, 124, 165), RGB(82, 145, 165), RGB(0, 95, 127), RGB(63, 111, 127), RGB(0, 57, 76),
	RGB(38, 66, 76), RGB(0, 28, 38), RGB(19, 33, 38), RGB(0, 127, 255), RGB(127, 191, 255),
	RGB(0, 82, 165), RGB(82, 124, 165), RGB(0, 63, 127), RGB(63, 95, 127), RGB(0, 38, 76),
	RGB(38, 57, 76), RGB(0, 19, 38), RGB(19, 28, 38), RGB(0, 63, 255), RGB(127, 159, 255),
	RGB(0, 41, 165), RGB(82, 103, 165), RGB(0, 31, 127), RGB(63, 79, 127), RGB(0, 19, 76),
	RGB(38, 47, 76), RGB(0, 9, 38), RGB(19, 23, 38), RGB(0, 0, 255), RGB(127, 127, 255),
	RGB(0, 0, 165), RGB(82, 82, 165), RGB(0, 0, 127), RGB(63, 63, 127), RGB(0, 0, 76),
	RGB(38, 38, 76), RGB(0, 0, 38), RGB(19, 19, 38), RGB(63, 0, 255), RGB(159, 127, 255),
	RGB(41, 0, 165), RGB(103, 82, 165), RGB(31, 0, 127), RGB(79, 63, 127), RGB(19, 0, 76),
	RGB(47, 38, 76), RGB(9, 0, 38), RGB(23, 19, 38), RGB(127, 0, 255), RGB(191, 127, 255),
	RGB(82, 0, 165), RGB(124, 82, 165), RGB(63, 0, 127), RGB(95, 63, 127), RGB(38, 0, 76),
	RGB(57, 38, 76), RGB(19, 0, 38), RGB(28, 19, 38), RGB(191, 0, 255), RGB(223, 127, 255),
	RGB(124, 0, 165), RGB(145, 82, 165), RGB(95, 0, 127), RGB(111, 63, 127), RGB(57, 0, 76),
	RGB(66, 38, 76), RGB(28, 0, 38), RGB(33, 19, 38), RGB(255, 0, 255), RGB(255, 127, 255),
	RGB(165, 0, 165), RGB(165, 82, 165), RGB(127, 0, 127), RGB(127, 63, 127), RGB(76, 0, 76),
	RGB(76, 38, 76), RGB(38, 0, 38), RGB(38, 19, 38), RGB(255, 0, 191), RGB(255, 127, 223),
	RGB(165, 0, 124), RGB(165, 82, 145), RGB(127, 0, 95), RGB(127, 63, 111), RGB(76, 0, 57),
	RGB(76, 38, 66), RGB(38, 0, 28), RGB(38, 19, 33), RGB(255, 0, 127), RGB(255, 127, 191),
	RGB(165, 0, 82), RGB(165, 82, 124), RGB(127, 0, 63), RGB(127, 63, 95), RGB(76, 0, 38),
	RGB(76, 38, 57), RGB(38, 0, 19), RGB(38, 19, 28), RGB(255, 0, 63), RGB(255, 127, 159),
	RGB(165, 0, 41), RGB(165, 82, 103), RGB(127, 0, 31), RGB(127, 63, 79), RGB(76, 0, 19),
	RGB(76, 38, 47), RGB(38, 0, 9), RGB(38, 19, 23), RGB(84, 84, 84), RGB(118, 118, 118),
	RGB(152, 152, 152), RGB(186, 186, 186), RGB(220, 220, 220), RGB(255, 255, 255)
};

//通过AutoCAD Color Index值来获取对应的RGB值
COLORREF GetRGBByACI(unsigned int ACI)
{
	if (ACI > sizeof(ColorMap))
	{
		ASSERT(0);
		ACI = 255;
	}
	return ColorMap[ACI];
}
 
osg::Vec4 GetColorByIndex(int colorIndex)
{
	if (colorIndex < 0 || colorIndex > 256)
		return osg::Vec4(1, 1, 1,1);
	else
	{
		COLORREF colorValue = GetRGBByACI(colorIndex);
		return osg::Vec4(1.0*GetRValue(colorValue) / 255.0, 1.0*GetGValue(colorValue) / 255.0, 1.0*GetBValue(colorValue) / 255.0, 1.0);
	}

}
int GetColorIndexByColorValue(osg::Vec4 color)
{
	int R = 255 * color.x();
	int G = 255 * color.y();
	int B = 255 * color.z();
	COLORREF colorValue = RGB(R, G, B);
	for (int i = 0; i < 255; i++)
	{
		if (ColorMap[i] == colorValue)
		{
			return i;
		}
	}
	return 0;

} 

osg::Vec3 GetVertexDir(std::vector<osg::Vec3>& allVertexs,int index,BOOL bClose,BOOL bClockWise)
{
	int nCount = allVertexs.size();


	if (allVertexs.size() < 2)
	{
		return osg::Vec3(0, 1, 0);
	}
	std::vector<osg::Vec3> vertexs;
	if (index == 0  )
	{
		if (bClose)
		{
			vertexs.push_back(allVertexs[nCount-1]);
		}		 
		vertexs.push_back(allVertexs[index]);
		vertexs.push_back(allVertexs[index + 1]);

	}
	else if (index == nCount-1 )
	{
		vertexs.push_back(allVertexs[index - 1]);
		vertexs.push_back(allVertexs[index]);
		if (bClose)
			vertexs.push_back(allVertexs[0]);
	}
	else
	{
		vertexs.push_back(allVertexs[index -1]);
		vertexs.push_back(allVertexs[index]);
		vertexs.push_back(allVertexs[index + 1]);
	}

	osg::Vec3 dir1;
	if (vertexs.size() == 3)
	{
		osg::Vec3 dir11 = vertexs[1] - vertexs[0];
		osg::Vec3 dir12 = vertexs[2] - vertexs[1];
		dir11.normalize();
		dir12.normalize();


		dir1 = dir11 + dir12;
		dir1.normalize();

	 
	}
	else if (vertexs.size() == 2)
	{
		dir1 = vertexs[1] - vertexs[0];
		dir1.normalize();
	}

	osg::Quat rot(M_PI / 2, osg::Z_AXIS);
	if (!bClockWise)
	{
		rot.makeRotate(-M_PI / 2, osg::Z_AXIS);
	}
	osg::Matrix matrix;
	matrix.makeRotate(rot);
	osg::Vec3 newDir = dir1*matrix;
	newDir.normalize();
	return newDir;
 
}


//factor 比例 ，计算构件的截面三维坐标
void CreateElementGeometry(BOOL bVertSection, osg::Vec3 axis, int factor, osg::Vec3 lpt, osg::Vec3 rpt, std::vector<std::vector<osg::Vec3>>& leftSecs2, std::vector<std::vector<osg::Vec3>>& rightSecs2, double lcx, double rcx, double lcy, double rcy)
{
	std::vector<std::vector<osg::Vec3> > leftSecs;
	std::vector<std::vector<osg::Vec3> > rightSecs;

	axis.normalize();

	///////////////////////////////////////
	osg::Vec3d worldN = rpt - lpt;
	worldN.normalize();

	osg::Vec3d localXN;  //截面的局部坐标系的X轴平行全局坐标系的X0Y平面；
	osg::Vec3d localYN;
	
	localXN = axis;	
	if (bVertSection)
	{
		localYN = osg::Vec3(0, 0, 1);
	}
	else
	{
		localYN = worldN^localXN;
		localYN.normalize();
	}
 	 
	////////////////////////////left section
	{
		osg::Vec3 baseLPT(lpt);
		int nEdgeCount = leftSecs2.size();
		for (int j = 0; j < nEdgeCount; j++)
		{
			std::vector<osg::Vec3> oneEdge = leftSecs2[j];
 
			std::vector<osg::Vec3>pts;

			int pointCount = oneEdge.size();
			for (int k = 0; k < pointCount; k++)
			{
				osg::Vec3 coor = oneEdge[k];

				double ddx = coor.x() - lcx;
				double ddy = coor.y() - lcy;

				osg::Vec3 PPX = -localXN*ddx / factor;
				osg::Vec3 PPY = localYN*ddy / factor;

				osg::Vec3 lastPT = baseLPT + (PPX + PPY);

				pts.push_back(lastPT);
			}
			leftSecs.push_back(pts);
 		}
	}
	//right section
	{
		osg::Vec3 baseRPT(rpt);

		int nEdgeCount = rightSecs2.size();
		for (int j = 0; j < nEdgeCount; j++)
		{
			std::vector<osg::Vec3> oneEdge = rightSecs2[j];
 
			std::vector<osg::Vec3>pts;

			int pointCount = oneEdge.size();
			for (int k = 0; k < pointCount; k++)
			{
				osg::Vec3 coor = oneEdge[k];

				double ddx = coor.x() - rcx;
				double ddy = coor.y() - rcy;

				osg::Vec3 PPX = -localXN*ddx / factor;
				osg::Vec3 PPY = localYN*ddy / factor;

				osg::Vec3 lastPT = baseRPT + PPX + PPY;
				pts.push_back(lastPT);
			}
			rightSecs.push_back(pts);
 		}
	}

	leftSecs2  = leftSecs;
	rightSecs2 = rightSecs;
}

bool VecIsNotEqual(osg::Vec3 v1, osg::Vec3 v2)
{
	if (cdb_compare_tools::is_not_equal(v1.x(), v2.x(), 1.0e-3))
		return true;
	if (cdb_compare_tools::is_not_equal(v1.y(), v2.y(), 1.0e-3))
		return true;
	if (cdb_compare_tools::is_not_equal(v1.z(), v2.z(), 1.0e-3))
		return true;
	return false;
}

bool VecIsEqual(osg::Vec3 v1, osg::Vec3 v2)
{
	if (cdb_compare_tools::is_not_equal(v1.x(), v2.x(), 1.0e-3))
		return false;
	if (cdb_compare_tools::is_not_equal(v1.y(), v2.y(), 1.0e-3))
		return false;
	if (cdb_compare_tools::is_not_equal(v1.z(), v2.z(), 1.0e-3))
		return false;
	return true;
}

DB_3DMODELING_API void GetSectionMainAxis(double angle,BOOL bVertSection, osg::Vec3 leftAxis, osg::Vec3 rightAxis, osg::Vec3 leftTangent, osg::Vec3 rightTangent, int factor, osg::Vec3 lpt, osg::Vec3 rpt,
	BOOL bL, BOOL bR, osg::Vec3d& localLXN, osg::Vec3d& localLYN, osg::Vec3d& localRXN, osg::Vec3d& localRYN )
{
	osg::Vec3 world = rpt - lpt;
	world.normalize();
	if (bVertSection == 2)
	{
		double alfa = osg::Vec3(0, 0, 1)*world;
		if (alfa <= 0)
		{
			leftTangent = osg::Vec3(0, 0, -1);
			rightTangent = osg::Vec3(0, 0, -1);
		}
		else
		{
			leftTangent = osg::Vec3(0, 0, 1);
			rightTangent = osg::Vec3(0, 0, 1);
		}
	}
	leftTangent.normalize();
	rightTangent.normalize();

	localLXN = leftAxis;
	localLXN.normalize();

	if (bVertSection==1)
	{
		localLYN = osg::Vec3(0, 0, 1);

		osg::Vec3 temp = osg::Vec3d(0, 0, 1) ^ world;
		osg::Vec3 axisV = temp^osg::Vec3d(0, 0, 1);
	
		osg::Quat rotation; rotation.makeRotate(angle*M_PI / 180, axisV);
		osg::Matrixd mat1;
		mat1.makeRotate(rotation);
		localLYN = localLYN*mat1;
		localLXN = localLYN^axisV;

		localLXN.normalize();
		localLYN.normalize();
	}
	else
	{
		localLYN = leftTangent^localLXN;
		localLYN.normalize();
	}
	double cosa = 1;

	BOOL bY = FALSE;
	if (bL && ( bVertSection == 0 || bVertSection ==3 ) )
	{
		cosa = leftTangent *world;
		if (cdb_compare_tools::is_equal(cosa, 0))
			cosa = 1;

		osg::Vec3 verAxis = leftTangent^world;
		verAxis.normalize();
		if ( VecIsNotEqual(verAxis, osg::Vec3(0, 0, 1) ) && VecIsNotEqual( verAxis, osg::Vec3(0, 0, -1) ) )
		{
			bY = TRUE;
		}
 	}

	if (bY)
		localLYN /= cosa;
	else
		localLXN /= cosa;

	localRXN = rightAxis;
	localRXN.normalize();
	if (bVertSection==1)
	{ 
		localRYN = osg::Vec3(0, 0, 1);
		osg::Vec3 temp = osg::Vec3d(0, 0, 1) ^ world;
		osg::Vec3 axisV = temp^osg::Vec3d(0, 0, 1);

		osg::Quat rotation; rotation.makeRotate(angle*M_PI / 180, axisV);
		osg::Matrixd mat1;
		mat1.makeRotate(rotation);
		localRYN = localRYN*mat1;
		localRXN = localRYN^axisV;

		localRXN.normalize();
		localRYN.normalize();
	}
	else
	{
		localRYN = rightTangent^localRXN;
		localRYN.normalize();
	}

	cosa = 1;
	bY = FALSE;

	if (bR && (bVertSection == 0 || bVertSection == 3) )
	{
		cosa = rightTangent *world;
		if (cdb_compare_tools::is_equal(cosa, 0))
		{
			cosa = 1;
		}	
		osg::Vec3 verAxis = rightTangent^world;
		verAxis.normalize();
		if (VecIsNotEqual(verAxis, osg::Vec3(0, 0, 1)) && VecIsNotEqual(verAxis, osg::Vec3(0, 0, -1)))
		{
			bY = TRUE;
		}
	}
	if (bY)
		localRYN /= cosa;
	else
		localRXN /= cosa;
}

//factor 比例 ，计算构件的截面三维坐标
void CreateElementGeometryNew(double angle, BOOL bVertSection, osg::Vec3 leftAxis, osg::Vec3 rightAxis, osg::Vec3 leftTangentLine, osg::Vec3 rightTangentLine, int factor, osg::Vec3 lpt, osg::Vec3 rpt,
	std::vector<std::vector<osg::Vec3>>& leftSecs2, std::vector<std::vector<osg::Vec3>>& rightSecs2, double lcx, double rcx, double lcy, double rcy)
{
	CreateElementGeometryNewByBrokenLine(angle, bVertSection, factor, leftAxis, rightAxis, leftTangentLine, rightTangentLine, lpt, rpt, FALSE, FALSE, leftSecs2, rightSecs2, lcx, rcx, lcy, rcy);
}
void CreateElementGeometryNewByBrokenLine(double angle, BOOL bVertSection, int factor, osg::Vec3 leftAxis, osg::Vec3 rightAxis, osg::Vec3 leftTangentLine, osg::Vec3 rightTangentLine, osg::Vec3 lpt, osg::Vec3 rpt,
	BOOL bL, BOOL bR, std::vector<std::vector<osg::Vec3>>& leftSecs2, std::vector<std::vector<osg::Vec3>>& rightSecs2, double lcx, double rcx, double lcy, double rcy)
{
	std::vector<std::vector<osg::Vec3> > leftSecs;
	std::vector<std::vector<osg::Vec3> > rightSecs;
 
	osg::Vec3d localLXN;
	osg::Vec3d localLYN;
	osg::Vec3d localRXN;
	osg::Vec3d localRYN;
	GetSectionMainAxis(angle, bVertSection, leftAxis, rightAxis, leftTangentLine, rightTangentLine, factor, lpt, rpt, bL, bR, localLXN, localLYN, localRXN, localRYN);
	////////////////////////////left section
	{
		osg::Vec3 baseLPT(lpt);
		int nEdgeCount = leftSecs2.size();
		for (int j = 0; j < nEdgeCount; j++)
		{
			std::vector<osg::Vec3> oneEdge = leftSecs2[j];

			std::vector<osg::Vec3>pts;

			int pointCount = oneEdge.size();
			for (int k = 0; k < pointCount; k++)
			{
				osg::Vec3 coor = oneEdge[k];

				double ddx = coor.x() - lcx;
				double ddy = coor.y() - lcy;

				osg::Vec3 PPX = -localLXN*ddx / factor;
				osg::Vec3 PPY = localLYN*ddy / factor;

				osg::Vec3 lastPT = baseLPT + (PPX + PPY);

				pts.push_back(lastPT);
			}
			leftSecs.push_back(pts);
		}
	}

	/////////Right Section
	{
		osg::Vec3 baseRPT(rpt);
		int nEdgeCount = rightSecs2.size();
		for (int j = 0; j < nEdgeCount; j++)
		{
			std::vector<osg::Vec3> oneEdge = rightSecs2[j];

			std::vector<osg::Vec3>pts;

			int pointCount = oneEdge.size();
			for (int k = 0; k < pointCount; k++)
			{
				osg::Vec3 coor = oneEdge[k];

				double ddx = coor.x() - rcx;
				double ddy = coor.y() - rcy;

				osg::Vec3 PPX = -localRXN*ddx / factor;
				osg::Vec3 PPY = localRYN*ddy / factor;

				osg::Vec3 lastPT = baseRPT + PPX + PPY;
				pts.push_back(lastPT);
			}
			rightSecs.push_back(pts);
		}
	}

	leftSecs2   = leftSecs;
	rightSecs2 = rightSecs;
}


void CreateConstraint3DModel(const osg::Vec3& position, osg::Geode*  pGeode, double supSize, osg::Vec4 zzcolor)
{
	pGeode->removeDrawables(0, pGeode->getNumDrawables()); 

	{
		osg::ref_ptr<osg::Geometry> pointsGeom = new osg::Geometry();

		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(zzcolor);
		pointsGeom->setColorArray(colors.get() );
		pointsGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		pointsGeom->setVertexArray(vertices.get() );

 		float fL1 = supSize;
		float fL2 = supSize;	 
		{
			vertices->push_back(position);
			vertices->push_back(osg::Vec3(position.x() - fL1 / 2, position.y(), position.z() - fL2));
			vertices->push_back(osg::Vec3(position.x() + fL1 / 2, position.y(), position.z() - fL2));
		}

		pointsGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 3));
		pGeode->addDrawable(pointsGeom.get() );
	}

	osg::StateSet* stateset = pGeode->getOrCreateStateSet();

	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
}


osg::Geometry*  CreateDimArror(osg::Vec4 color, osg::Vec3 pt1, osg::Vec3 lineDir, osg::Vec3 nodeDir, double lineL, double nodeL)
{
	osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry();

	osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
	pGeometry->setVertexArray(pyramidVertices);
	pyramidVertices->push_back(pt1);
	pyramidVertices->push_back(pt1 + nodeDir*nodeL*.5 + lineDir*lineL);
	pyramidVertices->push_back(pt1 - nodeDir*nodeL*.5 + lineDir*lineL);	 
	{
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(color);
		pGeometry->setColorArray(colors);
		pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

		osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON);
		pyramidBase->push_back(0);
		pyramidBase->push_back(1);
		pyramidBase->push_back(2);

		pGeometry->addPrimitiveSet(pyramidBase.get());

		osgUtil::SmoothingVisitor::smooth(*pGeometry);
		pGeometry->setUseVertexBufferObjects(false);
	}
	
	return pGeometry.release();
}


osg::Geometry* CreateRectByTexture(const CString& strTexture, osg::Vec3 top_left, osg::Vec3 bottom_left, osg::Vec3 bottom_right, osg::Vec3 top_right)
{
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(4);
	(*vertices)[0] = top_left;
	(*vertices)[1] = bottom_left;
	(*vertices)[2] = bottom_right;
	(*vertices)[3] = top_right;
	geom->setVertexArray(vertices.get() );

	osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array(4);
	(*texcoords)[0].set(0.0f, 1.0f);
	(*texcoords)[1].set(0.0f, 0.0f);
	(*texcoords)[2].set(1.0f, 0.0f);
	(*texcoords)[3].set(1.0f, 1.0f);
	geom->setTexCoordArray(0, texcoords.get() );

	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(1);
	(*normals)[0].set(0.0f, -1.0f, 0.0f);
	geom->setNormalArray(normals.get(), osg::Array::BIND_OVERALL);

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(1);
	(*colors)[0].set(1.0f, 1.0f, 1.0f, 1.0f);
	geom->setColorArray(colors.get() , osg::Array::BIND_OVERALL);

	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

	// set up the texture state.
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setDataVariance(osg::Object::DYNAMIC); // protect from being optimized away as static state.
	texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
	texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
	osg::ref_ptr<osg::Image> pImage = osgDB::readImageFile( cdb_string_tools::CString_to_string(strTexture));
	texture->setImage(pImage);
	osg::StateSet* stateset = geom->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture.get() , osg::StateAttribute::ON);

	return geom.release();
}



class GJTextColorCallback : public osg::Uniform::Callback
{
public:
	osgText::Text* pObj;
public:
	GJTextColorCallback(osgText::Text* pNode)
	{
		pObj = pNode;
	}
	virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv)
	{
		uniform->set(pObj->getColor() );
	}
};
/*
{
	"    gl_FragColor   = texture2D(sampler0, gl_TexCoord[0].xy);\n"
		"    gl_FragColor.x	 = userColor.x;\n"
		"    gl_FragColor.y	 = userColor.y;\n"
		"    gl_FragColor.z	 = userColor.z;\n"
		"    if (gl_FragColor.w > 0.5)		   \n"
		"    {									           \n"
		"		     gl_FragColor.w = 1;         \n"
		"   }												\n"
		"  	else										    \n"
		"  	{												 \n"
		"			gl_FragColor.w =2* gl_FragColor.w;   \n"
		" 	}										\n"
}
*/
 void CreateTextEffect(osg::Node* pNode, osg::Vec4 color,osgText::Text* pObj,CViewControlData* pOSG)
{
	if (!pOSG->ShaderIsSupport())
	{
		return;
	}

	osg::ref_ptr<osg::Program> program = new osg::Program;
	program->setName("TextEffect");

	osg::ref_ptr<osg::Shader> mainF = new osg::Shader(osg::Shader::FRAGMENT);
	mainF->setShaderSource
		(
		"uniform sampler2D sampler0;\n"
		"uniform vec4 userColor;\n"
		"void main(void)\n"
		"{\n"
		"    gl_FragColor   = texture2D(sampler0, gl_TexCoord[0].xy);\n"
		"    gl_FragColor.x	 = userColor.x;\n"
		"    gl_FragColor.y	 = userColor.y;\n"
		"    gl_FragColor.z	 = userColor.z;\n"
		"    if (gl_FragColor.w > 0.5)		   \n"
		"    {									           \n"
		"		     gl_FragColor.w = 1;         \n"
		"   }												\n"
		"  	else										    \n"
		"  	{												 \n"
		"			gl_FragColor.w =2* gl_FragColor.w;   \n"
		" 	}										\n"
		"}											\n"
		);

	program->addShader(mainF.get() );

	osg::ref_ptr<osg::Uniform> textTexture = new osg::Uniform("sampler0", 0);
	pNode->getOrCreateStateSet()->addUniform(textTexture);
	textTexture->setDataVariance(osg::Object::DYNAMIC);

	osg::ref_ptr<osg::Uniform> uniColor = new osg::Uniform("userColor", color);

	if (pObj)
	{
		uniColor->setUpdateCallback(new GJTextColorCallback(pObj));
	}

	pNode->getOrCreateStateSet()->addUniform(uniColor.get() );
	uniColor->setDataVariance(osg::Object::DYNAMIC);

	pNode->getOrCreateStateSet()->setAttributeAndModes(program.get() ); 
}


void CreateStatusEffect(CViewControlData* pOSG, osg::Node* pNode, bool bselect)
{
	if (!pOSG->ShaderIsSupport())
	{
		return;
	}

	osg::ref_ptr<osg::Program> program = new osg::Program;
	osg::ref_ptr<osg::Shader> mainF = new osg::Shader(osg::Shader::FRAGMENT);
	mainF->setShaderSource
		(
		"uniform sampler2D sampler0;													  \n"
		"uniform bool bselect;															  \n"
		"void main(void)																  \n"
		"{																				  \n"
		"	if (bselect)																  \n"
		"	{																			  \n"
		"		vec4 texture_color = texture2D(sampler0, gl_TexCoord[0].xy);			  \n"
		"		if (texture_color.x + texture_color.y + texture_color.z < 0.1)			  \n"
		"			discard;															  \n"
		"		texture_color.x = 1;													  \n"
		"		gl_FragColor = texture_color;											  \n"
		"	}																			  \n"
		"	else																		  \n"
		"	{																			  \n"
		"		vec4 texture_color = texture2D(sampler0, gl_TexCoord[0].xy);			  \n"
		"		if (texture_color.x + texture_color.y + texture_color.z < 0.1)			  \n"
		"			discard;															  \n"
		"		gl_FragColor = texture_color;											  \n"
		"	}                                                                             \n"
		"}                                                                                \n"
		);

	 
	program->addShader(mainF.get() );

	osg::ref_ptr<osg::Uniform> textTexture = new osg::Uniform("sampler0", 0);
	pNode->getOrCreateStateSet()->addUniform(textTexture);

	osg::ref_ptr<osg::Uniform> selectStatusUnit = new osg::Uniform("bselect", bselect);
	pNode->getOrCreateStateSet()->addUniform(selectStatusUnit.get() );

	pNode->getOrCreateStateSet()->setAttributeAndModes(program.get() );
}

void SetNodeVisible(osg::Node* pDimObj, BOOL bVisible)
{
	if (bVisible)
	{
		pDimObj->setNodeMask(0xffffffff);
	}
	else
	{
		pDimObj->setNodeMask(0x0);
	}

}


///////////////
void RestoreView(osgViewer::View* mView, osg::Matrixd ProjectionMatrix, osg::Vec3 eye, osg::Vec3 center, osg::Vec3 up)
{
	mView->getCameraManipulator()->setHomePosition(eye, center, up);
	mView->home();

	double left;
	double right;
	double bottom;
	double top;
	double zNear;
	double zFar;

	ProjectionMatrix.getOrtho(left, right, bottom, top, zNear, zFar);
	double dWidth = right - left;
	double cx = 0.5*(left + right);
	double dHeight = top - bottom;
	double cy = 0.5*(top + bottom);

	osg::Viewport* pViewPort = mView->getCamera()->getViewport();
	double newWidth = pViewPort->width();
	double newHeight = pViewPort->height();

	if (abs(newWidth / newHeight - dWidth / dHeight) > 1.0e-3)
	{
		if (fabs(newHeight*dWidth) <= fabs(newWidth*dHeight))
		{
			dWidth = dWidth;
			dHeight = dWidth*newHeight / newWidth;
		}
		else
		{
			dHeight = dHeight;
			dWidth = dHeight*(newWidth / newHeight);
		}
		left = cx - 0.5*dWidth;
		right = cx + 0.5*dWidth;
		top = cy + 0.5*dHeight;
		bottom = cy - 0.5*dHeight;

		ProjectionMatrix.makeOrtho(left, right, bottom, top, zNear, zFar);
	}
	mView->getCamera()->setProjectionMatrix(ProjectionMatrix);
}
