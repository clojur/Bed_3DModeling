#include "stdafx.h"
#include "resource.h"
#include "string_tools.h"
#include "compare_tools.h"

#include "PickHandler.h"
#include <osgmanipulator/Dragger>
#include <osgmanipulator/Projector>
#include <osgUtil/Optimizer>
#include <osg/Geometry>
#include <osg/ComputeBoundsVisitor>
#include <osg/Point>
#include "DbManipulator.h"
#include "2dCameraManipulator.h"
#include "BoxSelector.h"
#include "Db3DModelingUnitity.h"

#include "osgInterface.h"

#include "osgtools.h"

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
 
#include <osgviewer/CompositeViewer>
#include <assert.h>
#include "MultiViewPortOSG.h"
#include "ViewControlData.h"
#include "OffsetHandler.h"
#include "OSGDimObj.h"
#include "CommonFunctions.h"
#include "IDHandler.h"
#include "eraseHandler.h"
#include "FastPolytopeIntersector.h"
#include <iostream>
#include "IDb3DCommandLine.h"
#include"string_tools.h"
  
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////property_list_MultiObject
property_list_MultiObject::property_list_MultiObject()
{

}
property_list_MultiObject::~property_list_MultiObject()
{

}


void CViewControlData::DoDelView()
{
	if(m_Osg)
	{
		m_Osg->DelView(this);
	}
}

void CViewControlData::DoMax()
{
	if(m_Osg)
	{
		m_Osg->DoMultiViewMax();
	}
}
void CViewControlData::DoFullSceen()
{
	if (viewType >= 4)
		bNeedFullSceen = 1;
	else
	{
		if (viewType == 1)
			TopView2();
		else if (viewType == 2)
			FrontView2();
		else if (viewType == 3)
			LeftView2();
	}
}


BOOL CViewControlData::SceenIsEmpty()
{
	return m_pUserData->FindObjByType(OT_ALL).empty();

}

osg::BoundingSphere CViewControlData::CalBoundBox()
{
	osg::BoundingSphere boundingSphere;

	osg::ComputeBoundsVisitor cbbv;
	GraphicsGroup->accept(cbbv);

	if (pTerrain)
	{
		pTerrain->accept(cbbv);
	}
	osg::BoundingBox bb   = cbbv.getBoundingBox();	

	dimObjs->accept(cbbv);
	osg::BoundingBox bb2 = cbbv.getBoundingBox();
	bb.expandBy(bb2);


	if (bb.valid()) boundingSphere.expandBy(bb);
	else boundingSphere = GraphicsGroup->getBound();

	if(boundingSphere.radius() <MINRADIUS )
	{
		boundingSphere.radius() = defaultRaidus;
		bb.expandBy(boundingSphere);
	}
	return boundingSphere;
}

void CViewControlData::FullSceenImpl()
{
	if( mView)
	{
		if(mView->getCamera()->getViewport() )
		{
			double width = 0;
			double height = 0;

			width = mView->getCamera()->getViewport()->width();
			height = mView->getCamera()->getViewport()->height();

			double radio = 1.0*width / height;

			double dOldBoxW=0;
			double dOldBoxH =0;

			osg::ComputeBoundsVisitor cbbv;
			GraphicsGroup->accept(cbbv);
			dimObjs->accept(cbbv);
			tempObjs->accept(cbbv);
			if (pTerrain)
				pTerrain->accept(cbbv);
			osg::BoundingBox bb   = cbbv.getBoundingBox();	
 	/*		if(bb.valid() )*/
			{
				if ( SceenIsEmpty() || !bb.valid()  )
				{
					osg::Vec3 center =  localOrg;
					bb.set( center - osg::Vec3(defaultRaidus, defaultRaidus, defaultRaidus) ,center+osg::Vec3(defaultRaidus, defaultRaidus, defaultRaidus) );
				}
				if (1)//以前实现的功能
				{
					bool bBreak=FALSE;
					int nCount = 1;
					while (!bBreak)
					{				
						double dBoxW =0;
						double dBoxH  = 0;
						double dBoxL=0;

						double dBoxCx =  0;
						double dBoxCy =  0;
						double dBoxCz = 0;

						CalBoundBoxWH(dBoxW , dBoxH ,dBoxL, dBoxCx, dBoxCy , dBoxCz, mView , bb);

						//处理异常情况 ，包围框可能是一维的
						if (abs(dBoxW) < 1.0e-6 && abs(dBoxH) < 1.0e-6)
						{
							dBoxW = defaultRaidus;
							dBoxH = defaultRaidus;
						}
						else if (abs(dBoxW) < 1.0e-6)
						{
							dBoxW = dBoxH*radio;
						}
						else if (abs(dBoxH) < 1.0e-6)
						{
							dBoxH = dBoxW / radio;
						}

						if( (abs(dOldBoxW -dBoxW ) <0.001  && abs(dOldBoxH - dBoxH) <0.001) || nCount >5 )//超过五次退出
						{
							bBreak=TRUE;
						}
						else
						{
							nCount++;
							dOldBoxH  = dBoxH;
							dOldBoxW = dBoxW;
						}
						//长宽处理
						{
							double radioBox = dBoxW / dBoxH;
							if (radio > radioBox)
							{
								dBoxH = dBoxW / radioBox;
							}
							else
							{
								dBoxH = dBoxW / radio;
							}
						}

						osg::Vec2 pt1,pt2;
						float x1 = 2.0f*(dBoxCx- dBoxW)/width -1;
						float x2 = 2.0f*(dBoxCx+dBoxW)/width -1;

						float y1 = 2.0f*(dBoxCy -dBoxH)/height -1;
						float y2 = 2.0f*(dBoxCy+dBoxH)/height -1;

						if(bBreak)
						{
							x1 -=0.1;
							y1 -=0.1;
							x2 +=0.1;
							y2 +=0.1;
						}

						pt1.set(  x1,  y1 );
						pt2.set( x2 ,  y2 );

						BoxZoom(pt1, pt2);
					}	

		
				}
				else//2016-7-9日调整
				{
					osg::Vec3d eye;
					osg::Vec3d center;
					osg::Vec3d up;
					mView->getCamera()->getViewMatrixAsLookAt(eye, center, up);

					double left;
					double right;
					double bottom;
					double top;
					double zNear;
					double zFar;
					mView->getCamera()->getProjectionMatrixAsOrtho(left,right ,top,bottom,zNear , zFar);
 
					osg::Vec3d dir =  center-eye;
					dir.normalize();

					double bbRadius = bb.radius();

					mView->getCamera()->setViewMatrixAsLookAt(bb.center() - dir*(bbRadius*3.0), bb.center(), up);
					mView->getCamera()->setProjectionMatrixAsOrtho( - bbRadius, bbRadius, -bbRadius / radio, bbRadius / radio, zNear, zFar);
				}
			
			}	
		}	
	}
}
void CViewControlData::setManipulator(int viewType , osg::ref_ptr<osg::Group> pNode, osgViewer::View* pView ,int width)
{
	CDb2DCameraManipulator*  p2DManipulator = new CDb2DCameraManipulator(viewType, pNode, pView);
	p2DManipulator->pOSG = this;
	mView->setCameraManipulator(p2DManipulator);
	p2DManipulator->myHome(width);
}

//////////四元数插值
void slerp(float result[4], float starting[4], float ending[4], float t)
{
	float cosa = starting[0] * ending[0] + starting[1] * ending[1] +
		starting[2] * ending[2] + starting[3] * ending[3];
	if (cosa < 0.0f) 
	{
		ending[0] = -ending[0];
		ending[1] = -ending[1];
		ending[2] = -ending[2];
		ending[3] = -ending[3];
		cosa = -cosa;
	}
	float k0, k1;
	if (cosa > 0.9999f)
	{
		k0 = 1.0f - t;
		k1 = t;
	}
	else 
	{
		float sina = sqrt(1.0f - cosa*cosa);
		float a = atan2(sina, cosa);
		float invSina = 1.0f / sina;
		k0 = sin((1.0f - t)*a) * invSina;
		k1 = sin(t*a) * invSina;
	}
	result[0] = starting[0] * k0 + ending[0] * k1;
	result[1] = starting[1] * k0 + ending[1] * k1;
	result[2] = starting[2] * k0 + ending[2] * k1;
	result[3] = starting[3] * k0 + ending[3] * k1;
}


 

void CViewControlData::ChangeViewType()
{
	if(bNeedChangeViewType==0 )
		return;
  
	bNeedChangeViewType--;
	double width,height;
	if (viewType < 4)       //2d相机
	{
		if (mView->getCamera()->getViewport())
		{
			width  = mView->getCamera()->getViewport()->width();
			height = mView->getCamera()->getViewport()->height();
		}
		setManipulator(viewType, GraphicsGroup, mView, width);
	}	 
	else
	{
		osg::BoundingSphere bs   = CalBoundBox();
		double radius = bs.radius();	

		osg::Vec3 up= osg::Z_AXIS;	

		osg::Vec3 lookDir;
		osg::Vec3 center=bs.center();

		if(viewType ==4)
		{
			lookDir = osg::Vec3(-1, 1, -.735);
		}
		else if (viewType == 5)//top
		{
			lookDir = -osg::Z_AXIS;
			up = osg::Y_AXIS;
		}
		else if (viewType == 6)//f
		{
			lookDir = osg::Y_AXIS;
		}
		else if (viewType == 7)//left
		{
			lookDir = -osg::X_AXIS;
		}
		else if (viewType == 10)
		{
			lookDir = -localXDir^localYDir;
			lookDir.normalize();
		}

		ChangeViewByType(center, radius, lookDir, up, mView);
	}
}



osg::Vec2 CViewControlData::getWindosPos( double dx , double dy , double dz  )
{
	osg::Matrixd masterCameraVPW =  mView->getCamera()->getViewMatrix() * mView->getCamera()->getProjectionMatrix();
	if(mView->getCamera()->getViewport())
	{
		osg::Viewport* viewport = mView->getCamera()->getViewport();
		masterCameraVPW *= computeWindowMatrix(viewport);
	}
	osg::Vec3 winPos= osg::Vec3(dx,dy,dz)*masterCameraVPW;

	return osg::Vec2(winPos.x(), winPos.y());
}

//该函数可能有问题
osg::Vec3d CViewControlData::getWorldPos(double dx , double dy , BOOL bSnap )
{

	//先求出世界坐标
	osg::Matrixd masterCameraVPW =  mView->getCamera()->getViewMatrix() * mView->getCamera()->getProjectionMatrix();
	if(mView->getCamera()->getViewport())
	{
		osg::Viewport* viewport = mView->getCamera()->getViewport();
		masterCameraVPW *= computeWindowMatrix(viewport);
	}
	osg::Matrixd mtxInverseMVPW = osg::Matrix::inverse(masterCameraVPW); 	
	//////////////////

	osg::Vec3d resPT;
	resPT.set(dx,dy,0);


	if (bSnap)
	{
		BOOL flag = getSnapPT(resPT, masterCameraVPW, mtxInverseMVPW);
		if (flag)
			return resPT;
	}
	//////////////正交捕捉
	if (m_bOrtho)
	{
		osg::Vec3d orthPoit = resPT;
		BOOL flag = OrthoPoint(orthPoit, resPT, masterCameraVPW,mtxInverseMVPW);
		if (flag)
		{
			resPT = orthPoit;
			if (flag == 2)
				return resPT;
		}
	}

	////////////////窗口坐标转世界坐标
	{
		osg::Vec3d vec   = osg::Vec3d(resPT.x(), resPT.y(), 0)*mtxInverseMVPW;
		osg::Vec3d vec2 = osg::Vec3d(resPT.x(), resPT.y(), 1)*mtxInverseMVPW;
		if ((vec - vec2).length2() < 1.0e-3)
			return vec;

		osg::Vec3d rtn;
		osgManipulator::PointerInfo pi;
		pi.setNearFarPoints( vec , vec2 );

		osg::ref_ptr<osgManipulator::PlaneProjector> pp = new osgManipulator::PlaneProjector;
		if(viewType ==4 || viewType>7&&viewType <=10)//透视图的工作平面取过（0,0，osgz）的x0y平面
		{
			pp->setPlane( osg::Plane(osg::Vec3d(0,0,1) , osg::Vec3d(0,0,osgZ) ) );  	
			if (pp->project(pi, rtn))
				resPT = rtn;
			else
			{
				resPT = (vec + vec2)*0.5;
 			}
		}
		else
		{
			osg::Vec3d eye;
			osg::Vec3d center;
			osg::Vec3d up;
			mView->getCameraManipulator()->getHomePosition(eye, center, up);
			osg::Vec3d normal = center-eye;
			normal.normalize();
			pp->setPlane(osg::Plane(normal, osg::Vec3d(0, 0, 0)));  //投影到和相机轴线垂直并过原点的平面；	
		
			if (pp->project(pi, rtn))
				resPT = rtn;
			else
			{
				if (viewType == 1 || viewType == 5) // xy
				{
					resPT = osg::Vec3(vec.x(), vec.y(), 0);
				}	
				if (viewType == 2 || viewType == 6) // xz
				{
					resPT = osg::Vec3(vec.x(), 0, vec.z() );
				}
				if (viewType == 3 || viewType == 7) //yz
				{
					resPT = osg::Vec3(0, vec.y(), vec.z() );
				}
			}
		}
	}
	return resPT;
 }


//VPW -》世界坐标-》窗口坐标； InverseVPW:窗口-》世界 resPT为窗口坐标
//返回值为捕捉成功标志； 如果成功resPT返回世界坐标

