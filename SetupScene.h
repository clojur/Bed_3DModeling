#pragma  once

 #include <osg/Geode>
 #include <osg/Group>
 #include <osg/Node>
#include <osgViewer/View>
 class CViewControlData;

  
 class DB_3DMODELING_API LoadManager
 {
 public: 
	 /*
	 ����osgdb·��
	 */
     void LoadPath(); 

	 /*
	 ���ر��
	 */
     void LoadGrid(osg::ref_ptr<osg::Group> root ,int type,double z);

	 /*
	 ���ص���
	 */
	 osg::Group* LoadTerrain(osg::ref_ptr<osg::Group> root, const CString& strTerrainFile, const CString& strImage, double zFactor, osg::Vec3 center, double radius);
     

	 osg::Geode* LoadGrid_Org(osg::ref_ptr<osg::Group> root ,int type);
    LoadManager(void);
public: 
    ~LoadManager(void);
	int unit;
	int count;
	osg::Vec3 origion;
 };

