#include "stdafx.h"
#include "IDb3DCommandLine.h"


IDb3DCommandLine::IDb3DCommandLine()
{
}


IDb3DCommandLine::~IDb3DCommandLine()
{
}

bool IDb3DCommandLine::ProcessCommand(const CString& cmd, const CString& domain, const CString& strPath)
{
	return true;
}

// ����ת���ӿ� �����������㴰�ڵ�����ͳһתΪ����������
void IDb3DCommandLine::OnText(UINT strChar, UINT repeat_count, UINT flags)
{

}

// ��ȡ��ǰ���������룬����ǰ��������
CString IDb3DCommandLine::GetCurText() const
{
	CString res;
	return res;
}

// ��յ�ǰ�������������ǰ�������������
void IDb3DCommandLine::ClearCurText()
{

}

// �жϵ�ǰ�����е��Ƿ���ǰ�����ҽ�ΪϵͳĬ�ϵ�ǰ������ǰ�����������û�����Ĳ�����Ӱ���жϽ��
// ���ϵͳĬ�ϵ�ǰ����Ϊ"command:"����༭����Ϊ�������ݽԷ���false
// ""Ϊ��
// "input point:"Ϊ�û��Զ���ǰ����
// "command:input point:"ΪĬ��ǰ����+�û�ǰ����
bool IDb3DCommandLine::BeginOnlyDefaultPrompt()
{
	bool res = true;
	return true;
}

// �����������ʾ
// new_lineΪtrue���Ƚ��༭���е���Ч�����ϵ��б�Ȼ�󽫴�������д��༭�򣬷���ֱ�ӽ���������׷�ӵ��༭������ĩβ
// end_lineΪtrue��ʾ������Ӻ��Զ�ˢ���б�Ϊfalse��ʾ�¼ӵ��������ڱ༭����,ֻҪend_line��Ϊtrue����;�����������Ϣ�������������ǰ׺
// ���������ϢpromptΪ�գ����ʾ�������б༭���е�ǰ�����Լ�����ˢ���б��У�����յ�ǰ�༭�����ݲ�����ʾϵͳĬ��ǰ����
void IDb3DCommandLine::AddPrompt(const CString& prompt, bool new_line, bool end_line)
{
}


// �ڱ༭�������ĩβ�����µ��������������ӵ����ݲ��ᵱ����ʾ��������
// ��ӵ����ݲ��ᵼ�±༭���е�����ˢ���б���
void IDb3DCommandLine::AddInputText(const CString& str)
{

}

// �������һ��ִ�е�����
void IDb3DCommandLine::setLastCommand(const CString& strCommand)
{

}
static IDb3DCommandLine*  pSCommandLine = NULL;
void SetDb3DCommandLine(IDb3DCommandLine* pCommandLine)
{
	pSCommandLine = pCommandLine;
}
IDb3DCommandLine* GetDb3DCommandLine()
{
	return pSCommandLine;
}