void CViewControlData::ModiftSnapType(const CString& objType, BOOL bAdd)
{
	if (bAdd)
	{
		userDefineSnapObjType.push_back(objType);
	}
	else
	{
		std::vector<CString>::iterator it = std::find(userDefineSnapObjType.begin(), userDefineSnapObjType.end() ,objType);
		if (it != userDefineSnapObjType.end())
		{
			userDefineSnapObjType.erase(it);
		}
	}
}

 BOOL CViewControlData::getSnapPT(osg::Vec3d& resPT, const osg::Matrixd&VPW, const osg::Matrixd& InverseVPW)
{
	osg::Vec3d srcPT = resPT;

	osg::Vec3d snapNodeValue;
	BOOL nodeSnapValid = FALSE;

	//执行节点捕捉


	if (m_snapType &SNAP_NODE)
	{
		double dMin = 225;
		std::vector<CString> nodeTypes;	
		for (int i = 0; i < userDefineSnapObjType.size(); i++)
		{
			if (std::find(nodeTypes.begin(), nodeTypes.end(), userDefineSnapObjType[i]) == nodeTypes.end())
				nodeTypes.push_back(userDefineSnapObjType[i]);
		}
		
		std::vector<CDbObjInterface*> allObjects = FindObjByTypes(nodeTypes);
	 
		for (int i = 0; i < allObjects.size(); i++)
		{
			CDbObjInterface* pData = allObjects[i];

			if (!pData->GetVisible())
				continue;

			std::vector<osg::Vec3d> pts = pData->GetStretchPoint();
			for (int j = 0; j < pts.size(); j++)
			{
				osg::Vec3d pt = pts[j] * VPW;
				double dist = (pt - srcPT).length2();
 				if (dist < dMin)
				{
					dMin = dist;
					snapNodeValue = pts[j];
					nodeSnapValid = TRUE;
				}
			}
 		}

	}

	osg::Vec3d oriZero = localOrg;

	osg::Vec3d ptZero = oriZero * VPW;

	double dist = (ptZero - srcPT).length2();
	if (dist < 225)
	{
		snapNodeValue = oriZero;
		nodeSnapValid = TRUE; 
	}

	//中点捕捉
	osg::Vec3 snapMidValue;
	BOOL midSnapValid = FALSE;
	if (m_snapType &SNAP_MID)
	{
		double dMin = 225;

		std::vector<CString> nodeTypes;
		
		for (int i = 0; i < userDefineSnapObjType.size(); i++)
		{
			if (std::find(nodeTypes.begin(), nodeTypes.end(), userDefineSnapObjType[i]) == nodeTypes.end())
				nodeTypes.push_back(userDefineSnapObjType[i]);
		}

		for (int i = 0; i < userDefineSnapObjType.size(); i++)
		{
			if (std::find(nodeTypes.begin(), nodeTypes.end(), userDefineSnapObjType[i]) == nodeTypes.end())
				nodeTypes.push_back(userDefineSnapObjType[i]);
		}
 
		std::vector<CDbObjInterface*> allObjects = FindObjByTypes(nodeTypes);
  
		for (int i = 0; i < allObjects.size(); i++)
		{
			CDbObjInterface* pData = allObjects[i];

			if (!pData->GetVisible())
				continue;

			std::vector<osg::Vec3d> pts = pData->GetStretchPoint();
			if (pts.size() > 1)
			{
				int statIndex = 1;
				if (pData->IsClose())
					statIndex = 0;

				for (int j = statIndex; j < pts.size(); j++)
				{
					int pre = j - 1;
					if (pre < 0)
						pre = pts.size() - 1;
					osg::Vec3d pt = (pts[pre] + pts[j])*0.5*VPW;
					double dist = (pt - srcPT).length2();
 					if (dist < dMin)
					{
						dMin = dist;
						snapMidValue = (pts[pre] + pts[j])*0.5;
						midSnapValid = TRUE;
					}
				}
			}
 		}
	}

	osg::Vec3 snapJDValue;
	BOOL jdSnapValid = FALSE;
	if (m_snapType &SNAP_INTERSECTION) //交点扑捉，目前只支持辅助线之间的交点
	{
		double dMin = 225;
		std::vector<CString> nodeTypes;
		for (int i = 0; i < userDefineSnapObjType.size(); i++)
		{
			if (std::find(nodeTypes.begin(), nodeTypes.end(), userDefineSnapObjType[i]) == nodeTypes.end())
				nodeTypes.push_back(userDefineSnapObjType[i]);
		}
		std::vector<CDbObjInterface*> allObjects = FindObjByTypes(nodeTypes);
	
		for (int i = 0; i <allObjects.size(); i++)
		{
			CDbObjInterface* pData = allObjects[i];

			if (!pData->GetVisible())
				continue;

			for (int j = i + 1; j < allObjects.size(); j++)
			{
				CDbObjInterface* pOtherData = allObjects[j];

				if (!pData->GetVisible())
					continue;

				if (pOtherData->GetRTType() == pData->GetRTType())
				{
					std::vector<osg::Vec3d> crossPts;
					pData->GetCrossPoints(pOtherData, crossPts);

					for (int k = 0; k < crossPts.size(); k++)
					{
						osg::Vec3d pt = crossPts[k] * VPW;
						double dist = (pt - srcPT).length2();
 						if (dist < dMin)
						{
							dMin = dist;
							snapJDValue = crossPts[k];
							jdSnapValid = TRUE;
						}
					}
				}
			}
		}

		//辅助线单独处理
		for (int i = 0; i < m_IntersectionSnapPoints.size(); i++)
		{
			for (int j = 0; j < m_IntersectionSnapPoints[i].intersectionArr.size(); j++)
			{
				osg::Vec3d pt = m_IntersectionSnapPoints[i].intersectionArr[j] * VPW;
				double dist = (pt - srcPT).length2();
				if (dist < dMin)
				{
					dMin = dist;
					snapJDValue = m_IntersectionSnapPoints[i].intersectionArr[j];
					jdSnapValid = TRUE;
				}
			}
		}
	}


	


	osg::Vec3d snapPT = resPT*InverseVPW;

	//捕捉类型
	BOOL bSnapValid = TRUE;
	bUpdateSnap = FALSE;
	if (nodeSnapValid)
	{
		snapPT = snapNodeValue;
		snapValidType = SNAP_NODE;
	}
	else if (midSnapValid)
	{
		snapPT = snapMidValue;
		snapValidType = SNAP_MID;
	}
	else if (jdSnapValid)
	{
		snapPT = snapJDValue;
		snapValidType = SNAP_INTERSECTION;
	}
	else
	{
		bSnapValid = FALSE;
		bClearSnap = TRUE; 	
	}

	//捕捉位置，窗口坐标坐标
	if (bSnapValid)
	{
		bUpdateSnap = TRUE;
		snapPos = snapPT;//世界坐标 
		resPT = snapPT;
		return TRUE;
	}
	
	return FALSE;
}

CPoint CViewControlData::GetCursorPos()
{
	CPoint pt;
	::GetCursorPos(&pt); 

	if(mCView)
	{
		CRect viewRect;
		mCView->GetWindowRect(viewRect);
		pt.y -= viewRect.top;
		pt.x-=viewRect.left;
////////////////////////////
		osg::Viewport* pViewPort = mView->getCamera()->getViewport(); 
		if(pViewPort)
		{
			pt.x -= pViewPort->x();

			pt.y = viewRect.Height()- pt.y  - pViewPort->y();
		}
	}
	return pt;
}




BOOL CViewControlData::IsAddSelectEffect(osg::Node* pNode)
{
	if(!pNode)
		return FALSE;

	CString strName = pNode->getName().c_str();
	int nChild = 	SelectEffectRoot->getNumChildren();
	for(int i=0; i< nChild; i++)
	{
		if(SelectEffectRoot->getChild( i)->getName().c_str() == strName )
		{
			return TRUE;
		}
	}
	return FALSE;
}


void CViewControlData::RemoveSelectNodeEffect(osg::Node* pNode)
{
	//UNUSED
	
}

void   CViewControlData::removeSelectEffect( osg::Node* pNode)
{
	if(pNode)
	{
		CString strName = pNode->getName().c_str();
		int nCount  = SelectEffectRoot->getNumChildren();
		for(int i=0; i< nCount; i++)
		{
			osg::Node* pChild =  SelectEffectRoot->getChild(i);
			if (pChild && pChild->getName().c_str() == strName)
			{
				SelectEffectRoot->removeChild( pChild);
				break;
			}
		}	

	}	
}

void CViewControlData::addSelectEffect( osg::Node* pNode,osg::Vec4 color)
{
	if(IsAddSelectEffect( pNode ) )
		return;

	if(pNode)
	{
		osg::Node* pClone = osg::clone(pNode , osg::CopyOp::DEEP_COPY_ALL);

		osg::Group* pTotalEffect = new osg::Group;
		pTotalEffect->setName(pNode->getName() );

		//在上边覆盖点跨线， 渲染顺序和加入的先后有关，（当z相同时）？？  移除点划线？*****************2016-11-15
	

		//////先加入线宽	
		{
			SetLunKuoEffect(pNode, 3, color);
			pTotalEffect->addChild(pNode);
		}

		SelectEffectRoot->addChild( pTotalEffect);
	}
}

void CViewControlData::GetOffsetWindowSize(double& dx, double& dy)
{

}

void CViewControlData::GetWindowSize(double& dx, double& dy)
{
	if(mView )
	{
		osg::Viewport* pViewPort =  mView->getCamera() ->getViewport();
		if(pViewPort)
		{
			dx = pViewPort->width();
			dy = pViewPort->height();
		}
	}
}

void CViewControlData::BoxZoom(osg::Vec2 p1, osg::Vec2 p2)
{
 	if( mView)
	{	
		double left;
		double right;
		double bottom;
		double top;
		double zNear;
		double zFar;

		/////
		if (bParalled)
			mView->getCamera()->getProjectionMatrixAsOrtho(left,right,bottom,top,zNear,zFar );
		else
			mView->getCamera()->getProjectionMatrixAsFrustum(left, right, bottom, top, zNear, zFar);

		////

		double width  =  0.5*(right-left);
		double height = 0.5*(top-bottom);

		double cx  =  0.5*(right+left);
		double cy  =  0.5*(top+bottom);

		double dmaxx = max(p1.x() , p2.x() );
		double dminx  = min(p1.x() , p2.x());
		double dmaxy = max(p1.y() , p2.y() );
		double dminy = min(p1.y() , p2.y());

		double dx = abs(dmaxx-dminx)*0.5;
		double dy = abs(dmaxy-dminy)*0.5;
		double ccx = (dmaxx+dminx)*0.5;
		double ccy = (dmaxy+dminy)*0.5;
		if(dx > dy)
		{		
			dminy =  ccy - dx;
			dmaxy = ccy + dx;
		}
		else
		{	
			dminx =  ccx - dy;
			dmaxx = ccx + dy;
		}
		if (bParalled)
			mView->getCamera()->setProjectionMatrixAsOrtho(cx+dminx*width, cx+dmaxx*width, cy+dminy*height, cy+dmaxy*height,zNear,zFar  );
		else
			mView->getCamera()->setProjectionMatrixAsFrustum(cx + dminx*width, cx + dmaxx*width, cy + dminy*height, cy + dmaxy*height, zNear, zFar);

	}
}
 
CViewControlData::CViewControlData( ) 
{
	Init();
}

void CViewControlData::Init()
{
	axisSize = 400;
	cameraAnimation = false;

	m_operationFlag = 0;

	bNeedClearSelect = FALSE;

	bRotate = false;
	bCancle = false;
	bReforceUpdateCallBack = FALSE;

	unitFactor = 1;

	bfactor=false;

	m_bInstallValid = TRUE;

	pixelSize.set(1, 1, 1,1);

 	m_pUserData = NULL;
	m_pCurHandler = NULL;

	m_pPickModelHandler = NULL;
	bReDrawCount = 1;

	bNeedFullSceen = TRUE;
	MINRADIUS      = 1000;
	defaultRaidus  = 1000;
	bFirst=TRUE;
	bInit  =TRUE;

	bEnableSnap=FALSE;

	bClearSnap = FALSE;
	bUpdateSnap=FALSE;

	m_showType = OSG_TRUSS;
	bShowGrid = FALSE;
	girdFactor = 0.001;
	pTerrain = NULL;

	bNeedOptimize=FALSE;
	m_bOptimize = TRUE;

	bNeedChangeViewType=0;


	pointTexture = NULL;
	zzPtTexture = NULL;
	mgPtTexture = NULL;
	steelPtTexture = NULL;
	checkPtTexture = NULL;
	vwPtTexture = NULL;
	allignPtlTexture = NULL;

	textFontEn=NULL;
	textFontCn=NULL;

	m_worldMatrix.makeIdentity();

	optimizationOptions  =
		osgUtil::Optimizer::OptimizationOptions::FLATTEN_STATIC_TRANSFORMS |
		//		osgUtil::Optimizer::OptimizationOptions::REMOVE_REDUNDANT_NODES |
		osgUtil::Optimizer::OptimizationOptions::REMOVE_LOADED_PROXY_NODES |
		osgUtil::Optimizer::OptimizationOptions::COMBINE_ADJACENT_LODS |
		//	osgUtil::Optimizer::OptimizationOptions::SHARE_DUPLICATE_STATE |
		osgUtil::Optimizer::OptimizationOptions::MERGE_GEOMETRY |
		osgUtil::Optimizer::OptimizationOptions::MAKE_FAST_GEOMETRY |
		osgUtil::Optimizer::OptimizationOptions::CHECK_GEOMETRY |
		osgUtil::Optimizer::OptimizationOptions::OPTIMIZE_TEXTURE_SETTINGS |
		osgUtil::Optimizer::OptimizationOptions::STATIC_OBJECT_DETECTION;


	m_command =0;
	m_pCurHandler=NULL;
	m_pPickModelHandler = NULL;
	osgZ = 0;
	viewType =4;
	bShowGrid = FALSE;
	bShowAxis=FALSE	;
 
	mRoot = NULL;
	grids = NULL;

	backColor	=backColor;
	bParalled=TRUE;

 
	hudAxis = NULL;

	manipulatorType = 0;

	m_snapType = SNAP_NO;

	selectStatus=3;

	zoomFactor = 0.8;

	m_bOrtho=FALSE;

	m_pUserData = new CDbOSGDataCenter;
	m_pSelector = new CDbBoxSelector(this);

	mView = NULL;

	mViewer = NULL;
	m_Osg = NULL;
	mCView = NULL;

	backColor = osg::Vec4( 0.0,0.0,0.0,1.0);
	offsetBkColor = osg::Vec4(0.2, 0.2, 0.2, 1.0);	

	m_dOffset = 0;
	m_bOffset = false;


	InitCommandString();
} 

osg::Texture2D* CViewControlData::GetPointTexture(int ptType) //point texture
{
	std::string strTexturePath;
	if (ptType == 0)
	{
		strTexturePath = "FCP.bmp";
		return GetFunctionPtTexture(pointTexture, strTexturePath);
	}
	else if (ptType == 1)
	{
		strTexturePath = "ZZP.bmp";
		return GetFunctionPtTexture(zzPtTexture, strTexturePath);
	}
	else if (ptType == 2)
	{
		strTexturePath = "MGP.bmp";
		return GetFunctionPtTexture(mgPtTexture, strTexturePath);
	}
	else if (ptType == 3)
	{
		strTexturePath = "STP.bmp";
		return GetFunctionPtTexture(steelPtTexture, strTexturePath);
	}
	else if (ptType == 4)
	{
		strTexturePath = "CHP.bmp";
		return GetFunctionPtTexture(checkPtTexture, strTexturePath);
	}
	else if (ptType == 5)
	{
		strTexturePath = "VWP.bmp";
		return GetFunctionPtTexture(vwPtTexture, strTexturePath);
	}
	else if (ptType == 6)
	{
		strTexturePath = "ALP.bmp";
		return GetFunctionPtTexture(allignPtlTexture, strTexturePath);
	}
	return NULL;
}

osg::Texture2D* CViewControlData::GetFunctionPtTexture(osg::ref_ptr <osg::Texture2D>& pTextTure, const std::string& strTexturePath)
{
	if (pTextTure == NULL)
	{
		pTextTure = new osg::Texture2D;
	}
	if (pTextTure->getImage() == NULL)
	{
		CString strFullTexturePath = cdb_string_tools::GetApplicationSelfPath() +"res\\"+ strTexturePath.c_str();

		MakeTexture(TRUE, pTextTure.get(), strFullTexturePath, 2);
	}
	return pTextTure.get();
}
 
osgText::Font* CViewControlData::GetEnTextFont()
{
	if (!textFontEn)
	{
		strFontFileEn = OSGGetApplicationSelfPath() + "Fonts\\segoeuil.ttf";		
		textFontEn = osgText::readRefFontFile((LPCTSTR)strFontFileEn);
	}
	return textFontEn.get();
}
osgText::Font* CViewControlData::GetCnTextFont()
{
	if (!textFontCn)
	{
		strFontFileCn = OSGGetApplicationSelfPath() + "Fonts\\simfang.ttf";
		
		textFontCn = osgText::readRefFontFile((LPCTSTR)strFontFileCn);
	}
	return textFontCn.get();
 }

osgText::Font* CViewControlData::GetEnTextFontForColor(osg::Vec4 color)
{
	std::map<osg::Vec4, osg::ref_ptr<osgText::Font> >::iterator it = colorTextFontEnMap.find(color);
	if (it != colorTextFontEnMap.end())
	{
		return it->second.get();
	}
	else
	{
		CString strFontFileEn = OSGGetApplicationSelfPath() + "Fonts\\segoeuil.ttf";
		CString strFontFileEnTemp;

		strFontFileEnTemp.Format("%s%s_%d.ttf", OSGGetApplicationSelfPath(), "Fonts\\segoeuil", colorTextFontEnMap.size() + 1);

		CopyFile(strFontFileEn, strFontFileEnTemp, FALSE);

		osg::ref_ptr<osgText::Font> textFontEn = osgText::readRefFontFile((LPCTSTR)strFontFileEnTemp);
 		colorTextFontEnMap[color] = textFontEn;
		return textFontEn.get();
	}

}
osgText::Font* CViewControlData::GetCnTextFontForColor(osg::Vec4 color)
{
	std::map<osg::Vec4, osg::ref_ptr<osgText::Font> >::iterator it = colorTextFontCnMap.find(color);
	if (it != colorTextFontCnMap.end())
	{
		return it->second.get();
	}
	else
	{
		CString strFontFileCn = OSGGetApplicationSelfPath() + "Fonts\\simfang.ttf";
		CString strFontFileCnTemp;
		strFontFileCnTemp.Format("%s%s_%d.ttf", OSGGetApplicationSelfPath(), "Fonts\\simfang", colorTextFontCnMap.size() + 1);
		CopyFile(strFontFileCn, strFontFileCnTemp, FALSE);

		osg::ref_ptr<osgText::Font> textFontCn = osgText::readRefFontFile((LPCTSTR)strFontFileCnTemp);
 		colorTextFontCnMap[color] = textFontCn;
		return textFontCn.get();
	}
}


