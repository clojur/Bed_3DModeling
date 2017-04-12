#include "stdafx.h"

 
#include "Db3DModelingUnitity.h"
#include <osgUtil/SmoothingVisitor>
#include <osgDB/ReadFile>
#include <osg/Point>
#include <osg/LineWidth>
#include <osg/AutoTransform>
#include <osg/ValueObject>
#include "osgDB/ReadFile"
#include "osg/TexMat"
#include "osg/TexGen"
#include "osg/TextureCubeMap"
#include "osg/TexEnv"
#include "osg/Depth"
#include "osg/Drawable"
#include "osg/Shape"
#include "osg/ShapeDrawable"
#include "osg/CullFace"
#include "osg/AlphaFunc"
#include "string_tools.h"

//指定应用程序自身的路径(不含文件名及后缀,如E:\BackUp\BridgeWizard\bin)
CString OSGGetApplicationSelfPath()
{
	TCHAR exeFullPath[MAX_PATH]; 
	GetModuleFileName(NULL,exeFullPath,MAX_PATH); 
	CString strPath(exeFullPath);
	const int position=strPath.ReverseFind('\\'); 
	strPath=strPath.Left(position+1); 
	return strPath;
}
void AutoNormal(osg::Geometry* pGeomery ,double angle)
{
	if(pGeomery)
	{
		osgUtil::SmoothingVisitor::smooth(*pGeomery,angle);
	}
}
 
osg::Matrix computeWindowMatrix(osg::Viewport* pViewport) 
{
	if(pViewport)
	{
		double x = pViewport->x();
		double y = pViewport->y();
		double width = pViewport->width();
		double height = pViewport->height();
		return osg::Matrix::translate(1.0,1.0,1.0)*osg::Matrix::scale(0.5*width,0.5*height,0.5f)*osg::Matrix::translate(0,0,0.0f);
	}
	else
	{
		return osg::Matrix();
	}
}

BOOL CalBoundBoxWH(double& bW, double& bH ,double& bL, double& cX,double& cY, double& cZ,osgViewer::View*  pViw , osg::BoundingBox& bb)
{
	if(pViw)
	{
		if(bb.valid() )
		{		
			osg::Matrixd& masterCameraView     =  pViw->getCamera()->getViewMatrix() ;
			osg::Matrixd& masterCameraProject =  pViw->getCamera()->getProjectionMatrix();

			osg::Matrixd masterCameraVPW =  masterCameraView* masterCameraProject;
			if(pViw->getCamera()->getViewport())
			{
				osg::Viewport* viewport = pViw->getCamera()->getViewport();
				osg::Matrixd viewportMatrix =  computeWindowMatrix(viewport);
				masterCameraVPW *=viewportMatrix;
			}

			std::vector<osg::Vec3> allCorner;
			allCorner.push_back( bb.corner(0) );
			allCorner.push_back( bb.corner(1) );
			allCorner.push_back( bb.corner(2) );
			allCorner.push_back( bb.corner(3) );
			allCorner.push_back( bb.corner(4) );
			allCorner.push_back( bb.corner(5) );
			allCorner.push_back( bb.corner(6) );
			allCorner.push_back( bb.corner(7) );

			double dmaxX  = 0;
			double dminX  = 0;
			double dmaxY  = 0;
			double dminY  = 0;
			double dmaxZ  = 0;
			double dminZ  = 0;

			for(int i=0; i< 8; i++)
			{
				osg::Vec3 winPT = allCorner[i]*masterCameraVPW;
				if( i ==0)
				{
					dminX  = winPT.x(); 
					dmaxX  = winPT.x();
					dminY  = winPT.y();
					dmaxY  = winPT.y();
					dminZ  = winPT.z();
					dmaxZ  = winPT.z();
				}
				else
				{
					if(dmaxX  < winPT.x() )
					{
						dmaxX = winPT.x();
					}
					if(dmaxY  < winPT.y() )
					{
						dmaxY = winPT.y();
					}
					if(dmaxZ  < winPT.z() )
					{
						dmaxZ = winPT.z();
					}
					if(dminX > winPT.x() )
					{
						dminX = winPT.x();
					}
					if(dminY > winPT.y() )
					{
						dminY = winPT.y();
					}
					if(dminZ > winPT.z() )
					{
						dminZ = winPT.z();
					}

				}
			}
			///////////////////////////////////////////////////////////
			bW  = 0.5*abs( dmaxX- dminX );
			bH   = 0.5*abs( dmaxY- dminY );		
			bL    = 0.5*abs( dmaxZ - dminZ);
			cX   =  0.5*( dmaxX+ dminX );
			cY   =  0.5*( dmaxY+ dminY );
			cZ   =  0.5*( dmaxZ+ dminZ);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CalBoundBoxWH2(double& bW, double& bH ,double& bL, double& cX,double& cY, double& cZ,osgViewer::View*  pViw , osg::BoundingBox& bb)
{
	if(pViw)
	{
		if(bb.valid() )
		{		
			osg::Matrixd& masterCameraView     =  pViw->getCamera()->getViewMatrix() ;
			osg::Matrixd& masterCameraProject =  pViw->getCamera()->getProjectionMatrix();

			osg::Matrixd masterCameraVPW =  masterCameraView* masterCameraProject;
			if(pViw->getCamera()->getViewport())
			{
				osg::Viewport* viewport = pViw->getCamera()->getViewport();
				osg::Matrixd viewportMatrix =  computeWindowMatrix(viewport);
				masterCameraVPW *=viewportMatrix;
			}

			std::vector<osg::Vec3> allCorner;
			allCorner.push_back( bb.corner(0) );
			allCorner.push_back( bb.corner(1) );
			allCorner.push_back( bb.corner(2) );
			allCorner.push_back( bb.corner(3) );
			allCorner.push_back( bb.corner(4) );
			allCorner.push_back( bb.corner(5) );
			allCorner.push_back( bb.corner(6) );
			allCorner.push_back( bb.corner(7) );

			double dmaxX = 0;
			double dminX  = 0;
			double dmaxY =0;
			double dminY  =0;
			double dmaxZ =0;
			double dminZ  =0;
			for(int i=0; i< 8; i++)
			{
				osg::Vec3 winPT = allCorner[i];
				if( i ==0)
				{
					dminX  = winPT.x(); 
					dminY  = winPT.y();
					dminZ =  winPT.z();
					dmaxX = winPT.x();
					dmaxY = winPT.y();
					dmaxZ = winPT.z();
				}
				else
				{
					if(dmaxX < winPT.x() )
					{
						dmaxX = winPT.x();
					}
					if(dmaxY < winPT.y() )
					{
						dmaxY = winPT.y();
					}
					if(dmaxZ < winPT.z() )
					{
						dmaxZ = winPT.z();
					}
					if(dminX > winPT.x() )
					{
						dminX = winPT.x();
					}
					if(dminY > winPT.y() )
					{
						dminY = winPT.y();
					}
					if(dminZ > winPT.z() )
					{
						dminZ = winPT.z();
					}

				}
			}
			///////////////////////////////////////////////////////////
			bW  = 0.5*abs( dmaxX- dminX );
			bH   = 0.5*abs( dmaxY- dminY );		
			bL    = 0.5*abs( dmaxZ - dminZ);
			cX   =  0.5*( dmaxX+ dminX );
			cY   =  0.5*( dmaxY+ dminY );
			cZ   =  0.5*( dmaxZ+ dminZ);
			return TRUE;
		}
	}
	return FALSE;
}




CString GetObjType(osg::Node* pNode )
{
	if(pNode)
	{
		std::string strType(OT_UNKNOWN);
		pNode->getUserValue("type" , strType);
		return strType.c_str(); 
	}
	return OT_UNKNOWN;

}
#include "pointtextureData.h"

void MakeTexture(BOOL bCreate, osg::Texture2D* pTexture, const CString& strFile, int type)
{
	osg::Image* pImage = NULL;
	if (bCreate)
	{
		pImage = osgDB::readImageFile( cdb_string_tools::CString_to_string(strFile));	
	}
	else
	{
		pImage = new osg::Image();
		int w = 30;
		int h = 30;
		pImage->allocateImage(w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE);
		for (unsigned int i = 0; i < h; i++)
		{
			unsigned char* ptr = pImage->data(0, i);
			for (unsigned int j = 0; j < w; ++j)
			{
				unsigned int r = 0;
				unsigned int g = 0;
				unsigned int b = 0;
				unsigned int a = 0;
				if (type == 1)
				{
					r = pointTextureData[i + j*w][0];
					g = pointTextureData[i + j*w][1];
					b = pointTextureData[i + j*w][2];
					a = pointTextureData[i + j*w][3];
				}
				else if (type == 2)
				{
					r = tendonTextureData[i + j*w][0];
					g = tendonTextureData[i + j*w][1];
					b = tendonTextureData[i + j*w][2];
					a = tendonTextureData[i + j*w][3];
				}
				else if (type == 3)
				{
					r = steelTextureData[i + j*w][0];
					g = steelTextureData[i + j*w][1];
					b = steelTextureData[i + j*w][2];
					a = steelTextureData[i + j*w][3];
				}
				else if (type == 4)
				{
					r = checkPointTextureData[i + j*w][0];
					g = checkPointTextureData[i + j*w][1];
					b = checkPointTextureData[i + j*w][2];
					a = checkPointTextureData[i + j*w][3];
				}
				*ptr++ = r;
				*ptr++ = g;
				*ptr++ = b;
				*ptr++ = a;
			}
		}
	}
	pTexture->setImage(pImage);

}

//
//void MakeTexture(BOOL bCreate , osg::Texture2D* pTexture , const CString& strFile ,int type )
//{
//	{
//}



osg::Node	* GetChildByName(const CString& name , osg::Node* pNode)
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

				if(  pChild->getName().c_str() == name )
				{
						return pChild;
				}
			}	
		
		}

	}
	return NULL;
}

