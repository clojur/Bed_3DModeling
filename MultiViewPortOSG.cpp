// MFC_OSG.cpp : implementation of the CDbCompositeViewOSG class
//
#include "stdafx.h"
#include <osgga/trackballmanipulator>
#include <osg/AutoTransform>
#include <osg/LightModel>

#include "CompositeView.h"
#include "osgtools.h"
#include "DbHandlerInterface.h"
#include "ViewControlData.h"
#include "multiviewportosg.h"



CDbCompositeViewOSG::CDbCompositeViewOSG(CView* pView) :CDbOSGManagerInterface(pView)
{
	// Create the viewer for this window
	mViewer = new osgViewer::CompositeViewer();

	m_backColor = osg::Vec4(1.0, 1.0, 1.0, 1.0);
	vptSpace = 5;
	bFullSceen = false;
	bInit = true;
	bResize = false;
	layoutPattern = 2;

	m_bInit = FALSE;
}

CDbCompositeViewOSG::~CDbCompositeViewOSG()
{
	for (int i = 0; i < addViews.size(); i++)
	{
		delete addViews[i];
	}
	addViews.clear();

	if (mViewer)
	{
		mViewer->setDone(true);

#ifdef _DEBUG
		//Sleep(1000);zmz
#else
		//Sleep(500);zmz
#endif

		mViewer->stopThreading();
		delete mViewer;
	}

	std::map<CString, CViewControlData*>::iterator it = m_ControlDataMap.begin();
	for (; it != m_ControlDataMap.end(); it++)
	{
		if (CViewControlData::GetGlobalHandler() )
		{
			if (CViewControlData::GetGlobalHandler()->m_pOSG == it->second)
			{
				CViewControlData::SetGlobalHandler(NULL);
			}
		}
		delete it->second;
	}
	m_ControlDataMap.clear();
}

void CDbCompositeViewOSG::OnUpdateDBObject(const CString &strObjectPath, int modifyType, const CString& subTitle, const std::vector<CString> &vecVariable)
{
	std::map<CString, CViewControlData*>::iterator it = m_ControlDataMap.begin();
	for (; it != m_ControlDataMap.end(); it++)
	{
		if (it->first != subTitle)
		{
			it->second->OnUpdateDBObject(strObjectPath, modifyType, vecVariable);
		}
	}
}

void CDbCompositeViewOSG::SetViewCurrent(osgViewer::View* pCurrentView)
{
	std::vector<osgViewer::View*> views;
	mViewer->getViews(views);

	int nCount = views.size();
	for (int i = 0; i < nCount; i++)
	{
		mViewer->getView(i)->setUserValue("focus", false);
	}
	if (pCurrentView)
	{
		pCurrentView->setUserValue("focus", true);
	}
	// Edit by hzz,debug状态，鼠标左键按住窗口分割线死机状态
	else
	{
		//ASSERT(FALSE);
	}
}

/////////*******************有很多概率崩溃
osgViewer::View* CDbCompositeViewOSG::GetCurrentView()
{
	if (!m_bInit)
		return NULL;
	
	unsigned int nCount = mViewer->getNumViews();	 
	for (int i = 0; i < nCount; i++)
	{
		osgViewer::View* pView = mViewer->getView(i);
		if (pView)
		{
			bool flag = false;
			pView->getUserValue("focus", flag);
			if (flag)
			{
				return pView;
			}
		}		
	}
	return NULL;
}
osgViewer::ViewerBase* CDbCompositeViewOSG::GetCurrentViewer()
{
	return mViewer;
}

///////////////


int LayOutCount(int num)
{
	int total = 1;
	int i = 1;
	while (total < num)
	{
		++i;
		total = i*i;
	}
	return i;
}

void  GetIndex(int& nRow, int& nCol, std::vector<double> xPos, std::vector<double> yPos, double x, double y)
{
	std::sort(xPos.begin(), xPos.end());
	xPos.erase(unique(xPos.begin(), xPos.end()), xPos.end());

	int totalCol = xPos.size();

	nCol = -1;
	for (int i = 0; i< xPos.size(); i++)
	{
		if (x > xPos[i])
		{
			nCol = i;
			break;
		}
	}
	if (nCol == -1)
		nCol = xPos.size();

	std::sort(yPos.begin(), yPos.end());
	yPos.erase(unique(yPos.begin(), yPos.end()), yPos.end());
	int totalRow = yPos.size();
	nRow = -1;
	for (int i = 0; i< yPos.size(); i++)
	{
		if (y > yPos[i])
		{
			nRow = i;
			break;
		}
	}
	if (nRow == -1)
		nRow = yPos.size();
}

void CalPos(int nRow, int nCol, int indexRow, int indexCol, double& xPos, double& yPos)
{

}