CViewControlData::~CViewControlData()
{
	delete m_pUserData;
	delete m_pSelector;
 	if(m_pCurHandler)
		delete m_pCurHandler;
}
 
int CViewControlData::GetSelectObjCount()
{
	return m_pSelector->GetSelectNodeCount();
}


CString CViewControlData::GetSelectNodeName()
{
	return m_pSelector->GetSelectNodeName();
}

void CViewControlData::SetHandler(CDbHandlerInterface* pHandler)
{
	if(pHandler)
	{
		pHandler->m_pOSG = this;
	}

	if( pHandler && pHandler == m_pCurHandler )
	{
		if( m_pCurHandler->status ==0 )
		{
			m_pCurHandler->begin();
		}
		m_command = 1;
		return;
	}
	if(m_pCurHandler)
	{
		if(m_command >=1 )
			m_pCurHandler->finish();

		delete m_pCurHandler;
	}

	m_pCurHandler = pHandler;
	CViewControlData::SetGlobalHandler(pHandler);

	if(!m_pCurHandler)
	{
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			GetDb3DCommandLine()->AddPrompt(_T(""));
		}
		
	}
	else if( m_pCurHandler->status ==0 )
	{
		m_command = 1;
		m_pCurHandler->begin();
	}
}

#include <osgDB/ReadFile>

BOOL CViewControlData::IsHeightLight(const CString& strObj)
{
	osg::Node* preHightLightObj = m_pPickModelHandler->GetHeightLightNode();
	if (preHightLightObj)
	{
		if (preHightLightObj->getName().c_str() == strObj)
		{
			return TRUE;
		}		 
	}
	return FALSE;
}

BOOL CViewControlData::IsSelected(const CString& strObj)
{
	return m_pSelector->IsSelected(strObj);
}
void CViewControlData::PostObjSelected(osg::Node* pNode)
{
}
void CViewControlData::PostObjDClick(osg::Node* pNode)
{

}

std::vector<CDbObjInterface*> CViewControlData::GetCurSelectObject()
{
	std::vector<CDbObjInterface*> selectDatas;

	std::map<CString, osg::ref_ptr<osg::Node>> selectSet;
	m_pSelector->getSelectSet(selectSet);

	std::map<CString, osg::ref_ptr<osg::Node>>::iterator it = selectSet.begin();
	for(; it != selectSet.end() ; it++)
	{
		osg::ref_ptr<osg::Node>  pNode = it->second;
		if(pNode)
		{
			CDbObjInterface* pData =  FindObj(   pNode->getName().c_str() , GetObjType(pNode  ) );
			if(pData)
			{
				selectDatas.push_back( pData);
			}
		}
	}
	return selectDatas;
}

int  CViewControlData::GetSelectHotPointCount()
{
	return m_pSelector->GetSelectHotPointCount();
}

void CViewControlData::PreStretch()
{

}

CDbObjInterface* CViewControlData::PreTrim(CDbObjInterface* pData)
{
	return pData;
}
void CViewControlData::PostTrimNew(CDbObjInterface* pData, const CString& strOldTitle)
{
}

std::vector<int> CViewControlData::GetStretchIndex(const CString& strObj)
{
	return m_pSelector->GetStretchIndex(strObj);
}


void CViewControlData::SetEnableSnap(BOOL flag)
{
	bEnableSnap = flag;
}


osg::Group* CViewControlData::GetDataParent(CDbObjInterface* pData)
{ 
	if (pData->GetRTType() == OT_DIM)
	{
		CDbDimObj* pDim = (CDbDimObj*)pData;

		osg::Node * pParent = GetChildByName(pDim->m_strParentObjName, dimObjs);
		if (pParent)
			return pParent->asGroup();
		else
		{
			osg::Group* pCreateNode = new osg::Group;
			dimObjs->addChild(pCreateNode);
			pCreateNode->setName(pDim->m_strParentObjName);
			return pCreateNode;
		}
	}	
	else
		return GraphicsGroup;
}


void CViewControlData::addTempObject(osg::Node* pNode )
{
	std::lock_guard<std::mutex> lock(m_AddTempMutex);
 
	if(pNode)
	{  
		for(int i=0; i< tempObjects.size(); i++)
		{
			if(pNode->getName() == tempObjects[i]->getName() )
			{
				tempObjects[i]=pNode;
				return;
			}
		}
		tempObjects.push_back( pNode);	 
	}
 }


void CViewControlData::clearTempObject(osg::Node* pNode )
{
	m_DelTempMutex.Lock();

	if(pNode)
	{	 
		for(int i=0; i< delTempObjects.size(); i++)
		{
			if(pNode== delTempObjects[i] )
			{
				return;
			}
		}
		delTempObjects.push_back(pNode);
	}

	m_DelTempMutex.Unlock();
}

void CViewControlData::addOverLayoutObject(osg::Node* pNode)
{
	m_AddOverLayoutMutex.Lock();
	overLayoutObjects.push_back( pNode);
	m_AddOverLayoutMutex.Unlock();
}
void CViewControlData::clearOverLayoutObject(osg::Node* pNode)
{
	m_DelOverLayoutMutex.Lock();
	delOverLayoutObjects.push_back( pNode);
	m_DelOverLayoutMutex.Unlock();
}

//包围框对象
void CViewControlData::addBoundingBoxObj(const CString& strObj)
{
	if( std::find( addBoundingBox.begin(), addBoundingBox.end() , strObj ) == addBoundingBox.end())
	{
		addBoundingBox.push_back( strObj);
	}
}

void CViewControlData::clearBoundingBoxObj(const CString& strObj)
{
	if( std::find( removeBoundingBox.begin(), removeBoundingBox.end() , strObj ) == removeBoundingBox.end())
	{
		removeBoundingBox.push_back( strObj);
	}
}


void CViewControlData::addDataOperation(const CString& strOperation,CDbObjInterface* pData, BOOL bUpdateDB)
{
	beginOperation(strOperation);

	addData(pData, bUpdateDB);

	endOperation();
}

void CViewControlData::addData(CDbObjInterface* pData, BOOL bUpdateDB)
{
	/*
	1.加入到数据中心中
	*/
	m_pUserData->AddObj(pData);

 	if (bUpdateDB)
	{
		pData->UpdateDBData(db_adddata);
	} 

	pData->UpdateAndAddToScene();
}



//需要根据数据刷新的对象
void CViewControlData::addObject(CDbObjInterface* pData, osg::Node* pNode)
{
	m_AddMutex.Lock();

	if(pNode )
	{
		std::map<CDbObjInterface* ,std::vector<osg::Node*> >::iterator it = addObjects.find(pData);
		if(it != addObjects.end() )
		{
			it->second.push_back(pNode);
		}
		else
		{
			std::vector<osg::Node*> data;
			data.push_back( pNode);
			addObjects[pData]=data;
		}
	}
	m_AddMutex.Unlock();
}

//需要根据数据刷新的对象
void CViewControlData::delObject( osg::Node* pNode)
{
	if(pNode)
	{
		m_DelMutex.Lock();

		if (std::find(delObjects2.begin(), delObjects2.end(), pNode) == delObjects2.end())
		{
			BOOL bChild = FALSE;

			for (int i = 0; i < delObjects2.size(); i++)
			{
				std::vector<osg::Node*> nodes;

				GetAllChildNode(nodes, delObjects2[i]);
				if (std::find(nodes.begin(), nodes.end(), pNode) != nodes.end())
				{
					bChild = TRUE;
					break;
				}
			}

			if (!bChild)
			{
				delObjects2.push_back(pNode);
			}
		}

		m_DelMutex.Unlock();
	}
}

void CViewControlData::addUpdateObject(CDbObjInterface* pData, osg::Node* pNode)
{
	m_UpdateMutex.Lock();

	if(pNode && pData)
	{
		std::map<CDbObjInterface* ,std::vector<osg::Node*> >::iterator it = updateObjects.find(pData);
		if(it != updateObjects.end() )
		{
			it->second.push_back(pNode);
		}
		else
		{
			std::vector<osg::Node*> data;
			data.push_back( pNode);
			updateObjects[pData]=data;
		}
	}
	m_UpdateMutex.Unlock();
} 

void CViewControlData::removeUpdateObject(CDbObjInterface* pData)
{
	m_UpdateMutex.Lock();

	if (pData)
	{
		std::map<CDbObjInterface*, std::vector<osg::Node*> >::iterator it = updateObjects.find(pData);
		if (it != updateObjects.end())
		{
			updateObjects.erase(it);
		}
	}
	m_UpdateMutex.Unlock();
}

//刷新标注
void CViewControlData::addUpdateDim(CDbObjInterface* pData)
{
	if (pData)
	{
		std::vector<CDbObjInterface* >::iterator it = std::find(updateDimObjects.begin(), updateDimObjects.end(), pData);
		if (it == updateDimObjects.end())
		{
			updateDimObjects.push_back(pData);
		}	 
	}
}

void CViewControlData::UpdateRubbox(osg::Vec2 p1, osg::Vec2 p3)
{
	updateBoxGeode(pRubBoxGeometry, p1, p3);
}

void CViewControlData::InitRubbox(osg::Vec2 p1, osg::Vec2 p3)
{
	if (!pRubBoxGeometry)
		return;	
	pRubBoxGeometry->setNodeMask(0xffffffff);

	if (pRubBoxCamera)
	{
		double dx, dy;
		GetWindowSize(dx, dy);
		osg::Camera* pCameraHud = dynamic_cast<osg::Camera*>(pRubBoxCamera);

		pCameraHud->setProjectionMatrix(osg::Matrix::ortho2D(0, dx, 0, dy));
	}

	UpdateRubbox(p1, p3);
}

void CViewControlData::SetRubboxVisible(BOOL bVisible)
{
	if (!pRubBoxGeometry)
		return;

	if (bVisible)
	{
		pRubBoxGeometry->setNodeMask(0xffffffff);
	}
	else
	{
		pRubBoxGeometry->setNodeMask(0x0);
	}
}



void CViewControlData::addRubbline(osg::Geometry* pGeomory, osg::Vec2 p1 , osg::Vec2 p2)
{	 
	rubbleLine line;
	line.pGeometry=pGeomory;
	line.p1 = p1;
	line.p2 = p2;
	rubbleline.push_back( line);
}
void CViewControlData::addRubbbox(osg::Geometry* pGeomory, osg::Vec2 p1 , osg::Vec2 p2)
{
	rubbleBox line;
	line.pGeometry=pGeomory;
	line.p1 = p1;
	line.p2 = p2;
	rubblebox.push_back( line);
}
void CViewControlData::addRubbCircle(osg::Geometry* pGeomory, osg::Vec2 p1, osg::Vec2 p2)
{
	rubbleBox line;
	line.pGeometry = pGeomory;
	line.p1 = p1;
	line.p2 = p2;
	rubbleCircle.push_back(line);
}
void CViewControlData::addRubbEllipse(osg::Geometry* pGeomory, osg::Vec2 p1, osg::Vec2 p2, osg::Vec2 p3)
{
	rubbleBox line;
	line.pGeometry = pGeomory;
	line.p1 = p1;
	line.p2 = p2;
	line.p3 = p3;
	rubbleEllipse.push_back(line);
 }


void CViewControlData::clearAllChild( osg::Group* pNode)
{
 }
//开始操作
void CViewControlData::beginOperation(const CString& strOperation)
{
}
//结束操作
void CViewControlData::endOperation()
{
}
//直接修改
void CViewControlData::doOperation(const CString& strOperation,CDbObjInterface* pData, BOOL bUpdateDB)
{
	beginOperation(strOperation);
 	updateData(pData,bUpdateDB);
	endOperation();
}

void CViewControlData::updateData(CDbObjInterface* pData, BOOL bUpdateDB)
{
	if( NULL == pData )
		return;

	//突然发现施工、运营的属性表不能连续改值，而建模、截面窗口则没有问题
	//发现bNeedClearSelect被建模和截面的派生类赋值=2，施工、运营没有重写过updateData函数
	//为什么不能在基类中把这个变量赋好呢，因为这个变量是基类的成员变量，派生类都不知道这个变量是什么时候加的
	//added by zmz 2016.07.14
	bNeedClearSelect = 2;

	//1.更新桥博对象数据 ,
	if(bUpdateDB )
	{
		pData->UpdateDBData(db_changedata); 
	}
	//2、执行修改操作；
	pData->DoUpdate();	
}
 

void CViewControlData::preDelData(std::vector<CDbObjInterface*>& delDatas )
{
	//0, 预处理 ，子对象也需要删除
	std::vector<CDbObjInterface*> allDelObjects;
	{
		for (int i = 0; i < delDatas.size(); i++)
		{
			CDbObjInterface* pData = delDatas[i];

			//先加入子对象
			std::vector<CDbObjInterface*> tempObjects = pData->GetSubObjList(FALSE);
			for (int j = 0; j < tempObjects.size(); j++)
			{
				if (std::find(allDelObjects.begin(), allDelObjects.end(), tempObjects[j]) == allDelObjects.end())
					allDelObjects.push_back(tempObjects[j]);
			}

			//是否是标注，如果是标注，需要判断标注父对象是否删除
			if (pData->GetRTType() == OT_DIM)
			{
				CDbDimObj* pDim = dynamic_cast<CDbDimObj*>(pData);
				if (pDim)
				{
					CDbObjInterface* pDimParent = pDim->GetParentObj();
					if (std::find(delDatas.begin(), delDatas.end(), pDimParent) == delDatas.end())
						allDelObjects.push_back(pData);
				}
			}
			else
			{
				allDelObjects.push_back(pData);
			}
 		}
	}
	delDatas = allDelObjects;
}

void CViewControlData::delData(std::vector<CDbObjInterface*> delDatas, BOOL bUpdateDB)
{
	if (bUpdateDB)
		beginOperation("删除对象");

	//预处理
	preDelData(delDatas);

	////////开始删除图形对象和其他
	std::vector<CString> objnames;
	for (int i = 0; i< delDatas.size(); i++)
	{
		CDbObjInterface* pData = delDatas[i];
		objnames.push_back(pData->GetName());

		pData->DeleteObjectNode();
	}

	//后处理删除
	postDelData(delDatas,bUpdateDB);

	if (bUpdateDB)
		endOperation();
}

void CViewControlData::postDelData(std::vector<CDbObjInterface*> delDatas, BOOL bUpdateDB )
{
	for (int i = 0; i < delDatas.size(); i++)
	{
		CDbObjInterface* pData = delDatas[i];
		if (bUpdateDB)
			pData->UpdateDBData(db_deldata);

		m_pUserData->DeleteObj(pData->GetName(), pData->GetRTType());
	}
}
//把Data从应用程序数据库中删除 ，
void CViewControlData::ImplementDelDatas(std::vector<CString>& objNames)
{
 
}


void CViewControlData::OnUpdateDBObject(const CString &strObjectPath, int modifyType, const std::vector<CString> &vecVariable)
{
}

/*
设置当前光标
*/
void CViewControlData::SetCursorType(int type)
{
	if(m_Osg)
	{
		m_Osg->SetCursorType(type);
	}
}

void  CViewControlData::SetBehavorType(int type)
{
	manipulatorType = type;
	if(manipulatorType ==1 )
	{
		m_Osg->SetCursorType(OSG_SELECT_PAN);
	}
	if(manipulatorType ==2 )
	{
	 
 		m_Osg->SetCursorType(OSG_SELECT_ROTATE);
	}
	if(manipulatorType ==3 )
	{
		m_Osg->SetCursorType(OSG_SELECT_ZOOM);
	}
	if(manipulatorType ==0 )
	{
		m_Osg->SetCursorType(OSG_NORMAL_CUSSOR);
	}
}
int    CViewControlData::DoGetBehavorType()
{
	return manipulatorType;
} 
 

