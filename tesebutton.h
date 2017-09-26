#if !defined(AFX_TESEBUTTON_H__22A4FC65_8032_4910_8258_4D6F51A12689__INCLUDED_)
#define AFX_TESEBUTTON_H__22A4FC65_8032_4910_8258_4D6F51A12689__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tesebutton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Ctesebutton window

class Ctesebutton : public CButton
{
// Construction
public:
	Ctesebutton();

// Attributes
protected:
     //��ť����߿�
	CPen m_BoundryPen;

	//���ָ�����ڰ�ť֮��ʱ��ť���ڱ߿�
	CPen m_InsideBoundryPenLeft;
	CPen m_InsideBoundryPenRight;
	CPen m_InsideBoundryPenTop;
	CPen m_InsideBoundryPenBottom;

	//��ť��ý���ʱ��ť���ڱ߿�
	CPen m_InsideBoundryPenLeftSel;
	CPen m_InsideBoundryPenRightSel;
	CPen m_InsideBoundryPenTopSel;
	CPen m_InsideBoundryPenBottomSel;

	//��ť�ĵ�ɫ��������Ч����Ч����״̬
	CBrush m_FillActive;
	CBrush m_FillInactive;

	//��ť��״̬
	BOOL m_bOver;  //���λ�ڰ�ť֮��ʱ��ֵΪtrue,��֮Ϊflase
	BOOL m_bTracking; //����갴��û���ͷ�ʱ��ֵΪtrue
	BOOL m_bSelected; //��ť������ʱΪtrue
	BOOL m_bFocus;   //��ťΪ��ǰ��������ʱΪtrue

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Ctesebutton)
	protected:
		virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DoGradientFill(CDC *pDC, CRect* rect);
	virtual void DrawInsideBorder(CDC *pDC, CRect* rect);
	virtual ~Ctesebutton();
	// Generated message map functions
protected:
	//{{AFX_MSG(Ctesebutton)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESEBUTTON_H__22A4FC65_8032_4910_8258_4D6F51A12689__INCLUDED_)