//splitter
void CDbCompositeViewOSG::ResetViewSize(double cx, double cy, double dx, double dy, bool bVert)
{
	if (bVert)
	{
		if (dy > 0)
		{
			std::map<CString, CViewControlData*>::iterator it = m_ControlDataMap.begin();
			for (; it != m_ControlDataMap.end(); it++)
			{
				CViewControlData* pControlData = it->second;
				if (pControlData)
				{
					if ((pControlData->mViewSet.y - cy) < 0.01)
					{
						pControlData->mViewSet.y += dy;
						pControlData->mViewSet.height -= dy;
					}
					else if ((pControlData->mViewSet.y + pControlData->mViewSet.height - cy) < 0.01)
					{
						pControlData->mViewSet.height += dy;
					}
				}
			}
		}
		else
		{
			std::map<CString, CViewControlData*>::iterator it = m_ControlDataMap.begin();
			for (; it != m_ControlDataMap.end(); it++)
			{
				CViewControlData* pControlData = it->second;
				if (pControlData)
				{
					if ((pControlData->mViewSet.y - cy) < 0.01)
					{
						pControlData->mViewSet.y += dy;
						pControlData->mViewSet.height += dy;
					}
					else if ((pControlData->mViewSet.y + pControlData->mViewSet.height - cy) < 0.01)
					{
						pControlData->mViewSet.height -= dy;
					}
				}
			}
		}
	}
	else
	{
		if (dx > 0)
		{
			std::map<CString, CViewControlData*>::iterator it = m_ControlDataMap.begin();
			for (; it != m_ControlDataMap.end(); it++)
			{
				CViewControlData* pControlData = it->second;
				if (pControlData)
				{
					if ((pControlData->mViewSet.x - cx) < 0.01)
					{
						pControlData->mViewSet.x += dx;
						pControlData->mViewSet.width -= dx;
					}
					else if ((pControlData->mViewSet.x + pControlData->mViewSet.width - cx) < 0.01)
					{
						pControlData->mViewSet.width += dx;
					}
				}
			}
		}
		else
		{
			std::map<CString, CViewControlData*>::iterator it = m_ControlDataMap.begin();
			for (; it != m_ControlDataMap.end(); it++)
			{
				CViewControlData* pControlData = it->second;
				if (pControlData)
				{
					if ((pControlData->mViewSet.x - cx) < 0.01)
					{
						pControlData->mViewSet.x -= dx;
						pControlData->mViewSet.width += dx;
					}
					else if ((pControlData->mViewSet.x + pControlData->mViewSet.width - cx) < 0.01)
					{
						pControlData->mViewSet.width -= dx;
					}
				}
			}
		}
	}


 	std::map<CString, CViewControlData*>::iterator it = m_ControlDataMap.begin();
	for (; it != m_ControlDataMap.end(); it++)
	{
		CViewControlData* pControlData = it->second;
		if (pControlData)
		{
			pControlData->SetViewPort(3, vptSpace, width, height);
		}
	}	
}

void CDbCompositeViewOSG::ResetSize()
{
	std::map<int, CViewControlData*> posArray;
	int nCount = m_ControlDataMap.size();
	int nRowCount = 1;
	int nColCount = 1;

	//if (!CMiscTools::LayOutCount(nCount, layoutPattern, nRowCount, nColCount))
	//	return;
	//
	int nRow = 0;
	int nCol = 0;
	double dWidth = 1.0 / nColCount;
	double dHeight = 1.0 / nRowCount;


	std::map<CString, CViewControlData*>::iterator it = m_ControlDataMap.begin();

	int i = 0;
	for (; it != m_ControlDataMap.end(); it++)
	{
		CViewControlData* pControlData = it->second;
		if (pControlData)
		{
			nRow = i / nColCount;
			nCol = i - nRow*nColCount;
			if (i == nCount - 1)
			{
				dWidth *= (nColCount - nCol);
			}

			pControlData->bfactor = true;
			pControlData->mViewSet.x = 1.0*nCol / nColCount;
			pControlData->mViewSet.y = 1 - 1.0*(nRow + 1) / nRowCount;
			pControlData->mViewSet.width = dWidth;
			pControlData->mViewSet.height = dHeight;

			pControlData->SetViewPort(3, vptSpace, width, height);
			i++;
		}
	}
}


void CDbCompositeViewOSG::OnSize(int  cx, int  cy)
{
	if (abs(cx - width) < 5 && abs(cy - height) < 5)
		return;
	width = cx;
	height = cy;


	std::map<CString, CViewControlData*>::iterator it = m_ControlDataMap.begin();
	int i = 0;
	for (; it != m_ControlDataMap.end(); it++)
	{
		CViewControlData* pControlData = it->second;
		if (pControlData)
		{
			pControlData->SetViewPort(3, vptSpace, width, height);
		}
	}
}


CViewControlData* CDbCompositeViewOSG::FindControlData(const CString& strTitle)
{
	CString newTitle(strTitle);
	if (newTitle.IsEmpty())
	{
		osgViewer::View* pView = GetCurrentView();
		if (pView)
			newTitle = pView->getName().c_str();
		else
			return NULL;
	}

	std::map<CString, CViewControlData*>::iterator it = m_ControlDataMap.find(newTitle );
	if (it != m_ControlDataMap.end())
	{
		return it->second;
	}
	return NULL;
}


void CDbCompositeViewOSG::InitOSG()
{
	InitCameraConfig();
}

