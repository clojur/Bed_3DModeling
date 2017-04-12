#pragma once

class  DB_3DMODELING_API cdb_compare_tools
{
private:
	cdb_compare_tools();
	~cdb_compare_tools();

private:
	static double m_precision; // Ĭ��Ϊ1.0e-8

public:
	static bool is_equal(double first, double second, double precision = m_precision);
	static bool is_not_equal(double first, double second, double precision = m_precision);
	static bool is_great(double first, double second, double precision = m_precision);
	static bool is_great_equal(double first, double second, double precision = m_precision);
	static bool is_less(double first, double second, double precision = m_precision);
	static bool is_less_equal(double first, double second, double precision = m_precision);
	
	// ���½ӿڶԾ��ȵ��޸���ȫ���Ե�
	static double get_precision();
	static void set_precision(double precision);
	static void restore_default_precision();
};