void GetChildDimByName(std::vector<osg::Node*>& nodeVec, const CString& name, osg::Group* pParent)
{
	if (pParent)
	{		
		int childCount = pParent->getNumChildren();
		for (int i = 0; i < childCount; i++)
		{
			osg::Node* pChild = pParent->getChild(i);
			if (pChild)
			{
				CString strParentName = pChild->getName().c_str();
				if (strParentName.Find(name) != -1)
					nodeVec.push_back(pChild);
			}
		}
	}
}


void  GetChildByName2(std::vector<osg::Node*>&nodeVec, const CString& name , osg::Group* pParent,const CString& nodeType)
{
	if(pParent)
	{	
		int childCount = pParent->getNumChildren();
		for(int i=0; i< childCount; i++)
		{
			osg::Node* pChild = pParent->getChild(i);
			if (pChild)
			{
				if (pChild->getName().c_str() == name)
				{
					if (GetObjType(pChild) == nodeType || nodeType == OT_ALL)
						nodeVec.push_back(pChild);
				}
			}
		
		}	

		if (!pParent->asGeode()) //add cg 2016-4-20 
		{
			childCount = pParent->getNumChildren();
			for(int i=0; i< childCount; i++)
			{
				osg::Node* pChild = pParent->getChild(i);
				if (pChild)
				{
					osg::Group* pGroup =  pChild->asGroup();
					if(pGroup )  //wire节点加入了两次；
					{
						GetChildByName2(nodeVec, name, pGroup,nodeType);			
					}
				}
		
			}	
		}	
	}
}



void  UpdateSnaper( int snapTYpe , const osg::Vec3& lPT ,const osg::Vec4&  color, osg::Group* pGraph , int size)
{
	if(pGraph)
	{
		osg::AutoTransform* pat = (osg::AutoTransform*)pGraph;
		pat->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
		pat->setAutoScaleToScreen(true);
		pat->setMinimumScale(0);
		pat->setMaximumScale(FLT_MAX);	

		pat->setPosition(lPT );

		pGraph->removeChildren(0, pGraph->getNumChildren() );
		{
			osg::Geode* pGeode = new osg::Geode;
			pat->addChild(pGeode);

			osg::Geometry*pGeometry = new osg::Geometry();

			osg::Vec3Array* vertices = new osg::Vec3Array;
			pGeometry->setVertexArray(vertices);		

			osg::Vec4Array* colors = new osg::Vec4Array;
			colors->push_back( color );		
			pGeometry->setColorArray(colors);
			pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

			double lineWidth = 2.0;
			if(snapTYpe == SNAP_NODE  )
			{		
				double size =7;

				osg::Vec3 lt1 =osg::Vec3(-size , -size,0);
				osg::Vec3 lt2 = osg::Vec3(size ,  -size,0);
				osg::Vec3 lt3 =  osg::Vec3(size ,   size,0);
				osg::Vec3 lt4 = osg::Vec3(-size , size,0);		

				vertices->push_back( lt1 ); 
				vertices->push_back( lt2 ); 
				vertices->push_back( lt3 ); 
				vertices->push_back( lt4 ); 			
				pGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, vertices->size()));
			}
			else if(snapTYpe ==SNAP_MID  )
			{
				osg::Vec3 lt1 =osg::Vec3(-7.5 , -8,0);
				osg::Vec3 lt2 = osg::Vec3(7.5 , -8,0);
				osg::Vec3 lt3 =  osg::Vec3(0 ,   5,0);

				vertices->push_back( lt1 ); 
				vertices->push_back( lt2 ); 
				vertices->push_back( lt3 ); 	
				pGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, vertices->size()));
			}
			else if (snapTYpe == SNAP_INTERSECTION)
			{
				lineWidth = 3.0;
				double size = 8;

				osg::Vec3 lt1 = osg::Vec3(-size, -size, 0);
				osg::Vec3 lt2 = osg::Vec3(size, size, 0);
				osg::Vec3 lt3 = osg::Vec3(-size, size, 0);
				osg::Vec3 lt4 = osg::Vec3(size, -size, 0);

				vertices->push_back(lt1);
				vertices->push_back(lt2);
				vertices->push_back(lt3);
				vertices->push_back(lt4);
				pGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, vertices->size()));
			}


 			pGeode->addDrawable(pGeometry);			

			osg::StateSet* set = 	pGeode->getOrCreateStateSet();
 
			osg::LineWidth* linewidth = new osg::LineWidth();
			linewidth->setWidth( lineWidth );
			set->setAttributeAndModes(linewidth,osg::StateAttribute::ON);	
		}
	}
}




