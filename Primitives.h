
 
#pragma  once

 #include <osg/ArgumentParser>
 #include <osg/MatrixTransform>
 #include <osg/PositionAttitudeTransform>
 #include <osgGA/TrackballManipulator>
 #include <osgGA/StateSetManipulator>
 #include <osgGA/GUIEventHandler>
 #include <osgGA/GUIEventAdapter>
 #include <osgGA/GUIActionAdapter>
 #include <osg/ShapeDrawable>
 #include <osg/Geometry>
 #include <osg/Material>
 #include <osgDB/ReadFile>
 #include <osgUtil/Optimizer>
 #include <osgViewer/Viewer>
 #include <osg/CoordinateSystemNode>
 #include <osg/ComputeBoundsVisitor>
 #include <osgText/Text>
 #include <osg/ClipNode>
 #include <osg/ArgumentParser>
 #include <osg/MatrixTransform>
 #include <osg/PositionAttitudeTransform>
 #include <osgGA/TrackballManipulator>
  
 class Primitives
 {
 public:
 
         void createSphere();
         void createBox();
         void createCylinder();
         void createCone();
         void createTorus();
         void createTube();
         void createTeapot();
         void createPyramid();
         void createHedra();
 
         
         osg::ref_ptr<osg::Group> myroot;
         osg::ref_ptr<osg::StateSet> defaultNormals;  /*mnozina stavu pro scale objektu a normal zaroven*/
         osg::StateSet* nodeStateSet;
 
         int lightCount;
 
         Primitives(osg::ref_ptr<osg::Group> root)
         {
                 /* svazani s korenem*/
                 myroot = root.get();

				 defaultNormals = new osg::StateSet();

				 defaultNormals->setMode(GL_NORMALIZE,osg::StateAttribute::ON);
 
                 lightCount = 0;
 
                 nodeStateSet = new osg::StateSet;
                 myroot->setStateSet(nodeStateSet);
         }
 public: 
         ~Primitives(void);
 };