void CDbCompositeViewOSG::InitCameraConfig(void)
{
	RECT rect;

	::GetWindowRect(m_hWnd, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	// Init the GraphicsContext Traits
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

	// Init the Windata Variable that holds the handle for the Window to display OSG in.
	osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

	// Setup the traits parameters
	traits->x = 0;
	traits->y = 0;
	traits->width = width;
	traits->height = height;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->setInheritedWindowPixelFormat = true;
	traits->inheritedWindowData = windata;
	traits->useCursor = true;
	if (busedMulsample)
		traits->samples = 4;
	else
		traits->samples = 0;

	// Create the Graphics Context
	gc = osg::GraphicsContext::createGraphicsContext(traits.get());
	gc->setClearColor(m_backColor);
	gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//default create defult view
	{
		CViewControlData tempData;
		tempData.mViewSet.strTitle = "";
		tempData.mViewSet.strPath = "first_view";
		tempData.bfactor = true;
		tempData.mViewSet.x = 0;
		tempData.mViewSet.y = 0;
		tempData.mViewSet.width = 1;
		tempData.mViewSet.height = 1;
		DoAddView(&tempData, TRUE);
	}

	mViewer->setKeyEventSetsDone(false);

	mViewer->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);


}

void CDbCompositeViewOSG::PreFrameUpdate()
{
	//使用定制光标
	if (bInit)
	{
		bInit = false;
		osgViewer::ViewerBase::Windows windows;
		mViewer->getWindows(windows);
		if (windows.size() > 0)
		{
			windows[0]->setCursor(osgViewer::GraphicsWindow::MouseCursor::InheritCursor);
		}
	}

	//delete view
	for (int i = 0; i < delViews.size(); i++)
	{
		std::map<CString, CViewControlData*>::iterator it = m_ControlDataMap.begin();
		for (; it != m_ControlDataMap.end(); it++)
		{
			CViewControlData* pData = it->second;
			if (pData == delViews[i])
			{
				mViewer->removeView(pData->mView);

				delete pData;
				m_ControlDataMap.erase(it);
				break;
			}
		}
	}
	if (delViews.size() > 0)
		ResetSize();

	delViews.clear();

	std::vector<osgViewer::View*> Views;
	mViewer->getViews(Views);
	if (Views.size() == 0)
	{
		CViewControlData tempData;

		tempData.mViewSet.strPath = "first_view";
		tempData.bfactor = true;
		tempData.mViewSet.x = 0;
		tempData.mViewSet.y = 0;
		tempData.mViewSet.width = 1;
		tempData.mViewSet.height = 1;
		DoAddView(&tempData, TRUE);
	}
}

void CDbCompositeViewOSG::PostFrameUpdate()
{
	//add view
	for (int i = 0; i < addViews.size(); i++)
	{
		DoAddView(addViews[i]);
	}
	for (int i = 0; i < addViews.size(); i++)
	{
		CViewControlData*  pControlData = addViews[i]; 
		if (pControlData->mViewSet.bDefaultFocus)
		{		 
			SetViewCurrent(pControlData->mView);
		}
	}

	addViews.clear();

	//设置为上层窗口
	for (int k = 0; k < mViewer->getNumViews(); ++k)
	{
		mViewer->getView(k)->getCamera()->setRenderOrder(osg::Camera::PRE_RENDER);
	}
	osgViewer::View* pCurView = GetCurrentView();
	if (pCurView)
	{
		pCurView->getCamera()->setRenderOrder(osg::Camera::POST_RENDER);
	}

	if (bResize)
	{
		ResetSize();
		bResize = false;
	}

	m_bInit = TRUE;

}





WndOperationHandler::TOUCHPOINT WndOperationHandler::getTouchPoint(float x, float y, osg::Viewport* viewport)
{
	WndOperationHandler::TOUCHPOINT tp = TP_UNKNOW;

	if (NULL == viewport)
		return tp;

	double range = 5;

	float delta_x = x - viewport->x();
	float delta_y = y - viewport->y();

	float bottom = 0;
	float top = viewport->height();

	float left = 0;
	float right = viewport->width();

	if (fabs(delta_x - left) < range)
	{
		if (delta_y - bottom < range)
		{
			//左下点
			std::cout << "bottomleft" << std::endl;
			tp = TP_BOTTOMLEFT;
		}
		else if (fabs(delta_y - top) < range)
		{
			//左上点
			std::cout << "topleft" << std::endl;
			tp = TP_TOPLEFT;
		}
		else if (delta_y < top && delta_y > bottom)
		{
			//左边线
			std::cout << "left" << std::endl;
			tp = TP_LEFT;
		}
	}
	else if (fabs(delta_x - right) < range)
	{
		if (fabs(delta_y - bottom) < range)
		{
			//右下点
			std::cout << "bottomright" << std::endl;
			tp = TP_BOTTOMRIGHT;
		}
		else if (fabs(delta_y - top) < range)
		{
			//右上点
			std::cout << "topright" << std::endl;
			tp = TP_TOPRIGHT;
		}
		else if (delta_y < top && delta_y > bottom)
		{
			//右边线
			std::cout << "right" << std::endl;
			tp = TP_RIGHT;
		}
	}
	else if (delta_x > left && delta_x < right)
	{
		if (fabs(delta_y - bottom) < range)
		{
			std::cout << "bottom" << std::endl;
			tp = TP_BOTTOM;
		}
		else if (fabs(delta_y - top) < range)
		{
			std::cout << "top" << std::endl;
			tp = TP_TOP;
		}
	}
	return tp;
}

