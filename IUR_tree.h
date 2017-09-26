#if !defined(AFX_QUERY_H__511C4AF2_7288_4C40_80DD_507E2DAD30F3__INCLUDED_)
#define AFX_QUERY_H__511C4AF2_7288_4C40_80DD_507E2DAD30F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IUR_tree.h : header file
//
#include "tesebutton.h"
#include "IR2tree.h"



/////////////////////////////////////////////////////////////////////////////
// CIUR_tree dialog

class CIUR_tree : public CDialog
{
public:
// Dialog Data
	//{{AFX_DATA(CIUR_tree)
	enum { IDD = IDD_QUERY };
	CString	m_qkeywords;
	float	m_qlat;
	float	m_qlon;
	Ctesebutton m_queding;
	Ctesebutton m_quxiao;
	REALTYPE MAXLS;
	REALTYPE MINLS;
	REALTYPE MAXTS;
	REALTYPE MINTS;
	int countdataid;
	int iflazytraveldown;
	int ifinherit;
	int K;
	REALTYPE alfa;
	int methodi;

	CString	m_textinfo;
	CString	m_name;
	float	m_lat;
	float	m_lon;

	typedef struct  _ORGDATA
   {
	   int nodeid;
       char c[3];
       float weidu;
	   float jingdu;
	   char textstr[MaxTextStrLength];
   }ORGDATA;


	//////////////////////////////////////�����ļ�������/////////////////////////////////////////////
	typedef struct _RTREEMBR///////////////////////////////////////////////////////////MBR
	{
	 float bound[SIDES_NUMB]; /* xmin,ymin,...,xmax,ymax,... */
	}RTREEMBR, *pRTREEMBR;

	typedef struct _RTREEBRANCH////////////////////////////////////////////////////////��֧
	{ 
		RTREEMBR mbr;
		int vectorid;
		int intersectionlen; //�ı������ĳ���  
		int unionlen; //�ı������ĳ���
		int objnum;//�ýڵ�������objects���ܸ���
		int childid; // ��Ҷ�ӽڵ�ĺ��ӽڵ�ָ��
		int m_data;               //Ҷ�ӽڵ����ݵ�ID��
	}RTREEBRANCH,*pRTREEBRANCH;
	//vectorid ---> (for non-leaf nodes) file pointer of text (intersection+union, in order to reduce I/O) vectors. 
	//              (for leaf node) total vectors of all the child objects in the node.

	//intersectionlen --->(for non-leaf node) the length of intersection vector, thus int-vector is the former intersectionlen words in the int-uni vector located at vectorid.
	//                    (for leaf node) the begining of the vector of the child object in vectors located at vectorid

	//unionlen ---> (for non-leaf node) the lenght of union vector, thus uni-vector is the later unionlen words in the int-uni vector located at vectorid
	//              (for leaf node) the end of the vector of the child object in vectors located at vectorid

	//objnum ---> the total number of objects contained in the subtree of the node.

	//childid ---> (for non-leaf node) pointer of child nodes; 
	//             (for leaf node) the length of vectors of all the child objects of the node.

	//m_data ---> (for non-leaf node) pointer of child objects; 
	//            (for leaf node) 0.


	typedef struct _RTREENODE///////////////////////////////////////////////////////////�ڵ�
	{
	 int nodeid; //����ڵ��ID
	 int parentid;//���ڵ��
	 int count;//��֧������
	 int level; /* 0 is leaf, others positive */
	 RTREEBRANCH  branch[MAXCARD];
	}RTREENODE,*pRTREENODE; 


	//////////////////////////////////////�����ļ����Σ�/////////////////////////////////////////////
	typedef struct  _VectorCell //�䳤�ĵ�����ÿ��Item�Ľṹ
	{
		int position; //λ�� 
		WEIGHT weight;
	}VectorCell, *pVectorCell;

	typedef struct _FILL
	{
		VectorCell fillvct[MAXWORDS];
		int fillint;
	}FILL;


	typedef struct _RTREEPARTITION /////////////////////////////////////////////////////Ϊ�˷���֮��
	{
	 int   partition[MAXCARD+1]; //��Ӧroot�ṹ�еķ�֧����Ŷ�Ӧ
	 int   total; //ʵ�ʷ�֧��
	 int   minfill; //��С���ɵ����
	 int   taken[MAXCARD+1];//������Ӧ��֧�Ƿ�ռ�����Ƿ���з���
	 int   count[2];//������е�ʵ�ʷ�֧��
	 RTREEMBR cover[2];//�������֧��MBRֵ�ܺ�
	 REALTYPE area[2];//�������֧��MBRֵ��Ӧ�������
	} RTREEPARTITION;


	typedef struct _RTREEROOT
	{
	 RTREENODE*  root_node;
	 RTREEBRANCH  BranchBuf[MAXCARD+1];//ʵ�ʴ�ŵĸ������ѷ�֧
	 int    BranchCount;
	 RTREEMBR  CoverSplit;
	 REALTYPE  CoverSplitArea;
	 RTREEPARTITION Partitions[METHODS];
	} RTREEROOT, * HRTREEROOT;

	///////////////////////////////���ȼ�����for U and candidate
	typedef struct _BOUNDSIM
	{
		int effectid;      //��¼Ӱ���ID��
		REALTYPE boundsim; //��¼Ӱ��ĳ̶�
	}BOUNDSIM,*BOUNDSIMp;

