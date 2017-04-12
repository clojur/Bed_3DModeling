
#include "stdafx.h"
 #include "Primitives.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

 void Primitives::createSphere()
 {
         osg::Sphere* unitSphere = new osg::Sphere( osg::Vec3(0,0,0), 2.0f);
         // Transformace pro objekt - rucni posun, ne manipulatory
         osg::PositionAttitudeTransform* transform = new osg::PositionAttitudeTransform();
         transform->setName("myTransformace");
         osg::ref_ptr<osg::Group> selection = new osg::Group;    
         selection->setName("mySelection");
         
         unitSphere->setDataVariance(osg::Object::DYNAMIC);
         osg::ShapeDrawable* unitSphereDrawable = new osg::ShapeDrawable(unitSphere);
         osg::Geode* basicShapesGeode = new osg::Geode();
         basicShapesGeode->addDrawable(unitSphereDrawable);
         basicShapesGeode->setName("Sphere");
 
         osg::ref_ptr<osg::Node> temp = new osg::Node;
         // pretypovani geode na node 
         temp = dynamic_cast<osg::Node*>(basicShapesGeode);
  
         // Graf sceny
         myroot->addChild(transform); 
         transform->addChild(selection.get());
         selection->addChild(temp.get());
         
 }
 void Primitives::createBox()
 {
         osg::Box* unitCube = new osg::Box( osg::Vec3(0,0,0), 2.0f);
         /* Transformace pro objekt - rucni posun, ne manipulatory*/
         osg::PositionAttitudeTransform* transform = new osg::PositionAttitudeTransform();
         osg::ref_ptr<osg::Group> selection = new osg::Group;    
         transform->setName("myTransformace");
         selection->setName("mySelection");
         
         unitCube->setDataVariance(osg::Object::DYNAMIC);
         osg::ShapeDrawable* unitSphereDrawable = new osg::ShapeDrawable(unitCube);
         osg::Geode* basicShapesGeode = new osg::Geode();
         basicShapesGeode->addDrawable(unitSphereDrawable);
         basicShapesGeode->setName("Box");
 
         osg::ref_ptr<osg::Node> temp = new osg::Node;
         /* pretypovani geode na node */
         temp = dynamic_cast<osg::Node*>(basicShapesGeode);
 
         /* Graf sceny */
         myroot->addChild(transform); 
         transform->addChild(selection.get());
         selection->addChild(temp.get());
 }
 void Primitives::createCylinder()
 {
         osg::Cylinder* unitCyl = new osg::Cylinder( osg::Vec3(0,0,0), 2.0f, 2.0f);
         /* Transformace pro objekt - rucni posun, ne manipulatory*/
         osg::PositionAttitudeTransform* transform = new osg::PositionAttitudeTransform();
         osg::ref_ptr<osg::Group> selection = new osg::Group;    
         transform->setName("myTransformace");
         selection->setName("mySelection");
         
         unitCyl->setDataVariance(osg::Object::DYNAMIC);
         osg::ShapeDrawable* unitSphereDrawable = new osg::ShapeDrawable(unitCyl);
         osg::Geode* basicShapesGeode = new osg::Geode();
         basicShapesGeode->addDrawable(unitSphereDrawable);
         basicShapesGeode->setName("Cylinder");
 
         osg::ref_ptr<osg::Node> temp = new osg::Node;
         /* pretypovani geode na node */
         temp = dynamic_cast<osg::Node*>(basicShapesGeode);
 
         /* Graf sceny */
         myroot->addChild(transform); 
         transform->addChild(selection.get());
         selection->addChild(temp.get());
 }
 void Primitives::createCone()
 {
         osg::Cone* unitCone = new osg::Cone( osg::Vec3(0,0,0), 2.0f, 2.0f);
         /* Transformace pro objekt - rucni posun, ne manipulatory*/
         osg::PositionAttitudeTransform* transform = new osg::PositionAttitudeTransform();
         osg::ref_ptr<osg::Group> selection = new osg::Group;    
         transform->setName("myTransformace");
         selection->setName("mySelection");
         
         unitCone->setDataVariance(osg::Object::DYNAMIC);
         osg::ShapeDrawable* unitSphereDrawable = new osg::ShapeDrawable(unitCone);
         osg::Geode* basicShapesGeode = new osg::Geode();
         basicShapesGeode->addDrawable(unitSphereDrawable);
         basicShapesGeode->setName("Cone");
 
         osg::ref_ptr<osg::Node> temp = new osg::Node;
         /* pretypovani geode na node */
         temp = dynamic_cast<osg::Node*>(basicShapesGeode);
 
         /* Graf sceny */
         myroot->addChild(transform); 
         transform->addChild(selection.get());
         selection->addChild(temp.get());
 }
 void Primitives::createTorus()
 {
         osg::ref_ptr<osg::Node> unitTorus = new osg::Node;
         unitTorus = osgDB::readNodeFile("models/torus.3ds");
         /* Transformace pro objekt - rucni posun, ne manipulatory*/
         osg::PositionAttitudeTransform* transform = new osg::PositionAttitudeTransform();
         osg::ref_ptr<osg::Group> selection = new osg::Group;    
         transform->setName("myTransformace");
         selection->setName("mySelection");
         
         unitTorus->setDataVariance(osg::Object::DYNAMIC);
         unitTorus->setName("Torus");
         /*stateset defaultNormals zajisti spolecnou transformaci i pro normaly*/
         unitTorus->setStateSet(defaultNormals.get());
 
         /* Graf sceny */
         myroot->addChild(transform); 
         transform->addChild(selection.get());
         selection->addChild(unitTorus.get());
 }
 void Primitives::createTube()
 {
         osg::ref_ptr<osg::Node> unitTube = new osg::Node;
         unitTube = osgDB::readNodeFile("models/tube.3ds");
         /* Transformace pro objekt - rucni posun, ne manipulatory*/
         osg::PositionAttitudeTransform* transform = new osg::PositionAttitudeTransform();
         osg::ref_ptr<osg::Group> selection = new osg::Group;    
         transform->setName("myTransformace");
         selection->setName("mySelection");
         
         unitTube->setDataVariance(osg::Object::DYNAMIC);
         unitTube->setName("Tube");
         /*stateset defaultNormals zajisti spolecnou transformaci i pro normaly*/
         unitTube->setStateSet(defaultNormals.get());
 
         /* Graf sceny */
         myroot->addChild(transform); 
         transform->addChild(selection.get());
         selection->addChild(unitTube.get());
 }
 void Primitives::createTeapot()
 {
         osg::ref_ptr<osg::Node> unitTeapot = new osg::Node;
         unitTeapot = osgDB::readNodeFile("models/teapot.3ds");
         /* Transformace pro objekt - rucni posun, ne manipulatory*/
         osg::PositionAttitudeTransform* transform = new osg::PositionAttitudeTransform();
         osg::ref_ptr<osg::Group> selection = new osg::Group;    
         transform->setName("myTransformace");
         selection->setName("mySelection");
         
         unitTeapot->setDataVariance(osg::Object::DYNAMIC);
         unitTeapot->setName("Teapot");
         /*stateset defaultNormals zajisti spolecnou transformaci i pro normaly*/
         unitTeapot->setStateSet(defaultNormals.get());
 
         /* Graf sceny */
         myroot->addChild(transform); 
         transform->addChild(selection.get());
         selection->addChild(unitTeapot.get());
 }
 void Primitives::createPyramid()
 {
         osg::ref_ptr<osg::Node> unitPyramid = new osg::Node;
         unitPyramid = osgDB::readNodeFile("models/pyramid.3ds");
         /* Transformace pro objekt - rucni posun, ne manipulatory*/
         osg::PositionAttitudeTransform* transform = new osg::PositionAttitudeTransform();
         osg::ref_ptr<osg::Group> selection = new osg::Group;    
         transform->setName("myTransformace");
         selection->setName("mySelection");
         
         unitPyramid->setDataVariance(osg::Object::DYNAMIC);
         unitPyramid->setName("Pyramid");
         /*stateset defaultNormals zajisti spolecnou transformaci i pro normaly*/
         unitPyramid->setStateSet(defaultNormals.get());
 
         /* Graf sceny */
         myroot->addChild(transform); 
         transform->addChild(selection.get());
         selection->addChild(unitPyramid.get());
 }
 void Primitives::createHedra()
 {
         osg::ref_ptr<osg::Node> unitHedra = new osg::Node;
         unitHedra = osgDB::readNodeFile("models/hedra.3ds");
         /* Transformace pro objekt - rucni posun, ne manipulatory*/
         osg::PositionAttitudeTransform* transform = new osg::PositionAttitudeTransform();
         osg::ref_ptr<osg::Group> selection = new osg::Group;    
         transform->setName("myTransformace");
         selection->setName("mySelection");
         
         unitHedra->setDataVariance(osg::Object::DYNAMIC);
         unitHedra->setName("Hedra");
         /*stateset defaultNormals zajisti spolecnou transformaci i pro normaly*/
         unitHedra->setStateSet(defaultNormals.get());
 
         /* Graf sceny */
         myroot->addChild(transform); 
         transform->addChild(selection.get());
         selection->addChild(unitHedra.get());
 }