bool valid_less(double& v1, double value = 0)
{
	if (v1 < value)
	{
		v1 = value;
		return true;
	}
	return false;
}

bool valid_greate(double& v1, double value = 500)
{
	if (v1 > value)
	{
		v1 = value;
		return true;
	}
	return false;
}


double WndOperationHandler::GetX(const osgGA::GUIEventAdapter& ea, osgViewer::View* viewer)
{
	double x = ea.getX();
	if (viewer)
	{
		x -= viewer->getCamera()->getViewport()->x();
	}
	return x;
}

double WndOperationHandler::GetY(const osgGA::GUIEventAdapter& ea, osgViewer::View* viewer)
{
	double y = ea.getY();
	if (viewer)
	{
		y -= viewer->getCamera()->getViewport()->y();
	}
	return y;
}

bool WndOperationHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	bool bResult = false;

	osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
	if (NULL == viewer) return bResult;
	if (!m_pOSG)
		return false;

	osg::Viewport* viewport = viewer->getCamera()->getViewport();
	switch (ea.getEventType())
	{
	case osgGA::GUIEventAdapter::PUSH:
 		if (ea.getButtonMask() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			std::cout << "lbuttondown" << std::endl;
			if (_tp != TP_UNKNOW)
			{
				bResult = TRUE;
				m_pOSG->SetBehavorType(4);
			
				double x = ea.getXnormalized();
				double y = ea.getYnormalized();

				cx = 0.5+0.5*x;
				cy = 0.5 + 0.5*y;
			}
		}
		break;
	case osgGA::GUIEventAdapter::RELEASE:
	{
		if (_tp != TP_UNKNOW)
		{
			_tp = TP_UNKNOW;
			m_pOSG->SetCursorType(OSG_NORMAL_CUSSOR);
			bResult = true;
			m_pOSG->SetBehavorType(0);
		}
		else
		{
			bResult = false;
		}
	}
	break;
	case osgGA::GUIEventAdapter::DRAG:
	{
		if (ea.getButtonMask() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			double x = ea.getX();
			double y = ea.getY();


			osgViewer::ViewerBase::Windows windows;
			m_pOSG->mViewer->getWindows(windows);
			CRect rect;
			windows[0]->getWindowRectangle((int&) rect.left, (int&) rect.top, (int&) rect.right, (int&) rect.bottom);
			POINT Pt;
			Pt.x = x;
			Pt.y = y;
			rect.left = 0;
			rect.top = 0;
			if (!rect.PtInRect(Pt))
				break;

			x = ea.getXnormalized();
			y = ea.getYnormalized();

			double rx = viewport->x();
			double ry = viewport->y();
			double rw = viewport->width();
			double rh = viewport->height();

			//使用鼠标按下时接触点接触的位置
			bool bModify = true;//保存视图是否被修改

			bool bVert = false;
			double delta = 0;
			switch (_tp)
			{
			case TP_BOTTOMLEFT:
			{
				double deltax = x - rx;
				double deltay = y - ry;

				rx = x;
				ry = y;
				rw -= deltax;
				rh -= deltay;
			}
			break;
			case TP_TOPRIGHT:
			{
				double deltax = x - (rx + rw);
				double deltay = y - (ry + rh);

				rw += deltax;
				rh += deltay;
			}
			break;
			case TP_BOTTOMRIGHT:
			{
				double deltax = x - (rx + rw);
				double deltay = y - ry;

				ry += deltay;
				rw += deltax;
				rh -= deltay;
			}
			break;
			case TP_TOPLEFT:
			{
				double deltax = x - rx;
				double deltay = y - (ry + rh);

				rx += deltax;
				rw -= deltax;
				rh += deltay;
			}
			break;
			case TP_BOTTOM:
			{
				double deltay = y - ry;
				ry += deltay;
				rh -= deltay;

				//
				bVert = true;
				delta = deltay;
			}
			break;
			case TP_TOP:
			{
				double deltax = x - _x;
				double deltay = y - _y;
				rx = _vx + deltax;
				ry = _vy + deltay;

				//
				bVert = true;
				delta = deltay;

			}
			break;
			case TP_LEFT:
			{
				double deltax = x - rx;
				rx += deltax;
				rw -= deltax;

				//
				bVert = false;
				delta = deltax;
			}
			break;
			case TP_RIGHT:
			{
				double deltax = x - (rx + rw);
				rw += deltax;

				bVert = false;
				delta = deltax;
			}
			break;
			case TP_UNKNOW:
			{
				bModify = false;
			}
			break;
			default:
				bModify = false;
				break;
			}

			if (bModify)
			{
				//修改了视图的大小，返回true 禁用旋转操作
				bResult = true;
				//视图的最小尺寸 100*100
			/*	if (valid_less(rw, 100))
				{
					rx = viewport->x();
				}
				if (valid_less(rh, 100))
				{
					ry = viewport->y();
				}*/
		//		viewer->getCamera()->setViewport(rx, ry, rw, rh);

				double deltax = 0.5+0.5*x - cx;
				double deltay = 0.5+0.5*y - cy;

				m_pOSG->ResetViewSize(cx,cy,deltax,deltay, bVert);

				cx = 0.5 + 0.5*x;
				cy = 0.5 + 0.5*y;
			}
		}
		if (_tp != TP_UNKNOW)
		{
			bResult = TRUE;
		}
		else
		{
			bResult = false;
		}
	}
	break;
	case osgGA::GUIEventAdapter::MOVE:
	{
		if (m_pOSG->GetBehavorType() != 0)
		{
			return false;
		}
		_vx = viewport->x();
		_vy = viewport->y();

		_x = ea.getX();
		_y = ea.getY();

		_tp = getTouchPoint(_x, _y, viewport);
		SetCursor(_tp);
		if (_tp != TP_UNKNOW)
		{
			bResult = TRUE;

			double x = ea.getXnormalized();
			double y = ea.getYnormalized();

			cx = 0.5 + 0.5*x;
			cy = 0.5 + 0.5*y;
		}
	}
	break;
	default:
		break;
	}

	return bResult;
}