void CViewControlData::UpdateGrid()
{
	if (mView)
	{
		osg::Camera* camera = mView->getCamera();
		osg::Matrix viewMatrix = camera->getViewMatrix();
		osg::Matrix projectionMatrix = camera->getProjectionMatrix();
		osg::Viewport* pViewPort = camera->getViewport();
		pixelSize = osg::CullingSet::computePixelSizeVector(*pViewPort, projectionMatrix, viewMatrix);

		if (!bShowGrid)
			return;
		if (DoGetBehavorType() == 1 || DoGetBehavorType() == 2)
			return;
		
		double dx = 0;
		double dy = 0;
		GetWindowSize(dx, dy);

		osg::Vec3 center = getWorldPos(osg::Vec2(0.5*dx, 0.5*dy), FALSE);

		osg::Vec3 pt1 = getWorldPos(0, 0, FALSE);
		osg::Vec3 pt2 = getWorldPos(dx, dy, FALSE);

		double worldL = abs(pt1.x() - pt2.x());
		double worldW = abs(pt1.y() - pt2.y());

		double kk = girdFactor;
		//
		double dw = pixelSize.w();

		int xx = log10(dw / kk);

		int newUnit = T_setup.unit;
		int newCount = T_setup.count;
		osg::Vec3 newOrigion = T_setup.origion;

		newUnit = pow(5, xx);
		newCount = 5 + 0.5*max(worldL, worldW) / newUnit;
		while (newCount > 100)
		{
			xx++;
			newUnit = pow(10.0, xx);
			newCount = 5 + 0.5*max(worldL, worldW) / newUnit;
		}
		if (newCount % 5)
			newCount = 5*(newCount / 5 + 1);
		newOrigion = center;

		double ZDenpth = osgZ;
		if (viewType < 4)
			ZDenpth = -999;

		double precision = cdb_compare_tools::get_precision();
		cdb_compare_tools::set_precision(1.0e-4);
		if (newUnit != T_setup.unit || newCount != T_setup.count
			|| cdb_compare_tools::is_not_equal(newOrigion.x(), T_setup.origion.x())
			|| cdb_compare_tools::is_not_equal(newOrigion.y(), T_setup.origion.y())
			|| cdb_compare_tools::is_not_equal(newOrigion.z(), T_setup.origion.z()))

		{
			T_setup.unit = newUnit;
			T_setup.count = newCount;
			T_setup.origion = newOrigion;

			T_setup.LoadGrid(grids.get(), 1, osgZ);

		}
		cdb_compare_tools::set_precision(precision);
		
	}
}
void CViewControlData::AddObjectNow()
{
	m_AddMutex.Lock();

	if (addObjects.size() > 0)
	{
 		std::map<CDbObjInterface*, std::vector<osg::Node*> >::iterator it = addObjects.begin();
		for (; it != addObjects.end(); it++)
		{
			for (int i = 0; i < it->second.size(); i++)
			{
				osg::Group* pParent = GetDataParent(it->first);
				if (pParent)
				{ 
 					pParent->addChild(it->second[i]);
				}			
			}
		}
		addObjects.clear();
		GraphicsGroup->dirtyBound();
	}
	m_AddMutex.Unlock();
}

void CViewControlData::DeleteObjectNow()
{

	m_DelMutex.Lock();

	////////////new method
	for (int i = 0; i < delObjects2.size(); i++)
	{
		int  nParentCount = delObjects2[i]->getNumParents();
		if (nParentCount > 0)
		{
			osg::Group* pParent = delObjects2[i]->getParent(0);
			if (pParent)
			{
				pParent->removeChild(delObjects2[i]);
			}
		}

	}
	delObjects2.clear();

	m_DelMutex.Unlock();
}

void CViewControlData::DoUpdateCallBack()
{
 
	int nCount = 0;

	//是否重新计算包围框
	BOOL bDirtyBound = FALSE;

	//是否接触阻塞
	BOOL bRedraw = FALSE;
 

	//update grid
	UpdateGrid(); 

 

	//update snap;
	if(bClearSnap)
	{
		snapGraph->removeChildren(0, snapGraph->getNumChildren() );
		bClearSnap = FALSE;
		bRedraw          = TRUE;
	}
	if(bUpdateSnap && bEnableSnap)
	{
		UpdateSnaper(snapValidType , snapPos , greenColor, snapGraph.get() );
		bUpdateSnap=FALSE;
		bRedraw          = TRUE;
	}	
	//先刷新,需要刷新的对象
	{
		m_UpdateMutex.Lock();

		std::map<CDbObjInterface* , std::vector<osg::Node*> >::iterator it = updateObjects.begin();
		for( ; it != updateObjects.end() ; it++)
		{
			if (it->second.size() > 1)
			{
				TRACE("updateObjects\n");
			}
			for(int i=0; i< it->second.size() ; i++ )
			{
				nCount++;
				it->first->OnUpdate(it->second[i] );

				ClearHandlers( it->second[i]);

				//**********
				/*	if( m_pCurHandler&&  m_pCurHandler->strName == "stretch")
					{
					AddHandlers( it->second[i]);
					}*/
				bDirtyBound = TRUE;
				bRedraw          = TRUE;
			}	
		}
		updateObjects.clear();

		m_UpdateMutex.Unlock();
	}


	//先刷新,需要刷新的对象
	{
		std::vector<CDbObjInterface* >::iterator it = updateDimObjects.begin();
		for (int i = 0; i < updateDimObjects.size(); i++)
		{
			updateDimObjects[i]->RemoVeDimNodes();
			updateDimObjects[i]->AddDimNodes();
		}
		updateDimObjects.clear();
	}


	//删除临时对象overlayout对象

	m_DelOverLayoutMutex.Lock();
	for(int i=0; i< delOverLayoutObjects.size(); i++)
	{
 		bool flag = tempObjs->removeChild(delOverLayoutObjects[i]);
		bRedraw          = TRUE;
	}		
	delOverLayoutObjects.clear();		
	m_DelOverLayoutMutex.Unlock();

	//添加临时对象overlayout对象
	m_AddOverLayoutMutex.Lock();
	for(int i=0; i< overLayoutObjects.size(); i++)
	{		
 		bool flag = tempObjs->addChild(overLayoutObjects[i]);
		bRedraw          = TRUE;
	}	
	overLayoutObjects.clear();
	m_AddOverLayoutMutex.Unlock();

	//删除临时对象

	m_DelTempMutex.Lock();
	for(int i=0; i< delTempObjects.size(); i++)
	{
		bRedraw          = TRUE;
		bool flag = tempObjs->removeChild( delTempObjects[i]);
		if(!flag)
		{
			ASSERT(FALSE);
		}	
	}		
	delTempObjects.clear();	

	m_DelTempMutex.Unlock();

	//添加临时对象
	{
		std::lock_guard<std::mutex> lock(m_AddTempMutex);
 		for (int i = 0; i < tempObjects.size(); i++)
		{
			bRedraw = TRUE;
			bool flag = true;
			int nCount = tempObjs->getNumChildren();
			for (int j = 0; j < nCount; j++)
			{
				if (tempObjs->getChild(j) == tempObjects[i])
				{
					flag = false;
					break;
				}
			}
			if (flag)
			{
				nCount++;
				bool flag2 = tempObjs->addChild(tempObjects[i]);
				if (!flag2)
				{
					ASSERT(FALSE);
				}

			}
			else
			{
				ASSERT(FALSE);
			}
		}
		tempObjects.clear();
 	}

	//////////////////////////包围框对象,需要在删除对象之前调用 ；
	for(int i=0; i< removeBoundingBox.size(); i++)
	{
		bRedraw          = TRUE;
		CString strName = removeBoundingBox[i];
		if (m_pPickModelHandler)
		{
			osg::ref_ptr<osg::Node>  selectetNode = GetChildByName(strName, SelectEffectRoot);
			if (selectetNode)
			{
				m_pPickModelHandler->removeBoundary(selectetNode);
			}
			else
			{
				osg::ref_ptr<osg::Node>  selectetNode = GetChildByName(strName, dimObjs);
				if (selectetNode)
				{
					m_pPickModelHandler->removeBoundary(selectetNode);
				}
			}
		}
	}
	removeBoundingBox.clear(); 

	
	DeleteObjectNow();

	//添加对象
	if(addObjects.size() > 0 )
	{
		m_AddMutex.Lock();

		bRedraw          = TRUE;
		std::map<CDbObjInterface*,std::vector<osg::Node*> >::iterator it = addObjects.begin();
		for(; it != addObjects.end(); it++)
		{		
			CDbObjInterface* pAddObj = it->first;

			for(int i=0; i< it->second.size(); i++)
			{
				osg::Group* pParent = GetDataParent(pAddObj);
				if(pParent)
				{
					bDirtyBound=TRUE;

					nCount++;
					pParent->addChild( it->second[i]);
				}				
			}
		}
		addObjects.clear();	
		GraphicsGroup->dirtyBound();

		m_AddMutex.Unlock();
	}

	//需要在添加对象之后调用，每次处理1000个对象的选中
	int nBoundingCount = addBoundingBox.size(); 
	{
 		std::vector<CString> addBoundingBoxLeft;
		for (int i = 0; i < nBoundingCount; i++)
		{
			CString strName = addBoundingBox[i];
			if (i < 1000)
			{
				if (m_pPickModelHandler)
				{
					osg::ref_ptr<osg::Node> SelectedNode = m_pSelector->GetLastSelectNode(strName);
					if (SelectedNode)
					{
						m_pPickModelHandler->showBoundary(SelectedNode);
					}
				}
			}
			else
			{
				addBoundingBoxLeft.push_back(strName);
			}		
		}
		addBoundingBox = addBoundingBoxLeft;
	}
	/////////////////橡皮筋对象 ，包括矩形，直线
	for(int i=0; i< rubbleline.size(); i++)
	{
		bRedraw          = TRUE;
		updateLineGeode(rubbleline[i].pGeometry ,rubbleline[i].p1,rubbleline[i].p2);	
	}
	rubbleline.clear();

	for(int i=0; i< rubblebox.size(); i++)
	{
 		bRedraw          = TRUE;
		updateBoxGeode( rubblebox[i].pGeometry ,rubblebox[i].p1,rubblebox[i].p2);	
	}
	rubblebox.clear();

	for (int i = 0; i < rubbleCircle.size(); i++)
	{
		bRedraw = TRUE;
		updateCircleGeode(rubbleCircle[i].pGeometry, rubbleCircle[i].p1, rubbleCircle[i].p2);
 	}
	rubbleCircle.clear();

	for (int i = 0; i < rubbleEllipse.size(); i++)
	{
		bRedraw = TRUE;
		updateEllipseGeode(rubbleEllipse[i].pGeometry, rubbleEllipse[i].p1, rubbleEllipse[i].p2, rubbleEllipse[i].p3);
	}
	rubbleEllipse.clear();

	if (needSelectObjects.size() > 0 )
	{
		std::vector<CString> tempObjs;
		for (int i = 0; i < needSelectObjects.size(); i++)
		{
			CDbObjInterface* pLastObj = FindObj(needSelectObjects[i], OT_ALL);
			if (pLastObj)
			{
				osg::Group* pParent = GetDataParent(pLastObj);
				if (pParent)
				{
					std::vector<osg::Node*> nodeVec;
					osg::Node * pNode = GetChildByName(needSelectObjects[i], pParent);
					if (pNode)
					{
						nodeVec.push_back(pNode);
						MakeObjectSelected(nodeVec, this);
					}
					else
					{
						tempObjs.push_back(needSelectObjects[i]);
					}
				}
			}		
		}
		needSelectObjects = tempObjs;
	}
	//
	if( bNeedClearSelect)
	{	
		CString  lastSelectObj;

		if (bNeedClearSelect ==2)
			lastSelectObj = m_pSelector->GetSelectNodeName();

		std::map<CString, osg::ref_ptr<osg::Node> >::iterator it = m_pSelector->selectetSet.begin();
		for (; it != m_pSelector->selectetSet.end(); it++)
		{
			osg::ref_ptr<osg::Node> pSelectNode = it->second;
			if (m_pPickModelHandler)
			{
				m_pPickModelHandler->removeBoundary(pSelectNode);
			}
		}
		m_pSelector->selectetSet.clear();
		m_pSelector->selectetSetBak.clear();


		//最后清除所有临时对象
		tempObjs->removeChildren(0, tempObjs->getNumChildren() ) ;
		StretchHandlers->removeChildren(0, StretchHandlers->getNumChildren());
		HeightLightEffectRoot->removeChildren(0, HeightLightEffectRoot->getNumChildren());

		SelectEffectRoot->removeChildren(0, SelectEffectRoot->getNumChildren());	

		if (!lastSelectObj.IsEmpty() )
		{
			needSelectObjects.clear();
			needSelectObjects.push_back(lastSelectObj);			
		}
		else/* if(bNeedClearSelect ==1)*/
		{
			//refacor by wr 
		/*	property_grid_interface* pGrid = get_property_grid_instance();
			if (pGrid)
			{
				pGrid->clear();
			}*/
		}

		bNeedClearSelect = FALSE;

	}

	//最后删除克隆出来的临时对象
	for(int i=0; i< delCloneObjes.size(); i++)
	{
		delete delCloneObjes[i];
	}
	delCloneObjes.clear(); 

	if( bNeedOptimize)
	{
		//bNeedOptimize = FALSE;
		//if (m_bOptimize)//最大优化
		//{
		//	osgUtil::Optimizer optimizer;
		//	optimizer.optimize(mRoot, osgUtil::Optimizer::OptimizationOptions::ALL_OPTIMIZATIONS);
		//}
		//else//最小优化
		//{
		//	osgUtil::Optimizer optimizer;
		//	optimizer.optimize(mRoot, optimizationOptions);
		//}
	}

	if(bDirtyBound)
	{
		GraphicsGroup->dirtyBound();
	} 
	if (bNeedChangeViewType>0)
	{
		bRedraw = true;
		ChangeViewType();	
 	}	

	if (bNeedFullSceen )
	{	
		FullSceenImpl();
		bNeedFullSceen--;
	}
	if (m_bNeedChangeShowType)
	{
		m_bNeedChangeShowType = FALSE;
		OnChangeShowType();
	}	
	if (cameraAnimation)
		OnCameraAnimation(); 
}