void updateLineGeode(osg::Geometry* pGeometry , osg::Vec2 pt1 , osg::Vec2 pt2)
{
	if (pGeometry == NULL)
		return;

	if(pGeometry->getParents().size() == 0)
		return;

	osg::Vec3 p31(pt1, 0);
	osg::Vec3 p32(pt2, 0);


	osg::Vec3Array *pVertices =(osg::Vec3Array*)pGeometry->getVertexArray();
	if(pVertices && pVertices->size() == 2)
	{
		pVertices->at(0).set(p31);
		pVertices->at(1).set(p32);
		pGeometry->dirtyBound();
		pVertices->dirty();
	}
}
void updatePolyLineGeode(osg::Geometry* pGeometry ,  std::vector<osg::Vec3>& nextPTs)
{
	osg::Vec3Array *pVertices =(osg::Vec3Array*)pGeometry->getVertexArray();
	if(pVertices  )
	{
		osg::Vec3 firstPT = pVertices->at(0);

		pVertices->clear();
		pVertices->push_back(firstPT);
		for(int i=0; i< nextPTs.size(); i++)
		{
			pVertices->push_back( nextPTs[i]);
		}
		pGeometry->removePrimitiveSet(0, pGeometry->getNumPrimitiveSets());

		pGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, pVertices->size()));
		pGeometry->dirtyBound();
		pVertices->dirty();
	}
}


//窗口坐标
void updateBoxGeode(osg::Geometry* pGeometry , osg::Vec2 pt1 , osg::Vec2 pt2)
{
	if (pGeometry == NULL)
		return;

	int nCount = pGeometry->getParents().size();
	if(nCount == 0)
	{
		return;
	}

	osg::Vec3 p31(pt1, 0);
	osg::Vec3 p32(pt2, 0);


	osg::Vec3Array *pVertices =(osg::Vec3Array*)pGeometry->getVertexArray();
	if(pVertices && pVertices->size() == 4)
	{
		//pVertices->clear();

		pVertices->at(0).set(p31);
		pVertices->at(1).set(osg::Vec3(p31.x(), p32.y(), p31.z()));
		pVertices->at(2).set(p32);
		pVertices->at(3).set(osg::Vec3(p32.x(), p31.y(), p31.z()));

		pGeometry->dirtyBound();
		return pVertices->dirty();
	}
}	

//窗口坐标
void updateCircleGeode(osg::Geometry* pGeometry, osg::Vec2 pt1, osg::Vec2 pt2)
{
	int nCount = pGeometry->getParents().size();
	if (nCount == 0)
	{
		return;
	} 
 	double r = (pt2 - pt1).length();
	double alfa = M_PI * 2 / 36;
	osg::Vec3Array *pVertices = (osg::Vec3Array*)pGeometry->getVertexArray();
	if (pVertices && pVertices->size() == 36)
	{
		//pVertices->clear();
		for (int i = 0; i < 36; i++)
		{
			osg::Vec3 pt;
			pt.set(r*cos(i*alfa), r*sin(i*alfa), 0);
			pt[0] += pt1[0];
			pt[1] += pt1[1];
			pVertices->at(i).set(pt);
		}
		pGeometry->dirtyBound();
		return pVertices->dirty();
	}
}

//窗口坐标
void updateEllipseGeode(osg::Geometry* pGeometry, osg::Vec2 pt1, osg::Vec2 pt2, osg::Vec2 pt3)
{
	int nCount = pGeometry->getParents().size();
	if (nCount == 0)
	{
		return;
	}
	double a = (pt2 - pt1).length();
	double b = (pt3 - pt1).length();

	double alfa = M_PI * 2 / 36;
	osg::Vec3Array *pVertices = (osg::Vec3Array*)pGeometry->getVertexArray();
	if (pVertices && pVertices->size() == 36)
	{
		//pVertices->clear();
		for (int i = 0; i < 36; i++)
		{
			osg::Vec3 pt;
			pt.set(a*cos(i*alfa), b*sin(i*alfa), 0);
			pt[0] += pt1[0];
			pt[1] += pt1[1];
			pVertices->at(i).set(pt);
		}
		pGeometry->dirtyBound();
		return pVertices->dirty();
	}
	
}

void CreateHandler(osg::Vec3 lpt , osg::Vec4 color,osg::Geode* pGeode , int size)
{		
	osg::Geometry* pointsGeom = new osg::Geometry();

	osg::Vec3Array* vertices = new osg::Vec3Array;
	vertices->push_back(lpt);
	pointsGeom->setVertexArray(vertices);

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back( color );		
	pointsGeom->setColorArray(colors);
	pointsGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

	pointsGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,1));

	pGeode->addDrawable(pointsGeom);

	osg::StateSet* set = 	pGeode->getOrCreateStateSet();

	osg::Point* p = new osg::Point;
	p->setSize(size);
	set->setAttributeAndModes(p, osg::StateAttribute::ON );		
}

////////////////////////
///////////////////skybox

float refract = 1.02;          // ratio of indicies of refraction
float fresnel = 0.2;           // Fresnel multiplier

