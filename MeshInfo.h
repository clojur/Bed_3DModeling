#pragma  once
  

enum mesh_type
{
 	mt_lines=1,
	mt_line_strip = 2,
	mt_line_loop = 3,
	mt_trangles=4,
	mt_trangle_strip = 5,
	mt_trangle_fan = 6,
	mt_quads = 7,
	mt_quad_strip = 8,
	mt_polygon = 9,
};


class DB_3DMODELING_API CDb_Point3
{
public:
	CDb_Point3();
	CDb_Point3(double dx, double dy, double dz);
public:
	void Serialize(CArchive& ar);
	double x;
	double y;
	double z;
};


class DB_3DMODELING_API CDb_PrimitiveInfo
{
public:
	CDb_PrimitiveInfo();
	virtual ~CDb_PrimitiveInfo();

	virtual void Serialize(CArchive& ar);

 	CString strName;
	COLORREF color;
	mesh_type meshType;
};


//线对象
class DB_3DMODELING_API CDb_LineInfo : public CDb_PrimitiveInfo
{
public:
	CDb_LineInfo();
	virtual void Serialize(CArchive& ar);
	std::vector<CDb_Point3> m_Ptsvec;//顶点数组
	double lineWidth;
 };

//体对象
class DB_3DMODELING_API CDb_MeshInfo : public CDb_PrimitiveInfo
{
public:
	CDb_MeshInfo();
	virtual void Serialize(CArchive& ar);
	std::vector<CDb_Point3> m_Ptsvec;//顶点数组
	std::vector<int> m_Indexsvec;//索引数组
};