//point窗口坐标 ，VPW：世界转窗口,resPt窗口坐标
BOOL CViewControlData::OrthoPoint(osg::Vec3d& resPt, osg::Vec3d &point, const osg::Matrixd& VPW, const osg::Matrixd& InverseVPW)
{	
	if (m_pCurHandler == NULL)
		return FALSE;

	resPt = point;
  
	osg::Vec3d lastPoint = m_pCurHandler->lastConfirmPoint*VPW; //窗口坐标
	if (viewType > 0 && viewType < 4 || viewType >4 && viewType < 8 )
	{
		if (m_pCurHandler && m_pCurHandler->lastConfirmPointBeValid)
		{
			double dx = abs(resPt.x() - lastPoint.x());
			double dy = abs(resPt.y() - lastPoint.y());
			if (dx > dy)
			{
				resPt[1] = lastPoint[1];
			}
			else
			{
				resPt[0] = lastPoint[0];
			}
			return TRUE;
		}
 	}
	else
	{ 	 
		//if (m_pCurHandler && m_pCurHandler->lastConfirmPointBeValid)
		//{
		//	osgManipulator::PointerInfo _pointer;
		//	_pointer.setCamera(mView->getCamera());
		//	_pointer.setMousePosition(lastPoint.x(), lastPoint.y());

		//	osg::ref_ptr< osgManipulator::LineProjector >   _projector = NULL;

		//	osg::Vec3d zero = osg::Vec3(0, 0, 0)*VPW;
		//	osg::Vec3d wndX = osg::Vec3(1, 0, 0)*VPW;
		//	osg::Vec3d wndY = osg::Vec3(0, 1, 0)*VPW;
		//	osg::Vec3d wndZ = osg::Vec3(0, 0, 1)*VPW;

		//	double dx = abs(resPt.x() - lastPoint.x());
		//	double dy = abs(resPt.y() - lastPoint.y());

		//	CLine2 line(resPt.x(), resPt.y(), lastPoint.x(), lastPoint.y());			 

		//	CLine2 lineX(wndX.x(), wndX.y(), zero.x(), zero.y() );
		//	CLine2 lineY(wndY.x(), wndY.y(), zero.x(), zero.y());
		//	CLine2 lineZ(wndZ.x(), wndZ.y(), zero.x(), zero.y());

		//	double angleX0 = (line.Angle(lineX));
		//	double angleX = abs(angleX0);
		//	if (angleX > M_PI_2)
		//		angleX = M_PI - angleX;

		//	double angleY0 = (line.Angle(lineY));
		//	double angleY = abs(angleY0);
		//	if (angleY > M_PI_2)
		//		angleY = M_PI - angleY;

		//	double angleZ0 = (line.Angle(lineZ));
		//	double angleZ = abs(angleZ0);
		//	if (angleZ > M_PI_2)
		//		angleZ = M_PI - angleZ;

		//	if (lineX.Length() < 1.0e-6)
		//		angleX = M_PI_2;
		//	if (lineY.Length() < 1.0e-6)
		//		angleY = M_PI_2;
		//	if (lineZ.Length() < 1.0e-6)
		//		angleZ = M_PI_2;

		//	if (angleX > angleY  )
		//	{
		//		if (angleY > angleZ)  //Z方向
		//		{
		//			 _projector = new osgManipulator::LineProjector(osg::Vec3d(0,0,0) , osg::Z_AXIS);				 
 	//			}
		//		else //Y方向
		//		{
		//			_projector = new osgManipulator::LineProjector(osg::Vec3d(0, 0, 0), osg::Y_AXIS);
		//		}
		//	}
		//	else
		//	{
		//		if (angleX > angleZ) //Z方向
		//		{
		//			_projector = new osgManipulator::LineProjector(osg::Vec3(0, 0, 0), osg::Z_AXIS);
 	//			}
		//		else //X方向
		//		{
		//			_projector = new osgManipulator::LineProjector(osg::Vec3(0, 0, 0), osg::X_AXIS);
		//		}
		//	}	 
		//	
		//	/////////////////////
		//	if (_projector)
		//	{
		//		osg::NodePath nodePathToRoot;
		//		osgManipulator::computeNodePathToRoot(*validGraph, nodePathToRoot);
		//		osg::Matrix localToWorld = osg::computeLocalToWorld(nodePathToRoot);
		//		_projector->setLocalToWorld(localToWorld);
		//	
		//		osg::Vec3d projectedPoint1;
		//		_projector->project(_pointer, projectedPoint1);

		//		_pointer.setMousePosition(point.x(), point.y());

		//		osg::Vec3d projectedPoint2;
		//		_projector->project(_pointer, projectedPoint2);

		//		resPt = m_pCurHandler->lastConfirmPoint + projectedPoint2 - projectedPoint1;
		//		return 2;
		//	}	
		//}

		//if (viewType==10 &&m_pCurHandler && m_pCurHandler->lastConfirmPointBeValid)//用户自定义坐标系需要使用世界坐标系
		//{
		//	osgManipulator::PointerInfo _pointer;
		//	_pointer.setCamera(mView->getCamera());
		//	_pointer.setMousePosition(lastPoint.x(), lastPoint.y());

		//	osg::ref_ptr< osgManipulator::LineProjector >   _projector = NULL;

		//	osg::Vec3d zero    = localOrg*VPW;
		//	osg::Vec3d wndX   = localXDir*VPW;
		//	osg::Vec3d wndY   = localYDir*VPW;
		//	osg::Vec3d tempZ = localXDir^localYDir;
		//	tempZ.normalize();
		//	osg::Vec3d wndZ = tempZ*VPW;

		//	double dx = abs(resPt.x() - lastPoint.x());
		//	double dy = abs(resPt.y() - lastPoint.y());

		//	CLine2 line(resPt.x(), resPt.y(), lastPoint.x(), lastPoint.y());

		//	CLine2 lineX(wndX.x(), wndX.y(), zero.x(), zero.y());
		//	CLine2 lineY(wndY.x(), wndY.y(), zero.x(), zero.y());
		//	CLine2 lineZ(wndZ.x(), wndZ.y(), zero.x(), zero.y());

		//	double angleX0 = (line.Angle(lineX));
		//	double angleX = abs(angleX0);
		//	if (angleX > M_PI_2)
		//		angleX = M_PI - angleX;

		//	double angleY0 = (line.Angle(lineY));
		//	double angleY = abs(angleY0);
		//	if (angleY > M_PI_2)
		//		angleY = M_PI - angleY;

		//	double angleZ0 = (line.Angle(lineZ));
		//	double angleZ = abs(angleZ0);
		//	if (angleZ > M_PI_2)
		//		angleZ = M_PI - angleZ;

		//	if (lineX.Length() < 1.0e-6)
		//		angleX = M_PI_2;
		//	if (lineY.Length() < 1.0e-6)
		//		angleY = M_PI_2;
		//	if (lineZ.Length() < 1.0e-6)
		//		angleZ = M_PI_2;

		//	if (angleX > angleY)
		//	{
		//		if (angleY > angleZ)  //Z方向
		//		{
		//			_projector = new osgManipulator::LineProjector(localOrg, tempZ);
		//		}
		//		else //Y方向
		//		{
		//			_projector = new osgManipulator::LineProjector(localOrg, localYDir);
		//		}
		//	}
		//	else
		//	{
		//		if (angleX > angleZ) //Z方向
		//		{
		//			_projector = new osgManipulator::LineProjector(localOrg, tempZ);
		//		}
		//		else //X方向
		//		{
		//			_projector = new osgManipulator::LineProjector(localOrg, localXDir);
		//		}
		//	}

		//	/////////////////////
		//	if (_projector)
		//	{
		//		osg::NodePath nodePathToRoot;
		//		osgManipulator::computeNodePathToRoot(*validGraph, nodePathToRoot);
		//		osg::Matrix localToWorld = osg::computeLocalToWorld(nodePathToRoot);
		//		_projector->setLocalToWorld(localToWorld);

		//		osg::Vec3d projectedPoint1;
		//		_projector->project(_pointer, projectedPoint1);

		//		_pointer.setMousePosition(point.x(), point.y());

		//		osg::Vec3d projectedPoint2;
		//		_projector->project(_pointer, projectedPoint2);

		//		resPt = m_pCurHandler->lastConfirmPoint + projectedPoint2 - projectedPoint1;
		//		return 2;
		//	}
		//}
	}
	return FALSE;
}

void CViewControlData::SetBackColor(const osg::Vec4& color)
{
	backColor = color;
	osg::Camera* camera            = mView->getCamera();
	if(camera)
	{
		camera->setClearColor(backColor);
	}
}

void CViewControlData::SetOffsetViewBkColor(const osg::Vec4& color)
{
	offsetBkColor = color;
}

//快捷键
bool CViewControlData::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_F6)
	{
		SetBehavorType(1);
	}
	else if(nChar == VK_F5)
	{
		SetBehavorType(2);
	}
	else if(nChar == VK_F7)
	{
		SetBehavorType(0);
	}
	else if(nChar == VK_F8)
	{
		m_bOrtho= !m_bOrtho;
	}	
	else if(nChar == VK_F11)
	{
		osg::Vec4 oldBackColor = backColor;
		SetBackColor(osg::Vec4( 1.0f-oldBackColor.x() ,1.0f-oldBackColor.y(), 1.0f-oldBackColor.z() ,1.0f) );	
	}
	else if (nChar == VK_F12)
	{
		//SetbOffset(!m_bOffset);
	}
	else if (nChar == VK_LEFT && IsCTRLpressed() )
	{
		CDb3DManipulator* pManipulator = (CDb3DManipulator*)mView->getCameraManipulator();
		pManipulator->DoRotate(10, 1, 0);
	}
	else if (nChar == VK_LEFT && IsSHIFTpressed())
	{
		CDb3DManipulator* pManipulator = (CDb3DManipulator*)mView->getCameraManipulator();
		pManipulator->DoRotate(10, 0.1, 0);
	}
	else if (nChar == VK_RIGHT && IsCTRLpressed() )
	{
		CDb3DManipulator* pManipulator = (CDb3DManipulator*)mView->getCameraManipulator();
		pManipulator->DoRotate(10, -1, 0);
	}
	else if (nChar == VK_RIGHT && IsSHIFTpressed())
	{
		CDb3DManipulator* pManipulator = (CDb3DManipulator*)mView->getCameraManipulator();
		pManipulator->DoRotate(10, -0.1, 0);
	}
	else if (nChar == VK_UP && IsCTRLpressed())
	{
		CDb3DManipulator* pManipulator = (CDb3DManipulator*)mView->getCameraManipulator();
		pManipulator->DoRotate(10, 0, 1);
	}
	else if (nChar == VK_UP && IsSHIFTpressed())
	{
		CDb3DManipulator* pManipulator = (CDb3DManipulator*)mView->getCameraManipulator();
		pManipulator->DoRotate(10, 0, 0.1);
	}
	else if (nChar == VK_DOWN && IsCTRLpressed() )
	{
		CDb3DManipulator* pManipulator = (CDb3DManipulator*)mView->getCameraManipulator();
		pManipulator->DoRotate(10, 0, -1);
	}
	else if (nChar == VK_DOWN && IsSHIFTpressed())
	{
		CDb3DManipulator* pManipulator = (CDb3DManipulator*)mView->getCameraManipulator();
		pManipulator->DoRotate(10, 0, -0.1);
	}
	else
	{
		return false;
	}
	return true;
}
 
 
//选择是否清除上次的选择结果
bool CViewControlData::SetSelectFlag(bool bClearPreSel)
{
	if (m_pPickModelHandler)
	{
		bool oldFlag = m_pPickModelHandler->bclearPre;

		m_pPickModelHandler->bclearPre = bClearPreSel;
		return oldFlag;
	}
	 return true; 
}



std::vector<CDbObjInterface*> CViewControlData::OnPreCopy(const std::map<osg::Node*, CDbObjInterface*>& selectetSetMap)
{
	std::vector<CDbObjInterface*> realCopyObjs;

	std::map<osg::Node*, CDbObjInterface*>::const_iterator it = selectetSetMap.begin();
	for (; it != selectetSetMap.end(); it++)
	{
		CDbObjInterface* pData = it->second;
		realCopyObjs.push_back(pData);
	}

	return realCopyObjs;
}

CView* CViewControlData::GetCurrentCWnd()
{
	return mCView;
}
void CViewControlData::SetReDraw(BOOL flag)
{
	if(flag)
		bReDrawCount++;
	else
		bReDrawCount--;
}

std::vector<osg::Node*> CViewControlData::BoxPick(float mx0,float my0,float mx1,float my1,osgViewer::View* viewer,BOOL bReserveSelect)
{  
	osg::Node* node     = 0;
	osg::Group* parent = 0;

	std::vector<osg::Node*	> allSelects;

	double minX = 0;
	double maxX = 0;
	double minY = 0;
	double maxY = 0;
///////////////////
	if (mx0 > mx1)
	{
		minX = mx1;
		maxX = mx0;
	}
	else
	{
		minX = mx0;
		maxX = mx1;
	}
	if (my0 > my1)
	{
		minY = my1;
		maxY = my0;
	}
	else
	{
		minY = my0;
		maxY = my1;
	}

	DWORD t1 = GetTickCount();	 
	osg::Node* scene = viewer->getSceneData();
	if (!scene) return allSelects;

 	osg::Viewport* viewport = viewer->getCamera()->getViewport();
	{
		FastPolytopeIntersector* picker = new FastPolytopeIntersector(osgUtil::Intersector::WINDOW, minX, minY, maxX, maxY);
		picker->bReserve = bReserveSelect;
		MyPickVisitor iv(picker);

		if (bReserveSelect)
		{
 			viewer->getCamera()->accept(iv);
			if (picker->containsIntersections())
			{
				FastPolytopeIntersector::Intersections osgIntersection = picker->getIntersections();
				FastPolytopeIntersector::Intersections::iterator it = osgIntersection.begin();
				for (; it != osgIntersection.end(); it++)
				{
					FastPolytopeIntersector::Intersection intersection = *it;
					osg::NodePath& nodePath = intersection.nodePath;
					node = (nodePath.size() >= 1) ? nodePath[nodePath.size() - 1] : 0;
					if (node)
					{
						if (m_pSelector->ApplySelect(node,TRUE))
						{
							if (std::find(allSelects.begin(), allSelects.end(), node) == allSelects.end())
								allSelects.push_back(node);
						}
						else
						{
							osg::Node* pParent = node->getParent(0);
							if (pParent&&  m_pSelector->ApplySelect(pParent, TRUE))
							{
								if (std::find(allSelects.begin(), allSelects.end(), pParent) == allSelects.end())
									allSelects.push_back(pParent);
							}
						}
					}
				}		 
			}
		}
		else
		{
			std::vector<std::pair<osg::Node*, int> > allSelectNodePair;
			viewer->getCamera()->accept(iv);
			if (picker->containsIntersections())
			{
				FastPolytopeIntersector::Intersections osgIntersection = picker->getIntersections();
				FastPolytopeIntersector::Intersections::iterator it = osgIntersection.begin();
				for (; it != osgIntersection.end(); it++)
				{
					FastPolytopeIntersector::Intersection intersection = *it;
					osg::NodePath& nodePath = intersection.nodePath;

					node = (nodePath.size() >= 1) ? nodePath[nodePath.size() - 1] : 0;
					if (node)
					{
						bool bfind = false;
						for (int i = 0; i < allSelectNodePair.size(); i++)
						{
							if (allSelectNodePair[i].first == node)
							{
								bfind = true;
								allSelectNodePair[i].second++;
								break;
							}
						}
						if (!bfind)
						{
							allSelectNodePair.push_back(std::make_pair(node, 1));
						}
					}
				}

				for (int i = 0; i < allSelectNodePair.size(); i++)
				{
					osg::Node* node = allSelectNodePair[i].first;

					osg::Geode* pGeode = node->asGeode();
					if (pGeode)
					{
						int nNumDrawables = pGeode->getNumDrawables();
						for (int j = 0; j < nNumDrawables; j++)
						{
							if (pGeode->getChild(j)->getNodeMask() == 0x0)
								nNumDrawables--;
						}

						if (nNumDrawables <= allSelectNodePair[i].second)
						{
							if (m_pSelector->ApplySelect(node, TRUE))
							{
								if (std::find(allSelects.begin(), allSelects.end(), node) == allSelects.end())
									allSelects.push_back(node);
							}
							else
							{
								osg::Node* pParent = node->getParent(0);
								if (pParent&&  m_pSelector->ApplySelect(pParent, TRUE))
								{
									if (std::find(allSelects.begin(), allSelects.end(), pParent) == allSelects.end())
										allSelects.push_back(pParent);
								}
							}
						}
					}
					
				}
			}
		}
	}

	DWORD	 t2 = GetTickCount();
	if (t2 - t1 > 16)
	{
		TRACE("pickSingle erase time: %d\n", t2 - t1);
	}

	return allSelects; 
}

