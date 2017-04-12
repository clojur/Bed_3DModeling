#pragma once

class DB_3DMODELING_API IDb3DCommandLine
{
public:
	IDb3DCommandLine();
	virtual ~IDb3DCommandLine();

	virtual bool ProcessCommand(const CString& cmd, const CString& domain, const CString& strPath = _T(""));

	// ����ת���ӿ� �����������㴰�ڵ�����ͳһתΪ����������
	virtual void OnText(UINT strChar, UINT repeat_count, UINT flags);

	// ��ȡ��ǰ���������룬����ǰ��������
	virtual CString GetCurText() const;

	// ��յ�ǰ�������������ǰ�������������
	virtual void ClearCurText();

	// �жϵ�ǰ�����е��Ƿ���ǰ�����ҽ�ΪϵͳĬ�ϵ�ǰ������ǰ�����������û�����Ĳ�����Ӱ���жϽ��
	// ���ϵͳĬ�ϵ�ǰ����Ϊ"command:"����༭����Ϊ�������ݽԷ���false
	// ""Ϊ��
	// "input point:"Ϊ�û��Զ���ǰ����
	// "command:input point:"ΪĬ��ǰ����+�û�ǰ����
	virtual bool BeginOnlyDefaultPrompt();

	// �����������ʾ
	// new_lineΪtrue���Ƚ��༭���е���Ч�����ϵ��б�Ȼ�󽫴�������д��༭�򣬷���ֱ�ӽ���������׷�ӵ��༭������ĩβ
	// end_lineΪtrue��ʾ������Ӻ��Զ�ˢ���б�Ϊfalse��ʾ�¼ӵ��������ڱ༭����,ֻҪend_line��Ϊtrue����;�����������Ϣ�������������ǰ׺
	// ���������ϢpromptΪ�գ����ʾ�������б༭���е�ǰ�����Լ�����ˢ���б��У�����յ�ǰ�༭�����ݲ�����ʾϵͳĬ��ǰ����
	virtual void AddPrompt(const CString& prompt, bool new_line = true, bool end_line = false);

	// �ڱ༭�������ĩβ�����µ��������������ӵ����ݲ��ᵱ����ʾ��������
	// ��ӵ����ݲ��ᵼ�±༭���е�����ˢ���б���
	virtual void AddInputText(const CString& str);

	// �������һ��ִ�е�����
	virtual void setLastCommand(const CString& strCommand);
};

DB_3DMODELING_API void SetDb3DCommandLine(IDb3DCommandLine* pCommandLine);
DB_3DMODELING_API IDb3DCommandLine* GetDb3DCommandLine();

