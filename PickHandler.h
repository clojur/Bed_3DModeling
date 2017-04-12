
#pragma  once

 
 #include <osg\ArgumentParser>
 #include <osg/MatrixTransform>
 #include <osg/PositionAttitudeTransform>
 #include <osgGA/StateSetManipulator>
 #include <osgGA/GUIEventHandler>
 #include <osgGA/GUIEventAdapter>
 #include <osgGA/GUIActionAdapter>

 #include <osg/ShapeDrawable>
 #include <osg/Geometry>
  #include <osgUtil/Optimizer>
 #include <osgViewer/Viewer>
  #include <osg/ComputeBoundsVisitor>
 #include <osgText/Text>
 #include <osg/ClipNode>
 #include <osg/AutoTransform>
 
 #include <osgManipulator/CommandManager>
 #include <osgManipulator/Selection>

 #include <osgUtil/Simplifier>
 
#define _INSERT 0
 #define _DELETE 1
 #define _ROTATE 2
 #define _MOVE 3
 #define _SCALE 4
 
 class CViewControlData;

//osg 界面交互类，移动、选中
 class DB_3DMODELING_API  CDbPickModeHandler : public osgGA::GUIEventHandler
 { 
     public: 
         CDbPickModeHandler(osg::ref_ptr<osg::Group> rootNode)
         { 
			 pOSG = NULL;

             graphicsGroup = rootNode.get();

             view_perspective = NULL;

             temp = 0;
			 bclearPre     =false;
			  tempSelectObj=NULL;
         } 
		 enum Modes
         {
             VIEW = 0,
             PICK
         }; 
		 osgViewer::View* view_perspective; 
		 /*
		 命令处理
		 */
		 bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa); 
		 bool ImplementHandle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
		 /*
		 选择节点
		 */
		 virtual  osg::Node* pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Vec3& insectionPoint);
		 /*
		 选择DrawAble
		 */
		 virtual osg::ref_ptr<osg::Drawable> pickDrawAble(osgViewer::View* view, const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);   

		 osg::Node* GetHeightLightNode();
		 void SetNodeHeightLight(osg::Node* pNode,bool flag );

	

		 void showBoundary(osg::Node* pNode );
		 void removeBoundary(osg::Node* pNode);


		 virtual void OnDeleteNode(const CString& strObj, int objID){ ; }
		 double GetX(const osgGA::GUIEventAdapter& ea,osgViewer::View* viewer);
		 double GetY(const osgGA::GUIEventAdapter& ea,osgViewer::View* viewer);
 private:
     osgManipulator::PointerInfo _pointer; 

     osg::ref_ptr<osg::Group> graphicsGroup;

     float _x,_y;    
     int temp;

	 float mouseMovePosX;
	 float mouseMovePosY;
	 float mouseMoveTime;
 public:
	 bool bclearPre;
	 CViewControlData* pOSG;
 private:
	CString tempSelectObjName;
	 osg::Node* tempSelectObj;
 };