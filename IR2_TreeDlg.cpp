// IR2_TreeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IR2_Tree.h"
#include "IR2_TreeDlg.h"
#include "IUR_tree.h"
#include "tesebutton.h"
#include "string.h"
#include "stdio.h"
#include "math.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern double diskaccess;
extern double querytime;
extern int querynum;
extern char mainindex[100];
extern char minorindex[100];

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
		ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect r;
	GetClientRect(&r);
	CBrush brush(RGB(255,255,255));
	pDC->FillRect(r,&brush);
	
	return true;
	//return CDialog::OnEraseBkgnd(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CIR2_TreeDlg dialog

CIR2_TreeDlg::CIR2_TreeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIR2_TreeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIR2_TreeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIR2_TreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIR2_TreeDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
//	DDX_Control(pDX, IDOK, m_OK);
//	DDX_Control(pDX, IDCANCEL, m_cancel);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIR2_TreeDlg, CDialog)
	//{{AFX_MSG_MAP(CIR2_TreeDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(BuildIUR_tree, &CIR2_TreeDlg::OnBnClickedtree)
	ON_BN_CLICKED(QueryIUR_tree, &CIR2_TreeDlg::OnClickedQueryiurTree)
	ON_BN_CLICKED(QueryCIUR_tree, &CIR2_TreeDlg::OnClickedQueryciurTree)
	ON_BN_CLICKED(BuildCIUR_tree, &CIR2_TreeDlg::OnClickedBuildciurTree)
	ON_BN_CLICKED(IDC_BUTTON1, &CIR2_TreeDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

BOOL CIR2_TreeDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect r;
	GetClientRect(&r);
	CBrush brush(RGB(122,201,194));
	pDC->FillRect(r,&brush);
	
	return true;
	//return CDialog::OnEraseBkgnd(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CIR2_TreeDlg message handlers

BOOL CIR2_TreeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	//设定各个算法的默认参数

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIR2_TreeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIR2_TreeDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIR2_TreeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



void CIR2_TreeDlg::OnBnClickedtree() //Build IUR-tree
{
	//strcpy(mainindex, "Index Files_queries_output\\IUR-tree\\iur_tree977K.index");
	//strcpy(minorindex, "Index Files_queries_output\\IUR-tree\\textvector977K.index");
	CIUR_tree quIUR_tree;
	quIUR_tree.BuildTree();
	AfxMessageBox("Finish Building IUR-tree!");
}


void CIR2_TreeDlg::OnClickedQueryiurTree() 
{
}


void CIR2_TreeDlg::OnClickedQueryciurTree()
{			
	
}


void CIR2_TreeDlg::OnClickedBuildciurTree()
{
	
}


void CIR2_TreeDlg::OnBnClickedButton1()
{

}