void WndOperationHandler::SetCursor(WndOperationHandler::TOUCHPOINT tp)
{
	osgViewer::GraphicsWindow::MouseCursor cursor = osgViewer::GraphicsWindow::LeftArrowCursor;

	bool bvalid = true;

	switch (tp)
	{
	case TP_BOTTOMLEFT:
	case TP_TOPRIGHT:
	{
		cursor = osgViewer::GraphicsWindow::TopRightCorner;
	}
	break;
	case TP_BOTTOMRIGHT:
	case TP_TOPLEFT:
	{
		cursor = osgViewer::GraphicsWindow::BottomRightCorner;
	}
	break;
	case TP_BOTTOM:
	{
		cursor = osgViewer::GraphicsWindow::UpDownCursor;
	}
	break;
	case TP_TOP:
	{
		cursor = osgViewer::GraphicsWindow::SprayCursor;
	}
	break;
	case TP_LEFT:
	case TP_RIGHT:
	{
		cursor = osgViewer::GraphicsWindow::LeftRightCursor;
	}
	break;
	case TP_UNKNOW:
	{
		bvalid = false;
	}
	break;
	default:
	{
		bvalid = false;
	}
	break;
	}

	if (bvalid)
		m_pOSG->SetCursorType(cursor);
	else
	{
		m_pOSG->SetCursorType(OSG_NORMAL_CUSSOR);
	}

	//	gcWnd->setCursor(cursor);
}

////////////////////
void CDbCompositeViewOSG::DelView(CViewControlData* pControlData)
{
	delViews.push_back(pControlData);
	//
	if (!pControlData->mViewSet.strPath.IsEmpty() )
	{
		std::vector<CString> delObjes;
		delObjes.push_back( (pControlData->mViewSet.strPath));

		pControlData->ImplementDelDatas(delObjes);
	}
}



//
#include <osg/Program>
#include <osg/Shader>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>



float random(float min, float max)
{
	return min + (max - min)*(float) rand() / (float) RAND_MAX;
}

osg::Geometry* createLineGeode(const std::vector<osg::Vec3>&botPTs, const std::vector<osg::Vec3>&topPTs, osg::Vec4 color)
{
	osg::Geometry* pGeometry = new osg::Geometry();

	osg::Vec3Array *pyramidVertices = new osg::Vec3Array;
	pGeometry->setVertexArray(pyramidVertices);

	osg::Vec3Array *pPosArray = new osg::Vec3Array;
	pPosArray->setName("posam");

	//pPosArray->setBinding(osg::Array::BIND_PER_VERTEX);
	//pPosArray->setNormalize(false);
	pGeometry->setVertexAttribArray(6, pPosArray);
	pGeometry->setVertexAttribNormalize(6, false);
	pGeometry->setVertexAttribBinding(6, osg::Geometry::BIND_PER_VERTEX);


	int nCount = botPTs.size();

	for (int i = 0; i < botPTs.size(); i++)
	{
		pyramidVertices->push_back(botPTs[i]);
		pPosArray->push_back(osg::Vec3(random(0, 3), random(0, 1), random(1, 5)));
	}
	for (int i = 0; i < topPTs.size(); i++)
	{
		pyramidVertices->push_back(topPTs[i]);
		pPosArray->push_back(osg::Vec3(random(0, 3), random(0, 1), random(1, 5)));
	}

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(color);
	pGeometry->setColorArray(colors);
	pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);


	osg::ref_ptr<osg::DrawElementsUShort> elements = new osg::DrawElementsUShort(osg::PrimitiveSet::QUAD_STRIP);
	std::vector<GLushort> idLines;
	for (int i = 0; i < nCount; i++)
	{
		elements->push_back(i);
		elements->push_back(i + nCount);
	}
	pGeometry->addPrimitiveSet(elements);

	pGeometry->setUseVertexBufferObjects(false);
	pGeometry->setDataVariance(osg::Object::DYNAMIC);

	return pGeometry;
}