const char vpstr[] =
"!!ARBvp1.0 # Refraction                                    \n"
"                                                           \n"
"ATTRIB iPos         = vertex.position;                     \n"
"#ATTRIB iCol        = vertex.color.primary;                \n"
"ATTRIB iNormal      = vertex.normal;                       \n"
"PARAM  esEyePos     = { 0, 0, 0, 1 };                      \n"
"PARAM  const0123    = { 0, 1, 2, 3 };                      \n"
"PARAM  fresnel      = program.local[0];                    \n"
"PARAM  refract      = program.local[1];                    \n"
"PARAM  itMV[4]      = { state.matrix.modelview.invtrans }; \n"
"PARAM  MVP[4]       = { state.matrix.mvp };                \n"
"PARAM  MV[4]        = { state.matrix.modelview };          \n"
"PARAM  texmat[4]    = { state.matrix.texture[0] };         \n"
"TEMP   esPos;        # position in eye-space               \n"
"TEMP   esNormal;     # normal in eye-space                 \n"
"TEMP   tmp, IdotN, K;                                      \n"
"TEMP   esE;          # eye vector                          \n"
"TEMP   esI;          # incident vector (=-E)               \n"
"TEMP   esR;          # first refract- then reflect-vector  \n"
"OUTPUT oPos         = result.position;                     \n"
"OUTPUT oColor       = result.color;                        \n"
"OUTPUT oRefractMap  = result.texcoord[0];                  \n"
"OUTPUT oReflectMap  = result.texcoord[1];                  \n"
"                                                           \n"
"# transform vertex to clip space                           \n"
"DP4    oPos.x, MVP[0], iPos;                               \n"
"DP4    oPos.y, MVP[1], iPos;                               \n"
"DP4    oPos.z, MVP[2], iPos;                               \n"
"DP4    oPos.w, MVP[3], iPos;                               \n"
"                                                           \n"
"# Transform the normal to eye space.                       \n"
"DP3    esNormal.x, itMV[0], iNormal;                       \n"
"DP3    esNormal.y, itMV[1], iNormal;                       \n"
"DP3    esNormal.z, itMV[2], iNormal;                       \n"
"                                                           \n"
"# normalize normal                                         \n"
"DP3    esNormal.w, esNormal, esNormal;                     \n"
"RSQ    esNormal.w, esNormal.w;                             \n"
"MUL    esNormal, esNormal, esNormal.w;                     \n"
"                                                           \n"
"# transform vertex position to eye space                   \n"
"DP4    esPos.x, MV[0], iPos;                               \n"
"DP4    esPos.y, MV[1], iPos;                               \n"
"DP4    esPos.z, MV[2], iPos;                               \n"
"DP4    esPos.w, MV[3], iPos;                               \n"
"                                                           \n"
"# vertex to eye vector                                     \n"
"ADD    esE, -esPos, esEyePos;                              \n"
"#MOV   esE, -esPos;                                        \n"
"                                                           \n"
"# normalize eye vector                                     \n"
"DP3    esE.w, esE, esE;                                    \n"
"RSQ    esE.w, esE.w;                                       \n"
"MUL    esE, esE, esE.w;                                    \n"
"                                                           \n"
"# calculate some handy values                              \n"
"MOV    esI, -esE;                                          \n"
"DP3    IdotN, esNormal, esI;                               \n"
"                                                           \n"
"# calculate refraction vector, Renderman style             \n"
"                                                           \n"
"# k = 1-index*index*(1-(I dot N)^2)                        \n"
"MAD    tmp, -IdotN, IdotN, const0123.y;                    \n"
"MUL    tmp, tmp, refract.y;                                \n"
"ADD    K.x, const0123.y, -tmp;                             \n"
"                                                           \n"
"# k<0,  R = [0,0,0]                                        \n"
"# k>=0, R = index*I-(index*(I dot N) + sqrt(k))*N          \n"
"RSQ    K.y, K.x;                                           \n"
"RCP    K.y, K.y;                           # K.y = sqrt(k) \n"
"MAD    tmp.x, refract.x, IdotN, K.y;                       \n"
"MUL    tmp, esNormal, tmp.x;                               \n"
"MAD    esR, refract.x, esI, tmp;                           \n"
"                                                           \n"
"# transform refracted ray by cubemap transform             \n"
"DP3    oRefractMap.x, texmat[0], esR;                      \n"
"DP3    oRefractMap.y, texmat[1], esR;                      \n"
"DP3    oRefractMap.z, texmat[2], esR;                      \n"
"                                                           \n"
"# calculate reflection vector                              \n"
"# R = 2*N*(N dot E)-E                                      \n"
"MUL    tmp, esNormal, const0123.z;                         \n"
"DP3    esR.w, esNormal, esE;                               \n"
"MAD    esR, esR.w, tmp, -esE;                              \n"
"                                                           \n"
"# transform reflected ray by cubemap transform             \n"
"DP3    oReflectMap.x, texmat[0], esR;                      \n"
"DP3    oReflectMap.y, texmat[1], esR;                      \n"
"DP3    oReflectMap.z, texmat[2], esR;                      \n"
"                                                           \n"
"# Fresnel approximation = fresnel*(1-(N dot I))^2          \n"
"ADD    tmp.x, const0123.y, -IdotN;                         \n"
"MUL    tmp.x, tmp.x, tmp.x;                                \n"
"MUL    oColor, tmp.x, fresnel;                             \n"
"                                                           \n"
"END                                                        \n";


osg::TextureCubeMap* readCubeMap()
{
	osg::TextureCubeMap* cubemap = new osg::TextureCubeMap;

#define CUBEMAP_FILENAME(face) "G:\\DoctorBridge\\Main\\Debug\\bin\\res\\"#face".jpg"
	osg::Image* imagePosX = osgDB::readImageFile(CUBEMAP_FILENAME(posx));
	osg::Image* imageNegX = osgDB::readImageFile(CUBEMAP_FILENAME(negx));
	osg::Image* imagePosY = osgDB::readImageFile(CUBEMAP_FILENAME(posy));
	osg::Image* imageNegY = osgDB::readImageFile(CUBEMAP_FILENAME(negy));
	osg::Image* imagePosZ = osgDB::readImageFile(CUBEMAP_FILENAME(posz));
	osg::Image* imageNegZ = osgDB::readImageFile(CUBEMAP_FILENAME(negz));

	if (imagePosX && imageNegX && imagePosY && imageNegY && imagePosZ && imageNegZ)
	{
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_X, imagePosX);
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X, imageNegX);
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y, imagePosY);
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y, imageNegY);
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z, imagePosZ);
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z, imageNegZ);

		cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
		cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

		cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
		cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	}

	return cubemap;
}


// Update texture matrix for cubemaps
struct TexMatCallback : public osg::NodeCallback
{
public:

	TexMatCallback(osg::TexMat& tm) :
		_texMat(tm)
	{
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			const osg::Matrix& MV = *(cv->getModelViewMatrix());
			const osg::Matrix R = osg::Matrix::rotate(osg::DegreesToRadians(112.0f), 0.0f, 0.0f, 1.0f)*
				osg::Matrix::rotate(osg::DegreesToRadians(90.0f), 1.0f, 0.0f, 0.0f);

			osg::Quat q = MV.getRotate();
			const osg::Matrix C = osg::Matrix::rotate(q.inverse());

			_texMat.setMatrix(C*R);
		}

