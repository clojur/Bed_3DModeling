#pragma  once

 #include <osgGA/CameraManipulator>
 #include <osgViewer/Viewer>
 #include <osgViewer/ViewerEventHandlers>
 #include <osg/Quat> 
 class CViewControlData;

 /*definice viewportu*/
 #define TOP 1
 #define FRONT 2
 #define LEFT 3
 #define BOTTOM 4
 #define RIGHT 5
 #define BACK 6
 
 /*
 二维图相机操作器
 */
 class DB_3DMODELING_API CDb2DCameraManipulator : public osgGA::CameraManipulator
 {
     public:
 
         CDb2DCameraManipulator(int viewDef, osg::ref_ptr<osg::Group> GraphicsGroup, osgViewer::View* myview);
         virtual ~CDb2DCameraManipulator();
		 
		 osgViewer::View* myView;
 
         virtual void setByMatrix(const osg::Matrixd& matrix);
 
         virtual void setByInverseMatrix(const osg::Matrixd& matrix) { setByMatrix(osg::Matrixd::inverse(matrix)); }
 
         virtual osg::Matrixd getMatrix() const;
 
         virtual osg::Matrixd getInverseMatrix() const;
 
         virtual void setNode(osg::Node*);
 
         virtual const osg::Node* getNode() const;
 
         virtual osg::Node* getNode();
 
         virtual void home(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);
		 
		 void myHome(int width);
		 void myHomeLocal(osg::Vec3 org , osg::Vec3 xDir , osg::Vec3 yDir );

         virtual void init(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);
 
 
         virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);

		 osg::ref_ptr<osg::Group> graphicsGroup;   
		 int viewDefine;
		 bool maximalizedFlag; 
     private: 
         void flushMouseEventStack();
         void addMouseEvent(const osgGA::GUIEventAdapter& ea);
 
         void computePosition(const osg::Vec3& eye,const osg::Vec3& lv,const osg::Vec3& up);
		 void orthoHome( );
		 bool calcMovement();   
         bool isMouseMoving();

		 bool calcZoom();
 
         osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t1;
         osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t0;
 
         osg::ref_ptr<osg::Node>       _node;
 
         float _modelScale;
         float _minimumZoomScale;
 
         bool _thrown; 
		 osg::Vec3   _center;
		 osg::Quat   _rotation;
		 float           _distance;
		 bool once; 
 public:
	 void SetCenter(osg::Vec3 center);
 public:
	 CViewControlData*  pOSG;
 };