class UniformVarying : public osg::Uniform::Callback
{
	virtual void operator () (osg::Uniform* uniform, osg::NodeVisitor* nv)
	{
		const osg::FrameStamp* fs = nv->getFrameStamp();
		float time = fs->getSimulationTime();
		uniform->set(time);
	}
};


class UniformVarying2 : public osg::Uniform::Callback
{
	virtual void operator () (osg::Uniform* uniform, osg::NodeVisitor* nv)
	{
		uniform->set(10);
	}
	bool bAdd;
	float initTime;
	int nCount;
};

struct DrawableUpdateCallback : public osg::Drawable::UpdateCallback
{
public:
	virtual void update(osg::NodeVisitor*, osg::Drawable* drawable)
	{
		if (pView)
		{
			bool flag = false;
			pView->getUserValue("focus", flag);

			double x = pView->getCamera()->getViewport()->x();
			double y = pView->getCamera()->getViewport()->y();
			double width = pView->getCamera()->getViewport()->width();
			double height = pView->getCamera()->getViewport()->height();

			if (abs(dx - width) < 2 && abs(dy - height) < 2 && oldFlag == flag && !pControlData->bReforceUpdateCallBack)
			{
				return;
			}

			pControlData->bReforceUpdateCallBack = FALSE;
			oldFlag = flag;

			////////////////////////////////////////////
			dx = width;
			dy = height;
			if (pCamera)
			{
				pCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, dx, 0, dy));
			}
			if (pTitle)
			{
				pTitle->setPosition(osg::Vec3(dx*0.5, dy - 10, 0));
				{
					WCHAR szwChar[512];
					int nChar = MultiByteToWideChar(CP_ACP, 0, pControlData->mViewSet.strTitle , -1, szwChar, 512);
					pTitle->setText(szwChar);
				}
			}
			osg::Geometry* pGeometry = (osg::Geometry*)drawable;

			osg::Vec4Array *pColors = (osg::Vec4Array*)pGeometry->getColorArray();
			if (pColors && pColors->size() == 1)
			{
				osg::Vec4 color = backColor*0.9;
				if (flag)
					color = blueColor;
				pColors->at(0).set(color.x(), color.y(), color.z(), color.w());
				pColors->dirty();
			}

			osg::Vec3Array *pVertices = (osg::Vec3Array*)pGeometry->getVertexArray();
			if (pVertices && pVertices->size() == 4)
			{
				pVertices->at(0).set(osg::Vec3(1, 1, 0));
				pVertices->at(1).set(osg::Vec3(dx-2, 1, 0));
				pVertices->at(2).set(osg::Vec3(dx-2, dy-2, 0));
				pVertices->at(3).set(osg::Vec3(1, dy-2, 0));

				pGeometry->dirtyBound();
				pVertices->dirty();
			}
		}
	}
	osgViewer::View* pView;
	osgViewer::CompositeViewer*   pViewer;
	osg::Camera*        pCamera;
	osgText::Text* pTitle;
	CViewControlData* pControlData;
	double dx;
	double dy;
	bool oldFlag;
};


#include <osg/LineWidth>
#include <osgDB/ReadFile>
osg::Camera*  CreateRect(CViewControlData* pControlData, osg::Vec4  color, osgViewer::View* pView, osgViewer::CompositeViewer* pViewer)
{
	osg::Geode* pGeode = new osg::Geode;
	pGeode->setName("CreateRectGeode");

	osg::Geometry* pGeometry = new osg::Geometry();
	pGeometry->setName("CreateRectGeometry");
	pGeode->addDrawable(pGeometry);


	osg::Vec3Array *pyramidVertices = new osg::Vec3Array;
	pGeometry->setVertexArray(pyramidVertices);
	pyramidVertices->push_back(osg::Vec3(0, 0, 0));
	pyramidVertices->push_back(osg::Vec3(200, 0, 0));
	pyramidVertices->push_back(osg::Vec3(200, 200, 0));
	pyramidVertices->push_back(osg::Vec3(0, 200, 0));

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(color); //索引0 红色 
	pGeometry->setColorArray(colors);
	pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::DrawElementsUInt* pyramidBase = NULL;
	pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
	pyramidBase->push_back(0);
	pyramidBase->push_back(1);
	pyramidBase->push_back(2);
	pyramidBase->push_back(3);

	pGeometry->addPrimitiveSet(pyramidBase);
	pGeometry->setUseVertexBufferObjects(true);
	pGeometry->setDataVariance(osg::Object::DYNAMIC);


	double dx;
	double dy;
	pControlData->GetWindowSize(dx, dy);
	osgText::Text* pText = create3DTextNoScale(pControlData->GetCnTextFont(), osg::Vec3(0.5*dx, dy - 10, 0), pControlData->mViewSet.titleSize, pControlData->mViewSet.strTitle, osg::Vec4(1.0, 0.0, 0.0, 1.0), osgText::Text::CENTER_TOP, FALSE);
	pGeode->addDrawable(pText);

	osg::Camera* pHUDCamera = createHUDCamera(TRUE, pControlData);

	DrawableUpdateCallback* pDrawCallBack = new DrawableUpdateCallback;
	pDrawCallBack->pView = pView;
	pDrawCallBack->pViewer = pViewer;
	pDrawCallBack->pCamera = pHUDCamera;
	pDrawCallBack->pTitle = pText;
	pDrawCallBack->dx = 0;
	pDrawCallBack->dy = 0;
	pDrawCallBack->oldFlag = false;

	pDrawCallBack->pControlData = pControlData;

	pGeometry->setUpdateCallback(pDrawCallBack);

	osg::StateSet* set = pGeometry->getOrCreateStateSet();	

	osg::LineWidth* linewidth = new osg::LineWidth();
	linewidth->setWidth(3);
	set->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
 	set->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);


	pHUDCamera->addChild(pGeode);
	return pHUDCamera;
}