osg::Node* CViewControlData::pickSingle(double x, double y, osgViewer::View* viewer, BOOL _usePolytopeIntersector, BOOL _useWindowCoordinates, osg::Vec3& insectPoint)
{	 
	osg::Node* scene = viewer->getSceneData();
	if (!scene) return NULL;

	DWORD	 t1 = GetTickCount();

	osg::Node* node = 0;
	osg::Group* parent = 0;
	osg::Group* parent2 = 0;
	std::vector<osg::Node*	> allSelects;
	std::vector<osg::Vec3	> allInsectPoints;

	osg::Viewport* viewport = viewer->getCamera()->getViewport();
 	if (_usePolytopeIntersector)
	{
	//	osgUtil::PolytopeIntersector* picker;
		FastPolytopeIntersector* picker;
		if (_useWindowCoordinates)
		{
			double mx = viewport->x() + (int)((double )viewport->width()*(x*0.5+0.5));
			double my = viewport->y() + (int)((double )viewport->height()*(y*0.5+0.5));

			double w = 5;
			double h = 5;
	//		picker = new osgUtil::PolytopeIntersector(osgUtil::Intersector::WINDOW, mx - w, my - h, mx + w, my + h);
			picker = new FastPolytopeIntersector(osgUtil::Intersector::WINDOW, mx - w, my - h, mx + w, my + h);
			picker->bReserve = true;
		}
		MyPickVisitor iv(picker);
 
		 
			viewer->getCamera()->accept(iv);
			if (picker->containsIntersections())
			{
				FastPolytopeIntersector::Intersections osgIntersection = picker->getIntersections();
				FastPolytopeIntersector::Intersections::iterator it = osgIntersection.begin();
				for( ; it != osgIntersection.end(); it++)
				{
					FastPolytopeIntersector::Intersection intersection = *it;
					osg::NodePath& nodePath = intersection.nodePath;
					node = (nodePath.size()>=1)?nodePath[nodePath.size()-1]:0;
					parent = (nodePath.size()>=2)?dynamic_cast<osg::Group*>(nodePath[nodePath.size()-2]):0;
					parent2 = (nodePath.size() >= 3) ? dynamic_cast<osg::Group*>(nodePath[nodePath.size() - 3]) : 0;
					if (node)
					{
						if( m_pSelector->ApplySelect(node) )
						{
							allSelects.push_back( node);
							allInsectPoints.push_back(intersection.localIntersectionPoint);
						}
						else if (m_pSelector->ApplySelect(parent)) //有后遗症？
						{
							allSelects.push_back(parent);
							allInsectPoints.push_back(intersection.localIntersectionPoint);
						}
						else if (m_pSelector->ApplySelect(parent2)) //有后遗症？
						{
							allSelects.push_back(parent2);
							allInsectPoints.push_back(intersection.localIntersectionPoint);
						}
					}		
				}		
			}	
		 
	}

	DWORD	 t2 = GetTickCount();

	if (t2 - t1 > 16)
	{
		TRACE("pickSingle erase time: %d\n", t2 - t1);
	}
	if(allSelects.size() > 0 )
	{
		//已经选择对象最优先0
		for (int i = 0; i < allSelects.size(); i++)
		{
 			if (IsSelected(allSelects[i]->getName().c_str()))
			{
				insectPoint = allInsectPoints[i];
				return allSelects[i];
			}
		}

		insectPoint = allInsectPoints[0];
		return allSelects[0];    
	}
	else
	{
		std::map<CString, osg::ref_ptr<osg::Node>> selectMap;
		m_pSelector->getSelectSet(selectMap);
		std::map<CString, osg::ref_ptr<osg::Node> >::iterator it = selectMap.begin();
		for (; it != selectMap.end(); it++)
		{
			CDbObjInterface* pData = FindObj(it->first, GetObjType(it->second));
			if (pData)
			{
				std::vector<osg::Vec3d> stretchPts = pData->GetStretchPoint();

				for (int i = 0; i < stretchPts.size(); i++)
				{
					osg::Vec2 pt = getWindosPos(stretchPts[i].x(), stretchPts[i].y(), stretchPts[i].z());
					
					double mx =  (int)((double)viewport->width()*(x*0.5 + 0.5));
					double my =  (int)((double)viewport->height()*(y*0.5 + 0.5));
					if ( abs(mx - pt.x()) < 2 && abs(my - pt.y()) < 2)
					{
						return  it->second.get();
					}
				}
			}			
		}
		
	}
	return NULL;
}

CDbObjInterface* CViewControlData::FindObjByID(const DWORD& objID, const CString& type)
{
	return  m_pUserData->FindObjByID(objID, type);
}
 

CDbObjInterface* CViewControlData::FindObj(const CString& objName, const CString& type )
{
	return m_pUserData->FindObj(objName, type);
 
}

osg::BoundingBox CViewControlData::computeBoundBox() const
{
	osg::ComputeBoundsVisitor cbbv;
	GraphicsGroup->accept(cbbv);
	osg::BoundingBox bb = cbbv.getBoundingBox();
	dimObjs->accept(cbbv);
	osg::BoundingBox bb2 = cbbv.getBoundingBox();
	bb.expandBy(bb2);


 
	return bb;
}

void CViewControlData::LocalView()
{
	viewType = 10;
	bNeedChangeViewType++;
}


void CViewControlData::TopView()
{
	if(viewType != 5)
	{
		bNeedChangeViewType++;

		viewType = 5;
		localXDir.set(1, 0, 0);
		localYDir.set(0, 1, 0);
		localOrg.set(0, 0, 0);

		if (strCurCoordSys != "T")
			strLastCoordSys = strCurCoordSys;

		strCurCoordSys = "T";
	}
}


void CViewControlData::FrontView()
{
	if(viewType != 6)
	{
		bNeedChangeViewType++;

		viewType = 6;

		localXDir.set(1, 0, 0);
		localYDir.set(0, 0, 1);
		localOrg.set(0, 0, 0);

		if (strCurCoordSys != "F")
			strLastCoordSys = strCurCoordSys;

		strCurCoordSys = "F";
	}


}
void CViewControlData::LeftView()
{
	if(viewType != 7)
	{
		bNeedChangeViewType++;
		viewType = 7;

		localXDir.set(0, 1, 0);
		localYDir.set(0, 0, 1);
		localOrg.set(0, 0, 0);

		if(strCurCoordSys != "L")
			strLastCoordSys = strCurCoordSys;

		strCurCoordSys = "L";
	}
}

void CViewControlData::PerspectiveView()
{
	//if( viewType != 4 )
	{
		bNeedChangeViewType++;
		viewType = 4;
		localXDir.set(1, 0, 0);
		localYDir.set(0, 1, 0);
		localOrg.set(0, 0, 0);

		if(strCurCoordSys!="W")
			strLastCoordSys = strCurCoordSys;

		strCurCoordSys = "W";	
	}

}

void CViewControlData::TopView2()
{
	viewType = 1;
	localXDir.set(1, 0, 0);
	localYDir.set(0, 1, 0);
	localOrg.set(0, 0, 0);

	bNeedChangeViewType++;
}


void CViewControlData::FrontView2()
{
	localXDir.set(1, 0, 0);
	localYDir.set(0, 0, 1);
	localOrg.set(0, 0, 0);
	viewType = 2;
	bNeedChangeViewType++;
}

void CViewControlData::LeftView2()
{
	localXDir.set(0, 1, 0);
	localYDir.set(0, 0, 1);
	localOrg.set(0, 0, 0);
	viewType = 3;
	bNeedChangeViewType++;
}

void CViewControlData::AutoLayout()
{
	 
}

void CViewControlData::ShowGrid()
{
	bShowGrid =!bShowGrid;

	if( bShowGrid )
	{
		grids->setNodeMask(0xffffffff); // visible
	}
	else
	{
		grids->setNodeMask(0x0); // visible
	}

}
void CViewControlData::ShowAxis()
{
	bShowAxis =!bShowAxis;
	if(bShowAxis)
	{
		hudAxis->setNodeMask(0xffffffff);	
	}	
	else
	{
		hudAxis->setNodeMask(0x0);	
	}
}
void CViewControlData::GridSnap(const osg::Vec3& resPT,osg::Vec3& snapPT )
{
	int nCountX = int(resPT.x()/T_setup.unit );
	int nCountY = int(resPT.y()/T_setup.unit);

	double x =(resPT.x()- nCountX*T_setup.unit)/T_setup.unit;
	double y =(resPT.y()- nCountY*T_setup.unit)/T_setup.unit;

	if(x <-0.5)
		x = T_setup.unit*(nCountX-1);
	else if(x<0.5)
		x = T_setup.unit*nCountX;
	else 
		x=T_setup.unit*(nCountX+1);

	if(y < -0.5)
		y = T_setup.unit*(nCountY-1);
	else if(y<0.5)
		y = T_setup.unit*nCountY;
	else
		y=T_setup.unit*(nCountY+1);

	snapPT.set(x,y,0 );
}
///////
void CViewControlData::SetViewPort( int type, int vptSpace,double width , double height)
{
	if(mView && ( type&0x1 )  )
	{
		double dx = mViewSet.x + vptSpace;
		double dy = mViewSet.y + vptSpace;
		double dWidth  = mViewSet.width -2*vptSpace -GetOffset();
		double dHeight = mViewSet.height-2*vptSpace;
		if(bfactor)
		{
			dx = mViewSet.x*width + vptSpace;
			dy = mViewSet.y*height+ vptSpace;

			dWidth  = mViewSet.width*width -2*vptSpace -GetOffset();
			dHeight = mViewSet.height*height-2*vptSpace;
		}
		mView->getCamera()->setViewport(dx,dy , dWidth , dHeight);	 
	}
 	
}

void  CViewControlData::ClearHandlers( osg::Node* pNode)
{
	if(pNode )
	{
		CString strName =pNode->getName().c_str();		

		int nCount  = StretchHandlers->getNumChildren();
		for(int i=0; i< nCount; i++)
		{
			osg::Node* pChild =  StretchHandlers->getChild(i);
			if( pChild && pChild->getName().c_str() ==strName )
			{
				StretchHandlers->removeChild( pChild);
				break;
			}
		}
	}	
}

//增加高亮显示效果 ， 鼠标移动到对象身上时；
void CViewControlData::addHightLigntEffect( osg::Node* pNode,osg::Vec4 color )
{
	if(pNode)
	{
		pNode->setName( "HeightLightEffectNode");

		osg::Node* pClone = osg::clone(pNode , osg::CopyOp::DEEP_COPY_ALL);

		osg::Group* pTotalEffect = new osg::Group;
		pTotalEffect->setName("HeightLightEffect");

		//在上边覆盖点跨线， 渲染顺序和加入的先后有关，（当z相同时）？？
		SetLunKuoCenterEffect( pClone,color);
		pTotalEffect->addChild(pClone);

		//先加入线宽
		SetLunKuoEffect(pNode,3,color);
		pTotalEffect->addChild(pNode);

		HeightLightEffectRoot->addChild( pTotalEffect);

	}	
}


//删除高亮显示效果 ， 鼠标移走时；
void CViewControlData::removeHightLigntEffect(osg::Node* pNode )
{ 
	m_pPickModelHandler->SetNodeHeightLight(pNode, false);
 	int nCount  = HeightLightEffectRoot->getNumChildren();
	for(int i=0; i< nCount; i++)
	{
		osg::Node* pChild =  HeightLightEffectRoot->getChild(i);
		if( pChild && pChild->getName() =="HeightLightEffect" )
		{
			HeightLightEffectRoot->removeChild( pChild);
			break;
		}
	}	
	///////////////////////////		
	
}

void CViewControlData::AddHandlers(osg::Node* pNode)
{
	if(pNode  )
	{
		if(IsHandlers(   pNode ) )
			return;

		CString strName = pNode->getName().c_str();
		CDbObjInterface*  pData =  FindObj( strName   ,GetObjType(pNode) );
		if(pData)
		{
			osg::Group* pSelectHandler= new osg::Group;
			pSelectHandler->setName( strName );
			 StretchHandlers->addChild( pSelectHandler);

			std::vector<osg::Vec3d> stretchPT = pData->GetStretchPoint();
			for(int i=0; i< stretchPT.size(); i++)
			{
				osg::Geode* pHandler= new osg::Geode;
				CreateHandler( stretchPT[i] ,HandlerColor ,pHandler , handlersize);

				pSelectHandler->addChild( pHandler);
			}
		}
	}
}


BOOL CViewControlData::IsHandlers(   osg::Node* pNode)
{
	if(!pNode)
		return FALSE;

	std::string strName = pNode->getName();
	int nChild = 	StretchHandlers->getNumChildren();
	for(int i=0; i< nChild; i++)
	{
		if(StretchHandlers->getChild( i)->getName() == strName )
		{
			return TRUE;
		}
	}
	return FALSE;
}	

//


//设置文字大小
void CViewControlData::SetObjSize(const CString& type , int size)
{
	objSizeMap[type] = size;
}

//返回文字大小
int  CViewControlData::GetObjSize(const CString& type)
{
	std::map<CString, int >::iterator it = objSizeMap.find( type);
	if( it != objSizeMap.end() )
	{
		return it->second;
	}
	return 20;
}

//设置文字大小
void CViewControlData::SetTextSize(const CString& type , int size)
{
	objTextSizeMap[type] = size;
}
//返回文字大小
int  CViewControlData::GetTextSize(const CString& type)
{
	std::map<CString, int >::iterator it = objTextSizeMap.find( type);
	if( it != objTextSizeMap.end() )
	{
		return it->second;
	}
	return 20;
}



//设置文字大小
void CViewControlData::SetObjColor(const CString& type , osg::Vec4 color)
{
	objColorMap[type] = color;
}
//返回文字大小
osg::Vec4  CViewControlData::GetObjColor(const CString& type)
{
	std::map<CString , osg::Vec4 >::iterator it = objColorMap.find( type);
	if( it != objColorMap.end() )
	{
		return it->second;
	}
	return redColor;
}



void CViewControlData::InitPickManipulator()
{
	m_pPickModelHandler= new CDbPickModeHandler( GraphicsGroup.get() );
	m_pPickModelHandler->view_perspective =mView;
	m_pPickModelHandler->pOSG = this;

	mView->addEventHandler(m_pPickModelHandler);




}


void CViewControlData::LoadFromXML( CMenu& menu , const std::wstring& strType)
{
	/*Cx_Interface<Ix_ConfigXml> pIFXml(x3::CLSID_ConfigXmlFile);
	ASSERT(pIFXml.IsNotNull());
	CString strPath = OSGGetApplicationSelfPath() +_T("Config\\ViewMenu.xml");

	Cx_Interface<Ix_Commander> pCommandAllocator(x3::CLSID_Commander);
	if (!pCommandAllocator)
	{
		return ; 
	}

	size_t len = strPath.GetLength() + 1;
	size_t converted = 0;
	wchar_t *wstrPath = (wchar_t*)malloc(len*sizeof(wchar_t));
	mbstowcs_s(&converted, wstrPath, len, strPath, _TRUNCATE);

	pIFXml->SetFileName(wstrPath);
	pIFXml->SetRootName(L"Root");

	Cx_ConfigSection sec(pIFXml->GetData()->GetSection(L"ViewMenu",false));	
 
	if (sec->IsValid())
	{
		int k = 0;
		bool bFind = false;
		do 
		{
			Cx_ConfigSection menusec(sec.GetSectionByIndex(L"menu",k++));
			if (!menusec->IsValid())
			{
				break;
			}
			int j = 0;
			std::wstring nodetype = menusec->GetString(L"node");
			if(nodetype == strType) 
			{
				do 
				{
					Cx_ConfigSection menusecEx(menusec.GetSectionByIndex(L"item",j++));
					if(!menusecEx->IsValid())
					{
						break;
					} 
					std::wstring strCmd     = menusecEx->GetString(L"cmd");
					std::wstring strImage   = menusecEx->GetString(L"image");	
					std::wstring strTip        = menusecEx->GetString(L"des");
					std::wstring strCaption = menusecEx->GetString(L"name");		

					if (  strCaption==L"---")
					{
						menu.AppendMenu(MF_SEPARATOR, (UINT)-1, (LPCTSTR)NULL);
					}
					else if(MenuItemIsValid( strCmd) )
					{
						UINT nID = pCommandAllocator->AllocateID(strCmd);
						UINT nFlag = MF_STRING|MF_ENABLED;				 
						CString strTitle = x3::w2t(strCaption).c_str();
						menu.AppendMenu(nFlag, nID, strTitle);
					}
				} while (true);		
				break;
			} 
		} while (true);
	}*/
}

bool CViewControlData::MenuItemIsValid(const std::wstring& strMenuItem)
{
	return true;
}

