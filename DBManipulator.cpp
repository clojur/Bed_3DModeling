#include "stdafx.h"
#include "OSGWraper.h"

#include "dbmanipulator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDb3DManipulator::CDb3DManipulator(osg::Camera* pCamera, CViewControlData* pOSG, int flags ):osgGA::TrackballManipulator(flags)
{
	m_pCamera=pCamera;
	myOSG      = pOSG;
	setAllowThrow(false);
}


bool CDb3DManipulator::performMovement()
{
	// return if less then two events have been added
	if( _ga_t0.get() == NULL || _ga_t1.get() == NULL )
		return false;

	// get delta time
	double eventTimeDelta = _ga_t0->getTime() - _ga_t1->getTime();
	if( eventTimeDelta < 0. )
	{
		OSG_WARN << "Manipulator warning: eventTimeDelta = " << eventTimeDelta << std::endl;
		eventTimeDelta = 0.;
	}

	// get deltaX and deltaY
	float dx = _ga_t0->getXnormalized() - _ga_t1->getXnormalized();
	float dy = _ga_t0->getYnormalized() - _ga_t1->getYnormalized();

	// return if there is no movement.
	if( dx == 0. && dy == 0. )
		return false;


	// call appropriate methods
	unsigned int buttonMask = _ga_t1->getButtonMask();
	int modKeyMask          = _ga_t1->getModKeyMask();

	if( buttonMask & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON )
	{
		if (myOSG->DoGetBehavorType() == 1)
		{
			return performMovementMiddleMouseButton(eventTimeDelta, dx, dy);
		}
		else if ( myOSG->DoGetBehavorType() == 2 )
		{
			return performMovementLeftMouseButton(eventTimeDelta, dx, dy);
		}
		else if(myOSG->DoGetBehavorType() ==3)
		{
 			double wheelZooomFactor = 0.01;
			if(dy>0)
				wheelZooomFactor=-0.01;		
			centerZoom( wheelZooomFactor );
			return true;
		}
		else
			return false;
	}
	else if ( buttonMask & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON  )
	{
		if ((modKeyMask& osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT) || myOSG->bRotate)
		{
			myOSG->bRotate = true;
			myOSG->viewType = 4;
			myOSG->SetCurCroodSys("W");

			return performMovementLeftMouseButton(eventTimeDelta, dx, dy);
		}
		else
		{
			return performMovementMiddleMouseButton(eventTimeDelta, dx, dy);
		}		
	}
	else if( buttonMask & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
	{
		return performMovementRightMouseButton( eventTimeDelta, dx, dy );
	}

	return false;
}

// doc in parent
bool CDb3DManipulator::performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
	if(myOSG && myOSG->bParalled)
	{
		double left;
		double right;
		double bottom;
		double top;
		double zNear;
		double zFar;
		m_pCamera->getProjectionMatrixAsOrtho(left,right,bottom,top,zNear,zFar );
		double width  =  0.5*(right-left);
		double height = 0.5*(top-bottom);
		m_pCamera->setProjectionMatrixAsOrtho(left - width*dx, right-width*dx, bottom- height*dy,top-height*dy,zNear,zFar  );
	}
	else
	{
		float scalex = -0.55 * _distance ;/** getThrowScale( eventTimeDelta );*/
		float scaley = -0.32 * _distance ;/** getThrowScale( eventTimeDelta );*/
		panModel( dx*scalex, dy*scaley );
	}
	return true;
}

void CDb3DManipulator::zoomModel( const float dy, bool pushForwardIfNeeded )
{
	if(myOSG == NULL)
		return;
	if(myOSG->bParalled)
	{
		double left;
		double right;
		double bottom;
		double top;
		double zNear;
		double zFar;
		m_pCamera->getProjectionMatrixAsOrtho(left,right,bottom,top,zNear,zFar );
		double width  =  0.5*(right-left);
		double height = 0.5*(top-bottom);

		if(width > 1.0e9 || height > 1.0e9)  //限制最大缩放范围
		{
			if(dy > 0)
				return;
		}
		if(width < 1.0e-9 || height < 1.0e-9)  //限制最大缩放范围
		{
			if(dy < 0)
				return;
		}
		CPoint pt=myOSG->GetCursorPos();

		double wx,wy;
		myOSG->GetWindowSize(wx,wy);

		float x1 = 2.0f*pt.x/wx -1.0f;
		float y1 = 2.0f*pt.y/wy -1.0f;
		double alfa = (1-dy);//与cad操作一致

		osg::Vec2 pt1, pt2;
		pt1.set( x1- alfa, y1- alfa);
		pt2.set( x1+alfa, y1+alfa);

		myOSG->BoxZoom(pt1,pt2  ); //以光标所在点为中心进行缩放 ，缩放完成后再把中心点平移到光标点；
		{		
			m_pCamera->getProjectionMatrixAsOrtho(left,right,bottom,top,zNear,zFar );
			width  =  0.5*(right-left);
			height = 0.5*(top-bottom);
			m_pCamera->setProjectionMatrixAsOrtho(left - width*x1, right-width*x1, bottom- height*y1,top-height*y1,zNear,zFar  );
		}
		return;
	}
	else
	{
		return osgGA::TrackballManipulator::zoomModel(dy,pushForwardIfNeeded);
	}

}
void CDb3DManipulator::centerZoom( const float dy )
{
	if(myOSG == NULL)
		return;
	if(myOSG->bParalled)
	{
		double wx,wy;
		myOSG->GetWindowSize(wx,wy);

		CPoint pt(0.5*wx,0.5*wy);

		osg::Vec2 pt1,pt2;
		float x1 = 2.0f*pt.x/wx -1.0f;
		float y1 = 2.0f*pt.y/wy -1.0f;
		double alfa = (1+dy);

		pt1.set( x1- alfa, y1- alfa);
		pt2.set( x1+alfa, y1+alfa);

		myOSG->BoxZoom(pt1,pt2  ); //以光标所在点为中心进行缩放 ，缩放完成后再把中心点平移到光标点；
		{
			double left;
			double right;
			double bottom;
			double top;
			double zNear;
			double zFar;
			m_pCamera->getProjectionMatrixAsOrtho(left,right,bottom,top,zNear,zFar );

			double width  =  0.5*(right-left);
			double height = 0.5*(top-bottom);
			m_pCamera->setProjectionMatrixAsOrtho(left - width*x1, right-width*x1, bottom- height*y1,top-height*y1,zNear,zFar  );
		}
		return;
	}
	else
	{
		return osgGA::TrackballManipulator::zoomModel(dy);	
	}
}

/** Make movement step of manipulator.
This method implements movement for right mouse button.*/
bool CDb3DManipulator::performMovementRightMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
	return false;
}

bool CDb3DManipulator::handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
	return false;
}

void CDb3DManipulator::DoRotate(int delta, float dx, float dy)
{
	if (myOSG->viewType ==4 )
		rotateWithFixedVertical( 0.01*dx, 0.002*dy);
} 