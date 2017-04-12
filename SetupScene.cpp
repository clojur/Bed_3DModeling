
#include "stdafx.h"
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Quat>
#include <osgText/Text>
#include <osg/Projection>
#include <osg/MatrixTransform>


#include <osg/Material>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Projection>
#include <osg/AutoTransform>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osgDB/ReadFile> 
#include <osgDB/FileUtils>

#include "Db3DModelingUnitity.h"
#include "osgtools.h"
#include "SetupScene.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define M_PI_2 1.57079632679489661923


const osg::Vec4 GRID_COLOR         = osg::Vec4(0.196f, 0.21568f, 0.2823f, 1.0f);

const osg::Vec4 GRID_BORDER_COLOR  = osg::Vec4(0.28627f, 0.3098f, 0.41176f, 1.0f);
const osg::Vec4 GRID_BORDER_COLOR2 = osg::Vec4(0.28627f, 0.3098f, 0.41176f, 1.0f);
const osg::Vec4 GRID_CEN_COLOR  = osg::Vec4(0.5f, 0.0f, 0.0f, 0.8f);
const osg::Vec4 GRID_CEN_COLOR2 = osg::Vec4(0.0f, 0.5f, 0.0f, 0.8f);


void LoadManager::LoadPath()
{
        osgDB::FilePathList pathList = osgDB::getDataFilePathList();

		pathList.push_back("Models/");
        pathList.push_back("Fonts/");

		osgDB::setDataFilePathList(pathList);   
}