//设置视图之间的间隙
void CDbCompositeViewOSG::SetSpace(int nSpace)
{
	if (nSpace > 0 && nSpace < 50)
		vptSpace = nSpace;
}



void CDbCompositeViewOSG::InitBoundingBoxEffect(CViewControlData* pControlData, osgViewer::View* view)
{
	if (pControlData)
	{
		osg::Camera* pBoundingBox = CreateRect(pControlData, greenColor, view, mViewer);
		pBoundingBox->setName("ViewBoxCamera");
		pControlData->validGraph->addChild(pBoundingBox);

	}
}




void CDbCompositeViewOSG::DoAddView(CViewControlData* pControlData, bool bTemp)
{
	if (pControlData)
	{
		double viewWidth = 0;
		double viewHeight = 0;
		if (pControlData->bfactor)
		{
			viewWidth = pControlData->mViewSet.width*width - 2 * vptSpace - pControlData->GetOffset();
			viewHeight = pControlData->mViewSet.height*height - 2 * vptSpace;
		}
		else
		{
			viewWidth = pControlData->mViewSet.width - 2 * vptSpace - pControlData->GetOffset();
			viewHeight = pControlData->mViewSet.height - 2 * vptSpace;
		}
		if (!bTemp)
		{
			std::vector<osgViewer::View*> views;
			mViewer->getViews(views);
			int nCount = views.size();
			for (int i = 0; i < nCount; i++)
			{
				osgViewer::View* pView = mViewer->getView(i);
				if (pView->getName() == "first_view")
				{
					mViewer->removeView(pView);
					break;
				}
			}
		}

		osgViewer::View* view = new osgViewer::View;
		mViewer->addView(view);

		view->setName(pControlData->mViewSet.strPath);
		if (!bTemp)
		{
			m_ControlDataMap[pControlData->mViewSet.strPath] = pControlData;
		}

		 
		pControlData->m_Osg = this;
		pControlData->mView = view;
		pControlData->mViewer = mViewer;


		osg::Camera* camera = view->getCamera();
		camera->setName(pControlData->mViewSet.strPath);


		pControlData->SetViewPort(1, vptSpace, width, height);

		camera->setGraphicsContext(gc.get());

		camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		camera->setClearColor(pControlData->backColor);


		if (!pControlData->bParalled)
			camera->setProjectionMatrixAsPerspective(35.0f, viewWidth / viewHeight, 0.1, 10000.0);
		else
		{
			double ratio = viewWidth / viewHeight;
			double width1 = viewWidth*1.5;
			double height1 = width1 / ratio;
			camera->setProjectionMatrixAsOrtho(-width1, width1, -height1, height1, 1.0, 10000.0);
		}
		view->setCamera(camera);

		// add the state manipulator 
		view->setCameraManipulator(new  osgGA::TrackballManipulator);


		osg::CullStack::CullingMode cullingMode = camera->getCullingMode();
		cullingMode &= ~(osg::CullStack::SMALL_FEATURE_CULLING);
		camera->setCullingMode(cullingMode);

		view->setLightingMode(osgViewer::View::HEADLIGHT);
		osg::Light* pLight = view->getLight();
		pLight->setAmbient(osg::Vec4(0.10f, 0.1f, 0.10f, 1.0f));
		pLight->setDiffuse(osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f));
		pLight->setSpecular(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		if (camera)
		{
			osg::StateSet* globalStateset = camera->getOrCreateStateSet();
			if (globalStateset)
			{
				osg::LightModel* pLightModel = (osg::LightModel*)globalStateset->getAttribute(osg::StateAttribute::LIGHTMODEL);
				if (pLightModel)
				{
					pLightModel->setTwoSided(TRUE);
					pLightModel->setAmbientIntensity(osg::Vec4(0.3f, 0.3f, 0.3f, 1.0f));
				}
			}
		}

		//初始化场景图数据
		if (!bTemp)
		{

	//		view->addEventHandler( new WndOperationHandler(this, view, mViewer) ); 

			pControlData->InitSceneGraph();
			pControlData->InitPickManipulator();

			InitBoundingBoxEffect(pControlData, view);

			view->setSceneData(pControlData->mRoot.get());
		}
		else
		{
			view->setSceneData(NULL);
		}

		pControlData->CreateSreen();

		if (!bTemp)
			pControlData->setUpdateCallback(new CUpdateCallback(pControlData));
	}
}


