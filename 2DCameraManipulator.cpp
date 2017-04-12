#include "stdafx.h" 
#include <osg/Notify>

#include "PickHandler.h"
#include "ViewControlData.h"
#include "2DCameraManipulator.h"
#include "Db3DModelingUnitity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


 using namespace osg;
 using namespace osgGA;


 CDb2DCameraManipulator::CDb2DCameraManipulator(int viewDef, osg::ref_ptr<osg::Group> GraphicsGroup, osgViewer::View* myview)
 { 
     myView = myview;
     _modelScale = 0.1f;
     _minimumZoomScale = 0.05f;
     _distance = 1.0f;
     once = false;
     viewDefine = viewDef;

     graphicsGroup = GraphicsGroup.get();
     maximalizedFlag = false; 
	 pOSG = NULL;
 }
  CDb2DCameraManipulator::~CDb2DCameraManipulator()
 {
  }
 
  void CDb2DCameraManipulator::SetCenter(osg::Vec3 center)
  {
	  _center = center;
  }

 void CDb2DCameraManipulator::orthoHome( )
 {
	 osg::BoundingSphere boundingSphere = pOSG->CalBoundBox();
	 osg::BoundingBox box = pOSG->computeBoundBox();
 
  		 double radius = boundingSphere.radius();

		 float width  = 0;
         float height = 0;

		 width  = (myView->getCamera()->getViewport()->width());
		 height = (myView->getCamera()->getViewport()->height());
		 
		 double rx = radius;
		 double ry = radius;
		if (box.valid())
		{
			if (viewDefine == 1) //xy
			{
				rx = 0.5*(box.xMax() - box.xMin());
				ry = 0.5*(box.yMax() - box.yMin());
			}
			else if (viewDefine == 2) //xz
			{
				rx = 0.5*(box.xMax() - box.xMin());
				ry = 0.5*(box.zMax() - box.zMin());
			}
			else if (viewDefine == 3) //zy
			{
				rx = 0.5*(box.yMax() - box.yMin());
				ry = 0.5*(box.zMax() - box.zMin());
			}
			radius = max(rx, ry);
		}
		//if (abs(rx) < 1)
		//	rx = pOSG->defaultRaidus;
		//else if (abs(ry) <1)
		//	ry = pOSG->defaultRaidus;
		 

		if (radius < pOSG->MINRADIUS)
		{
			radius = pOSG->defaultRaidus;
			rx = pOSG->defaultRaidus;
			ry = pOSG->defaultRaidus;
		}


		 float _left;
		 float _right;
		 float _bottom;
		 float _top;
		 if (abs(rx) < 1 && abs(ry) <1)
		 {
			 rx = pOSG->defaultRaidus;
			 ry = pOSG->defaultRaidus;
		 }

		 /////////
		 if (abs(rx) < 1  )
		 {
			 float _scale = height / ry;
			 _left = -width / _scale;
			 _right = width / _scale;
			 _bottom = -height / _scale;
			 _top = height / _scale;
		 }
		 else if (abs(ry) < 1)
		 {
			 float _scale = width / rx;
			 _left = -width / _scale;
			 _right = width / _scale;
			 _bottom = -height / _scale;
			 _top = height / _scale;
		 }
		 else if(width/rx > height/ry)
		 {
			 float _scale = height/ry ;

			 _left   = -width/_scale;
			 _right = width/_scale;

			 _bottom = -height/_scale;
			 _top       = height/_scale;
		 }
		 else
		 {
			 float _scale = width/rx ;
			 _left   = -width/_scale;
			 _right = width/_scale;
			 _bottom = -height/_scale;
			 _top       = height/_scale;
		 }

		 double dFactor = 1.2;
		 myView->getCamera()->setProjectionMatrixAsOrtho2D(_left*dFactor, _right*dFactor, _bottom*dFactor, _top*dFactor);
 }
 void CDb2DCameraManipulator::myHomeLocal(osg::Vec3 org, osg::Vec3 xDir, osg::Vec3 yDir )
 { 
	 osg::Vec3 zDir = xDir^yDir;
	 xDir.normalize();
	 yDir.normalize();
	 zDir.normalize();

	 double zoomFactor = 1;

	 osg::Vec3 lookAtDir;
	 osg::Vec3 center;
	 osg::Vec3 upDir;

	 osg::BoundingSphere boundingSphere = pOSG->CalBoundBox();

	 double radius = boundingSphere.radius();

	 if (radius < pOSG->MINRADIUS)
		 radius = pOSG->defaultRaidus;
 
	 {
		 computePosition(org + zDir*radius, -zDir, osg::Vec3(0.0f, 0.0f, 1.0f));


		 orthoHome();
  	 }
 }
 void CDb2DCameraManipulator::myHome(int width_x)
 {
	 osg::BoundingSphere boundingSphere = pOSG->CalBoundBox();
 
	 double radius = boundingSphere.radius();
	 
	 if (radius < pOSG->MINRADIUS)
		 radius = pOSG->defaultRaidus;

	 double zoomFactor = 1;

      switch(viewDefine)
     {
     case 1: /*Top*/
             {
                      computePosition(osg::Vec3(boundingSphere.center().x(), boundingSphere.center().y(),boundingSphere.center().z() + radius/zoomFactor),
                                         osg::Vec3(0.0f,  0.0f,  -0.001f),
										 osg::Vec3(0.0f,  1.0f,  0.0f)  );            
             
                           orthoHome();
                 
                     break;
             }
     case 2: /*Front*/
             {
 
				 computePosition(osg::Vec3(boundingSphere.center().x(), boundingSphere.center().y() - radius / zoomFactor, boundingSphere.center().z()),
                                         osg::Vec3(0.0f,  0.001f, 0.0f),
										 osg::Vec3(0.0f,  0.0f,  1.0f));
					 orthoHome();
               
                     break;
             }
     case 3: /*Left*/
             {
 				 computePosition(osg::Vec3(boundingSphere.center().x() + radius / zoomFactor, boundingSphere.center().y(), boundingSphere.center().z()),
                                         osg::Vec3(0.001f, 0.0f,  0.0f),
										 osg::Vec3(0.0f, 0.0f,  1.0f));
					 orthoHome();           

                     break;
             }
  
     }
 
 }
 
 /**********************************************************************/
 /*  Puvodni funkce tridy                                                                                          */    
 /**********************************************************************/
 void CDb2DCameraManipulator::setNode(osg::Node* node)
 {
     _node = node;
     if (_node.get())
     {
         const osg::BoundingSphere& boundingSphere=_node->getBound();
         _modelScale = boundingSphere._radius;
     }
 }
 
 const osg::Node* CDb2DCameraManipulator::getNode() const
 {
     return _node.get();
 }
 
 osg::Node* CDb2DCameraManipulator::getNode()
 {
     return _node.get();
 }
 
 
 void CDb2DCameraManipulator::home(const GUIEventAdapter& ,GUIActionAdapter& us)
 {
	 osg::BoundingSphere boundingSphere = graphicsGroup->getBound();
 		 double radius = boundingSphere.radius();
		 if(radius <pOSG->MINRADIUS )
			 radius = pOSG->defaultRaidus;

         switch(viewDefine)
         {
         case 1: /*Top*/
                 {
                         computePosition(osg::Vec3(boundingSphere.center().x(), boundingSphere.center().y(),boundingSphere.center().z() + radius/0.35),
                                             osg::Vec3(0.0f, 0.0f, 0.0f),
											 osg::Vec3(0.0f,  0.0f,  1.0f));
                         break;
                 }
         case 2: /*Front*/
                 {
                         computePosition(osg::Vec3(boundingSphere.center().x(), boundingSphere.center().y() - radius/0.15 ,boundingSphere.center().z()),
                                             osg::Vec3(0.0f, 0, 0.0f),
											 osg::Vec3(0.0f,  0.0f,  1.0f));
                         break;
                 }
         case 3: /*Left*/
                 {
                         computePosition(osg::Vec3(boundingSphere.center().x() - radius/0.15, boundingSphere.center().y() ,boundingSphere.center().z()),
                                             osg::Vec3(0.010, 0.0f, 0.0f),
											 osg::Vec3(0.0f,  0.0f,  1.0f));

 
                         break;
                 }
 
         }
 
 }
 
 
 void CDb2DCameraManipulator::init(const GUIEventAdapter& ,GUIActionAdapter& )
 {
     flushMouseEventStack();
 }
 
 bool CDb2DCameraManipulator::handle(const GUIEventAdapter& ea,GUIActionAdapter& us)
 {
     switch(ea.getEventType())
     { 
		 case(GUIEventAdapter::PUSH):
		 {
			 flushMouseEventStack();
			 return true;
		 }
		 break;
		 case(GUIEventAdapter::RELEASE):
		 {
			 once = true;
			 return true;
		 }
		 break;

 
         case(GUIEventAdapter::DRAG):
         { 
			 if( ea.getButtonMask() & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
			 {
				 addMouseEvent(ea);                         
				 if ( graphicsGroup )
				 {
					 calcMovement();
					 us.requestRedraw();
					 us.requestContinuousUpdate( false );
				 }
				 return true;
			 }
			/* else if(ea.getButtonMask() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON )
			 {
				 pOSG->m_pSelector->begin();
				 return true;
			 }   */    
		 }
		 break; 
		 case (GUIEventAdapter::SCROLL):
		 {
			 addMouseEvent(ea);  		
			 if ( graphicsGroup )
			 {
				 calcZoom();			
				 us.requestRedraw();
				 us.requestContinuousUpdate( false );
			 }
			 return true;
		 }
		 break;
         case(GUIEventAdapter::MOVE):
         {
             return false;
         }
		 break;
         case(GUIEventAdapter::KEYDOWN):
         {
             if (ea.getKey()==' ')
             {
                 flushMouseEventStack();
                 _thrown = false;
                 home(ea,us);
                 us.requestRedraw();
                 us.requestContinuousUpdate(false);
                 return true;
             } 
		 }
		 break;
         default:
             return false;
     }
	 return false;
 }
 
 
 bool CDb2DCameraManipulator::isMouseMoving()
 {
     if (_ga_t0.get()==NULL || _ga_t1.get()==NULL) return false;
 
     static const float velocity = 0.1f;
 
     float dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
     float dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();
 
   
     float len = sqrtf(dx*dx+dy*dy);
     float dt = _ga_t0->getTime()-_ga_t1->getTime();
 
      return (len>dt*velocity);
 }
 
 
 void CDb2DCameraManipulator::flushMouseEventStack()
 {
     _ga_t1 = NULL;
     _ga_t0 = NULL;
 }
 void CDb2DCameraManipulator::addMouseEvent(const GUIEventAdapter& ea)
 {
     _ga_t1 = _ga_t0;
     _ga_t0 = &ea;
 }
 void CDb2DCameraManipulator::setByMatrix(const osg::Matrixd& matrix)
 {
     _center = matrix.getTrans();
     _rotation = matrix.getRotate();
     _distance = 1.0f;
 }
 
 osg::Matrixd CDb2DCameraManipulator::getMatrix() const
 {
     return osg::Matrixd::rotate(_rotation)*osg::Matrixd::translate(_center);
 }
 
 osg::Matrixd CDb2DCameraManipulator::getInverseMatrix() const
 {
     return osg::Matrixd::translate(-_center)*osg::Matrixd::rotate(_rotation.inverse());
 }
 
 void CDb2DCameraManipulator::computePosition(const osg::Vec3& eye,const osg::Vec3& lv,const osg::Vec3& up)
 { 
     osg::Vec3 f(lv);
     f.normalize();
     osg::Vec3 s(f^up);
     s.normalize();
     osg::Vec3 u(s^f);
     u.normalize();
     
     osg::Matrixd rotation_matrix(s[0],     u[0],     -f[0],     0.0f,
                                  s[1],     u[1],     -f[1],     0.0f,
                                  s[2],     u[2],     -f[2],     0.0f,
                                  0.0f,     0.0f,     0.0f,      1.0f);
                    
     _center = eye+lv;
     _distance = lv.length();
     _rotation = rotation_matrix.getRotate().inverse();
 }
 

 bool CDb2DCameraManipulator::calcZoom()
 {
	 if( pOSG)
	 {
		 double delta = -0.1;//与cad操作一致
		 if(GUIEventAdapter::SCROLL_DOWN == _ga_t0->getScrollingMotion() )
			 delta=0.1;

		 CPoint pt=pOSG->GetCursorPos();

		 double wx,wy;
		 pOSG->GetWindowSize(wx,wy );

		 float x1 = 2.0f*pt.x/wx -1.0f;
		 float y1 = 2.0f*pt.y/wy -1.0f;
		 double alfa = (1+delta);

		 osg::Vec2 pt1, pt2;
		 pt1.set( x1- alfa, y1- alfa);
		 pt2.set( x1+alfa, y1+alfa);

		 pOSG->BoxZoom(pt1,pt2  ); //以光标所在点为中心进行缩放 ，缩放完成后再把中心点平移到光标点；
		 {
			 double left;
			 double right;
			 double bottom;
			 double top;
			 double zNear;
			 double zFar;
			 myView->getCamera()->getProjectionMatrixAsOrtho(left,right,bottom,top,zNear,zFar );
			 double width  =  0.5*(right-left);
			 double height = 0.5*(top-bottom);
			 myView->getCamera()->setProjectionMatrixAsOrtho(left - width*x1, right-width*x1, bottom- height*y1,top-height*y1,zNear,zFar  );
		 }
		 return false;
	 }	
	 return false;
 }

 bool CDb2DCameraManipulator::calcMovement()
 { 
	 if(true)
	 {
		 if (_ga_t0.get()==NULL || _ga_t1.get()==NULL)
		 {
			 return false;
		 } 
		 // get deltaX and deltaY
		 float dx = _ga_t0->getXnormalized() - _ga_t1->getXnormalized();
		 float dy = _ga_t0->getYnormalized() - _ga_t1->getYnormalized();

		 // return if there is no movement.
		 if( dx == 0. && dy == 0. )
			 return false;

		 double left;
		 double right;
		 double bottom;
		 double top;
		 double zNear;
		 double zFar;
		 myView->getCamera()->getProjectionMatrixAsOrtho(left,right,bottom,top,zNear,zFar );
		 double width  =  0.5*(right-left);
		 double height = 0.5*(top-bottom);
		myView->getCamera()->setProjectionMatrixAsOrtho(left - width*dx, right-width*dx, bottom- height*dy,top-height*dy,zNear,zFar  );
		return false;
	 }
 }