// IR2_Tree.h : main header file for the IR2_TREE application
//

#if !defined(AFX_IR2_TREE_H__72533789_CB31_43EE_AED7_87BAC2FD459E__INCLUDED_)
#define AFX_IR2_TREE_H__72533789_CB31_43EE_AED7_87BAC2FD459E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CIR2_TreeApp:
// See IR2_Tree.cpp for the implementation of this class
//

class CIR2_TreeApp : public CWinApp
{
public:
	CIR2_TreeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIR2_TreeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CIR2_TreeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IR2_TREE_H__72533789_CB31_43EE_AED7_87BAC2FD459E__INCLUDED_)