		traverse(node, nv);
	}

	osg::TexMat& _texMat;
};


class MoveEarthySkyWithEyePointTransform : public osg::Transform
{
public:
	/** Get the transformation matrix which moves from local coords to world coords.*/
	virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.preMultTranslate(eyePointLocal);
		}
		return true;
	}

	/** Get the transformation matrix which moves from world coords to local coords.*/
	virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.postMultTranslate(-eyePointLocal);
		}
		return true;
	}
};


osg::Node* createSkyBox()
{
	osg::StateSet* stateset = new osg::StateSet();

	osg::TexEnv* te = new osg::TexEnv;
	te->setMode(osg::TexEnv::REPLACE);
	stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::ON);

	osg::TexGen *tg = new osg::TexGen;
	tg->setMode(osg::TexGen::NORMAL_MAP);
	stateset->setTextureAttributeAndModes(0, tg, osg::StateAttribute::ON);

	osg::TexMat *tm = new osg::TexMat;
	stateset->setTextureAttribute(0, tm);

	osg::TextureCubeMap* skymap = readCubeMap();
	stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON);

	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
 	stateset->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);

	// clear the depth to the far plane.
	osg::Depth* depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1.0, 1.0);
	stateset->setAttributeAndModes(depth, osg::StateAttribute::ON);

	stateset->setRenderBinDetails(-1, "RenderBin");

	osg::Drawable* drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 8192*4));

	osg::Geode* geode = new osg::Geode;
	geode->setCullingActive(false);
	geode->setStateSet(stateset);
	geode->addDrawable(drawable);


	osg::Transform* transform = new MoveEarthySkyWithEyePointTransform;
	transform->setCullingActive(false);
	transform->addChild(geode);

	osg::ClearNode* clearNode = new osg::ClearNode;
	//  clearNode->setRequiresClear(false);
	clearNode->setCullCallback(new TexMatCallback(*tm));
	clearNode->addChild(transform);

	return clearNode;
}



////////////

#include <osgTerrain/TerrainTile>

