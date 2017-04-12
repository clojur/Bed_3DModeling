#include "stdafx.h"
#include <cmath>
#include "compare_tools.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


double cdb_compare_tools::m_precision = 1e-8;


cdb_compare_tools::cdb_compare_tools()
{
}


cdb_compare_tools::~cdb_compare_tools()
{
}

//////////////////////////////////////////////////////////////////////////
// 其它的所有比较函数都只能依赖于相等和小于来实现
bool cdb_compare_tools::is_equal(double first, double second, double precision)
{
	return (abs(first - second) <= precision);
}

bool cdb_compare_tools::is_less(double first, double second, double precision)
{
	return (!is_equal(first, second, precision) && first < second);
}

//////////////////////////////////////////////////////////////////////////

bool cdb_compare_tools::is_not_equal(double first, double second, double precision)
{
	return !is_equal(first, second, precision);
}

bool cdb_compare_tools::is_great(double first, double second, double precision)
{
	return !is_equal(first, second, precision) && !is_less(first, second, precision);
}

bool cdb_compare_tools::is_great_equal(double first, double second, double precision)
{
	return !is_less(first, second, precision);
}

bool cdb_compare_tools::is_less_equal(double first, double second, double precision)
{
	return is_equal(first, second, precision) || is_less(first, second, precision);
}

double cdb_compare_tools::get_precision()
{
	return m_precision;
}

void cdb_compare_tools::set_precision(double precision)
{
	m_precision = precision;
}

void cdb_compare_tools::restore_default_precision()
{
	m_precision = 1e-8; // 保持与初始化的值相同
}