void CViewControlData::OnRButtonDown()
{


}
void CViewControlData::InitCommandString()
{
	if (strCommandMap.size() == 0)
	{
		strCommandMap.push_back(std::make_pair("all",  "视图内容全屏显示"));
		strCommandMap.push_back(std::make_pair("sc",   "动态缩放"));
		strCommandMap.push_back(std::make_pair("zoom", "区域缩放"));
		strCommandMap.push_back(std::make_pair("full", "建模窗口全屏"));

		strCommandMap.push_back(std::make_pair("pan",  "视图平移"));
		strCommandMap.push_back(std::make_pair("ro",   "视图旋转"));
		strCommandMap.push_back(std::make_pair("s",    "框选"));
		strCommandMap.push_back(std::make_pair("se",   "进入默认选择态"));
		strCommandMap.push_back(std::make_pair("grid", "显示/隐藏栅格"));
		strCommandMap.push_back(std::make_pair("axis", "显示/隐藏坐标轴"));
		strCommandMap.push_back(std::make_pair("co", "复制"));
		strCommandMap.push_back(std::make_pair("mi", "镜像"));
		strCommandMap.push_back(std::make_pair("ar", "阵列"));
		strCommandMap.push_back(std::make_pair("m",  "移动"));
		strCommandMap.push_back(std::make_pair("o",  "偏移"));
		strCommandMap.push_back(std::make_pair("di", "测距"));
		strCommandMap.push_back(std::make_pair("id", "测点"));

		strCommandMap.push_back(std::make_pair("tr",    "剪切"));
		strCommandMap.push_back(std::make_pair("ex",    "延伸"));

		strCommandMap.push_back(std::make_pair("top",   "顶视图"));
		strCommandMap.push_back(std::make_pair("left",  "侧视图"));
		strCommandMap.push_back(std::make_pair("f",     "立面视图"));
		strCommandMap.push_back(std::make_pair("pr",    "斜视图"));

		strCommandMap.push_back(std::make_pair("top2",  "二维顶视图"));
		strCommandMap.push_back(std::make_pair("left2", "二维侧视图"));
		strCommandMap.push_back(std::make_pair("f2",    "二维立面视图"));

		strCommandMap.push_back(std::make_pair("gf0",   "增加栅格尺寸"));
		strCommandMap.push_back(std::make_pair("gf1",   "减小栅格尺寸"));
	}
}

void CViewControlData::OnCommandUpdate(const CString& strCommand, bool& enable, bool& checked)
{
	enable  = true;
	checked = false;
}

bool CViewControlData::bInCommand()
{
	if (m_command > 0 && m_pCurHandler && m_pCurHandler->status > 0)
	{
		return true;
	}
	return false;
}
bool CViewControlData::pre_do_new_command(const CString& domain, const CString& command, const CString& param)
{
	SetHandler(NULL);
	return true;
}

bool CViewControlData::OnCommand(const CString& strCommand)
{
	CString cmd_text = strCommand/*do_command::parse_command_cmd(strCommand)*/; // 暂不关心命令的域

	BOOL bCommand = bInCommand();

	if (bCommand )
	{ 
		int nChar = osgGA::GUIEventAdapter::KEY_Return;

		if (cmd_text == CString(TCHAR(VK_ESCAPE)) || cmd_text == "esc")
			nChar = osgGA::GUIEventAdapter::KEY_Escape;
		m_Osg->GetCurrentView()->getEventQueue()->keyPress(nChar);

		return true; 
	}

	if (cmd_text == "esc")
	{
		if(m_pSelector )
		{
			SetBehavorType(0);

			bNeedClearSelect=TRUE;
  			return true;
		}
	}
	else if (cmd_text.IsEmpty())
	{
		if(m_pCurHandler)
		{
			SetHandler(m_pCurHandler );
			return true;	
		}
	}
	else if (!cmd_text.CompareNoCase("gf0"))
	{
		girdFactor*=2;
		return true;
	}
	else if (!cmd_text.CompareNoCase("gf1"))
	{
		girdFactor/=2;
		return true;
	}
 
	else if (!cmd_text.CompareNoCase("S"))
	{
		SetBehavorType(0);

		SetHandler(new CDbBoxSelectorHandler(this) );
		return true;
	}
 
	else if (!cmd_text.CompareNoCase("sc"))
	{
		SetBehavorType(3);
		auto  cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		 
		return true;
	}
	else if (!cmd_text.CompareNoCase("all"))
	{
		DoFullSceen();
		return true;
	} 
	else if (!cmd_text.CompareNoCase("full"))
	{
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt("主窗口需要实现当前编辑器全屏功能!");
			cmdLine->AddPrompt("");
		}
	
		return true;
	}
	else if (!cmd_text.CompareNoCase("zoom"))
	{
		SetHandler(new CDbBoxZoomHandler(this) );
		return true;
	}	
	else if (!cmd_text.CompareNoCase("pan"))
	{
		SetBehavorType(1);
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		
		return true;
	}	
	else if (!cmd_text.CompareNoCase("RO"))
	{
		SetBehavorType(2);
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
	
		return true;
	}	
	else if (!cmd_text.CompareNoCase("SE"))
	{
		SetBehavorType(0);
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		
		return true;
	}
	else if (!cmd_text.CompareNoCase("TOP"))
	{
		TopView();
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		return true;
	}
	else if (!cmd_text.CompareNoCase("TOP2"))
	{
		TopView2();
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		return true;
	}
	else if (!cmd_text.CompareNoCase("LEFT"))
	{
		LeftView();
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		return true;
	}
	else if (!cmd_text.CompareNoCase("LEFT2"))
	{
		LeftView2();
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		return true;
	} 
	else if (!cmd_text.CompareNoCase("F") || cmd_text.CompareNoCase("front") == 0)
	{
		FrontView();
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		return true;
	}
	else if (!cmd_text.CompareNoCase("F2") || cmd_text.CompareNoCase("front2") == 0)
	{
		FrontView2();
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		return true;
	}

	else if(!cmd_text.CompareNoCase("PR"))
	{
		PerspectiveView();
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		return true;
	}
	else if(!cmd_text.CompareNoCase("grid"))
	{
		ShowGrid();
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		return true;
	}
	else if(!cmd_text.CompareNoCase("axis"))
	{
		ShowAxis();
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		return true;
	}
	else if(!cmd_text.CompareNoCase("CO"))
	{
		SetHandler(new CDbCopyHandler(this) );
		return true;
	}
	else if(!cmd_text.CompareNoCase("MI"))
	{
		SetHandler(new CDbMirrorHandler(this) );
		return true;
	}
	else if(!cmd_text.CompareNoCase("AR"))
	{
		SetHandler(new CDbArrayHandler(this) );
		return true;
	}

	else if(!cmd_text.CompareNoCase("M"))
	{
		SetHandler(new CDbMoveObjhandler(this) );	
		return true;
	}
	else if (!cmd_text.CompareNoCase("O"))
	{
		SetHandler(new CDbOffsethandler(this));
		return true;
	}
	else if(!cmd_text.CompareNoCase("DI"))
	{
		SetHandler(new CDbDistHandler(this) );	
		return true;
	}
	else if (!cmd_text.CompareNoCase("ID"))
	{
		SetHandler(new CDbIDHandler(this));
		return true;
	}
	else if(!cmd_text.CompareNoCase("TR"))
	{
		SetHandler(new CDbExtendHandler(this) );	
		return true;
	}
	else if(!cmd_text.CompareNoCase("EX"))
	{
		CDbExtendHandler* pHandler = new CDbExtendHandler(this);
		pHandler->bTrim=FALSE;
		SetHandler(pHandler);	
		
		return true;
	}
	else if (!cmd_text.CompareNoCase("e"))
	{
		CDbEraseHandler* pHandler = new CDbEraseHandler(this);
 		SetHandler(pHandler);
		return true;
	}

	else if(!cmd_text.CompareNoCase("all"))
	{
		DoFullSceen();
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		return true;
	}
	else if(!cmd_text.CompareNoCase("max"))
	{
		DoMax();
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		return true;
	}
	else if(!cmd_text.CompareNoCase("delView"))
	{
		DoDelView();

		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		return true;
	}	 
	else if(!cmd_text.CompareNoCase("snap1"))
	{
		m_snapType = SNAP_NO;
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt("取消捕捉");
			cmdLine->AddPrompt(_T(""));
		}
		
		return true;
	}
	else if(!cmd_text.CompareNoCase("snap2"))
	{
		m_snapType|=SNAP_GRID;
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt("栅格捕捉");
			cmdLine->AddPrompt(_T(""));
		}
		
		return true;
	}
	else if(!cmd_text.CompareNoCase("snap-2"))
	{
		m_snapType=m_snapType&~SNAP_GRID;
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt("删除栅格捕捉");
			cmdLine->AddPrompt(_T(""));
		}
	
		return true;
	}	
	else if(!cmd_text.CompareNoCase("snap4"))
	{
		m_snapType|=SNAP_NODE;
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt("节点捕捉");
			cmdLine->AddPrompt(_T(""));
		}
		
		return true;
	}
	else if(!cmd_text.CompareNoCase("snap-4"))
	{
		m_snapType = m_snapType&~SNAP_NODE;
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt("删除节点捕捉");
			cmdLine->AddPrompt(_T(""));
		}
		
		return true;
	}

	else if(!cmd_text.CompareNoCase("snap10"))
	{
		m_snapType|=SNAP_MID;
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt("中点捕捉");
			cmdLine->AddPrompt(_T(""));
		}
	
		return true;
	}
	else if(!cmd_text.CompareNoCase("snap-10"))
	{
		m_snapType = m_snapType&~SNAP_MID;
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt("删除中点捕捉");
			cmdLine->AddPrompt(_T(""));
		}
		
		return true;
	}
	else if(!cmd_text.CompareNoCase("snap20"))
	{
		m_snapType|=SNAP_INTERSECTION;
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt("线段交点捕捉");
			cmdLine->AddPrompt(_T(""));
		}
	
		return true;
	}
	else if(!cmd_text.CompareNoCase("snap-20"))
	{
		m_snapType = m_snapType&~SNAP_INTERSECTION;
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt("删除线段交点捕捉");
			cmdLine->AddPrompt(_T(""));
		}
	
		return true;
	}

	return false;
}

void CViewControlData::InitAxis()
{
	
}

void CViewControlData::InitSceneGraph()
{
	mRoot = new osg::Group;

	pTerrain = new osg::Group;
	pTerrain->setName("terrain");

	allText = new osg::Group;
	allText->setName("allText");

	snapGraph = new osg::AutoTransform;
	snapGraph->setDataVariance(osg::Object::DYNAMIC);
	snapGraph->setName("snapPT");
	snapGraph->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	snapGraph->getOrCreateStateSet()->setRenderBinDetails(102, "RenderBin");
	snapGraph->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);


	hudAxis = new osg::Group;
	hudAxis->setName("aixs");
	hudAxis->setDataVariance(osg::Object::DYNAMIC);
	hudAxis->getOrCreateStateSet()->setRenderBinDetails(999, "RenderBin");
	hudAxis->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
 
	tempObjs = new osg::Group;
	tempObjs->setName("temp Objects");
	tempObjs->setDataVariance(osg::Object::DYNAMIC);
	tempObjs->getOrCreateStateSet()->setRenderBinDetails(101, "RenderBin");

	dimObjs = new osg::Group;
	dimObjs->setName("dimObjs");
	dimObjs->setDataVariance(osg::Object::DYNAMIC);

	dimObjs->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF  );
	dimObjs->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

	StretchHandlers = new osg::Group;
	StretchHandlers->setName("StretchHandlers");
	StretchHandlers->setDataVariance(osg::Object::DYNAMIC);

	StretchHandlers->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	StretchHandlers->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
 	StretchHandlers->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");

	SelectEffectRoot = new osg::Group;
	SelectEffectRoot->setName("SelectEffectRoot");

	HeightLightEffectRoot= new osg::Group;
	HeightLightEffectRoot->setName("HeightLightEffectRoot");

	validGraph = new osg::Group;	
	validGraph->setName("validGraph");

	GraphicsGroup = new osg::Group;
	GraphicsGroup->setName("GraphicsGroup");
 	GraphicsGroup->setDataVariance(osg::Object::DYNAMIC);

	mOffsetViewRoot = new osg::Group;
	mOffsetViewRoot->setName("mOffsetViewRoot");

	validGraph->addChild( GraphicsGroup);

	validGraph->addChild(allText);


	grids = new osg::Group();
	grids->setName("grids");
	validGraph->addChild(grids);

	pRubBoxCamera = createHudSelectBOX(this, osg::Vec2(0, 0), osg::Vec2(0, 0), pRubBoxGeometry);
	validGraph->addChild(pRubBoxCamera);
 		 
	CreateAxis2(hudAxis );

	InitGraph(validGraph);

	//初始化颜色
	InitColor();

	//初始化大小
	InitSize();

	//初始化可见
	InitVisible();


	if(mRoot)
	{
		mRoot->addChild(pTerrain);
		mRoot->addChild(tempObjs);
		mRoot->addChild(StretchHandlers);
		mRoot->addChild(SelectEffectRoot);
		mRoot->addChild(HeightLightEffectRoot);
		mRoot->addChild(validGraph);
		mRoot->addChild(snapGraph);
		mRoot->addChild(dimObjs);
		mRoot->addChild(hudAxis);
	}
}

double CViewControlData::GetOffset()
{
	if (m_bOffset)
	{
		return m_dOffset;
	}
	return 0;
}


void CViewControlData::SetbOffset( bool bOffSet )
{
	m_bOffset = bOffSet;
	double vpSpace = 0, width = 0, height = 0;
	CDbCompositeViewOSG* pOsg = dynamic_cast<CDbCompositeViewOSG*>(m_Osg);
	if (pOsg)
	{
		vpSpace = pOsg->vptSpace;
		width = pOsg->width;
		height = pOsg->height;
	}
	SetViewPort(3, vpSpace, width, height);
}

//通过标题来判断对象是否存在
BOOL  CViewControlData::TitleisValid(const CString& objType, CString& strTitle)
{
	std::vector<CDbObjInterface*> allObjs = FindObjByType(objType);
	for (int i = 0; i < allObjs.size(); i++)
	{
		if (allObjs[i]->GetTitle()  == strTitle)
		{
			return FALSE;
		}
	}
	return TRUE;
}

std::vector<CDbObjInterface*> CViewControlData::FindObjByTypes(std::vector<CString>& objTypes)
{
	return m_pUserData->FindObjByTypes(objTypes);

	
}

std::vector<CDbObjInterface*> CViewControlData::FindObjByType(const CString& type)
{
	return m_pUserData->FindObjByType(type);

}

std::vector<CString>  CViewControlData::GetObjTitlesByType(const CString& objType)
{
	return m_pUserData->GetObjTitlesByType(objType);
}

CDbObjInterface* CViewControlData::FindObjByTitle(const CString& strTitle, const CString& type)
{
	return m_pUserData->FindObjByTitle(strTitle,type);
} 

void CViewControlData::SelectChanged()
{
 
	//refacor by wr
	/*property_grid_interface* pGrid = get_property_grid_instance();
	if (pGrid)
	{
	std::vector<CDbObjInterface*> allSelect = GetCurSelectObject();
	if (allSelect.size() == 0)
	{
	CDbObjInterface* pIdleObj = GetIdleObj();
	if (pGrid && pIdleObj)
	{
	pGrid->populate(pIdleObj);
	}
	else
	pGrid->clear();
	}
	else
	{
	if (allSelect.size() == 1)
	{
	pGrid->populate(allSelect[0]);
	}
	else
	{
	pGrid->populate(&multiObject);
	}
	}
	}*/
}
void CViewControlData::UpdateSNAPINFOWhenModifyObj(DWORD uID)
{
	UpdateSNAPINFOWhenDeleteObj(uID);
	UpdateSNAPINFOWhenAddObj(uID);
}

void CViewControlData::UpdateSNAPINFOWhenDeleteObj(DWORD uID)
{ 
	//交点
	std::vector<SNAPINFO> pointArrNew1;
	for (int i = 0; i <m_IntersectionSnapPoints.size(); i++)
	{
		if (m_IntersectionSnapPoints[i].firstNo != uID && m_IntersectionSnapPoints[i].secondNo != uID)
		{
			pointArrNew1.push_back(m_IntersectionSnapPoints[i]);
		}
	}
	m_IntersectionSnapPoints = pointArrNew1;
}

osg::Matrix* CViewControlData::GetWorldMatrix()
{
	return NULL;
	//osg::Vec3 norX = osg::Vec3(xDir.x, xDir.y, xDir.z);
	//osg::Vec3 norY = osg::Vec3(yDir.x, yDir.y, yDir.z);
	//osg::Vec3 norZ = norX^norY;

	//norX.normalize();
	//norY.normalize();
	//norZ.normalize();

	//m_pMatrix->set(
	//	norX.x(), norX.y(), norX.z(), 0,
	//	norY.x(), norY.y(), norY.z(), 0,
	//	norZ.x(), norZ.y(), norZ.z(), 0,
	//	orgion.x, orgion.y, orgion.z, 1);
}