static unsigned int heightTexture[] = {
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00010101, 0x02000000, 0x00020809,
	0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x00000001, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x05070200, 0x07090c06, 0x0e010004, 0x01061824,
	0x00010102, 0x00000000, 0x02020000, 0x04050705, 0x02060604, 0x00000000, 0x00000000, 0x00000000,
	0x00010000, 0x00000000, 0x00000000, 0x01000000, 0x2a37250a, 0x262a2d2b, 0x3518121c, 0x1f19273c,
	0x03151a1c, 0x00000000, 0x0e070000, 0x0e0d1b19, 0x161c1d15, 0x00010109, 0x00020905, 0x00010100,
	0x03030000, 0x00000000, 0x00010401, 0x06000000, 0x484c3714, 0x3d403b45, 0x534e443d, 0x42433c4b,
	0x102a2f34, 0x00000001, 0x05010000, 0x110c110c, 0x2a313226, 0x0c090715, 0x0009201d, 0x00010101,
	0x0b040000, 0x00000005, 0x00010301, 0x04000000, 0x45403315, 0x423a3242, 0x5e5a584f, 0x474b4b5a,
	0x20282a36, 0x00000006, 0x00000000, 0x180a0301, 0x3946402d, 0x281b1523, 0x041b3d3d, 0x00000001,
	0x0c020000, 0x0001060e, 0x00000000, 0x05020000, 0x4a3c280f, 0x3a39414e, 0x625a5143, 0x353c4152,
	0x242b3336, 0x00000009, 0x00000000, 0x2c160100, 0x3f46382d, 0x3e322b33, 0x082a5753, 0x00000001,
	0x08020000, 0x040a1114, 0x00000002, 0x0f0e0801, 0x4f382819, 0x41546562, 0x5c625743, 0x21212943,
	0x1b292f28, 0x00000005, 0x00000000, 0x2d180100, 0x30322a2a, 0x3e2d292f, 0x1e3f6058, 0x0000020d,
	0x14060000, 0x0d151a1f, 0x00000309, 0x03070401, 0x402a1509, 0x5c707463, 0x5b6a645d, 0x15142041,
	0x0a131511, 0x00000001, 0x00000000, 0x1d0e0100, 0x1c1d2322, 0x22182226, 0x444e4f3e, 0x00011a34,
	0x2a1a0500, 0x091a222c, 0x00010204, 0x00010000, 0x3e2c1201, 0x7c82765f, 0x6979797d, 0x060f264d,
	0x00020202, 0x00000000, 0x00000000, 0x0b020000, 0x0d141812, 0x0a091514, 0x3e362c1d, 0x00022c4f,
	0x412e0800, 0x09202e3a, 0x00000000, 0x00000000, 0x3c2b1001, 0x8d836f59, 0x7185898d, 0x01112c53,
	0x00000000, 0x00000000, 0x00000000, 0x0e020000, 0x141d1713, 0x01020b10, 0x2a221a09, 0x00032041,
	0x523a0f01, 0x071c2f47, 0x00000000, 0x00000000, 0x3d1d0400, 0x8a796358, 0x708d9392, 0x07263d57,
	0x00000000, 0x00000000, 0x00000000, 0x15030000, 0x1f1e171b, 0x00010b1c, 0x1e130f02, 0x00021a36,
	0x533e1301, 0x0c1d283f, 0x00000001, 0x00000000, 0x30110000, 0x856d5543, 0x6f889293, 0x143a4e5f,
	0x00000001, 0x00000000, 0x00000000, 0x07000000, 0x1919120d, 0x00000919, 0x14040200, 0x0001102b,
	0x46331002, 0x121a2035, 0x00000002, 0x00000000, 0x1c0a0000, 0x80654d2c, 0x677f8e92, 0x1a425058,
	0x00000003, 0x00000000, 0x00000000, 0x00000000, 0x08100801, 0x00000409, 0x05000000, 0x0001040e,
	0x321e0c02, 0x0e171c30, 0x00000001, 0x00000000, 0x1a050000, 0x705a4830, 0x667f8c86, 0x203d434c,
	0x0000020e, 0x00000000, 0x00000000, 0x00000000, 0x0d070000, 0x00021015, 0x00000000, 0x00000001,
	0x291e0c02, 0x08121423, 0x00000001, 0x00000000, 0x250a0000, 0x60524b3d, 0x68808676, 0x1e2c3246,
	0x00000412, 0x00000000, 0x00000000, 0x00000000, 0x22160500, 0x010c232b, 0x00000000, 0x00000000,
	0x1e180a01, 0x080c0e18, 0x00000001, 0x00000000, 0x260b0503, 0x5a514940, 0x6d828373, 0x1322314c,
	0x00000007, 0x00000000, 0x00000000, 0x00000000, 0x2f200e03, 0x04192d35, 0x00000000, 0x00000000,
	0x14100701, 0x0302020b, 0x00000001, 0x05000000, 0x2d141716, 0x453e3f45, 0x71817f63, 0x051a2f52,
	0x00000001, 0x00000000, 0x00000000, 0x07020000, 0x3321140f, 0x0d172738, 0x02020103, 0x00000001,
	0x01010000, 0x00000001, 0x00000000, 0x0e010000, 0x24121d21, 0x33323b3c, 0x71796b4f, 0x010e2a57,
	0x00000000, 0x00000000, 0x00000000, 0x20140200, 0x30281f23, 0x1b272d36, 0x09110d0c, 0x00000001,
	0x00000000, 0x00000000, 0x00000000, 0x180a0000, 0x1e182025, 0x33373e33, 0x65625747, 0x02142a4e,
	0x09010000, 0x00000003, 0x00000000, 0x372b0c01, 0x30262736, 0x2c414440, 0x081a211f, 0x00000003,
	0x00000000, 0x00000000, 0x00000000, 0x1f120000, 0x2a1f1c24, 0x3940473f, 0x56574e44, 0x15242b41,
	0x0f060106, 0x00000007, 0x00000000, 0x2e290e00, 0x2f1f242e, 0x374b4f48, 0x0c1b262a, 0x00000004,
	0x00000000, 0x00000000, 0x00000000, 0x19120200, 0x3721181c, 0x45525b51, 0x56554e4c, 0x333c3d48,
	0x09141523, 0x01010001, 0x00000000, 0x121b0d01, 0x2f1c1510, 0x2a3a4445, 0x19232321, 0x0000010c,
	0x00000000, 0x00000000, 0x06020000, 0x17130301, 0x3c231e1e, 0x4b596558, 0x59545452, 0x36424b5a,
	0x04172430, 0x0c0b0200, 0x00000001, 0x03120e03, 0x1e0a0201, 0x282f332d, 0x212e2d23, 0x0000010e,
	0x00000000, 0x00000000, 0x06030000, 0x1d170501, 0x35252422, 0x495a6450, 0x5751514d, 0x2c3b4a5a,
	0x091b2429, 0x0f170c04, 0x00000001, 0x010a0501, 0x09010000, 0x2e2e2c1d, 0x2336352d, 0x00010414,
	0x00000000, 0x00000000, 0x01010000, 0x271a0400, 0x34242324, 0x4f5e6953, 0x4e454951, 0x242c394b,
	0x1e2d2521, 0x040f0f10, 0x00000000, 0x010a0300, 0x01000000, 0x241f1f11, 0x2e3d3226, 0x000b1a23,
	0x00000000, 0x00000000, 0x04020000, 0x35240a04, 0x41353232, 0x58656a5b, 0x3a424d56, 0x171a242f,
	0x272e2218, 0x00030c1c, 0x00000000, 0x01040100, 0x00000000, 0x16100d06, 0x36332117, 0x01183334,
	0x00000000, 0x00000000, 0x07020000, 0x36250a04, 0x43403837, 0x515b5953, 0x29364245, 0x02030a1b,
	0x181d1708, 0x00030b17, 0x00000000, 0x00000000, 0x00000000, 0x231a0a02, 0x3730221c, 0x01162c33,
	0x00000000, 0x00000000, 0x05010000, 0x38260d05, 0x3b3e3c3d, 0x3c464847, 0x20293332, 0x0000010f,
	0x0e160e01, 0x00060c11, 0x00000000, 0x00000000, 0x00000000, 0x2e240f04, 0x31302826, 0x010c1429,
	0x00000000, 0x00000000, 0x00000000, 0x25150602, 0x242d3731, 0x242b3131, 0x0b1a2621, 0x00000002,
	0x080e0800, 0x060e1511, 0x00000001, 0x00000000, 0x00000000, 0x31281707, 0x24242429, 0x00020b1c,
	0x00000000, 0x00000000, 0x00000000, 0x0b030000, 0x08111f1a, 0x1a101314, 0x0109171b, 0x00000000,
	0x17100801, 0x1f262f27, 0x00000311, 0x00000000, 0x01000000, 0x3133230c, 0x1f252829, 0x00010815,
	0x00000000, 0x00000000, 0x00000000, 0x01000000, 0x00020a09, 0x09020402, 0x00010207, 0x00000000,
	0x241c1203, 0x28344037, 0x00030d1c, 0x00000000, 0x0e030000, 0x383b291a, 0x1b222931, 0x00010410,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x01060601, 0x00000000, 0x00000000,
	0x29291e07, 0x25343f34, 0x0208101b, 0x00000000, 0x1e0b0100, 0x353e3229, 0x1618202e, 0x00000206,
	0x00000000, 0x00000000, 0x00000000, 0x00010101, 0x00000103, 0x030f0f03, 0x00000000, 0x00000000,
	0x2b291a08, 0x2131342a, 0x070d1118, 0x00000000, 0x21150c06, 0x2a35312d, 0x0e111929, 0x00000003,
	0x00000000, 0x00000000, 0x00000000, 0x0c0a0a02, 0x00010b16, 0x08151708, 0x00000002, 0x01000000,
	0x26231812, 0x152d2c20, 0x0c060306, 0x04010004, 0x2431321f, 0x192a2d27, 0x01020715, 0x00000001,
	0x00010100, 0x00010100, 0x00000000, 0x170e0a01, 0x00051820, 0x070f0c02, 0x00000307, 0x02000000,
	0x1f1a0d0b, 0x10221915, 0x0f040001, 0x1e0f1418, 0x31475645, 0x0618242d, 0x00000002, 0x00000000,
	0x03050200, 0x01040202, 0x00000000, 0x0f0c0901, 0x00091b19, 0x05020000, 0x0001050a, 0x01010101,
	0x18140503, 0x0b140a0c, 0x17030001, 0x42383d33, 0x44576a62, 0x010d233a, 0x00000000, 0x00000000,
	0x1a130200, 0x03070910, 0x03010001, 0x02040b05, 0x00030d0a, 0x00000000, 0x00000001, 0x05010101,
	0x0e070002, 0x02050104, 0x15010000, 0x51494e3d, 0x556c776a, 0x0b1a3447, 0x00000002, 0x00000000,
	0x3b220801, 0x09101b37, 0x17090105, 0x0006171d, 0x00000101, 0x00000000, 0x00000000, 0x02000000,
	0x03020002, 0x00010001, 0x11010000, 0x4c4a4e37, 0x6677755d, 0x242d4559, 0x0000000e, 0x00000000,
	0x41270700, 0x0a0e1636, 0x1e0b0004, 0x010d1d2a, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x01020000, 0x15050000, 0x48474934, 0x6066594a, 0x2f384955, 0x00000117, 0x00000000,
	0x2f2d0b01, 0x030b2b40, 0x1d120502, 0x020d1d2a, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x09090000, 0x2d1d0903, 0x413f423e, 0x4e55453b, 0x25333f42, 0x0000000f, 0x00000000,
	0x29160c01, 0x1b253b3e, 0x24251a15, 0x0007202b, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x0e0b0000, 0x3c2f150a, 0x373f3f42, 0x3e3b2927, 0x15253132, 0x00000008, 0x00000000,
	0x24100401, 0x3b323939, 0x262f2e36, 0x00072027, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x0f100401, 0x43381e0e, 0x223c4649, 0x261e0c0d, 0x0e212722, 0x00000003, 0x00010000,
	0x2c110200, 0x4e434442, 0x303e3f4a, 0x00081d25, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x1c1a0800, 0x4d413321, 0x1233535b, 0x190e0103, 0x182b2821, 0x00000003, 0x00020000,
	0x2d240800, 0x5d5a5742, 0x46555257, 0x04172a35, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x241f0e02, 0x4d47412e, 0x0f305058, 0x13060003, 0x24312920, 0x0401000b, 0x00010002,
	0x26221201, 0x67635f45, 0x58656064, 0x041c2c3e, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x2d2e200d, 0x3d3a3f37, 0x0c263c3f, 0x08010003, 0x21312516, 0x0601020f, 0x00000003,
	0x1b0d0601, 0x73695f43, 0x5e6e6f77, 0x01102a45, 0x00000000, 0x00000000, 0x00000000, 0x00010000,
	0x04000000, 0x2c312a19, 0x342d3032, 0x202f3432, 0x05000112, 0x1b2d2111, 0x0902030e, 0x00000004,
	0x0b020100, 0x71675432, 0x5367727b, 0x0006253f, 0x00000000, 0x00000000, 0x01000000, 0x080a0302,
	0x0e030201, 0x171f2220, 0x2b211417, 0x2d372823, 0x00000115, 0x09120a03, 0x04010208, 0x00000001,
	0x07050100, 0x655c4627, 0x4b56606d, 0x00021b38, 0x00010000, 0x00000000, 0x0e070000, 0x0e0e080d,
	0x0e111108, 0x06111714, 0x1a100303, 0x23302216, 0x0000000a, 0x02010000, 0x080c1110, 0x00000207,
	0x06030000, 0x645e4324, 0x4f585b66, 0x00051d37, 0x01020102, 0x00000000, 0x1b140501, 0x080c0d19,
	0x0a141607, 0x0b080d0d, 0x0a070509, 0x14212010, 0x00000003, 0x01000000, 0x080c1610, 0x00000311,
	0x12060100, 0x6d5f4730, 0x5a606673, 0x05112a44, 0x00010309, 0x07020000, 0x161e1e18, 0x0204040d,
	0x080a0a03, 0x21100f0e, 0x040e1923, 0x05101107, 0x00000001, 0x00000000, 0x04020503, 0x00020810,
	0x18110200, 0x675d4832, 0x4d52606b, 0x121c2c41, 0x0000010b, 0x1f110100, 0x1d2b3a35, 0x0000010e,
	0x00010000, 0x2f1c1306, 0x000e202f, 0x00020100, 0x00000000, 0x00000000, 0x0e020000, 0x00061618,
	0x16170300, 0x5c614c2c, 0x3d3f4e57, 0x0b102239, 0x00000006, 0x27190400, 0x30384136, 0x01060a1e,
	0x00000000, 0x2d281905, 0x03142029, 0x00000001, 0x00000000, 0x01000000, 0x1c0a0101, 0x00091f25,
	0x22210300, 0x535b4d35, 0x2b2a3948, 0x00062130, 0x00000000, 0x211d0f03, 0x30312e21, 0x0a120e1d,
	0x00000001, 0x26291503, 0x17262826, 0x0000020d, 0x00000000, 0x0d030000, 0x2316090f, 0x00071c28,
	0x311d0200, 0x48504c44, 0x2e2c373f, 0x04102934, 0x03010001, 0x1c271f0c, 0x2a291c11, 0x131a1119,
	0x00000003, 0x1c1f0e01, 0x2e2d2720, 0x00021129, 0x00000000, 0x2e120101, 0x2b282133, 0x010d232c,
	0x34180100, 0x434d4d48, 0x2929343c, 0x0e1c2629, 0x0f070002, 0x141f1b13, 0x2c250c06, 0x111d1a1e,
	0x00000004, 0x17191001, 0x40392f1f, 0x030e2740, 0x04010000, 0x48290702, 0x36474a4f, 0x01112936,
	0x31110000, 0x30424845, 0x20242d2c, 0x19242621, 0x090c0508, 0x13100806, 0x1e150307, 0x0715171b,
	0x00000001, 0x17140a01, 0x4c433723, 0x15203750, 0x0f0a0206, 0x563a170c, 0x5a67675d, 0x01112e49,
	0x26080000, 0x23313836, 0x23292d28, 0x1b2c312a, 0x00040a0f, 0x0b060000, 0x0a030003, 0x0d191715,
	0x00000001, 0x05020000, 0x41342813, 0x393e494f, 0x211f1724, 0x5c4a2f22, 0x71777368, 0x01143758,
	0x1e070000, 0x252d2f2b, 0x34272429, 0x0f283a3f, 0x01000204, 0x00010002, 0x01010001, 0x191f1409,
	0x00000005, 0x00000000, 0x2f180c03, 0x52525349, 0x30303344, 0x5f58483b, 0x7c7f7d75, 0x02174c71,
	0x13050000, 0x161d1815, 0x36271919, 0x0f26383b, 0x01000001, 0x00000001, 0x00000000, 0x1a1d0f01,
	0x00000006, 0x00000000, 0x1a070100, 0x46424234, 0x39383d48, 0x635b594f, 0x8082837c, 0x00063e79,
	0x0b070001, 0x0e120908, 0x2a26170f, 0x15242c28, 0x00000003, 0x00030400, 0x06020000, 0x201e1107,
	0x00000009, 0x00000000, 0x04010000, 0x342f2916, 0x49483f3a, 0x615a5d57, 0x7f828379, 0x0102296b,
	0x01020001, 0x110d0301, 0x1a18110c, 0x121f1e1b, 0x00000002, 0x04080400, 0x20130302, 0x1c291f1c,
	0x00000004, 0x00000000, 0x00000000, 0x31231104, 0x564f372e, 0x5d565d5d, 0x7a828578, 0x02001157,
	0x00000001, 0x0c0d0200, 0x0c0c0503, 0x10120d0b, 0x00000002, 0x09020000, 0x331e0d0b, 0x13343837,
	0x00000001, 0x00000000, 0x00000000, 0x2c140201, 0x564e382f, 0x6054585b, 0x5f7b7c75, 0x03010e37,
	0x00000001, 0x04030000, 0x0e080001, 0x1117120d, 0x00000001, 0x01000000, 0x2d1a0e06, 0x0b2b4040,
	0x00000000, 0x00000000, 0x00000000, 0x210b0000, 0x4e43362e, 0x58484752, 0x345a5d60, 0x03010713,
	0x00000001, 0x00000000, 0x03010000, 0x03070504, 0x00000001, 0x00000000, 0x0b030101, 0x010b1819,
	0x00000000, 0x00000000, 0x00000000, 0x11040000, 0x201f1b1e, 0x20131325, 0x13202426, 0x01000007,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x02000001, 0x02030407, 0x01000003, 0x01030202, 0x00000001,
};
 ////////////////////////////////////////////////////////////////////////////////
