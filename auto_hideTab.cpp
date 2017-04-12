#include "stdafx.h"
#include "auto_hideTab.h"

////////////////////
//////////////////

//CHideClientTab::CHideClientTab(CXTPSplitterWnd* pSplitter)
//{
//	m_bHideMode = FALSE;
//	m_tHeight = 200;
//	m_minY = 10;
//	m_tabHeight = 30;
//	m_pSplitter = pSplitter;
////	GetNavigateButtons()->InsertAt(0, new CHideButton(this));
//
//}
//void CHideClientTab::OnItemClick(CXTPTabManagerItem* pItem)
//{
//	 
//	return CXTPTabControl::OnItemClick(pItem);
//}
//
//// 标签被点击或拖动导致活动标签状态变化时调用
//void CHideClientTab::OnHide()
//{
//	 
//}
//
//CHideButton::CHideButton(CXTPTabManager* pManager)
//	: CXTPTabManagerNavigateButton(pManager, xtpTabNavigateButtonHideBot, xtpTabNavigateButtonAlways)
//{
//}
//void CHideButton::DrawEntry(CDC* pDC, CRect rc)
//{
//	CPoint pt = rc.CenterPoint();
//	if (m_pManager)
//	{
//		CHideClientTab* pTab = (CHideClientTab*)m_pManager;
//		if (!pTab->m_bHideMode)
//		{
//			pDC->MoveTo(pt.x - 4, pt.y - 4);
//			pDC->LineTo(pt.x + 4, pt.y - 4);
//
//			pDC->MoveTo(pt.x - 4, pt.y - 2);
//			pDC->LineTo(pt.x + 4, pt.y - 2);
//
//			pDC->MoveTo(pt.x - 4, pt.y - 2);
//			pDC->LineTo(pt.x, pt.y + 2);
//			pDC->MoveTo(pt.x + 4, pt.y - 2);
//			pDC->LineTo(pt.x, pt.y + 2);
//		}
//		else
//		{
//			pDC->MoveTo(pt.x - 4, pt.y + 4);
//			pDC->LineTo(pt.x + 4, pt.y + 4);
//
//			pDC->MoveTo(pt.x - 4, pt.y + 2);
//			pDC->LineTo(pt.x + 4, pt.y + 2);
//
//			pDC->MoveTo(pt.x - 4, pt.y + 2);
//			pDC->LineTo(pt.x, pt.y - 2);
//			pDC->MoveTo(pt.x + 4, pt.y + 2);
//			pDC->LineTo(pt.x, pt.y - 2);
//		}
//	}
//}
//void CHideButton::OnExecute(BOOL bTick)
//{
//	if (m_pManager && !bTick)
//	{
//		CHideClientTab* pTab = (CHideClientTab*)m_pManager;
//		pTab->OnHide();
//	}
//}