void LoadManager::LoadGrid( osg::ref_ptr<osg::Group> root,int type ,double z)
{
	if(root == NULL)
		return ;

	root->removeChildren(0, root->getNumChildren());

	osg::ref_ptr<osg::Geode> Grid = new osg::Geode();

   osg::ref_ptr<osg::Geometry> GridGeometry                = new osg::Geometry();
   osg::ref_ptr<osg::Geometry> GridGeometry_border   = new osg::Geometry();
   osg::ref_ptr<osg::Geometry> GridGeometry_center    = new osg::Geometry();
   osg::ref_ptr<osg::Geometry> GridGeometry_border2 = new osg::Geometry();
   osg::ref_ptr<osg::Geometry> GridGeometry_center2  = new osg::Geometry();

   Grid->addDrawable(GridGeometry); 
   Grid->addDrawable(GridGeometry_border);
   Grid->addDrawable(GridGeometry_center);
   Grid->addDrawable(GridGeometry_border2);
   Grid->addDrawable(GridGeometry_center2);

   Grid->setName("noSelectable");
   root->addChild(Grid);


   osg::StateSet* stateset = Grid->getOrCreateStateSet();
   stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
   stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
   //取消深度测试
   stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
   stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
   stateset->setRenderBinDetails(-2, "RenderBin");

   osg::ref_ptr<osg::Vec4Array> color_other = new osg::Vec4Array;   
  // color_other->push_back(GRID_COLOR );
   color_other->push_back(osg::Vec4(0.0588,0.06667,0.0862745,1));

   osg::ref_ptr<osg::Vec4Array> color_border = new osg::Vec4Array;  
 //  color_border->push_back(GRID_BORDER_COLOR);// 0.15f 
   color_border->push_back(osg::Vec4(0.121568, 0.1333333, 0.17647,1));

   osg::ref_ptr<osg::Vec4Array> color_border2 = new osg::Vec4Array;
//   color_border2->push_back(GRID_BORDER_COLOR2);// 0.6f 
   color_border2->push_back(osg::Vec4(0.121568, 0.1333333, 0.17647,1));

   osg::ref_ptr<osg::Vec4Array> color_center = new osg::Vec4Array;  
  // color_center->push_back(GRID_CEN_COLOR);// 0.15f 
   color_center->push_back(osg::Vec4(0.121568, 0.1333333, 0.17647, 1));

   osg::ref_ptr<osg::Vec4Array> color_center2 = new osg::Vec4Array;  
//   color_center2->push_back(GRID_CEN_COLOR2);// 0.15f 
   color_center2->push_back(osg::Vec4(0.121568, 0.1333333, 0.17647, 1));

   osg::ref_ptr<osg::Vec3Array> GridVertices = new osg::Vec3Array;
   osg::ref_ptr<osg::Vec3Array> GridVerticesBorder = new osg::Vec3Array;
   osg::ref_ptr<osg::Vec3Array> GridVerticesBorder2 = new osg::Vec3Array;
   osg::ref_ptr<osg::Vec3Array> GridVerticesCenter = new osg::Vec3Array;
   osg::ref_ptr<osg::Vec3Array> GridVerticesCenter2 = new osg::Vec3Array;

   int offsetx = origion.x()/unit;
   int offsety = origion.y()/unit;


   if(type == 1 || type ==4)
   {
	   for( int i = offsetx-count; i<= offsetx+count;  i++ )
	   {
		   if (i == offsetx)
		   {
			   GridVerticesCenter->push_back(osg::Vec3(i*unit, (offsety - count)*unit, z));
			   GridVerticesCenter->push_back(osg::Vec3(i*unit, (offsety + count)*unit, z));
		   }
		   else if (i == offsetx - count || i == offsetx + count)
		   {
			   GridVerticesBorder2->push_back(osg::Vec3(i*unit, (offsety - count)*unit, z));
			   GridVerticesBorder2->push_back(osg::Vec3(i*unit, (offsety + count)*unit, z));
		   }
		   else if ( (offsetx-i) % 5 == 0)
		   {
			   GridVerticesBorder->push_back(osg::Vec3(i*unit, (offsety - count)*unit, z));
			   GridVerticesBorder->push_back(osg::Vec3(i*unit, (offsety + count)*unit, z));
		   }
		   else
		   {
			   GridVertices->push_back(osg::Vec3(i*unit, (offsety - count)*unit, z));
			   GridVertices->push_back(osg::Vec3(i*unit, (offsety + count)*unit, z));
		   }
	   }
	   for( int i = offsety-count; i<= offsety+count;  i++ )
	   {
		   if (i == offsety)
		   { 
			   GridVerticesCenter2->push_back(osg::Vec3((offsetx - count)*unit, i*unit, z));
			   GridVerticesCenter2->push_back(osg::Vec3((offsetx + count)*unit, i*unit, z));
		   }
		   else if (i == offsety - count || i == offsety + count)
		   {
			   GridVerticesBorder2->push_back(osg::Vec3((offsetx - count)*unit, i*unit, z));
			   GridVerticesBorder2->push_back(osg::Vec3((offsetx + count)*unit, i*unit, z));
		   }
		   else if ( (offsety-i) % 5 == 0)
		   {		 
			   GridVerticesBorder->push_back(osg::Vec3((offsetx - count)*unit, i*unit, z));
			   GridVerticesBorder->push_back(osg::Vec3((offsetx + count)*unit, i*unit, z));
		   }
		   else
		   {		 
			   GridVertices->push_back(osg::Vec3((offsetx - count)*unit, i*unit, z));
			   GridVertices->push_back(osg::Vec3((offsetx + count)*unit, i*unit, z));
		   }
	   }
   }

   GridGeometry->setVertexArray( GridVertices );
   GridGeometry_border->setVertexArray( GridVerticesBorder );
   GridGeometry_center->setVertexArray(GridVerticesCenter);
   GridGeometry_border2->setVertexArray(GridVerticesBorder2);
   GridGeometry_center2->setVertexArray( GridVerticesCenter2 );

   if (GridVertices->size() > 0)
   {
	   osg::ref_ptr<osg::DrawArrays> GridUnits = new osg::DrawArrays(osg::PrimitiveSet::LINES, 0,GridVertices->size() );   
	   GridGeometry->addPrimitiveSet(GridUnits);
	   GridGeometry->setColorArray(color_other);//
	   GridGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

   }
/////////
   if (GridVerticesBorder->size() > 0)
   {
	   osg::ref_ptr<osg::DrawArrays> GridUnitsBorder = new osg::DrawArrays(osg::PrimitiveSet::LINES, 0,GridVerticesBorder->size() ); 
	   GridGeometry_border->addPrimitiveSet(GridUnitsBorder);
	   GridGeometry_border->setColorArray(color_border);//
	   GridGeometry_border->setColorBinding(osg::Geometry::BIND_OVERALL);
   }
   if (GridVerticesBorder2->size() > 0)
   {
	   osg::ref_ptr<osg::DrawArrays> GridUnitsBorder2 = new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, GridVerticesBorder2->size());
	   GridGeometry_border2->addPrimitiveSet(GridUnitsBorder2);
	   GridGeometry_border2->setColorArray(color_border2);//
	   GridGeometry_border2->setColorBinding(osg::Geometry::BIND_OVERALL);
   }

   if (GridVerticesCenter->size() > 0)
   {
	   osg::ref_ptr<osg::DrawArrays> GridUnitsCenter = new osg::DrawArrays(osg::PrimitiveSet::LINES, 0,GridVerticesCenter->size() ); 
	   GridGeometry_center->addPrimitiveSet(GridUnitsCenter);
	   GridGeometry_center->setColorArray(color_center);//
	   GridGeometry_center->setColorBinding(osg::Geometry::BIND_OVERALL);
   }

   if (GridVerticesCenter2->size() > 0)
   {
	   osg::ref_ptr<osg::DrawArrays> GridUnitsCenter2 = new osg::DrawArrays(osg::PrimitiveSet::LINES, 0,GridVerticesCenter2->size() ); 
	   GridGeometry_center2->addPrimitiveSet(GridUnitsCenter2);
	   GridGeometry_center2->setColorArray(color_center2);//
	   GridGeometry_center2->setColorBinding(osg::Geometry::BIND_OVERALL);
   }
 }

