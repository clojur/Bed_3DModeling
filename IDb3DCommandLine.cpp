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

// 文字转发接口 ，将其他焦点窗口的输入统一转为命令行输入
void IDb3DCommandLine::OnText(UINT strChar, UINT repeat_count, UINT flags)
{

}

// 获取当前命令行输入，不含前导符内容
CString IDb3DCommandLine::GetCurText() const
{
	CString res;
	return res;
}

// 清空当前命令行中输入的前导符后面的内容
void IDb3DCommandLine::ClearCurText()
{

}

// 判断当前命令行的是否有前导符且仅为系统默认的前导符，前导符后面有用户输入的参数不影响判断结果
// 如果系统默认的前导符为"command:"，则编辑框中为以下内容皆返回false
// ""为空
// "input point:"为用户自定义前导符
// "command:input point:"为默认前导符+用户前导符
bool IDb3DCommandLine::BeginOnlyDefaultPrompt()
{
	bool res = true;
	return true;
}

// 添加命令行提示
// new_line为true则先将编辑框中的有效文字上到列表，然后将传入内容写入编辑框，否则直接将传入内容追加到编辑框内容末尾
// end_line为true表示内容添加后自动刷到列表，为false表示新加的内容留在编辑窗口,只要end_line不为true则中途输出的所有信息均不添加命令行前缀
// 如果输入信息prompt为空，则表示将命令行编辑框中的前导符以及数据刷到列表中，且清空当前编辑框内容并仅显示系统默认前导符
void IDb3DCommandLine::AddPrompt(const CString& prompt, bool new_line, bool end_line)
{
}


// 在编辑框的内容末尾插入新的输入参数，所添加的内容不会当作提示符来处理
// 添加的内容不会导致编辑框中的内容刷到列表中
void IDb3DCommandLine::AddInputText(const CString& str)
{

}

// 设置最后一次执行的命令
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