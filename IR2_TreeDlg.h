// IR2_TreeDlg.h : header file
//

#if !defined(AFX_IR2_TREEDLG_H__4EE4371C_DE6F_4199_878C_824CA46B4367__INCLUDED_)
#define AFX_IR2_TREEDLG_H__4EE4371C_DE6F_4199_878C_824CA46B4367__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "tesebutton.h"

/////////////////////////////////////////////////////////////////////////////
// CIR2_TreeDlg dialog

class CIR2_TreeDlg : public CDialog
{
// Construction
public:
	CIR2_TreeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CIR2_TreeDlg)
	enum { IDD = IDD_IR2_TREE_DIALOG };


		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIR2_TreeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CIR2_TreeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedtree();
	afx_msg void OnClickedQueryiurTree();
	afx_msg void OnClickedQueryciurTree();
	afx_msg void OnClickedBuildciurTree();
	afx_msg void OnBnClickedButton1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IR2_TREEDLG_H__4EE4371C_DE6F_4199_878C_824CA46B4367__INCLUDED_)