void CDbCompositeViewOSG::AddView(CViewControlData* pControlData)
{
	if (pControlData->mViewSet.strPath.IsEmpty())
		pControlData->mViewSet.strPath = pControlData->mViewSet.strTitle;

	addViews.push_back(pControlData);
}


void CDbCompositeViewOSG::DeleteAllView()
{
	std::map<CString, CViewControlData*>::iterator it = m_ControlDataMap.begin();
	for (; it != m_ControlDataMap.end(); it++)
	{
		CViewControlData* pData = it->second;
		delViews.push_back(pData);
	}
}

osgViewer::View* CDbCompositeViewOSG::GetCurosrView(POINT point)
{
	if (m_pParentView)
	{
		CRect viewRect;
		m_pParentView->GetWindowRect(viewRect);
		point.y -= viewRect.top;
		point.x -= viewRect.left;
		point.y = viewRect.Height() - point.y;
	}

	std::vector<osgViewer::View*> views;
	mViewer->getViews(views);
	int nCount = views.size();

	views.clear();

	for (int i = 0; i< nCount; i++)
	{
		osgViewer::View* pView = mViewer->getView(i);

		double x = pView->getCamera()->getViewport()->x();
		double y = pView->getCamera()->getViewport()->y();
		double width = pView->getCamera()->getViewport()->width();
		double height = pView->getCamera()->getViewport()->height();

		if (point.x >x && point.y > y && point.x < x + width && point.y < y + height)
		{
			bool flag = false;
			pView->getUserValue("focus", flag);
			if (flag)
			{
				return pView;
			}
			else
			{
				views.push_back(pView);
			}
		}
	}
	if (views.size() > 0)
	{
		return views[0];
	}

	return NULL;
}


void CDbCompositeViewOSG::AutoCalSize(int nBlockCount, std::vector<viewSet>& newValues)
{
	int nRowCount = LayOutCount((int) nBlockCount);
	if (nRowCount <= 0)
	{//一行也没有
		return;
	}
	int nColCount = nRowCount*nRowCount >= (int) nBlockCount ? nRowCount : nRowCount + 1;
	for (int i = 0; i < nBlockCount; ++i)
	{
		viewSet oneView;
		int nRow = 0;
		int nCol = 0;
		int row = 1;
		int col = 1;
		while (true)
		{
			if ((int) i < row*nRowCount)
			{
				nRow = row - 1;
				nCol = i - nRow*nRowCount;
				break;
			}
			row++;
		}

		oneView.x = 1.0*nCol / nColCount;
		oneView.y = 1 - 1.0*(nRow + 1) / nRowCount;
		oneView.width = i != nBlockCount - 1 ? 1.0 / nColCount : (nColCount - nCol)*1.0 / nColCount;
		oneView.height = 1.0 / nRowCount;

		newValues.push_back(oneView);
	}
}

void CDbCompositeViewOSG::DoMultiViewMax()
{
	bFullSceen = !bFullSceen;
	if (bFullSceen)
	{
		std::map<CString, CViewControlData*>::iterator it = m_ControlDataMap.begin();
		for (; it != m_ControlDataMap.end(); it++)
		{
			oldViewStatus[it->first] = it->second->mViewSet;
		}
		int nCont = fullSceenViews.size();
		if (nCont == 0)
		{
			osgViewer::View* pCur = GetCurrentView();
			if (pCur)
			{
				fullSceenViews.push_back(FindControlData(pCur->getName().c_str()));
			}
		}

		nCont = fullSceenViews.size();
		if (nCont > 0)
		{
			//minize
			std::vector<osgViewer::View*> views;
			mViewer->getViews(views);
			for (int i = 0; i < views.size(); i++)
			{
				osgViewer::View* pView = views[i];
				if (pView)
				{
					pView->getCamera()->setViewport(width * 2, height * 2, 100, 100);
				}
			}
			//end minize		
			std::vector<viewSet> newValues;
			AutoCalSize(nCont, newValues);
			for (int i = 0; i < nCont; i++)
			{
				viewSet viewset = newValues[i];
				CViewControlData* pControlData = fullSceenViews[i];
				if (pControlData)
				{
					pControlData->mViewSet.height = viewset.height;
					pControlData->mViewSet.width = viewset.width;
					pControlData->mViewSet.x = viewset.x;
					pControlData->mViewSet.y = viewset.y;
					pControlData->SetViewPort(3, vptSpace, width, height);
				}
			}
		}
	}
	else
	{
		std::map<CString, CViewControlData*>::iterator it = m_ControlDataMap.begin();
		for (; it != m_ControlDataMap.end(); it++)
		{
			CViewControlData* pControlData = it->second;
			if (pControlData)
			{
				std::map<CString, viewSet>::iterator it2 = oldViewStatus.find(pControlData->mViewSet.strPath);
				if (it2 != oldViewStatus.end())
				{
					pControlData->mViewSet.x = it2->second.x;
					pControlData->mViewSet.y = it2->second.y;
					pControlData->mViewSet.width = it2->second.width;
					pControlData->mViewSet.height = it2->second.height;
				}
				pControlData->SetViewPort(3, vptSpace, width, height);
			}
		}
		fullSceenViews.clear();
	}
}
 