// Rearange pixels if Big Endian hardware or CPU word is greater than 4 bytes.
// I am on 32 bit Intel so this code is dead on my machine.
// If you happen to find a bug let us know or better fix it and let us know.
////////////////////////////////////////////////////////////////////////////////
static unsigned char * orderBytes(unsigned int *raster, unsigned int size)
{
	unsigned int endian_check = 0x1;

	unsigned char & little_endian = *(unsigned char*)&endian_check;
	unsigned char * dst = (unsigned char*)raster;
	unsigned char * src = (unsigned char*)raster;

	unsigned int dst_step = 4;
	unsigned int src_step = sizeof(unsigned int);

	if (little_endian && src_step == dst_step)
		return src;

	unsigned counter = size / src_step;

	// pack bytes if 64 (or more) bit machine
	if (src_step > dst_step) {
		if (!little_endian)
			src += src_step - dst_step;

		for (unsigned int i = 0; i < counter; i++)
		{
			dst[0] = src[0];
			dst[1] = src[1];
			dst[2] = src[2];
			dst[3] = src[3];
			src += src_step;
			dst += dst_step;
		}
	}

	// reorder bytes if big endian machine
	if (!little_endian) {
		dst = (unsigned char*)raster;
		while (counter--)
		{
			std::swap(dst[0], dst[3]);
			std::swap(dst[1], dst[2]);
			dst += dst_step;
		}
	}

	return (unsigned char*)raster;
}
 
 osg::Node* createIsland(const osg::Vec3& center , float radius)
{
	float height = radius/2;//1000

	osg::ref_ptr<osg::Group> group = new osg::Group;

	osg::ref_ptr<osg::Image> heightMap = new osg::Image();

	heightMap->setImage(64, 64, 1,	GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE,orderBytes(heightTexture, sizeof(heightTexture)),osg::Image::NO_DELETE);

	osg::ref_ptr<osg::Image> colorMap = NULL; // osgDB::readImageFile("Images/colorMap.png");
	if (!colorMap)
	{
		struct colorElevation
		{
			colorElevation(unsigned int elev, const osg::Vec4ub& c) :
				elevation(elev), color(c) {}
			unsigned int elevation;
			osg::Vec4ub color;
		};

		colorElevation colorElevationMap[] =
		{
			colorElevation(0,   osg::Vec4ub(0, 128, 255, 255)),
			colorElevation(8,   osg::Vec4ub(192, 192, 128, 255)),
			colorElevation(32,  osg::Vec4ub(0, 255, 0, 255)),
			colorElevation(128, osg::Vec4ub(128, 128, 128, 255)),
			colorElevation(192, osg::Vec4ub(96, 96, 96, 255)),
			colorElevation(255, osg::Vec4ub(255, 255, 255, 255)),
			colorElevation(256, osg::Vec4ub(255, 255, 255, 255))
		};

		colorMap = new osg::Image();
		colorMap->allocateImage(heightMap->s(), heightMap->t(), 1, GL_RGBA, GL_UNSIGNED_BYTE);
		unsigned int margin = 0;
		for (unsigned int r = margin; r < colorMap->t() - margin; ++r)
		{
			for (unsigned int c = margin; c< colorMap->s() - margin; ++c)
			{
				unsigned int h = *heightMap->data(c, r);
				unsigned int i = 0;
				while (h > colorElevationMap[i + 1].elevation) ++i;
				float f0 = float(h - colorElevationMap[i].elevation) /(colorElevationMap[i + 1].elevation - colorElevationMap[i].elevation);

				float f1 = 1.f - f0;
				*(osg::Vec4ub*)colorMap->data(c, r) =colorElevationMap[i].color * f1 + colorElevationMap[i + 1].color * f0;
			}
		}
	}

	osg::Vec3 origin(center - osg::Vec3(radius, radius, 0));
	origin.z() = 0.0;

	osg::HeightField* grid = new osg::HeightField;
	grid->allocate(heightMap->s(), heightMap->t());
	grid->setOrigin(origin);
	grid->setXInterval(radius*2.0f / (grid->getNumColumns() - 1.0));
	grid->setYInterval(radius*2.0f / (grid->getNumRows() - 1.0));

	for (unsigned int r = 0; r < grid->getNumRows() - 0; ++r)
	{
		for (unsigned int c = 0; c < grid->getNumColumns() - 0; ++c)
		{
			grid->setHeight(c, r, height * exp(*heightMap->data(c, r) / 255.f) / exp(1.0));
		}
	}

	osg::ref_ptr<osgTerrain::TerrainTile> terrainTile = new osgTerrain::TerrainTile;

	osg::ref_ptr<osgTerrain::Locator> locator = new osgTerrain::Locator;
	locator->setCoordinateSystemType(osgTerrain::Locator::PROJECTED);
	locator->setTransformAsExtents(center.x() - radius, center.y() - radius, center.x() + radius, center.y() + radius);

	terrainTile->setLocator(locator.get());

	osg::ref_ptr<osgTerrain::HeightFieldLayer> hfl = new osgTerrain::HeightFieldLayer;
	hfl->setHeightField(grid);
	hfl->setLocator(locator.get());
	terrainTile->setElevationLayer(hfl.get());

	osg::ref_ptr<osgTerrain::ImageLayer> imageLayer = new osgTerrain::ImageLayer;
	imageLayer->setImage(colorMap.get());
	imageLayer->setLocator(locator.get());
	terrainTile->setColorLayer(0, imageLayer.get());

	osg::StateSet* stateset = terrainTile->getOrCreateStateSet();
	stateset->setAttributeAndModes(new osg::CullFace(), osg::StateAttribute::ON);

 	group->addChild(terrainTile.get());

 
	return group.release();
}


 CString MakeFixLength(const CString& input, int len)
 {
	 int tlen = input.GetLength();
	 if (tlen >= len)
		 return input;
	 return input + CString(_T(' '), len - tlen);
 }