	typedef struct _PRINODE
	{
		int nodeid;   //��¼����Ӱ���ID��
		int parentid; //��¼���ڵ�ID�ţ�Ϊ�̳�����֮��
		int objnum;   //��¼�Ը�Ԫ��Ϊ���ڵ����������objects����
		int intersectionlen;
		int unionlen;
		RTREEMBR mbr;  //�����ڴ�
		pVectorCell vectorp;  //the intersection and union pointor of the entry �����ڴ棬��̬����
		BOUNDSIMp LBSimp; //����Ϊ[K+1], LBSim[0]���MostSim(N,q), ���� 
		BOUNDSIMp UBSimp; //����Ϊ[K+1], UBSim[0]���LeastSim(N,q), ����UBSim[0]Ϊ����Ȩ���׼
		struct _PRINODE *next;
	} PRINODE,*PRINODEp;

	typedef struct _PRIQUEUE
	{
		PRINODEp front;
		PRINODEp rear;
		PRINODEp minpoint;
	} PRIQUEUE,*PRIQUEUEp;


	typedef struct _DEQI
	{
		int nodeid;//Ϊ�˼̳ж�����,��ǰ���ǵ�travelp�ĸ����ڵ�ID��
		BOUNDSIMp LBSimp;//���践��nodeid�Լ���query֮��ľ���
		BOUNDSIMp UBSimp;
	}DEQI,*pDEQI;
	//}}AFX_DATA
    
	CStatic m_StaticCtrl;
	CListCtrl m_ListCtrl;


	// Construction
	public:
	CIUR_tree(CWnd* pParent = NULL);   // standard constructor

	/////branch֮��Ŀ���   b1<-b2
	void CopyBranch(pRTREEBRANCH b1,pRTREEBRANCH b2);

	/////�ڵ�֮��Ŀ������� n1<-n2
	void Copy(pRTREENODE n1,pRTREENODE n2);

	void InitBranch(pRTREEBRANCH p);

	void InitNode(pRTREENODE p);

	void EmptyNode(pRTREENODE p);

	void _RTreeInitPart( RTREEPARTITION *p, int maxrects, int minfill);

	RTREEMBR RTreeCombineRect( RTREEMBR *rc1, RTREEMBR *rc2 );

	RTREEMBR RTreeNodeCover( RTREENODE *node );

	int ObjnumCover(RTREENODE *np);

	int StoreVector(pVectorCell vectorp, int vectorlen);

	void ReadVector(int vectorid, pVectorCell vectorp, int vectorlen);

	void DeleteVector(int vectorid);

	int UpdateVector(int vectorid, pVectorCell NewVectorp, int newvectorlen);

	void VectorTwoCover(pVectorCell OldCoverVctp, int &CoverIntLen, int &CoverUniLen, pVectorCell OneVctp, int IntLen, int UniLen);

	int VectorCover(RTREENODE *np, int &IntLen, int &UniLen);

	REALTYPE RTreeRectSphericalVolume( pRTREEMBR mbr );

	void _RTreeGetBranches(HRTREEROOT root, RTREENODE *node, RTREEBRANCH *br);

	void _RTreeClassify(HRTREEROOT root, int i, int group, RTREEPARTITION *p);

	void _RTreePickSeeds(HRTREEROOT root, RTREEPARTITION *p);

	int RTreePickBranch( RTREEMBR *mbr, RTREENODE *node);

	void _RTreeMethodZero(HRTREEROOT root, RTREEPARTITION *p, int minfill );

	void _RTreeLoadNodes(HRTREEROOT root, RTREENODE *n, RTREENODE *q, RTREEPARTITION *p);

	void SplitNode(HRTREEROOT root, RTREENODE *node, RTREEBRANCH *br, RTREENODE **new_node);

	int AddBranch(HRTREEROOT root, RTREEBRANCH *br, RTREENODE *node, RTREENODE **new_node);

	int _RTreeInsertRect(HRTREEROOT root, RTREEBRANCH *pbranch,  pVectorCell vctcellpp, int branchlen, RTREENODE *node, RTREENODE **new_node, int level);

	void FindRoot(RTREENODE* rootp);

	void BuildTree();

	void QuickSort(pVectorCell numberstrp, int low, int high);

	void Signature(pVectorCell shuzichuan, CString str, int & duan);

	//---------------------following about query----------------------------------//
	afx_msg void OnOKQuery();

	void InitQueue(PRIQUEUEp q);

	void DeQueue(PRIQUEUEp q, DEQI* dequeue);

	void EnQueue(PRIQUEUEp UU, PRINODEp x);

	void ScanProcess(PRINODEp travelpp, int rearfou, PRIQUEUEp UU, PRIQUEUEp candidate, PRIQUEUEp answer, PRIQUEUEp prunedo, PRIQUEUEp prunedn);

	REALTYPE MinDist(RTREEMBR* mbr1, RTREEMBR* mbr2);

	REALTYPE MaxDist(RTREEMBR* mbr1,RTREEMBR* mbr2);

	REALTYPE MostSim(PRINODEp entry1, PRINODEp entry2);

	REALTYPE LeastSim(PRINODEp entry1, PRINODEp entry2);
			
	void AddObjects(PRIQUEUEp list, int nodeid);

	void Apply_prune_queue(int rearfou, PRINODEp e1,  PRINODEp e2,  PRIQUEUEp answer, PRIQUEUEp prunedo, PRIQUEUEp prunedn, int &ff1, int& ff2);
		
	void Apply_prune_list(int rearfou, PRINODE* listp, PRINODEp e2, PRIQUEUEp answer, PRIQUEUEp prunedo, PRIQUEUEp prunedn, int &f);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIUR_tree)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIUR_tree)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUERY_H__511C4AF2_7288_4C40_80DD_507E2DAD30F3__INCLUDED_)
