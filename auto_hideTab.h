#pragma once


#define xtpTabNavigateButtonHideBot 10



//class DB_3DMODELING_API  CHideClientTab : public CXTPTabControl
//{
//public:
//	BOOL m_bHideMode;
//	CXTPSplitterWnd* m_pSplitter;
//private:
//	int  m_tHeight;
//	int  m_minY;
//	int  m_tabHeight;
//public:
//	CHideClientTab(CXTPSplitterWnd* pSplitter);
//	virtual void OnItemClick(CXTPTabManagerItem* pItem);
//public:
//	//
//	void OnHide();
//};
//
//
//class CHideButton : public CXTPTabManagerNavigateButton
//{
//public:
//	CHideButton(CXTPTabManager* pManager);
//protected:
//	virtual void DrawEntry(CDC* pDC, CRect rc);
//	virtual void OnExecute(BOOL bTick);
//};