osg::Geode* LoadManager::LoadGrid_Org( osg::ref_ptr<osg::Group> root,int type )
{
	osg::Geode* Grid = new osg::Geode();
	osg::Geometry* GridGeometry = new osg::Geometry();
	osg::Geometry* GridGeometry_center = new osg::Geometry();

	Grid->addDrawable(GridGeometry); 

	Grid->addDrawable(GridGeometry_center);

	Grid->setName("noSelectable");
	root->addChild(Grid);


	osg::StateSet* stateset = Grid->getOrCreateStateSet();
 	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

	osg::Vec4Array* color_white = new osg::Vec4Array;   
	color_white->push_back(osg::Vec4(0.45f, 0.45f, 0.45f, 1.0f) );
	osg::Vec4Array* color_black = new osg::Vec4Array;  
	color_black->push_back(osg::Vec4(0.45f, 0.45f, 0.45f, 1.0f) );// 0.15f 


	osg::Vec3Array* GridVertices = new osg::Vec3Array;

	if(type == 1 || type ==4)
	{
		for(int i=1; i<= count; i++)
		{
			GridVertices->push_back( osg::Vec3( 0,  i*unit, 0) ); 
			GridVertices->push_back( osg::Vec3( count*unit, i*unit, 0) ); 
		}   
		for(int i=1; i<= count; i++)
		{
			GridVertices->push_back( osg::Vec3(i*unit, 0,  0) ); 
			GridVertices->push_back( osg::Vec3(i*unit, count*unit, 0) ); 
		}
	}
	else if(type ==2)//left
	{
		for(int i=1; i<= count; i++)
		{
			GridVertices->push_back( osg::Vec3( 0,0, i*unit) ); 
			GridVertices->push_back( osg::Vec3(0, count*unit,  i*unit) ); 
		}   
		for(int i=1; i<= count; i++)
		{
			GridVertices->push_back( osg::Vec3(0,i*unit, 0) ); 
			GridVertices->push_back( osg::Vec3(0,i*unit, count*unit) ); 
		}
	}
	else if(type ==3)
	{
		for(int i=1; i<= count; i++)
		{
			GridVertices->push_back( osg::Vec3( 0, 0, i*unit) ); 
			GridVertices->push_back( osg::Vec3( count*unit, 0,  i*unit) ); 
		}   
		for(int i=1; i<= count; i++)
		{
			GridVertices->push_back( osg::Vec3(i*unit,  0, 0) ); 
			GridVertices->push_back( osg::Vec3(i*unit, 0, count*unit) ); 
		}
	}

	osg::Vec3Array* GridVertices2 = new osg::Vec3Array;

	for(int i=0; i< GridVertices->size(); i++)
	{
		GridVertices2->push_back( GridVertices->at(i)+ origion) ;
	}
	GridGeometry->setVertexArray( GridVertices2 );

	osg::DrawElementsUInt* GridUnits = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
	for (int i=0; i<4*count; i++ )
	{
		GridUnits->push_back(i);
	}
	GridGeometry->addPrimitiveSet(GridUnits);
	GridGeometry->setColorArray(color_black);//
	GridGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);


	osg::Vec3Array* GridVertices_center = new osg::Vec3Array;
	if(type ==1 || type ==4)
	{
		GridVertices_center->push_back( osg::Vec3( 0, 0, 0) ); 
		GridVertices_center->push_back( osg::Vec3(count*unit, 0, 0) ); 
		GridVertices_center->push_back( osg::Vec3( 0, 0, 0) ); 
		GridVertices_center->push_back( osg::Vec3(0, count*unit, 0) ); 
	}
	else if(type ==2)
	{
		GridVertices_center->push_back( osg::Vec3( 0,0, 0) ); 
		GridVertices_center->push_back( osg::Vec3(0,count*unit, 0) ); 
		GridVertices_center->push_back( osg::Vec3(0, 0, 0) ); 
		GridVertices_center->push_back( osg::Vec3(0,0, count*unit) ); 

	}
	else if(type ==3)
	{
		GridVertices_center->push_back( osg::Vec3( 0, 0, 0) ); 
		GridVertices_center->push_back( osg::Vec3(count*unit, 0, 0) ); 
		GridVertices_center->push_back( osg::Vec3( 0, 0,0) ); 
		GridVertices_center->push_back( osg::Vec3(0,0, count*unit) ); 
	}
	osg::Vec3Array* GridVertices_center2 = new osg::Vec3Array;
	for(int i=0; i< GridVertices_center->size(); i++)
	{
		GridVertices_center2->push_back(GridVertices_center->at(i)+origion)  ;
	}
	GridGeometry_center->setVertexArray( GridVertices_center2 );

	osg::DrawElementsUInt* GridUnits_center = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);

	GridUnits_center->push_back(1);
	GridUnits_center->push_back(0);
	GridUnits_center->push_back(3);
	GridUnits_center->push_back(2);

	GridGeometry_center->addPrimitiveSet(GridUnits_center);
	GridGeometry_center->setColorArray(color_black);
	GridGeometry_center->setColorBinding(osg::Geometry::BIND_OVERALL);

	return Grid;
}
LoadManager::LoadManager(void)
{
	count = 20 ;
	unit    = 1000;
} 

LoadManager::~LoadManager(void)
{
}


osg::Group* LoadManager::LoadTerrain(osg::ref_ptr<osg::Group> root, const CString& strTerrainFile, const CString& strImage, double zFactor, osg::Vec3 center, double radius)
{
	osg::Group* pGroup =  new osg::Group;

	return pGroup;
}
