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
     //按钮的外边框
	CPen m_BoundryPen;

	//鼠标指针置于按钮之上时按钮的内边框
	CPen m_InsideBoundryPenLeft;
	CPen m_InsideBoundryPenRight;
	CPen m_InsideBoundryPenTop;
	CPen m_InsideBoundryPenBottom;

	//按钮获得焦点时按钮的内边框
	CPen m_InsideBoundryPenLeftSel;
	CPen m_InsideBoundryPenRightSel;
	CPen m_InsideBoundryPenTopSel;
	CPen m_InsideBoundryPenBottomSel;

	//按钮的底色，包括有效和无效两种状态
	CBrush m_FillActive;
	CBrush m_FillInactive;

	//按钮的状态
	BOOL m_bOver;  //鼠标位于按钮之上时该值为true,反之为flase
	BOOL m_bTracking; //在鼠标按下没有释放时该值为true
	BOOL m_bSelected; //按钮被按下时为true
	BOOL m_bFocus;   //按钮为当前焦点所在时为true

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
