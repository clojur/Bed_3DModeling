
#include "stdafx.h"

 

#include "osg/LineWidth"
#include <osg/Drawable>
#include <osg/Geometry>
#include <osg/Node>
#include <osgUtil/SmoothingVisitor>
#include <osg/Point>
#include "osg/BlendFunc"
#include "osg/LineStipple"

#include "osgwraper.h"
#include "BoxSelector.h"
#include "Stretchhandler.h"
#include "MoveObjHandler.h"
#include "osgtools.h"

#include "PickHandler.h"
#include "eraseHandler.h"
#include "IDb3DCommandLine.h"
//#include <osg/ValueObject>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

 bool CDbPickModeHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
 {  
	 bool flag = false;
	 osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);	
	 if(!pOSG->m_Osg)
		 return false;

	 if(pOSG->m_Osg->GetCurrentView() != view )
		 return false;

	 if(pOSG && pOSG->m_command > 0 &&  pOSG->m_pCurHandler && pOSG->m_pCurHandler->status>0 )
	 {
		 pOSG->bInit=FALSE;
		 pOSG->bFirst=FALSE;

		 return pOSG->m_pCurHandler->handle(ea, aa);
	 }
	
	 return ImplementHandle(ea, aa);  
 }

 double CDbPickModeHandler::GetX(const osgGA::GUIEventAdapter& ea,osgViewer::View* viewer)
 {
	 double x =  ea.getX();
	 if(viewer)
	 {
		 x-=viewer->getCamera()->getViewport()->x();
	 }
	 return x;
 }

 double CDbPickModeHandler::GetY(const osgGA::GUIEventAdapter& ea,osgViewer::View* viewer)
 {
	 double y =  ea.getY();
	 if(viewer)
	 {
		 y-=viewer->getCamera()->getViewport()->y();
	 }
	 return y;
 }
 bool CDbPickModeHandler::ImplementHandle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
 {	
	 bool bDoRotate = false;
	 int buttonMask = ea.getButtonMask();
	 int modKeyMask = ea.getModKeyMask();

	 if ((buttonMask & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON) && (modKeyMask& osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT))
	 {
		 pOSG->bRotate=true;
	 }
	 else
	 {
		 pOSG->bRotate = false;
	 }
 
	 if (pOSG->manipulatorType != 0 || pOSG->bRotate)
		 return false;

	 DWORD t1 = GetTickCount();

	 osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);

	 if( !pOSG->m_pSelector->IsFinish() )
	 {
		 return pOSG->m_pSelector->handle(ea, aa);
	 }
	 switch (ea.getEventType())
	 { 
	 case(osgGA::GUIEventAdapter::KEYDOWN) : /*Key Delete*/
	 {
		 if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Delete)
		 {
			 auto cmdLine = GetDb3DCommandLine();
			 if (cmdLine)
			 {
				 cmdLine->setLastCommand("e");
			 }
		
			 CDbEraseHandler::DoEraseObjects(pOSG);
			 return false;
		 }
	 }
	 break;


	 case osgGA::GUIEventAdapter::DOUBLECLICK:
	 {
		 if (ea.getButtonMask() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		 {
			 _x = GetX(ea, viewer);
			 _y = GetY(ea, viewer);
			 if (viewer)
			 {
				 osg::Vec3 insectPoint;

				 osg::Node* pNode = pick(viewer, ea, aa,insectPoint);
				 if (!tempSelectObjName.IsEmpty())
				 {
					 CDbObjInterface* pObject = pOSG->FindObj(tempSelectObjName, OT_ALL);
					 if (pObject)
					 {
 						 pOSG->coordPos = insectPoint;

						 pOSG->m_pSelector->clearSelectSet();

						 std::vector<osg::Node*> allOb;
						 allOb.push_back(pNode);
						 MakeObjectSelected(allOb, pOSG);

						 pOSG->GetCurrentCWnd()->SendMessage(DB_DBLCLICKOBJ, WPARAM(pObject), 0);

 					 }
					 else
					 {
						 pOSG->PostObjDClick(pNode);
					 }
				 }
			 }
		 }
	 }
	 break;
	 case osgGA::GUIEventAdapter::PUSH:
		 {
			 if(ea.getButtonMask() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON )
			 {
				 _x = GetX(ea,viewer);
				 _y = GetY(ea,viewer);
				 if (viewer) 
				 {
					 osg::Vec3 insectPoint;
					 osg::Node* pNode = pick(viewer, ea, aa, insectPoint);

					 if( !tempSelectObjName.IsEmpty() )
					 {
						 BOOL unSelectMode = FALSE;
						 int modKeyMask = ea.getModKeyMask();

						 if ((modKeyMask& osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT))
						 {
							 unSelectMode = TRUE;
						 }

						 if (unSelectMode) //退选
						 {
 							 std::vector<osg::Node*> newSelectObjs;
							 newSelectObjs.push_back(pNode);
							 MakeObjectUnSelected(newSelectObjs, pOSG);
						 }
						 else
						 {
							 if (pOSG->IsSelected(tempSelectObjName))
							 {
								 osg::Vec2 p1(_x - 5, _y - 5);
								 osg::Vec2 p2(_x + 5, _y + 5);

								 if (pOSG->m_pSelector->bInStretchHandler(p1, p2))
								 {
									 pOSG->PreStretch();

									 std::vector<osg::Node*> hotHhandlers = pOSG->m_pSelector->UpdateStretchHandler(p1, p2);								
								
									 CDbQuickStretchHandler* pHandler = new CDbQuickStretchHandler(pOSG);
									 pHandler->hotHhandlers = hotHhandlers;
									 pOSG->SetHandler(pHandler);
									 pOSG->m_pCurHandler->handle(ea, aa);									
								 }
								 else
								 {
									 CDbMoveObjhandler* pHandler = new CDbMoveObjhandler(pOSG);

									 pHandler->bQuickModel = true;
									 pHandler->status = 2;
									 pOSG->SetHandler(pHandler);
									 pOSG->m_pCurHandler->handle(ea, aa);
									 pOSG->m_command = 1;
								 }							
							 }
							 else
							 {
								 pOSG->PostObjSelected(pNode);

								 std::vector<osg::Node*> newSelectObjs;
								 newSelectObjs.push_back(pNode);
								 MakeObjectSelected(newSelectObjs, pOSG);
							 }
						 }
 											
					 }
					 else if (tempSelectObjName.IsEmpty())
					 {			
						 if (pNode)
						 {
							 pOSG->PostObjSelected(pNode);
						 }
						 else
						 {
							 pOSG->OnIdle();
							 pOSG->m_pSelector->status = 1;
							 pOSG->m_pSelector->handle(ea, aa);
						 }
					 }
				 }
			 }		
		 }
	 break;		 
	 case osgGA::GUIEventAdapter::MOVE:
		 {
			 //鼠标经过没有选中的对象时，会高亮显示该对象
			 if (viewer)  
			 {
				 float x = GetX(ea, viewer);
				 float y = GetY(ea, viewer);
				 if (abs(mouseMovePosX - x) > 1 || abs(mouseMovePosY - y) > 1 )
				 {
					 osg::Node* preHightLightObj = GetHeightLightNode();
					 if (preHightLightObj)
					 {
						 pOSG->removeHightLigntEffect(preHightLightObj);
					 }
				 }				
				 mouseMovePosX = x;
				 mouseMovePosY = y;
				 mouseMoveTime =ea.getTime();

				 if (0)
				 {
					 if (pOSG->m_Osg->GetCursorType() == OSG_HandCursor)
					 {
						 pOSG->SetCursorType(OSG_NORMAL_CUSSOR);
					 }
				 }
			

			 }
 		 }
		 break;
	 case osgGA::GUIEventAdapter::FRAME:
		 {		 
			 break;
		 }
	 default: break;
	 }
	 DWORD t2 = GetTickCount();

	 if ((t2 - t1) > 200)
	 {
		 CString strInfo;
		 strInfo.Format("CDbPickModeHandler::ImplementHandle erase time:%d\n", t2 - t1);
		 TRACE(strInfo);
	 }

	 return false;
 }

 osg::ref_ptr<osg::Drawable> CDbPickModeHandler::pickDrawAble(osgViewer::View* view, const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
 {
	 if(!view) return 0;
	 osg::MatrixTransform* pMT = 0;

	 osgUtil::LineSegmentIntersector::Intersections intersections;

	 CString gdlist="";
	 float x = GetX(ea,view);
	 float y = GetY(ea,view);

	 if (view->computeIntersections(x,y,intersections))
	 {
		 for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin(); hitr != intersections.end();++hitr)	
		 {
			 CString strClassName = hitr->drawable->className();
			 return hitr->drawable;		
		 }
	 }
	 return 0;
 }


 osg::Node* CDbPickModeHandler::GetHeightLightNode( )
 {
	 FindHeightLightNodeVisitor rsnv;
	 pOSG->GraphicsGroup->accept(rsnv);
	 return rsnv.pNode;	
 }

 void CDbPickModeHandler::SetNodeHeightLight(osg::Node* pNode,bool flag )
 {
	 if(pNode)
		 pNode->setUserValue("heightLieht",flag);
 }

 osg::Node* CDbPickModeHandler::pick(osgViewer::View* viewer, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Vec3& insectionPoint)
 { 
	 osg::Node* singleNode = pOSG->pickSingle(ea.getXnormalized(), ea.getYnormalized(), viewer, TRUE, TRUE,insectionPoint);
	 if (singleNode != NULL)
	{
		tempSelectObjName =  singleNode->getName().c_str();
	} 
	 else
	 {
		 tempSelectObjName = "";
	 }
	 return singleNode;
 }


 void CDbPickModeHandler::showBoundary(osg::Node* SelectedNode)
 {
	 if(SelectedNode)
	 {
		 CDbObjInterface*  pData = pOSG->FindObj(  SelectedNode->getName().c_str() ,GetObjType(SelectedNode) );
		 if(pData == NULL)
			 return;

		 //添加夹点
		 if (pData->IsDrawHandler() )
			 pOSG->AddHandlers(SelectedNode);


		 if (pOSG->IsAddSelectEffect(SelectedNode))
			 return;
		 osg::Node* pSelectNode =  pData->AddSelectEffect(SelectedNode);
		if(pSelectNode)
		{		
			pSelectNode->setName( SelectedNode->getName() );
			pOSG->addSelectEffect(pSelectNode, yellowColor);
		}		 
		 return;
	 }
 }


 void CDbPickModeHandler::removeBoundary(osg::Node* selectetNode)
 {
	 if ( selectetNode)
	 {	
		 //删除夹点
		  pOSG->ClearHandlers(  selectetNode);
	
		  pOSG->removeSelectEffect( selectetNode );
		 return;
	 }
 }