void CViewControlData::UpdateSNAPINFOWhenAddObj(DWORD uID)
{

}

void CViewControlData::setCameraManipulator(osgViewer::View* pView)
{
	if (pView)
	{
		pView->setCameraManipulator(new CDb3DManipulator(pView->getCamera(), this));
	}
}


CString CViewControlData::GetDefaultTitle(const CString& strType)
{
	std::vector<CDbObjInterface*> findObjs = FindObjByType(strType);

	vector<CString> vecTitles;
	for (int i = 0; i < findObjs.size(); i++)
		vecTitles.push_back(findObjs[i]->GetTitle());

	return  GetNonExistName(vecTitles, "", strType);
}
void CViewControlData::UpatateLocalMatrix()
{
	if (viewType == 1 || viewType ==5)
	{
		localXDir.set(1, 0, 0);
		localYDir.set(0, 1, 0);
		localOrg.set(0, 0, 0);
	}
	else if (viewType == 2 || viewType == 6)
	{
		localXDir.set(1, 0, 0);
		localYDir.set(0, 0, 1);
		localOrg.set(0, 0, 0);
	}
	else if (viewType == 3 || viewType == 7)
	{
		localXDir.set(0, -1, 0);
		localYDir.set(0, 0, 1);
		localOrg.set(0, 0, 0);
	}
	else if (viewType ==4)
	{
		localXDir.set(1, 0, 0);
		localYDir.set(0, 1, 0);
		localOrg.set(0, 0, 0);
	}
	else
	{
		ASSERT(FALSE);
	}
}

BOOL CViewControlData::RepeatLastCommand()
{
	if (m_pCurHandler)
	{
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->OnText(VK_SPACE, 1, 1);
		}
	
		return TRUE;
	}
	return FALSE;
}



void CViewControlData::setUpdateCallback(osg::NodeCallback* pCallBack)
{
	mRoot->setUpdateCallback(pCallBack);
}

//重载了点选函数
void MyPickVisitor::apply(osg::Group& group)
{
 
	{
		osgUtil::IntersectionVisitor::apply(group);
	}
} 
///////////////
void ClearSelectionNodeVisitor::apply( osg::Node& node )
{
	CString objName = node.getName().c_str();
	if (  !objName.IsEmpty() )
	{
		CDbObjInterface*  pData =  m_pOSG->FindObj(  objName ,  GetObjType(&node) );
		if(pData  && m_pOSG->m_pPickModelHandler)
		{
			m_pOSG->clearBoundingBoxObj(objName);
 		}
	}
	traverse( node );	
}



void CViewControlData::ChangeViewByType(const osg::Vec3& center, double radius, const osg::Vec3& lookDir, const osg::Vec3& up, osgViewer::View* mView)
{
	m_center = center;
	m_radius = radius;
	m_up = up;

	m_lookDir = lookDir;
	m_lookDir.normalize();

	osg::Vec3d oldEye;
	osg::Vec3d oldCenter;
	osg::Vec3d oldUp;
	osgGA::OrbitManipulator* pCameraManipulator = dynamic_cast<osgGA::OrbitManipulator*>(mView->getCameraManipulator());
	if (pCameraManipulator)
	{
		pCameraManipulator->getTransformation(oldEye, oldCenter, oldUp);
	}

	if (1)//add 2016-7-10
	{
		double left;
		double right;
		double  bottom;
		double top;
		double zNear;
		double zFar;
		mView->getCamera()->getProjectionMatrixAsOrtho(left, right, bottom, top, zNear, zFar);
		double dW = right - left;
		double dH = top - bottom;

		mView->getCamera()->setProjectionMatrixAsOrtho(-0.5*dW, 0.5*dW, -0.5*dH, 0.5*dH, zNear, zFar);
		setCameraManipulator(mView);
	}
	osg::Vec3 oldDir = oldCenter - oldEye;
	oldDir.normalize();

	double  delta = 0.5* acos(m_lookDir*oldDir);
	if (abs(delta) < 1.0e-6)
	{
		mView->getCameraManipulator()->setHomePosition(m_center - m_lookDir*(m_radius*3.0), m_center, m_up);
		mView->home();
	}
	else
	{
		cameraAnimation = true;
		animationFactor = 0;

		m_up.normalize();
		oldUp.normalize();
		double  deltaUp = 0.5* acos(m_up*oldUp);
		if (abs(deltaUp) < 1.0e-6)
		{
			upValid = false;
		}
		else
		{
			upValid = true;

			deltaUp = M_PI_4;

			rotationUp1.set(oldUp.x()*sin(deltaUp), oldUp.y()*sin(deltaUp), oldUp.z()*sin(deltaUp), cos(deltaUp));
			rotationUp2.set(m_up.x()*sin(deltaUp), m_up.y()*sin(deltaUp), m_up.z()*sin(deltaUp), cos(deltaUp));
		}

		delta = M_PI_4;
		rotation1.set(oldDir.x()*sin(delta), oldDir.y()*sin(delta), oldDir.z()*sin(delta), cos(delta));
		rotation2.set(lookDir.x()*sin(delta), lookDir.y()*sin(delta), lookDir.z()*sin(delta), cos(delta));
	}
}

/////
void CViewControlData::OnCameraAnimation()
{	 
	animationFactor += 0.1;
	if (animationFactor < 1.0001)
	{
		osg::Quat rotation;
		rotation.slerp(animationFactor, rotation1, rotation2);

		double curDelta = acos(rotation[3]);

		osg::Vec3 curLookDir = osg::Vec3(rotation[0], rotation[1], rotation[2]) / sin(curDelta);
		curLookDir.normalize();

		osg::Vec3 curUp = m_up;
		if (upValid)
		{
			osg::Quat rotationUp;
			rotationUp.slerp(animationFactor, rotationUp1, rotationUp2);

			double curDeltaUp = acos(rotationUp[3]);
			curUp = osg::Vec3(rotationUp[0], rotationUp[1], rotationUp[2]) / sin(curDeltaUp);
			curUp.normalize();
		}

		mView->getCameraManipulator()->setHomePosition(m_center - curLookDir*(m_radius*3.0), m_center, curUp);

		mView->home();
	}
	else
	{
		cameraAnimation = false;
	}
}





osg::MatrixTransform* CreateOneAxis(double size, osg::Quat* pMatrix, osg::Vec4 color)
{
	osg::MatrixTransform* pGroup = new osg::MatrixTransform;

	osg::Geode* geode = new osg::Geode;

	osg::Cone* pCone = new osg::Cone(osg::Vec3(0.0f, 0.0f, 1.0*size), 0.10f*size, 0.18*size);
	osg::ShapeDrawable* pDrawable = new osg::ShapeDrawable(pCone);
	pDrawable->setColor(color);
	geode->addDrawable(pDrawable);
 
	osg::Cylinder* pCylinder = new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.5f*size), 0.03f*size, 1.0f*size);
	osg::ShapeDrawable* pGeometry = new osg::ShapeDrawable(pCylinder);
	pGeometry->setColor(color);
	geode->addDrawable(pGeometry);

	pGroup->addChild(geode);

	if (pMatrix)
		pGroup->setMatrix(osg::Matrix(*pMatrix));

	return pGroup;
}

 



class AxisCameraUpdateCallback : public osg::NodeCallback
{
	CViewControlData* m_pOSG;
public:
	AxisCameraUpdateCallback(CViewControlData* pOSG)
	{
		m_pOSG = pOSG;
	}
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
		{
			osg::Camera* camera = dynamic_cast<osg::Camera*>(node);
			if (camera)
			{
				osg::View* view = camera->getView();
				if (view && view->getNumSlaves() > 0)
				{
					osg::View::Slave* slave = &view->getSlave(0);
					if (slave->_camera.get() == camera)
					{
						osg::Camera* masterCam = view->getCamera();
						osg::Vec3 eye, center, up;
						masterCam->getViewMatrixAsLookAt(eye, center, up, 30);
						osg::Matrixd matrix;
						matrix.makeLookAt(eye - center, osg::Vec3(0, 0, 0), up); // always look at (0, 0, 0)
						camera->setViewMatrix(matrix);
									
						if (m_pOSG)
						{
							double dx, dy;
							m_pOSG->GetWindowSize(dx, dy);
							camera->setProjectionMatrix(osg::Matrix::ortho2D(-20, 800, -20, 800 * dy / dx));
						}
					}
				}
			}
		}
		traverse(node, nv);
	}
};


 
class SmallCrossUpdateCallback : public osg::NodeCallback
{
public:
	CViewControlData* m_pOSG;
	SmallCrossUpdateCallback(CViewControlData* pOSG)
	{
		m_pOSG = pOSG;
	}
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
		{
			osg::PositionAttitudeTransform* pTtrans = dynamic_cast<osg::PositionAttitudeTransform*>(node);
			if (pTtrans)
			{
				osg::Vec3d ptZero = m_pOSG->localOrg;

				pTtrans->setPosition(ptZero);
			}	 
		}
		traverse(node, nv);
	}
};

osg::Camera* createHUD(CViewControlData* pOSG)
{
	double dx,dy;
	pOSG->GetWindowSize(dx,dy);

	osg::Camera* camera = new osg::Camera;
	camera->setProjectionMatrix(osg::Matrix::ortho2D(-20, 800, -20, 800*dy/dx));
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setAllowEventFocus(false);
	camera->setRenderOrder(osg::Camera::POST_RENDER);
	camera->setUpdateCallback(new AxisCameraUpdateCallback(pOSG));
	return camera;
}



/////////坐标原点
void CreateAxisLine(osg::ref_ptr<osg::Geode> pGeode, double size)
{
 	osg::Geometry*  pLine = createLineGeode(osg::Vec3(0, 0, -size), osg::Vec3(0, 0, size), blueColor);
	pGeode->addChild(pLine);
	pLine = createLineGeode(osg::Vec3(0, -size, 0), osg::Vec3(0,  size,0), greenColor);
	pGeode->addChild(pLine);

	pLine = createLineGeode(osg::Vec3(-size, 0, 0), osg::Vec3( size, 0,0), redColor);
	pGeode->addChild(pLine);
}


class SmallCrossUpdateSizeCallback : public osg::NodeCallback
{
public:
	CViewControlData* m_pOSG;
	double oldSize;
	SmallCrossUpdateSizeCallback(CViewControlData* pOSG)
	{
		m_pOSG = pOSG;
		oldSize = 1;
	}
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
		{
			osg::Geode* pGeode = dynamic_cast<osg::Geode*>(node);
			if (pGeode && m_pOSG)
			{
				double newSize = m_pOSG->axisSize / m_pOSG->unitFactor;
				if (oldSize == newSize)
					return;
				oldSize = newSize;
				pGeode->removeChild(0, pGeode->getNumChildren());
				//if (m_pOSG->axisSize > 0)
					//CreateAxisLine(pGeode, m_pOSG->axisSize / m_pOSG->unitFactor);
			}
		}
		traverse(node, nv);
	}
};


void CViewControlData::CreateAxis2(osg::ref_ptr<osg::Group> rootNode)
{
	osg::ref_ptr<osg::Geode>  pGeode = new osg::Geode;
	CreateAxisLine(pGeode, axisSize / unitFactor);

	pGeode->setUpdateCallback(new SmallCrossUpdateSizeCallback(this));

 	pGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

	///////////////
	{
		osg::ref_ptr<osg::PositionAttitudeTransform> pTrans = new osg::PositionAttitudeTransform;
		pTrans->setPosition(localOrg);

		pTrans->setUpdateCallback(new SmallCrossUpdateCallback(this));
		pTrans->addChild(pGeode);
		if (unitFactor==1)
		{	 
				osg::AutoTransform* at = new osg::AutoTransform;
				at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

				at->setAutoScaleToScreen(true);
				at->setMinimumScale(0);
				at->setMaximumScale(FLT_MAX);
				at->setPosition(osg::Vec3(0.0f, 0.0f, 0.0f));

				at->addChild(pTrans);
				rootNode->addChild(at);
		}
		else
		{
			rootNode->addChild(pTrans);
		}
	}


	osg::Group* axisRoot = new osg::Group;
	osg::Group* axis = new osg::Group;
	osg::Vec3 center(0, 0, 0);
	double colorFactor = 1;
	int size = 30;
	{
		osg::Quat rotation;
		rotation.makeRotate(osg::Z_AXIS, osg::X_AXIS);
		osg::MatrixTransform* geode = CreateOneAxis(size, &rotation, redColor);
		axis->addChild(geode);
	}
	{
		osg::Quat rotation;
		rotation.makeRotate(osg::Z_AXIS, osg::Y_AXIS);
		osg::MatrixTransform* geode = CreateOneAxis(size, &rotation, greenColor);
		axis->addChild(geode);
	}
	{
		osg::MatrixTransform* geode = CreateOneAxis(size, NULL, blueColor);
		axis->addChild(geode);
	}

	osg::Geode* pText = new osg::Geode;
	double textFactor = 1.2;
	if (pText)
	{
		pText->addDrawable(create3DTextNoScale(GetEnTextFont(), osg::Vec3(textFactor*size, 0, 0), 15, "X"));
		pText->addDrawable(create3DTextNoScale(GetEnTextFont(), osg::Vec3(0, textFactor*size, 0), 15, "Y"));
		pText->addDrawable(create3DTextNoScale(GetEnTextFont(), osg::Vec3(0, 0, textFactor*size), 15, "Z"));
	}
	axisRoot->addChild(pText);
	axisRoot->addChild(axis);

	//
	osg::Camera* pHudAxis = createHUD(this);

	pHudAxis->addChild(axisRoot);
 	rootNode->addChild(pHudAxis);
	mView->addSlave(pHudAxis, false);
}


float CViewControlData::GetGLVersion()
{
	osg::GLExtensions* pConfig = osg::GLExtensions::Get(mView->getCamera()->getGraphicsContext()->getState()->getContextID(), true);
	if (pConfig)
	{
		return pConfig->glVersion;
	}
	return 1.0;
}
float CViewControlData::GetGLSLVersion()
{
	osg::GLExtensions* pConfig = osg::GLExtensions::Get(mView->getCamera()->getGraphicsContext()->getState()->getContextID(), true);
	if (pConfig)
	{
		return pConfig->glslLanguageVersion;
	}
	return 0.0;
}
bool CViewControlData::ShaderIsSupport()
{
	osg::GLExtensions* pConfig = osg::GLExtensions::Get(mView->getCamera()->getGraphicsContext()->getState()->getContextID(), true);
	if (pConfig)
	{
		return  pConfig->isGlslSupported &&pConfig->isShaderObjectsSupported&&pConfig->isVertexShaderSupported;
	}
	return false;
}


CDbHandlerInterface* CViewControlData::g_Handler = NULL;

CDbHandlerInterface* CViewControlData::GetGlobalHandler()
{
	return g_Handler;
}
void CViewControlData::SetGlobalHandler(CDbHandlerInterface* pHandler)
{
	g_Handler = pHandler;
}

void FinishCommand()
{
	if (CViewControlData::GetGlobalHandler() && CViewControlData::GetGlobalHandler()->status >= 1)
	{
		osgGA::GUIEventAdapter* ea = new osgGA::GUIEventAdapter;
		ea->setEventType(osgGA::GUIEventAdapter::KEYDOWN);
		ea->setKey(osgGA::GUIEventAdapter::KEY_Escape);

		CViewControlData* pControlData = CViewControlData::GetGlobalHandler()->m_pOSG;
		if (pControlData)
			CViewControlData::GetGlobalHandler()->handle(*ea,  *(pControlData->mView) );
	}
}
void CViewControlData::SetCurCroodSys(const CString& strCur)
{
	if(strCurCoordSys != strCur)
	{
		strLastCoordSys = strCurCoordSys;
		strCurCoordSys = strCur;
	}
}
CString CViewControlData::GetCurCroodSys()
{
	return strCurCoordSys; 
}
void CViewControlData::SetLastCroodSys(const CString& strCur)
{
	strLastCoordSys = strCur;
}
CString CViewControlData::GetLastCroodSys()
{ 
	return strLastCoordSys;
}
void CViewControlData::OnIdle()
{
}
