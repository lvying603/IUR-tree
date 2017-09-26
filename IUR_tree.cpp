// IUR_tree.cpp : implementation file
//

#include "stdafx.h"
#include "IR2_Tree.h"
#include "IUR_tree.h"
#include "math.h"
#include "time.h"
#include "assert.h"
#include "stdlib.h"
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



extern FILE* fp;
extern FILE* fpminor;
extern char mainindex[30];
extern char minorindex[30];
extern double diskaccess;
extern double querytime;
extern int querynum;
extern int bitmap_index_main[MAIN_BITMAPSIZE/sizeof(int)];
extern int bitmap_index_minor[MINOR_BITMAPSIZE/sizeof(int)];


    typedef CIUR_tree::_ORGDATA ORGDATA;

	typedef CIUR_tree::_RTREEMBR RTREEMBR;

	typedef CIUR_tree::_RTREEBRANCH RTREEBRANCH,*pRTREEBRANCH;

	typedef CIUR_tree::_RTREENODE RTREENODE,*pRTREENODE; 

	typedef CIUR_tree::_VectorCell VectorCell, *pVectorCell;

	typedef CIUR_tree::_FILL FILL;

	typedef CIUR_tree::_RTREEPARTITION RTREEPARTITION;

	typedef CIUR_tree::_RTREEROOT RTREEROOT, * HRTREEROOT;

	typedef CIUR_tree::_BOUNDSIM BOUNDSIM,*BOUNDSIMp;

	typedef CIUR_tree::_PRINODE PRINODE,*PRINODEp;

	typedef CIUR_tree::_PRIQUEUE PRIQUEUE,*PRIQUEUEp;

	typedef CIUR_tree::_DEQI DEQI,*pDEQI;

/////////////////////////////////////////////////////////////////////////////
// CIUR_tree dialog


CIUR_tree::CIUR_tree(CWnd* pParent /*=NULL*/)
	: CDialog(CIUR_tree::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIUR_tree)
	m_qkeywords = _T("");
	m_qlat = 0.0f;
	m_qlon = 0.0f;
	MAXLS = 0.0f;
	MINLS = 0.0f;
	MAXTS = 0.0f;
	MINTS = 0.0f;
	countdataid = 0;
	iflazytraveldown = 1;
	ifinherit = 1;
	K = 3;
	alfa = (REALTYPE)0.6;

	m_textinfo = _T("");
	m_name = _T("");
	m_lat = 0.0f;
	m_lon = 0.0f;
	methodi = 0;
	//}}AFX_DATA_INIT
}


void CIUR_tree::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIUR_tree)
	DDX_Text(pDX, IDC_KEYWORDS, m_qkeywords);
	DDX_Text(pDX, IDC_QLAT, m_qlat);
	DDX_Text(pDX, IDC_QLON, m_qlon);
	DDX_Control(pDX, IDOK, m_queding);
	DDX_Control(pDX, IDCANCEL, m_quxiao);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIUR_tree, CDialog)
	//{{AFX_MSG_MAP(CIUR_tree)
	ON_BN_CLICKED(IDOK, OnOKQuery)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIUR_tree message handlers

BOOL CIUR_tree::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect r;
	GetClientRect(&r);
	CBrush brush(RGB(255,255,255));
	pDC->FillRect(r,&brush);
	
	return true;
	//return CDialog::OnEraseBkgnd(pDC);
}

/////branch之间的拷贝   b1<-b2
void CIUR_tree::CopyBranch(pRTREEBRANCH b1,pRTREEBRANCH b2)
{
	b1->childid=b2->childid;
	b1->m_data=b2->m_data;
    b1->objnum=b2->objnum;
	b1->vectorid=b2->vectorid;
	b1->intersectionlen=b2->intersectionlen;
	b1->unionlen=b2->unionlen;
	for(int j=0;j<SIDES_NUMB;j++)
	{
		b1->mbr.bound[j]=b2->mbr.bound[j];
	}
}

/////节点之间的拷贝函数 n1<-n2
void CIUR_tree::Copy(pRTREENODE n1,pRTREENODE n2)
{
	n1->nodeid=n2->nodeid;
	n1->parentid=n2->parentid;
	n1->level=n2->level;
	n1->count=n2->count;
	for(int i=0;i<MAXCARD;i++)
		CopyBranch(&n1->branch[i],&n2->branch[i]);
}


void CIUR_tree::InitBranch(pRTREEBRANCH p)
{
	p->childid=0;
	p->objnum=0;
	p->m_data=0;
	p->vectorid=-1;
	p->intersectionlen=-1;
	p->unionlen=-1;
	for(int i=0;i<SIDES_NUMB;i++) p->mbr.bound[i]=0;
}

void CIUR_tree::InitNode(pRTREENODE p)
{
	p->nodeid=0;
	p->parentid=-2;
	p->count=0;
	p->level=-1;
	for(int j=0;j<MAXCARD;j++)
	{
		InitBranch(&(p->branch[j]));
	}
}

void CIUR_tree::EmptyNode(pRTREENODE p)//与前者不同
{
	int node=p->nodeid;
	InitNode(p);
	p->nodeid=node;
	/*fseek(fp,0,0);
    fread(bitmap_index_main,sizeof(bitmap_index_main),1,fp);
    bitmap_index_main[(node-1)/(8*sizeof(int))]&=~(1<<(PAGE_SIZE*8-countdataid)%(8*sizeof(int)));
	fwrite(bitmap_index_main,sizeof(bitmap_index_main),1,fp);
	fflush(fp);*/
}

/**
 * Initialize a RTREEPARTITION structure.
 */
void CIUR_tree::_RTreeInitPart( RTREEPARTITION *p, int maxrects, int minfill)
{
 int i;
 assert(p);

 p->count[0] = p->count[1] = 0;
 for(int j=0;j<SIDES_NUMB;j++)
	{
		p->cover[0].bound[j]=0;
		p->cover[1].bound[j]=0;
	}
 p->area[0] = p->area[1] = (REALTYPE)0;
 p->total = maxrects;//最大分支数
 p->minfill = minfill;
 for (i=0; i<maxrects; i++)
 {
  p->taken[i] = FALSE;
  p->partition[i] = -1;
 }
}


/**
 * Combine two rectangles, make one that includes both.
 */
 RTREEMBR CIUR_tree::RTreeCombineRect( RTREEMBR *rc1, RTREEMBR *rc2 )//两MBR叠加
{
 int i, j;
 RTREEMBR new_rect;

 if (INVALID_RECT(rc1))
  return *rc2;

 if (INVALID_RECT(rc2))
  return *rc1;

 for (i = 0; i < DIMS_NUMB; i++)
 {
  new_rect.bound[i] = MIN(rc1->bound[i], rc2->bound[i]);//绝了!!!
  j = i + DIMS_NUMB;
  new_rect.bound[j] = MAX(rc1->bound[j], rc2->bound[j]);
 } 
 return new_rect;
}


 /**
 * Find the smallest rectangle that includes all rectangles in branches of a node.
 */
 RTREEMBR CIUR_tree::RTreeNodeCover( RTREENODE *node )
{
 int i, first_time=1;
 RTREEMBR mbr;
 assert(node);

 for(i=0;i<SIDES_NUMB;i++) mbr.bound[i]=0;

 for (i = 0; i < MAXCARD; i++)
 {
  if (node->branch[i].childid || node->branch[i].m_data)
  {
   if (first_time)
   {
    mbr = node->branch[i].mbr;
    first_time = 0;
   }
   else
    mbr = RTreeCombineRect(&mbr, &(node->branch[i].mbr));
  }
 }
 return mbr;
}


int CIUR_tree::ObjnumCover(RTREENODE *np)
{
	int sum=0;
	for(int i=0;i<np->count;i++)
		sum+=np->branch[i].objnum;
	return sum;
}


//-------------store a vector vectorp with length of vectorlen into the disk-------------//
//-------------return the pointer vectorid of the vector---------------------------------//
int CIUR_tree::StoreVector(pVectorCell vectorp, int vectorlen)
{
	if(vectorlen<=0) return -1;

	int vectorid, oldvectorid=0;
	fseek(fpminor,0,0);
	if( !fread(bitmap_index_minor,MINOR_BITMAPSIZE,1,fpminor) )
	{
		AfxMessageBox("read failed!");
	}

	FILL fill;

	for(int i=0; ; i++)
	{
		for(countdataid=0; countdataid<MINOR_BITMAPSIZE*8; countdataid++)
 		{
			if( !( bitmap_index_minor[countdataid/(8*sizeof(int))] & (1<<(8*sizeof(int)-1-countdataid%(sizeof(int)*8)) ) ) )
				break;
		}

		bitmap_index_minor[countdataid/(8*sizeof(int))]|=1<<(8*sizeof(int)-1-countdataid%(sizeof(int)*8));
		
		fseek(fpminor, MINOR_BITMAPSIZE+countdataid*sizeof(FILL), 0);
		if(i==0) vectorid=countdataid;

		
		int storelen=0;
		if( (i+1)*MAXWORDS < vectorlen ) 
		{
			storelen=MAXWORDS;
			for(storelen=0; storelen<MAXWORDS; storelen++)
			{
				fill.fillvct[storelen].position = vectorp[i*MAXWORDS+storelen].position;
				fill.fillvct[storelen].weight = vectorp[i*MAXWORDS+storelen].weight;
			}
			fill.fillint = 0; // will be updated!
			fwrite(&fill, sizeof(FILL), 1, fpminor);
			fflush(fpminor);

			if(i>0)
			{
				fseek(fpminor, MINOR_BITMAPSIZE+(oldvectorid+1)*sizeof(FILL)-sizeof(int), 0);
				fwrite(&countdataid, sizeof(int), 1, fpminor);
				fflush(fpminor);
			}
		}
		else
		{
			for(storelen=0; storelen<vectorlen - MAXWORDS*i; storelen++)
			{
				fill.fillvct[storelen].position = vectorp[i*MAXWORDS+storelen].position;
				fill.fillvct[storelen].weight = vectorp[i*MAXWORDS+storelen].weight;
			}
			fill.fillint = 0; //sure! 
			fwrite(&fill, sizeof(FILL), 1, fpminor);
			fflush(fpminor);

			if(i>0)
			{
				fseek(fpminor, MINOR_BITMAPSIZE+(oldvectorid+1)*sizeof(FILL)-sizeof(int), 0);
				fwrite(&countdataid, sizeof(int), 1, fpminor);
				fflush(fpminor);
			}

			break;
		}

		oldvectorid = countdataid;
			
	}
	fseek(fpminor,0,0);
	if( !fwrite(bitmap_index_minor, MINOR_BITMAPSIZE, 1, fpminor) )
	{
		AfxMessageBox("write failed!");
	}
	fflush(fpminor);
	//fclose(fpminor);
	return vectorid;
}


//-----------read a textual vector from the disk located at vectorid, ----------//
//-----------and return the vector vectorp with length of vectorlen.------------//
//-----------input: vectorid, vectorlen; output: vectorp------------------------//
void CIUR_tree::ReadVector(int vectorid, pVectorCell vectorp, int vectorlen) 
{
	if(vectorid<0) 
	{
		//will not change the input parameters if vectorid=-1
		return;
	}
	if(vectorlen<=0) 
	{
		vectorp=0;
		return;
	}
	int storelen;
	FILL fill;
	for(int i=0; ; i++)
	{
		fseek(fpminor, MINOR_BITMAPSIZE+vectorid*sizeof(FILL), 0);
		if( (i+1)*MAXWORDS <= vectorlen ) 
		{
			fread(&fill,sizeof(FILL),1,fpminor);
			diskaccess++;
			for(storelen=0; storelen<MAXWORDS; storelen++)
			{
				vectorp[i*MAXWORDS+storelen].position = fill.fillvct[storelen].position;
				vectorp[i*MAXWORDS+storelen].weight = fill.fillvct[storelen].weight;
			}
			vectorid = fill.fillint; //update vectorid
		}
		else
		{
			fread(&fill,sizeof(FILL),1,fpminor);
			diskaccess++;
			for(storelen=0; storelen<vectorlen - MAXWORDS*i; storelen++)
			{
				vectorp[i*MAXWORDS+storelen].position = fill.fillvct[storelen].position;
				vectorp[i*MAXWORDS+storelen].weight = fill.fillvct[storelen].weight;
			}
			break;
		}
	}
	//fclose(fpminor);
}

//-----------delete the textual vector from the disk located at vectorid, ----------//
void CIUR_tree::DeleteVector(int vectorid)
{
	if(vectorid<0) return;
	fseek(fpminor, 0, 0);
	fread(bitmap_index_minor, MINOR_BITMAPSIZE, 1, fpminor);
	while(vectorid != 0)
	{
		bitmap_index_minor[vectorid/(8*sizeof(int))]&=~(1<< (8*sizeof(int)-1-vectorid%(8*sizeof(int))) ); //just need set the corresponding bit as 0
		
		fseek(fpminor, MINOR_BITMAPSIZE+(vectorid+1)*sizeof(FILL)-sizeof(int), 0);
		fread(&vectorid, sizeof(int), 1, fpminor); //update vectorid
	}
	fseek(fpminor, 0, 0);
	fwrite(bitmap_index_minor, MINOR_BITMAPSIZE, 1,fpminor);
	//fclose(fpminor);
}

//------------Update the text vector of an index node or text vectors of objects in the leaf node-----------//
//------------with the new vector NewVectorp with length of newvectorlen-----------------//
//------------input: vectorid, NewVectorp, vectorlen; output: vectorid-------------------//
int CIUR_tree::UpdateVector(int vectorid, pVectorCell NewVectorp, int newvectorlen) 
{
	DeleteVector(vectorid);
	vectorid=StoreVector(NewVectorp, newvectorlen);
	return vectorid;
}


//------Cover two int-uni vectors OldCoverVctp and OneVctp, and the covered result is asigned in OldCoverVctp------//
void CIUR_tree::VectorTwoCover(pVectorCell OldCoverVctp, int &CoverIntLen, int &CoverUniLen, pVectorCell OneVctp, int IntLen, int UniLen)
{   //intersectionlen->CoverIntLen, unionlen->CoverUniLen, needilen->IntLen, needulen->UniLen
	//vctcellcoverintersectionp->OldCoverVctp, vctcellcoverunionp->OldCoverVctp+CoverIntLen
	//vctintersectionp->OneVctp, vctunionp->OneVctp+IntLen

	if(OneVctp==0) return;
	if(OldCoverVctp==0)
	{
		//OldCoverVctp = OneVctp;
		for(int onei=0; onei<IntLen+UniLen; onei++)
		{
			OldCoverVctp[onei].position = OneVctp[onei].position;
			OldCoverVctp[onei].weight = OneVctp[onei].weight;
		}
		CoverIntLen = IntLen;
		CoverUniLen = UniLen;
		return;
	}


	int j=0,p=0;
    for(j=0,p=0;j<UniLen && p<CoverUniLen;)//并集计算
	{
		if((OldCoverVctp+CoverIntLen)[p].position<(OneVctp+IntLen)[j].position) p++;
		else if((OldCoverVctp+CoverIntLen)[p].position>(OneVctp+IntLen)[j].position) 
			{
				for(int q=CoverUniLen; q>p; q--)
				{
					(OldCoverVctp+CoverIntLen)[q].position=(OldCoverVctp+CoverIntLen)[q-1].position;
				    (OldCoverVctp+CoverIntLen)[q].weight=(OldCoverVctp+CoverIntLen)[q-1].weight;
				}
				(OldCoverVctp+CoverIntLen)[p].position=(OneVctp+IntLen)[j].position;
				(OldCoverVctp+CoverIntLen)[p].weight=(OneVctp+IntLen)[j].weight;
				CoverUniLen++;
				p++;
				j++;
			}
			else //=
			{
				if((OldCoverVctp+CoverIntLen)[p].weight<(OneVctp+IntLen)[j].weight) 
					(OldCoverVctp+CoverIntLen)[p].weight=(OneVctp+IntLen)[j].weight;
				j++;
				p++;
			}
	}//endfor
    if(j<UniLen && p==CoverUniLen)
	{
		do
		{
			(OldCoverVctp+CoverIntLen)[p].position=(OneVctp+IntLen)[j].position;
			(OldCoverVctp+CoverIntLen)[p].weight=(OneVctp+IntLen)[j].weight;
			p++;
			j++;
			CoverUniLen++;
		}while(j<UniLen);
	}

	int OldCoverIntLen = CoverIntLen;

	for(j=0,p=0;j<IntLen && p<CoverIntLen;)//交集计算
	{
		if(OldCoverVctp[p].position<OneVctp[j].position) 
		{
			for(int t=p+1; t<CoverIntLen; t++)
			{
				OldCoverVctp[t-1].position=OldCoverVctp[t].position;
				OldCoverVctp[t-1].weight=OldCoverVctp[t].weight;
			}
			CoverIntLen--;
		}
		else if(OldCoverVctp[p].position>OneVctp[j].position) j++; 
			else //=
			{
				if(OldCoverVctp[p].weight > OneVctp[j].weight) 
					OldCoverVctp[p].weight=OneVctp[j].weight;
				j++;
				p++;
			}
	}//end for
	CoverIntLen=p;//更新!


	//------------union section move ahead---------------//
	if(CoverIntLen < OldCoverIntLen)
	{
		for(int movei=0; movei<CoverUniLen; movei++)
		{
			OldCoverVctp[movei+CoverIntLen].position = OldCoverVctp[movei+OldCoverIntLen].position;
			OldCoverVctp[movei+CoverIntLen].weight = OldCoverVctp[movei+OldCoverIntLen].weight;
		}
	}
}   

//----------cover the texts of all the child entries of np---------------//
int CIUR_tree::VectorCover(RTREENODE *np, int &IntLen, int &UniLen)
{
	int vectorid=0, j;

	int SelfIntLen = 0;
	int SelfUniLen = 0;

	if(np->level > 0)
	{
		for(j=0; j<np->count; j++) 		
		{
			if(np->branch[j].intersectionlen >= 0) SelfIntLen += np->branch[j].intersectionlen; // the maximum length possible to be
			if(np->branch[j].unionlen >= 0) SelfUniLen += np->branch[j].unionlen;
		}
	}
	else 
	{
		SelfIntLen = np->branch[0].childid;
		SelfUniLen = SelfIntLen;
	}

	if(SelfUniLen <= 0) 
	{
		return -1; //there is no text in the node
	}
	pVectorCell AllOverVctp=0, OneVctp=0;
	if(SelfUniLen + SelfIntLen> 0) AllOverVctp=new VectorCell[SelfUniLen + SelfIntLen];
	if(SelfUniLen + SelfIntLen > 0) OneVctp=new VectorCell[SelfUniLen + SelfIntLen];
	pVectorCell OldAllVctp=0;
	if(np->level == 0 )
	{
		if(np->branch[0].childid > 0) OldAllVctp=new VectorCell[np->branch[0].childid];
		ReadVector(np->branch[0].vectorid, OldAllVctp, np->branch[0].childid);
	}

	if(np->level > 0)//----Initialization
	{
		if(np->branch[0].intersectionlen >= 0) SelfIntLen = np->branch[0].intersectionlen;
		else SelfIntLen = 0;
		if(np->branch[0].unionlen >= 0) SelfUniLen = np->branch[0].unionlen;
		else SelfUniLen = 0;
		ReadVector(np->branch[0].vectorid, AllOverVctp, SelfIntLen + SelfUniLen); 
	}
	else if(np->level==0)
	{
		if(np->branch[0].unionlen >= 0) SelfUniLen = np->branch[0].unionlen - np->branch[0].intersectionlen + 1;
		else SelfUniLen = 0;
		SelfIntLen = SelfUniLen;
		for(j=0; j<SelfUniLen && OldAllVctp; j++)
		{
			AllOverVctp[j].position = OldAllVctp[j].position;
			AllOverVctp[j].weight =  OldAllVctp[j].weight;
			AllOverVctp[j+SelfIntLen].position = OldAllVctp[j].position;//union section
			AllOverVctp[j+SelfIntLen].weight = OldAllVctp[j].weight;
		}
	}
	for(j=1; j<np->count; j++) //----from j=1
	{
		if(np->level > 0) 
		{
			ReadVector(np->branch[j].vectorid, OneVctp, np->branch[j].intersectionlen + np->branch[j].unionlen);
			VectorTwoCover(AllOverVctp, SelfIntLen, SelfUniLen, OneVctp, np->branch[j].intersectionlen, np->branch[j].unionlen);
		}
		else 
		{  //leaf node
			for(int t=np->branch[j].intersectionlen; t>=0 && t<=np->branch[j].unionlen && OldAllVctp; t++)
			{
				OneVctp[t - np->branch[j].intersectionlen].position = OldAllVctp[t].position;
				OneVctp[t - np->branch[j].intersectionlen].weight = OldAllVctp[t].weight;
				OneVctp[t].position = OldAllVctp[t].position;
				OneVctp[t].position = OldAllVctp[t].position;
			}
			VectorTwoCover(AllOverVctp, SelfIntLen, SelfUniLen, OneVctp, np->branch[j].unionlen-np->branch[j].intersectionlen-1, np->branch[j].unionlen-np->branch[j].intersectionlen-1);
		}
	    //----IntLen and UniLen will be returned as the actual length of covered vector------//
	     
	}
	vectorid=StoreVector(AllOverVctp, SelfIntLen+SelfUniLen);


	if(AllOverVctp) 
	{
		delete[] AllOverVctp;
		AllOverVctp = 0;
	}
	if(OneVctp)
	{
		delete[] OneVctp;
		OneVctp = 0;
	}

	IntLen = SelfIntLen; 
	UniLen = SelfUniLen;
	return vectorid;
}


/**
 * Calculate the n-dimensional volume of the bounding sphere of a rectangle.
 * The exact volume of the bounding sphere for the given RTREEMBR.
 */
REALTYPE CIUR_tree::RTreeRectSphericalVolume( pRTREEMBR mbr )
{
 int i;
 REALTYPE sum_of_squares=0, radius;

 if (INVALID_RECT(mbr))
  return (REALTYPE) 0;
 
 for (i=0; i<DIMS_NUMB; i++) {
  REALTYPE half_extent = (mbr->bound[i+DIMS_NUMB] - mbr->bound[i]) / 2;
  sum_of_squares += half_extent * half_extent;
 }
 radius = (REALTYPE)sqrt(sum_of_squares);
 return (REALTYPE)(pow(radius, DIMS_NUMB) * UnitSphereVolume);
}


/**
 * Load branch buffer with branches from full node plus the extra branch.
 */
void CIUR_tree::_RTreeGetBranches(HRTREEROOT root, RTREENODE *node, RTREEBRANCH *br)
{
 int i;

 assert(node && br);
 
 /* load the branch buffer */
 for (i=0; i<MAXKIDS(node); i++)
 {
  assert(node->branch[i].childid || node->branch[i].m_data); /* n should have every entry full */
  CopyBranch(&(root->BranchBuf[i]),&(node->branch[i]));
 }

 CopyBranch(&(root->BranchBuf[MAXKIDS(node)]), br);
 root->BranchCount = MAXKIDS(node) + 1;

 /* calculate mbr containing all in the set */
 root->CoverSplit = root->BranchBuf[0].mbr;

 for (i=1; i<MAXKIDS(node)+1; i++)
 {
  root->CoverSplit = RTreeCombineRect(&root->CoverSplit, &root->BranchBuf[i].mbr);
 }

 root->CoverSplitArea = RTreeRectSphericalVolume(&root->CoverSplit);
 EmptyNode(node);////////////////关键!!!!!!!!!!!
}

 

/**
 * Put a branch in one of the groups.
 */
void CIUR_tree::_RTreeClassify(HRTREEROOT root, int i, int group, RTREEPARTITION *p)
{
 assert(p);
 assert(!p->taken[i]);//前提都尚未分组

 p->partition[i] = group;//第i个分支属于第group组
 p->taken[i] = TRUE;//第i个分支已分过组

 ////////////////////////////////////////////////////////////计算两组的mbr值总和
 if (p->count[group] == 0)//当第group组分支数为零时
  p->cover[group] = root->BranchBuf[i].mbr;
 else
  p->cover[group] = RTreeCombineRect(&root->BranchBuf[i].mbr, &p->cover[group]);
 
 p->area[group] = RTreeRectSphericalVolume(&(p->cover[group]));
 p->count[group]++;
}



/**
 * Pick two rects from set to be the first elements of the two groups.
 * Pick the two that waste the most area if covered by a single rectangle.
 */
void CIUR_tree::_RTreePickSeeds(HRTREEROOT root, RTREEPARTITION *p)
{
 int i, j, seed0=0, seed1=0;
 REALTYPE worst, waste, area[MAXCARD+1];

 for (i=0; i<p->total; i++)
  area[i] = RTreeRectSphericalVolume(&root->BranchBuf[i].mbr);
 
 worst = -root->CoverSplitArea - 1;
 
 for (i=0; i<p->total-1; i++)//扫描各个分支，两两组合，计算waste{area(A+B)-area(A)-area(B)}值最大的一对
 {
  for (j=i+1; j<p->total; j++)
  {
   RTREEMBR one_rect;
   one_rect = RTreeCombineRect(&root->BranchBuf[i].mbr, &root->BranchBuf[j].mbr);
   waste = RTreeRectSphericalVolume(&one_rect) - area[i] - area[j];
   if (waste > worst)
   {
    worst = waste;
    seed0 = i;
    seed1 = j;
   }
  }
 }
 _RTreeClassify(root, seed0, 0, p);
 _RTreeClassify(root, seed1, 1, p);
}


/**
 * Method #0 for choosing a partition:
 * As the seeds for the two groups, pick the two rects that would waste the
 * most area if covered by a single rectangle, i.e. evidently the worst pair
 * to have in the same group.
 * Of the remaining, one at a time is chosen to be put in one of the two groups.
 * The one chosen is the one with the greatest difference in area expansion
 * depending on which group - the mbr most strongly attracted to one group
 * and repelled from the other.
 * If one group gets too full (more would force other group to violate min
 * fill requirement) then other group gets the rest.
 * These last are the ones that can go in either group most easily.
 */
void CIUR_tree::_RTreeMethodZero(HRTREEROOT root, RTREEPARTITION *p, int minfill )
{
 int i;
 REALTYPE biggestDiff;
 int group, chosen=0, betterGroup=0;
 assert(p);

 _RTreeInitPart(p, root->BranchCount, minfill);
 _RTreePickSeeds(root, p);

 while (p->count[0] + p->count[1] < p->total && 
  p->count[0] < p->total - p->minfill && 
  p->count[1] < p->total - p->minfill)
 {
  biggestDiff = (REALTYPE)-1;
  for (i=0; i<p->total; i++)
  {
   if (!p->taken[i])//作用在于此，更新作用，使得while能有效进行下去
   {
    RTREEMBR *r, rect_0, rect_1;
    REALTYPE growth0, growth1, diff;
    
    r = &root->BranchBuf[i].mbr;
    rect_0 = RTreeCombineRect(r, &p->cover[0]);
    rect_1 = RTreeCombineRect(r, &p->cover[1]);
    growth0 = RTreeRectSphericalVolume(&rect_0) - p->area[0]; //计算第i个分支与两组的距离
    growth1 = RTreeRectSphericalVolume(&rect_1) - p->area[1];
    diff = growth1 - growth0;
    if (diff >= 0)//选择距离（面积）增加更少的
     group = 0;
    else
    {
     group = 1;
     diff = -diff;//因为此时diff是负值，而后面需要用其绝对值
    }
    if (diff > biggestDiff)
    {
     biggestDiff = diff;
     chosen = i;
     betterGroup = group;
    }
    else if (diff==biggestDiff && p->count[group]<p->count[betterGroup])//考虑两因素：首先距离，其次两组分支数，原则尽量紧凑，平衡
    {
     chosen = i;
     betterGroup = group;
    }
   }//end if taken
  }//end for
  _RTreeClassify(root, chosen, betterGroup, p);//对于每一个分支都要全盘考虑，也即是从剩余的分支中出发，根据增加的面积最少，挑选其中的分支
                                               //这是第一种方法；第二种则仅考虑自身，从各分支出发。
 }//end while

 /* if one group too full, put remaining rects in the other */
 if (p->count[0] + p->count[1] < p->total)
 {
  if (p->count[0] >= p->total - p->minfill)
   group = 1;
  else
   group = 0;
  
  for (i=0; i<p->total; i++)
  {
   if (!p->taken[i])
    _RTreeClassify(root, i, group, p);
  }
 }
 
 assert(p->count[0] + p->count[1] == p->total);
 assert(p->count[0] >= p->minfill && p->count[1] >= p->minfill);
}



/**
 * int AddBranch(HRTREEROOT root, RTREEBRANCH *br, RTREENODE *node, RTREENODE **new_node)
 * Copy branches from the buffer into two nodes according to the partition.
 */
void CIUR_tree::_RTreeLoadNodes(HRTREEROOT root, RTREENODE *n, RTREENODE *q, RTREEPARTITION *p)
{
 int i;
 assert(n && q && p);

 for (i=0; i<p->total; i++)
 {
  assert(p->partition[i] == 0 || p->partition[i] == 1);
  if (p->partition[i] == 0)
   AddBranch(root, &root->BranchBuf[i], n, NULL);
  else if (p->partition[i] == 1)
   AddBranch(root, &root->BranchBuf[i], q, NULL);
 }
}


/**
 * Split a node.
 * Divides the nodes branches and the extra one between two nodes.
 * Old node is one of the new ones, and one really new one is created.
 * Tries more than one method for choosing a partition, uses best result.
 * SplitNode(root, node, br, new_node);
 */
void CIUR_tree::SplitNode(HRTREEROOT root, RTREENODE *node, RTREEBRANCH *br, RTREENODE **new_node)
{
 RTREEPARTITION *p;
 int level,oldrootid;
 assert(node && br);
 
 /* load all the branches into a buffer, initialize old node */
 level = node->level;//关键!!!!!!!!!!!!记住原来root的层数和nodeid号，否则清空，后果不堪设想!!!
 oldrootid=node->parentid;
 _RTreeGetBranches(root, node, br);


 /* find partition */
 p = &(root->Partitions[0]);///////可见实际分配的内存是在root中，在一开始

 /* Note: can&apos;t use MINFILL(n) below since node was cleared by GetBranches() */
 _RTreeMethodZero(root, p, (level>0 ? MINNODEFILL : MINLEAFFILL));

 node->level=level;
 node->parentid=oldrootid;//count在下面的LoadNodes(,)里头中的AddBranch(,); nodeid在GetBranches()当中并未改变; parentid在增加新节点后才改(若其父亲节点是新的根结点)
 /* put branches from buffer into 2 nodes according to chosen partition */

 
 (*new_node)->level = level;
 (*new_node)->parentid=oldrootid;
 fseek(fp,0,0);
 if(!fread(bitmap_index_main,MAIN_BITMAPSIZE,1,fp))
 {
	 AfxMessageBox("main bitmap reaf fail!");
 }
 for(countdataid=0; countdataid<MAIN_BITMAPSIZE*8; countdataid++)
 {
	 if( !( bitmap_index_main[countdataid/(8*sizeof(int))] & (1<< (8*sizeof(int)-1-countdataid%(sizeof(int)*8)) ) ) )
		break;
 }
 (*new_node)->nodeid=countdataid+1;//定位新插入根节点的位置
 bitmap_index_main[countdataid/(8*sizeof(int))]|=1<<(8*sizeof(int)-1-countdataid%(sizeof(int)*8));
 fseek(fp,0,0);
 if(!fwrite(bitmap_index_main,MAIN_BITMAPSIZE,1,fp))
 {
	 AfxMessageBox("main bitmap write fail!");
 }
 fflush(fp);

 ///////////////////////!!!!!!!!!!!!!!!!!!!!
 fseek(fp,0,0);
 if(!fread(bitmap_index_main,MAIN_BITMAPSIZE,1,fp))
 {
	 AfxMessageBox("main bitmap reaf fail!");
 }
 for(countdataid=0; countdataid<MAIN_BITMAPSIZE*8; countdataid++)
 {
	 if( !( bitmap_index_main[countdataid/(8*sizeof(int))] & (1<< (8*sizeof(int)-1-countdataid%(sizeof(int)*8)) ) ) )
		break;
 }
 ///////////////////////!!!!!!!!!!!!!!!!!!!!

 _RTreeLoadNodes(root, node, *new_node, p);

 RTREENODE child;/////父亲节点分裂了，对应孩子节点的parentid也得改变，这就是动态指针和静态指针的区别


  for(int i=0;i<(*new_node)->count;i++)
 {
	 if((*new_node)->branch[i].childid && (*new_node)->level>0)
	 {
		 fseek(fp,MAIN_BITMAPSIZE+((*new_node)->branch[i].childid-1)*sizeof(RTREENODE),0);
	     fread(&child,sizeof(RTREENODE),1,fp);
	     child.parentid=(*new_node)->nodeid;
	     fseek(fp,MAIN_BITMAPSIZE+(child.nodeid-1)*sizeof(RTREENODE),0);
	     fwrite(&child,sizeof(RTREENODE),1,fp);
	     fflush(fp);
	 }
 }

	 

 assert(node->count+(*new_node)->count == p->total);
}



/**
 * AddBranch(root, &b, node, new_node);
 * Add a branch to a node.  Split the node if necessary.
 * Returns 0 if node not split.  Old node updated.
 * Returns 1 if node split, sets *new_node to address of new node.
 * Old node updated, becomes one of two. AddBranch(root, &b, node, new_node);
 */
 int CIUR_tree::AddBranch(HRTREEROOT root, RTREEBRANCH *br, RTREENODE *node, RTREENODE **new_node)
{
	int i;
	assert(br && node);

	if (node->count < MAXKIDS(node))  /* split won&apos;t be necessary */
	{
		for (i = 0; i < MAXKIDS(node); i++)  /* find empty branch */
		{
			if (node->branch[i].childid == 0 && node->branch[i].m_data==0)
			{
				CopyBranch(&(node->branch[i]),br);
				node->count++;  
				fseek(fp,MAIN_BITMAPSIZE+(node->nodeid-1)*sizeof(RTREENODE),0);
				fwrite(node,sizeof(RTREENODE),1,fp);
				fflush(fp);
				break;
			}
		}
		return 0;
	}
 
	assert(new_node);

	SplitNode(root, node, br, new_node);
 
	return 1;
}


/**
 * Pick a branch.  Pick the one that will need the smallest increase
 * in area to accomodate the new rectangle.  This will result in the
 * least total area for the covering rectangles in the current node.
 * In case of a tie, pick the one which was smaller before, to get
 * the best resolution when searching.
 */
 int CIUR_tree::RTreePickBranch( RTREEMBR *mbr, RTREENODE *node)
{
 RTREEMBR *r;
 int i, first_time = 1;
 REALTYPE increase, bestIncr=(REALTYPE)-1, area, bestArea=0;
 int best=0;
 RTREEMBR tmp_rect;
 assert(mbr && node);

 for (i=0; i<MAXKIDS(node); i++)
 {
  if (node->branch[i].childid)
  {
   r = &node->branch[i].mbr;
   area = RTreeRectSphericalVolume(r);
   tmp_rect = RTreeCombineRect(mbr, r);
   increase = RTreeRectSphericalVolume(&tmp_rect) - area;
   if (increase < bestIncr || first_time)
   {
    best = i;
    bestArea = area;
    bestIncr = increase;
    first_time = 0;
   }
   else if (increase == bestIncr && area < bestArea)
   {
    best = i;
    bestArea = area;
    bestIncr = increase;
   }
  }
 }
 return best;
}



/**
 *_RTreeInsertRect(&root, &branch, root.root_node, &newnode, 0)
 * Inserts a new object (branch) into the index structure.
 * From the root, recursively descends tree, find an appropriate leaf node to insert
 * the branch, and ajust the tree back up if split.
 * Returns 0 if node was not split.  Old node updated.
 * If node was split, returns 1 and sets the pointer pointed to by
 * new_node to point to the new node.  Old node updated to become one of two.
 * The level argument specifies the number of steps up from the leaf
 * level to insert; e.g. a data rectangle goes in at level = 0.
 */
 int CIUR_tree::_RTreeInsertRect(HRTREEROOT root, RTREEBRANCH *pbranch,  pVectorCell vctcellpp, int branchlen, RTREENODE *node, RTREENODE **new_node, int level)
{//root为树根；mbr为需插入的项值；tid为项标识；node为当前找到的节点；
 int i;
 RTREEBRANCH b;
 InitBranch(&b);

 //assert(pbranch->mbr && node && new_node);
 assert(level >= 0 && level <= node->level);//because node begin from the root

 /* Still above level for insertion, go down tree recursively */
 if (node->level > level)
 {
	  i = RTreePickBranch(&(pbranch->mbr), node);
	  RTREENODE child;
	  InitNode(&child);
	  fseek(fp,MAIN_BITMAPSIZE+(node->branch[i].childid-1)*sizeof(RTREENODE),0);
	  fread(&child,sizeof(RTREENODE),1,fp);

	   RTREENODE *n2;
	   n2=new RTREENODE;
	   InitNode(n2);

	  if (!_RTreeInsertRect(root, pbranch, vctcellpp, branchlen, &child, &n2, level))
	  {
		  //--- child was not split , update the info of parent node <adjust the tree from bottom to top>
		  node->branch[i].mbr = RTreeCombineRect(&(pbranch->mbr), &(node->branch[i].mbr));
		  node->branch[i].objnum+=pbranch->objnum;

		  int IntLen=0, UniLen=0;
		  if(node->branch[i].intersectionlen >= 0) IntLen=node->branch[i].intersectionlen; 
		  if(node->branch[i].unionlen >= 0) UniLen=node->branch[i].unionlen;
		  pVectorCell OldAllVctp=0;
		  if(IntLen + UniLen + 2*branchlen > 0)
		  {
			  OldAllVctp=new VectorCell[IntLen + UniLen + 2*branchlen]; // the maximum length possible to be
		  }
		  
		  ReadVector(node->branch[i].vectorid, OldAllVctp, IntLen + UniLen); //read the orignal vector of the node to OldAllVctp
		 
		  pVectorCell IntUniVctp=0;
		  if(branchlen>0)
		  {
			  IntUniVctp=new VectorCell[branchlen + branchlen];
			  for(int vectori=0; vectori<branchlen; vectori++)
			  {
				  IntUniVctp[vectori].position = vctcellpp[vectori].position;
				  IntUniVctp[vectori].weight =  vctcellpp[vectori].weight;
				  IntUniVctp[vectori+branchlen].position = vctcellpp[vectori].position;
				  IntUniVctp[vectori+branchlen].weight = vctcellpp[vectori].weight;
			  }
		  }
		  VectorTwoCover(OldAllVctp, IntLen, UniLen, IntUniVctp, branchlen, branchlen); //return the updated OldAllVctp and the actual covered length 

		  node->branch[i].vectorid=UpdateVector(node->branch[i].vectorid, OldAllVctp, IntLen+UniLen);
		  node->branch[i].intersectionlen = IntLen;
		  node->branch[i].unionlen = UniLen;

		  fseek(fp,MAIN_BITMAPSIZE+(node->nodeid-1)*sizeof(RTREENODE),0);
		  fwrite(node,sizeof(RTREENODE),1,fp);
		  fflush(fp);
		  if(IntUniVctp) 
		  {
			  delete[] IntUniVctp;
			  IntUniVctp = 0;
		  }
		  if(OldAllVctp!=0) 
		  {
			  delete[] OldAllVctp;
			  OldAllVctp = 0;
		  }
		  delete n2;
		  return 0;
	  }
  
	  /* child was split */
	  node->branch[i].mbr = RTreeNodeCover(&child);//不同于B+树，因为R树中节点分成的两子节点 是混的，
	  node->branch[i].childid=child.nodeid;
	  node->branch[i].objnum=ObjnumCover(&child);
	  DeleteVector(node->branch[i].vectorid); //!!!update vectorid
	  node->branch[i].vectorid=VectorCover(&child, node->branch[i].intersectionlen, node->branch[i].unionlen); 


	  b.childid = n2->nodeid;
	  b.objnum=ObjnumCover(n2);
	  b.vectorid=VectorCover(n2, b.intersectionlen, b.unionlen); //before adding the branch, the vector has been stored.
	  b.mbr = RTreeNodeCover(n2);
	  delete n2;
	  return AddBranch(root, &b, node, new_node);
 } 
 else if (node->level == level) /* Have reached level for insertion. Add mbr, split if necessary */
 {  
	CopyBranch(&b, pbranch);

	if(branchlen>0)
	{
		b.intersectionlen = node->branch[0].childid;
		b.unionlen = node->branch[0].childid + branchlen -1;
	}
	
	pVectorCell OldAllVctp; //---buffer the orignal vectors of all the child objects in the leaf node node.
	if(node->branch[0].childid+branchlen > 0) 
	{
		OldAllVctp = new VectorCell[node->branch[0].childid+branchlen];
	}
	else OldAllVctp = 0;

	ReadVector(node->branch[0].vectorid, OldAllVctp, node->branch[0].childid); //read the orignal vectors
	for(int j=node->branch[0].childid; j<node->branch[0].childid+branchlen; j++)
	{
		OldAllVctp[j].position = vctcellpp[j - node->branch[0].childid].position;
		OldAllVctp[j].weight = vctcellpp[j - node->branch[0].childid].weight;
	}


	//----------------borrow the entry of childid of the first child object in the leaf node 
	//----------------to store the total length of the vectors of all the child objects in the leaf node

	

	if(AddBranch(root, &b, node, new_node))//if split, then update texts for node and new_node
	{
		int totallen=0, j, i;
		pVectorCell AllOverVctp=0;

		//-------update the texts for new_node---------//
		for(j=0; j<(*new_node)->count; j++) 
		{
			if((*new_node)->branch[j].unionlen>=0)
			{
				totallen+=(*new_node)->branch[j].unionlen - (*new_node)->branch[j].intersectionlen + 1;
			}
		}
		if(totallen>0) AllOverVctp = new VectorCell[totallen];

		for(j=0, i=0; j<(*new_node)->count && OldAllVctp && AllOverVctp; j++) 
		{
			if( (*new_node)->branch[j].intersectionlen>=0 && (*new_node)->branch[j].unionlen>=0 ) 
			{
				for(int t=(*new_node)->branch[j].intersectionlen; t>=0 && t<= (*new_node)->branch[j].unionlen; t++)//////
				{
					AllOverVctp[i].position = OldAllVctp[t].position;
					AllOverVctp[i].weight = OldAllVctp[t].weight;

					if(t==(*new_node)->branch[j].intersectionlen) (*new_node)->branch[j].intersectionlen = i; //------update the text index for new_node
					if(t==(*new_node)->branch[j].unionlen) 
						(*new_node)->branch[j].unionlen = i;

					i++;
				}
			}
		}
		(*new_node)->branch[0].vectorid=StoreVector(AllOverVctp, totallen);/////!!!!!!!!!!!!

		for(j=1; j<(*new_node)->count; j++) 
		{
			if( (*new_node)->branch[j].intersectionlen>=0 && (*new_node)->branch[j].unionlen>=0)
			{
				(*new_node)->branch[j].vectorid = (*new_node)->branch[0].vectorid;
			}
		}
		(*new_node)->branch[0].childid = totallen;

		fseek(fp, MAIN_BITMAPSIZE+((*new_node)->nodeid-1)*sizeof(RTREENODE), 0);
		fwrite(*new_node, sizeof(RTREENODE), 1, fp);
		fflush(fp);
		if(AllOverVctp) 
		{
			delete[] AllOverVctp;
			AllOverVctp=0;
		}

		//-------update the texts for node---------//
		totallen=0;
		for(j=0; j<node->count; j++) 
		{
			if(node->branch[j].unionlen>=0) totallen+=node->branch[j].unionlen - node->branch[j].intersectionlen+1;
		}
		if(totallen>0) AllOverVctp = new VectorCell[totallen];
		else AllOverVctp = 0;

		for(j=0, i=0; j<node->count && OldAllVctp && AllOverVctp; j++) 
		{
			if(node->branch[j].intersectionlen>=0 && node->branch[j].unionlen>=0)
			{
				for(int t=node->branch[j].intersectionlen; t>=0 && t<= node->branch[j].unionlen; t++)
				{
					AllOverVctp[i].position = OldAllVctp[t].position;
					AllOverVctp[i].weight = OldAllVctp[t].weight;

					if(t==node->branch[j].intersectionlen) node->branch[j].intersectionlen = i; //------update the text index for new_node
					if(t==node->branch[j].unionlen) 
						node->branch[j].unionlen = i;

					i++;
				}
			}
		}
		node->branch[0].vectorid=UpdateVector(node->branch[0].vectorid, AllOverVctp, totallen);/////!!!!!!!!!!!!
		for(j=1; j<node->count; j++) 
		{
			if(node->branch[j].intersectionlen>=0 && node->branch[j].unionlen>=0)
			{
				node->branch[j].vectorid = node->branch[0].vectorid;
			}
		}
		node->branch[0].childid=totallen;

		fseek(fp, MAIN_BITMAPSIZE+(node->nodeid-1)*sizeof(RTREENODE), 0);
		fwrite(node, sizeof(RTREENODE), 1, fp);
		fflush(fp);
		if(AllOverVctp) 
		{
			delete[] AllOverVctp;
			AllOverVctp=0;
		}

		if(OldAllVctp)
		{
			delete[] OldAllVctp;
			OldAllVctp=0;
		}
		return 1;
	}
	else //not split
	{
		if(branchlen > 0)
		{
			node->branch[0].vectorid = UpdateVector(node->branch[0].vectorid, OldAllVctp, node->branch[0].childid+branchlen);

			for(int counti=1; counti<node->count; counti++)
			{
				if(node->branch[counti].intersectionlen>=0 && node->branch[counti].unionlen>=0)
				{
					node->branch[counti].vectorid=node->branch[0].vectorid;
				}
			}

			node->branch[0].childid += branchlen;
		}
			
		fseek(fp,MAIN_BITMAPSIZE+(node->nodeid-1)*sizeof(RTREENODE),0);
		fwrite(node, sizeof(RTREENODE), 1, fp); //update the node since the vectorid and length of its first object is changed.
		fflush(fp);
		
		delete[] OldAllVctp;
		return 0;
	}
 }
 
 /* Not supposed to happen */
 assert (FALSE);
 return 0;
}


void CIUR_tree::FindRoot(RTREENODE* rootp)
{
	fseek(fp,0L,SEEK_END);
	long leng=ftell(fp);
	for(int i=MAIN_BITMAPSIZE; i<(int)leng; i=i+sizeof(RTREENODE))//扫描每个节点,找根结点
	{
		  fseek(fp,i,0);
		  fread(rootp,sizeof(RTREENODE),1,fp);
		  if(rootp->parentid==-1) 
		  {                          ////////////////////////////////找到了根结点
			  break;
		  }//end if
		  
	}//end for
}


//-------------------------------------------------following about query-----------------------------------------//

void CIUR_tree::InitQueue(PRIQUEUEp q)
{
	q->front=0;
	q->rear=0;                           
	q->minpoint=0;
}


//返回出minpoint对应元素的信息(nodeid, LBSimp[K], UBSimp[K]),并调整minpoint指针
//返回该元素的同时并，释放其vector的空间以及元素本身的空间
void CIUR_tree::DeQueue(PRIQUEUEp q, DEQI* dequeue)
{
	PRINODEp p=q->front;
	PRINODEp temp=q->minpoint;
	int flagmin=0;
	dequeue->nodeid=q->minpoint->nodeid;//返回结果节点号
	dequeue->LBSimp = q->minpoint->LBSimp;
	dequeue->UBSimp = q->minpoint->UBSimp;
    
	if(p==q->minpoint) //当minpoint = front 时  调整指针minpoint.
	{
		q->front=p->next;
		if(p->vectorp)
		{
			delete[] p->vectorp;
		}
		delete p;
		p=q->front;
		if(!p) 
		{
			q->rear=p;
			q->minpoint=p;
		}
		if(p) 
		{
			q->minpoint=p;
		}
		flagmin=1;
	}

	q->minpoint=q->front;
	while(p)
	{
		if(!flagmin && p->next==temp)
		{
			p->next=p->next->next;
			if(temp == q->rear) q->rear=p;//删除的是队尾
			delete[] temp->vectorp;
			delete temp; 
		}
		else
		{
			if(p->UBSimp[0].boundsim > q->minpoint->UBSimp[0].boundsim) q->minpoint=p;//更新minpoint的位置
			p=p->next;
		}
	}
}


//候选优先队列元素x已经分配好空间，现在就需插入优先队列，并调整minpoint指针。
void CIUR_tree::EnQueue(PRIQUEUEp UU, PRINODEp x)
{
	int i;
	if(UU->front==0 && x->parentid==-1) //入队列的是根结点
	{
		for(i=0;i<SIDES_NUMB;i++)
		{
			x->mbr.bound[i]=0;
		}
        x->vectorp=0;
		x->intersectionlen=0;
		x->unionlen=0;
		x->objnum=0;
		for(i=0; i<=K; i++)
		{
			x->LBSimp[i].boundsim=0;
			x->LBSimp[i].effectid=0;
			x->UBSimp[i].boundsim=0;
			x->UBSimp[i].effectid=0;
		}
		x->next=NULL;
		UU->front=x;
		UU->minpoint=x;
		UU->rear=x;
		return;
	}
	
	///////////////////////对于入节点不是根结点的情况/////////////////////////
	x->next=NULL;//完成x的赋值
	
   	if(UU->front==NULL) //原优先队列为空
	{
		UU->front=x;
		UU->rear=x;
		UU->minpoint=x;
	}
	else 
	{
		//front指针不用改变
		UU->rear->next=x;
		UU->rear=x;
    	if(x->UBSimp[0].boundsim > UU->minpoint->UBSimp[0].boundsim) UU->minpoint=x; 
	}
}	


//遍历扫描answer, U,同时更新自己以及队列中的元素
//ScanProcess(travelp, 0, U, candidate, answer, prunedo, prunedn)
//
void CIUR_tree::ScanProcess(PRINODEp travelpp, int rearfou, PRIQUEUEp UU, PRIQUEUEp candidate, PRIQUEUEp answer, PRIQUEUEp prunedo, PRIQUEUEp prunedn)
{
	int f1=0, felemans;//分别标志e是否被answer里头的元素确定为pruned(answer)

	for(PRINODE* plist=answer->front; f1==0 && plist; plist=plist->next)//用answer里头的元素
	{
		//::diskaccess++;
		Apply_prune_list(rearfou, plist, travelpp, answer, prunedo, prunedn, f1); //f1被其中一个给确定了，就被确定了
	}
    PRINODE  *pqueue, *pre=0;
	for(pqueue=candidate->front, pre=pqueue; pqueue;)//用candidate里头的元素
	{
		//::diskaccess++;
		Apply_prune_queue(rearfou, pqueue, travelpp, answer, prunedo, prunedn, f1, felemans); //mutual effect!
		if(felemans==0)
		{
			pre = pqueue;
			pqueue=pqueue->next;
		}
		else
		{
			if(felemans==1)//e1 is an answer
			{
				if(pre==pqueue)
				{
					candidate->front=0;
				}
				else pre->next = pqueue->next;

				pre = pqueue->next;//---remove from candidate

				pqueue->next = answer->front;//---add to answer
				answer->front = pqueue;

				pqueue = pre;
			}
			else//felemans==2
			{
				if(pre==pqueue)
				{
					candidate->front=0;
				}
				else pre->next = pqueue->next; //----remove from candidate

				pre = pqueue->next; //----used for updating

				pqueue->next = prunedo->front;//---add to prunedo
				prunedo->front = pqueue;

				pqueue = pre;//---update
			}
		}
	}

    for(pqueue=UU->front, pre=pqueue; pqueue; )//用U里头的元素
	{
		//说明travelpp未被确定；注意对travelpp 和队列列表中的元素的处理不同
		Apply_prune_queue(rearfou, pqueue, travelpp, answer, prunedo, prunedn, f1, felemans);
		if(felemans==0)
		{
			pre = pqueue;
			pqueue=pqueue->next;
		}
		else
		{
			if(felemans==1)//e1 is an answer
			{
				if(pre==pqueue)
				{
					UU->front=0;
				}
				else pre->next = pqueue->next;//---remove from answer

				pre = pqueue->next; //used for updating pqueue!

				pqueue->next = answer->front;//---add to answer
				answer->front = pqueue;

				pqueue = pre;//update pqueue
			}
			else//felemans==2
			{
				if(pre==pqueue)
				{
					UU->front = 0;
				}
				else pre->next = pqueue->next;//---remove from candidate

				pre = pqueue->next; //----//used for updating pqueue!
				
				assert(pqueue->nodeid>0);
				pqueue->next = prunedn->front;//---add to prunedo
				prunedn->front = pqueue;

				pqueue = pre;
			}
		}
	}


//--------------------------------check whether using the "lazy travel down" technique---------------------------------//
	for(PRINODE* polist=prunedo->front; iflazytraveldown==0 && f1==0 && polist; polist=polist->next)//用answer里头的元素
	{
		//Apply_prune_list(rearfou, plist, travelpp, answer, prunedo, prunedn, f1); 
		//::diskaccess++;
		Apply_prune_list(rearfou, polist, travelpp, answer, prunedo, prunedn, f1); //f1被其中一个给确定了，就被确定了
	}
	

	if(travelpp->nodeid==421)
	{
		BOUNDSIM* temp = new BOUNDSIM[K+1];
		for(int yingi=0; yingi<K+1; yingi++)
		{
			temp[yingi].boundsim = travelpp->LBSimp[yingi].boundsim;
			temp[yingi].effectid = travelpp->LBSimp[yingi].effectid;
		}
		if(travelpp->LBSimp)
		{
			delete[] travelpp->LBSimp;
			travelpp->LBSimp = 0;
		}
		travelpp->LBSimp = temp;
	}


	pre=prunedn->front;
	for(PRINODE* pnlist=prunedn->front; iflazytraveldown==0 && f1==0 && pnlist; )//用answer里头的元素
	{
		
		assert(pnlist->nodeid>=0);
		//::diskaccess++;
		Apply_prune_list(rearfou, pnlist, travelpp, answer, prunedo, prunedn, f1); //f1被其中一个给确定了，就被确定了
		if(f1==1) break;

		PRINODE* templist=pnlist;
		if(pre==pnlist)
		{
			prunedn->front=pnlist->next;
			pre=pnlist->next;
			pnlist=pnlist->next;
		}
		else 
		{
			pre->next = pnlist->next;
			pnlist = pnlist->next;
		}
		
		if(templist->nodeid<0)
		{
			templist->next = prunedo->front;
			prunedo->front = templist;
			continue;
		}
		//-----------------------提前对已经prunedn的index nodes进行访问--------------------------//
		
		fseek(fp,MAIN_BITMAPSIZE+(templist->nodeid-1)*sizeof(RTREENODE),0);
		if(templist->vectorp) 
		{
			delete[] templist->vectorp;
			templist->vectorp=0;
		}
		if(templist->LBSimp)
		{
			delete[] templist->LBSimp;
			templist->LBSimp=0;
		}
		if(templist->UBSimp)
		{
			delete[] templist->UBSimp;
			templist->UBSimp=0;
		}
		delete templist;
		templist=0;


		RTREENODE node;
		fread(&node,sizeof(RTREENODE),1,fp);
		diskaccess++;
		pVectorCell LeafVctIntUnip=0;
		for(int chi=0; chi<node.count; chi++)
		{
			PRINODEp pn = new PRINODE;

			if(node.level>0) pn->nodeid = node.branch[chi].childid;
			else pn->nodeid = 0 - node.branch[chi].m_data;

			for(int i=0;i<SIDES_NUMB;i++)
			{
				pn->mbr.bound[i]=node.branch[chi].mbr.bound[i];
			}
			pn->next = 0;
			pn->objnum=node.branch[chi].objnum;
			pn->parentid = node.nodeid;
			pn->LBSimp = 0;
			pn->UBSimp = 0;

			/////////////////////!!!!!!!!!!!通过vectorid从文件中获取vector串!!!!!!!!!!!!!!!!!///////////////////
			if(node.level>0)
			{
				if(node.branch[chi].intersectionlen+node.branch[chi].unionlen>0)
				{
					pn->intersectionlen = node.branch[chi].intersectionlen;
					pn->unionlen = node.branch[chi].unionlen;
					pn->vectorp = new VectorCell[pn->intersectionlen+pn->unionlen];
					ReadVector(node.branch[chi].vectorid, pn->vectorp, pn->intersectionlen+pn->unionlen);
				}
				else 
				{
					pn->intersectionlen = 0;
					pn->unionlen = 0;
					pn->vectorp = 0;
				}
			}
			else
			{
				if(node.branch[0].childid>0)
				{
					if(chi==0)
					{
						LeafVctIntUnip = new VectorCell[node.branch[0].childid];
						ReadVector(node.branch[0].vectorid, LeafVctIntUnip, node.branch[0].childid);
						pn->intersectionlen = 0; //notice this! use nodeid<0 to identify.
						pn->unionlen = node.branch[0].unionlen - node.branch[0].intersectionlen + 1;
						pn->vectorp = LeafVctIntUnip;
					}
					else//chi>0 && chi<node.count
					{
						pn->intersectionlen = node.branch[chi].unionlen - node.branch[chi].intersectionlen + 1;
						pn->unionlen = pn->intersectionlen;
						pn->vectorp = LeafVctIntUnip + node.branch[chi].intersectionlen;
					}
				}
				else
				{
					pn->vectorp = 0;
					pn->intersectionlen = 0;
					pn->unionlen = 0;
				}
			}
			/////////////////////!!!!!!!!!!!通过vectorid从文件中获取vector串!!!!!!!!!!!!!!!!!///////////////////
			if(node.level>0)
			{
				pn->next=prunedn->front;
				prunedn->front=pn;
			}
			else
			{
				pn->next=prunedo->front;
				prunedo->front=pn;
			}
		}//end for each chi
	}




	if(f1==0)//表示travelpp未被确定
	{
		if(travelpp->nodeid>0)//标记为node
		{
			EnQueue(UU,travelpp);
		}
		else EnQueue(candidate, travelpp);
	}

}


REALTYPE CIUR_tree::MinDist(RTREEMBR* mbr1, RTREEMBR* mbr2)
{
	REALTYPE dist;
	//if((mbr1->bound[2]>mbr2->bound[0])&&(mbr1->bound[3]>mbr2->bound[1])||(mbr2->bound[2]>mbr1->bound[0])&&(mbr2->bound[3]>mbr1->bound[1]))
	if((mbr2->bound[0]>=mbr1->bound[0]&&mbr2->bound[0]<=mbr1->bound[2] || mbr2->bound[2]>=mbr1->bound[0]&&mbr2->bound[2]<=mbr1->bound[2]) && (mbr2->bound[1]>=mbr1->bound[3]&&mbr2->bound[1]<=mbr1->bound[3] || mbr2->bound[3]>=mbr1->bound[1]&&mbr2->bound[3]<=mbr1->bound[3]))
		return 0;
	if(mbr2->bound[1]>=mbr1->bound[3])
	{
		if(mbr2->bound[0]>=mbr1->bound[2])
			dist=(REALTYPE)sqrt(pow(mbr2->bound[0]-mbr1->bound[2],2)+pow(mbr2->bound[1]-mbr1->bound[3],2));
		else if(mbr2->bound[2]<mbr1->bound[0])
			dist=(REALTYPE)sqrt(pow(mbr2->bound[2]-mbr1->bound[0],2)+pow(mbr2->bound[1]-mbr1->bound[3],2));
		else dist=(REALTYPE)(mbr2->bound[1]-mbr1->bound[3]);
	}
	else if(mbr2->bound[3]<=mbr1->bound[1])
	{
		if(mbr2->bound[0]>=mbr1->bound[2])
			dist=(REALTYPE)sqrt(pow(mbr2->bound[0]-mbr1->bound[2],2)+pow(mbr1->bound[1]-mbr2->bound[3],2));
		else if(mbr2->bound[2]<=mbr1->bound[0])
			dist=(REALTYPE)sqrt(pow(mbr1->bound[0]-mbr2->bound[2],2)+pow(mbr1->bound[1]-mbr2->bound[3],2));
		else dist=(REALTYPE)(mbr1->bound[1]-mbr2->bound[3]);
	}
	else if(mbr2->bound[2]<mbr1->bound[0])
		dist=(REALTYPE)(mbr1->bound[0]-mbr2->bound[2]);
	else dist=(REALTYPE)(mbr2->bound[0]-mbr1->bound[2]);
	return dist;
}


REALTYPE CIUR_tree::MaxDist(RTREEMBR* mbr1,RTREEMBR* mbr2)
{
	REALTYPE dist;
	REALTYPE loc1[2]={0}, loc2[2]={0};
	for(int i=0;i<2;i++)//for each dimension
	{
		if((mbr1->bound[i]>=mbr2->bound[i+2]) || (mbr1->bound[i]>mbr2->bound[i] && mbr1->bound[i]<mbr2->bound[i+2]))
		{
			loc1[i]=mbr1->bound[i+2];
			loc2[i]=mbr2->bound[i];
		}
		else if((mbr1->bound[i+2]>mbr2->bound[i] && mbr1->bound[i+2]<mbr2->bound[i+2]) || mbr1->bound[i+2]<mbr2->bound[i])
		{
			loc1[i]=mbr1->bound[i];
		    loc2[i]=mbr2->bound[i+2];
		}
		else
		{
			if((mbr1->bound[i+2]-mbr2->bound[i])>=mbr1->bound[i]-mbr2->bound[i+2])
			{
				loc1[i]=mbr1->bound[i+2];
				loc2[i]=mbr1->bound[i];
			}
			else 
			{
				loc1[i]=mbr1->bound[i];
				loc2[i]=mbr2->bound[i+2];
			}
		}
	}
	dist=(REALTYPE)sqrt(pow(loc1[0]-loc2[0],2)+pow(loc1[1]-loc2[1],2));
    return dist;
}


REALTYPE CIUR_tree::MostSim(PRINODEp entry1, PRINODEp entry2)
{
	REALTYPE locationsim=alfa * (MAXLS-MinDist(&(entry1->mbr), &(entry2->mbr)))/(MAXLS-MINLS);
	if(alfa==1)
	{
		return locationsim;
	}
	if(entry1->intersectionlen==-1 || entry1->unionlen==-1) 
	{
		return locationsim;
	}
	if(entry2->intersectionlen==-1 || entry2->unionlen==-1)
	{
		return locationsim;
	}
	int E1IntLen=0, E1UniLen=0, E1Uoffset=0, E2Uoffset=0, E2IntLen=0, E2UniLen=0;
	if(entry1->nodeid<=0)//----------object
	{
		E1IntLen = entry1->unionlen;
		E1UniLen = E1IntLen;
		E1Uoffset = 0;
	}
	else
	{
		E1IntLen = entry1->intersectionlen;
		E1UniLen = entry1->unionlen;
		E1Uoffset = entry1->intersectionlen;
	}
	
	if(entry2->nodeid<=0)//----------object
	{
		E2IntLen = entry2->unionlen;
		E2UniLen = E2IntLen;
		E2Uoffset = 0;
	}
	else
	{
		E2IntLen = entry2->intersectionlen;
		E2UniLen = entry2->unionlen;
		E2Uoffset = entry2->intersectionlen;
	}

	int vectorlen=E1UniLen+E2UniLen;
	WEIGHT* vector1=0, *vector2=0;
	if(vectorlen>0)
	{
		vector1=new WEIGHT[vectorlen];
		vector2=new WEIGHT[vectorlen];
	}
	int i=0, i1=0, u1=0, i2=0, u2=0;
	for(;u1<E1UniLen && u2<E2UniLen; )
	{
		if((entry1->vectorp+E1Uoffset)[u1].position<(entry2->vectorp+E2Uoffset)[u2].position)//2中无对应项
		{
			vector1[i]=(entry1->vectorp)[i1].weight;
			vector2[i]=(REALTYPE)0.1;
			i++;
			if(i1<E1IntLen && (entry1->vectorp)[i1].position==(entry1->vectorp+E1Uoffset)[u1].position) 
			{
			    i1++;
			}
			u1++;
		}
		else if((entry1->vectorp+E1Uoffset)[u1].position>(entry2->vectorp+E2Uoffset)[u2].position)//1中无对应项
		{
			vector2[i]=(entry2->vectorp)[i2].weight;
			vector1[i]=(REALTYPE)0.1;
			i++;
			if(i2<E2IntLen && (entry2->vectorp)[i2].position==(entry2->vectorp+E2Uoffset)[u2].position) 
			{
			    i2++;
			}
			u2++;
		}
		else//=
		{
			WEIGHT itemi1,itemi2;
			if(i1<E1IntLen && (entry1->vectorp)[i1].position==(entry1->vectorp+E1Uoffset)[u1].position) 
			{
				itemi1=(entry1->vectorp)[i1].weight; 
				i1++;
			} 
			else {itemi1=(REALTYPE)0.1;}
			if(i2<E2IntLen && (entry2->vectorp)[i2].position==(entry2->vectorp+E2Uoffset)[u2].position) 
			{
				itemi2=(entry2->vectorp)[i2].weight; 
				i2++;
			}
			else {itemi2=(REALTYPE)0.1;}

			if(itemi1 >= (entry2->vectorp+E2Uoffset)[u2].weight) 
			{
			    vector1[i]=itemi1;
		    	vector2[i]=(entry2->vectorp+E2Uoffset)[u2].weight;
				u2++;
				u1++;
				i++;
			}
		    else if((entry1->vectorp+E1Uoffset)[u1].weight <= itemi2)
			{
			    vector1[i]=(entry1->vectorp+E1Uoffset)[u1].weight;
			    vector2[i]=itemi2;
				u1++;
				u2++;
				i++;
			}
		    else
			{
				u1++;
				u2++;
			}
		}
	}
	if(u2<E2UniLen)//说明u1越出，u1当中没有对应项
	{
		for(;u2<=E2UniLen;)
		{
			if(i2<E2IntLen && (entry2->vectorp)[i2].position==(entry2->vectorp+E2Uoffset)[u2].position) 
			{
				vector2[i]=(entry2->vectorp)[i2].weight;
			    vector1[i]=(REALTYPE)0.1;
			    i2++;
			    u2++;
			    i++;
			}
		    else // vector1[i]=0  (entry2->vectorp)[i2].position>(entry2->vectorp+E2Uoffset)[u2].position => vector2[i]=0;
			{
				u2++;
			}
		}
	}

	if(u1<E1UniLen)//说明u2越出，u2当中没有对应项
	{
		for(;u1<=E1UniLen;)
		{
			if(i1<E1IntLen && (entry1->vectorp)[i1].position==(entry1->vectorp+E1Uoffset)[u1].position) 
			{
				vector1[i]=(entry1->vectorp)[i1].weight;
			    vector2[i]=(REALTYPE)0.1;
			    i1++;
			    u1++;
			    i++;
			}
		    else // vector1[i]=0  (entry2->vectorp)[i2].position>(entry2->vectorp+E2Uoffset)[u2].position => vector2[i]=0;
			{
				u1++;
			}
		}
	}
    
	vectorlen=i;
        
	REALTYPE fenzi=0,fenmu1=0,fenmu2=0, textsim=0;
	for(i=0;i<vectorlen;i++)
	{
		if(vector1[i]>0 && vector2[i]>0)
		{
			fenzi+=vector1[i]*vector2[i];
		}
	}
	for(i=0;i<vectorlen;i++)
	{
		if(vector1[i]>0)
		{
			fenmu1+=vector1[i]*vector1[i];
		}
		if(vector2[i]>0)
		{
			fenmu2+=vector2[i]*vector2[i];
		}
	}
	if(fenmu1==0 && fenmu2==0) 
	{
		textsim=(REALTYPE)(1-alfa);
	}
	else 
	{
		textsim=(REALTYPE)((1-alfa) * (2*fenzi/(fenmu1+fenmu2) - MINTS)/(MAXTS - MINTS) );
		//textsim=(REALTYPE)(1-alfa);
	}
	if(vector1)
	{
		delete[] vector1;
		vector1=0;
	}
	if(vector2)
	{
		delete[] vector2;
		vector2=0;
	}
	return locationsim+textsim;
}


REALTYPE CIUR_tree::LeastSim(PRINODEp entry1, PRINODEp entry2)
{
	REALTYPE locationsim=alfa * (MAXLS-MaxDist(&(entry1->mbr), &(entry2->mbr)))/(MAXLS-MINLS);
	//REALTYPE locationsim=alfa * (100000-1*MaxDist(&(entry1->mbr), &(entry2->mbr)));
	if(alfa==1)
	{
		return locationsim;
	}
	if(entry1->intersectionlen==-1 || entry1->unionlen==-1) 
	{
		return locationsim;
	}
	if(entry2->intersectionlen==-1 || entry2->unionlen==-1)
	{
		return locationsim;
	}
	int E1IntLen=0, E1UniLen=0, E1Uoffset=0, E2Uoffset=0, E2IntLen=0, E2UniLen=0;
	if(entry1->nodeid<=0)//----------object
	{
		E1IntLen = entry1->unionlen;
		E1UniLen = E1IntLen;
		E1Uoffset = 0;
	}
	else
	{
		E1IntLen = entry1->intersectionlen;
		E1UniLen = entry1->unionlen;
		E1Uoffset = entry1->intersectionlen;
	}
	
	if(entry2->nodeid<=0)//----------object
	{
		E2IntLen = entry2->unionlen;
		E2UniLen = E2IntLen;
		E2Uoffset = 0;
	}
	else
	{
		E2IntLen = entry2->intersectionlen;
		E2UniLen = entry2->unionlen;
		E2Uoffset = entry2->intersectionlen;
	}

	int vectorlen=E1UniLen + E2UniLen; //the maximum length for both 
	WEIGHT* vector1=0, *vector2=0;
	if(vectorlen>0)
	{
		vector1=new WEIGHT[vectorlen];
		vector2=new WEIGHT[vectorlen];
	}
	int i=0,i1=0,u1=0,i2=0,u2=0;
	for(;u1<E1UniLen && u2<E2UniLen && i<vectorlen; )
	{
		if((entry1->vectorp+E1Uoffset)[u1].position < (entry2->vectorp+E2Uoffset)[u2].position)//2中无对应项
		{
			vector1[i]=(entry1->vectorp+E1Uoffset)[u1].weight;
		    vector2[i]=(REALTYPE)0.1;
			i++;
			if(i1<E1IntLen && (entry1->vectorp)[i1].position==(entry1->vectorp+E1Uoffset)[u1].position)
			{
				
			    i1++;
			    
			}
			u1++;
		}
		else if((entry1->vectorp+E1Uoffset)[u1].position>(entry2->vectorp+E2Uoffset)[u2].position)//1中无对应项
		{
			vector2[i]=(entry2->vectorp+E2Uoffset)[u2].weight;
		    vector1[i]=(REALTYPE)0.1;
			i++;
			if(i2<E2IntLen && (entry2->vectorp)[i2].position==(entry2->vectorp+E2Uoffset)[u2].position)
			{
			    i2++;
			}
			u2++;
		}
		else//=
		{
			WEIGHT itemi1,itemi2;
			if(i1<E1IntLen && (entry1->vectorp)[i1].position==(entry1->vectorp+E1Uoffset)[u1].position) 
			{
				itemi1=(entry1->vectorp)[i1].weight; 
				i1++;
			}//相似保持性需用到均值不等式，而均值不等式前提需满足两者都大于零
			else 
			{
				itemi1=(REALTYPE)0.1;
			}
			if(i2<E2IntLen && (entry2->vectorp)[i2].position==(entry2->vectorp+E2Uoffset)[u2].position) 
			{
				itemi2=(entry2->vectorp)[i2].weight; 
				i2++;
			}
			else {itemi2=(REALTYPE)0.1;}

			if(
				((itemi1>=itemi2)&&((entry1->vectorp+E1Uoffset)[u1].weight<=(entry2->vectorp+E2Uoffset)[u2].weight) || 
				(itemi1<=itemi2)&&((entry1->vectorp+E1Uoffset)[u1].weight>=(entry2->vectorp+E2Uoffset)[u2].weight))
				&&(u1<E1UniLen&&u2<E2UniLen)
			  )
			{
				if( ((entry1->vectorp+E1Uoffset)[u1].weight/itemi2) >= ((entry2->vectorp+E2Uoffset)[u2].weight/itemi1) )//接近程度按比例
				{
					vector1[i]=(entry1->vectorp+E1Uoffset)[u1].weight;
		    	    vector2[i]=itemi2;
					u1++;
					u2++;
					i++;
				}
				else
				{
					vector1[i]=itemi1;
					vector2[i]=(entry2->vectorp+E2Uoffset)[u2].weight;
					u2++;
					u1++;
					i++;
				}
			}
		    else if(
					(((entry2->vectorp+E2Uoffset)[u2].weight<=itemi1) 
					  || (itemi1>itemi2 && itemi1<(entry2->vectorp+E2Uoffset)[u2].weight))
					  &&(u1<E1UniLen&&u2<E2UniLen)
				)
			{
			    vector1[i]=(entry1->vectorp+E1Uoffset)[u1].weight;
			    vector2[i]=itemi2;
				u1++;
				u2++;
				i++;
			}
		    else
			{
				vector1[i]=itemi1;
				vector2[i]=(entry2->vectorp+E2Uoffset)[u2].weight;
				u2++;
				u1++;
				i++;
			}
		}
	}

	if(u2<E2UniLen)//说明u1越出，u1当中没有对应项
	{
		for(;u2<E2UniLen;)
		{
			if(i2<E2IntLen && (entry1->vectorp)[i2].position==(entry1->vectorp+E1Uoffset)[u2].position) 
			{
				vector2[i]=(entry2->vectorp)[i2].weight;
			    vector1[i]=(REALTYPE)0.1;
			    i2++;
			    u2++;
			    i++;
			}
		    else // vector1[i]=0  (entry2->vectorp)[i2].position>(entry2->vectorp+E2Uoffset)[u2].position => vector2[i]=0;
			{
				vector1[i]=(REALTYPE)0.1;
				vector2[i]=(entry2->vectorp+E2Uoffset)[u2].weight;
				u2++;
				i++;
			}
		}
	}

	if(u1<E1UniLen)//说明u2越出，u2当中没有对应项
	{
		for(;u1<E1UniLen;)
		{
			if(i1<E1IntLen && (entry1->vectorp)[i1].position==(entry1->vectorp+E1Uoffset)[u1].position) 
			{
				vector1[i]=(entry1->vectorp)[i1].weight;
			    vector2[i]=(REALTYPE)0.1;
			    i1++;
			    u1++;
			    i++;
			}
		    else // vector1[i]=0  (entry2->vectorp)[i2].position>(entry2->vectorp+E2Uoffset)[u2].position => vector2[i]=0;
			{
				vector1[i]=(entry1->vectorp+E1Uoffset)[u1].weight;
				vector2[i]=(REALTYPE)0.1;
				u1++;
				i++;
			}
		}
	}
	if(i<vectorlen)
	{
		vectorlen=i;
	}

	REALTYPE fenzi=0, fenmu1=0, fenmu2=0, textsim;
	for(i=0;i<vectorlen;i++)
	{
		if(vector1[i]>0 && vector2[i]>0)
		{
			fenzi+=vector1[i]*vector2[i];
		}
	}
	for(i=0;i<vectorlen;i++)
	{
		if(vector1[i]>0)
		{
			fenmu1+=vector1[i]*vector1[i];
		}
		if(vector2[i]>0)
		{
			fenmu2+=vector2[i]*vector2[i];
		}
	}
	if(fenmu1==0 && fenmu2==0) 
	{
		textsim=0;
		//textsim=(REALTYPE)(1-alfa);
	}
    else 
	{
		textsim=(REALTYPE)((1-alfa) * (2*fenzi/(fenmu1+fenmu2) - MINTS)/(MAXTS - MINTS));
		//textsim=(REALTYPE)(1-alfa);
	}
	if(vector1)
	{
		delete[] vector1;
		vector1=0;
	}
	if(vector2)
	{
		delete[] vector2;
		vector2=0;
	}
	return locationsim+textsim;
}
		
void CIUR_tree::AddObjects(PRIQUEUEp list, int nodeid)//将e.nodeid的所有子孙对象加入list当中
{
	int i;
	RTREENODE node;
	pVectorCell LeafVctIntUnip=0;

	fseek(fp,MAIN_BITMAPSIZE+(nodeid-1)*sizeof(RTREENODE),0);
	fread(&node,sizeof(RTREENODE),1,fp);

	diskaccess++;

	if(node.level==0)
	{
		for(i=0;i<node.count;i++)
		{
            PRINODEp listnodep=new PRINODE;/////////////自己另开辟了空间
		    listnodep->nodeid=0-node.branch[i].m_data;
            for(int ii=0;ii<SIDES_NUMB;ii++)
			{
				listnodep->mbr.bound[ii]=node.branch[i].mbr.bound[ii];
			}
		    /////////////////////!!!!!!!!!!!通过vectorid从文件中获取vector串!!!!!!!!!!!!!!!!!///////////////////
			if(node.branch[0].childid>0)
			{
				if(i==0)
				{
					LeafVctIntUnip = new VectorCell[node.branch[0].childid];
					ReadVector(node.branch[0].vectorid, LeafVctIntUnip, node.branch[0].childid);
					listnodep->intersectionlen = node.branch[0].unionlen - node.branch[0].intersectionlen + 1; //notice this! use nodeid<0 to identify.
					listnodep->unionlen = listnodep->intersectionlen;
					listnodep->vectorp = LeafVctIntUnip;
				}
				else//chi>0 && chi<node.count
				{
					listnodep->intersectionlen = 0;
					listnodep->unionlen = node.branch[i].unionlen - node.branch[i].intersectionlen + 1;
					listnodep->vectorp = LeafVctIntUnip + node.branch[i].intersectionlen;
				}
			}
			else
			{
				listnodep->vectorp = 0;
				listnodep->intersectionlen = 0;
				listnodep->unionlen = 0;
			}
            /////////////////////!!!!!!!!!!!通过vectorid从文件中获取vector串!!!!!!!!!!!!!!!!!///////////////////
	    	listnodep->next=list->front;
	    	list->front=listnodep;
		}//endfor
	}//endif
	else
	{
		for(i=0;i<node.count;i++)
		{
			AddObjects(list,node.branch[i].childid);
		}
	}
}

//Apply_prune_queue(rearfou, candidate, pqueue, 0, travelpp, answer, prunedo, prunedn, f1, felemans);
void CIUR_tree::Apply_prune_queue(int rearfou, PRINODEp e1,  PRINODEp e2,  PRIQUEUEp answer, PRIQUEUEp prunedo, PRIQUEUEp prunedn, int &ff1, int& ff2)
{
	int i,j;
	ff2=0;//标示e1是否被确定为answer,if yes, it needn't to determine whether it to be pruned.
	
	REALTYPE sim=MostSim(e1, e2);

	//-------------------------------------------update the upper bounds of e2--------------------------------------------//
	for(i=1;ff1==0 && i<=K;i++)//---------------clean conflict
	{
		while(e2->UBSimp[i].effectid==e1->nodeid || e2->UBSimp[i].effectid==e1->parentid)
		{
			for(j=i;j<K;j++) 
			{
				e2->UBSimp[j].boundsim=e2->UBSimp[j+1].boundsim;
				e2->UBSimp[j].effectid=e2->UBSimp[j+1].effectid;
			}
			e2->UBSimp[j].effectid=0;
			e2->UBSimp[j].boundsim=0;
		}
	}
    if(ff1==0 && e2->UBSimp[K].boundsim <= sim)//-------Update e2.UBSimp 降序，MostSim(e,Q)<all the e2.UBSimp[1...K]
	{
		for(i=1; e2->UBSimp[i].boundsim>=sim && i<=K; i++); 
	    if(K-i+1<e1->objnum)
		{
			for(j=i;j<=K;j++)
			{
				e2->UBSimp[j].boundsim=sim;
				e2->UBSimp[j].effectid=e1->nodeid;
			}
		}
		else
		{
			for(j=i+e1->objnum;j<=K && j-i-e1->objnum<e1->objnum;j++)
			{
				e2->UBSimp[j].boundsim=e2->UBSimp[j-e1->objnum].boundsim;
				e2->UBSimp[j].effectid=e2->UBSimp[j-e1->objnum].effectid;
			}
			for(j=i; j<i+e1->objnum; j++)
			{
				e2->UBSimp[j].boundsim=sim;
				e2->UBSimp[j].effectid=e1->nodeid;
			}
		}
	}
	//-------------------------------------------Is Hit ? for e2------------------------------------//
	if(ff1==0 && e2->UBSimp[K].boundsim<=e2->LBSimp[0].boundsim)
	{
		ff1=1;
		/*if(e2->nodeid>0)
		{
			AddObjects(answer, e2->nodeid);
		}
		else
		{
			e2->next = answer->front;
			answer->front = e2;
		}*/
		e2->next = answer->front;
		answer->front = e2;
	}
	

	//-------------------------------------------update upper bounds of e1-----------------------------------------//
	for(i=1;i<=K;i++)//------------clean conflict
	{
		while(e1->UBSimp[i].effectid==e2->nodeid || e1->UBSimp[i].effectid==e2->parentid)
		{
			for(j=i;j<K;j++) //前移
			{
				e1->UBSimp[j].boundsim=e1->UBSimp[j+1].boundsim;
				e1->UBSimp[j].effectid=e1->UBSimp[j+1].effectid;
			}
			e1->UBSimp[j].effectid=0;
			e1->UBSimp[j].boundsim=0;
		}
	}
	if(e1->UBSimp[K].boundsim <= sim)//---------------------Update e1.UBSimp
	{
	    for(i=1; e1->UBSimp[i].boundsim>=sim && i<=K; i++); ///////Update e2.LBSimp 亦降序.从i开始，LBSimp[i]<sim
	    if(K-i+1<e2->objnum)
		{
			for(j=i;j<=K;j++)
			{
				e1->UBSimp[j].boundsim=sim;
				e1->UBSimp[j].effectid=e2->nodeid;
			}
		}
		else
		{
			for(j=i+e2->objnum;j<=K && j-i-e2->objnum<e2->objnum;j++)
			{
				e1->UBSimp[j].boundsim=e1->UBSimp[j-e2->objnum].boundsim;
				e1->UBSimp[j].effectid=e1->UBSimp[j-e2->objnum].effectid;
			}

            for(j=i; j<i+e2->objnum; j++)
			{
				e1->UBSimp[j].boundsim=sim;
				e1->UBSimp[j].effectid=e2->nodeid;
			}
		}
	}
	
	//--------------------------------Is a hit ? for e1-----------------------------------//
    if(rearfou && e1->UBSimp[K].boundsim<e1->LBSimp[0].boundsim)/////////add e1 to answer 
	{
		ff2=1;
	}

	//-------------------------------determine e1 or e2 whether can be pruned--------------------------------//
    if(ff1==0 || ff2==0)
	{ 
		sim=LeastSim(e1, e2);
		//////////////对 e2 操作///////////////////////
		//-----------------------------update the lower bound of e2-----------------------------------//
		for(i=1;ff1==0 && i<=K;i++)//------------clean conflict
		{
			while(e2->LBSimp[i].effectid==e1->nodeid || e2->LBSimp[i].effectid==e1->parentid)
			{
				for(j=i;j<K;j++) 
				{
					e2->LBSimp[j].boundsim=e2->LBSimp[j+1].boundsim;
					e2->LBSimp[j].effectid=e2->LBSimp[j+1].effectid;
				}
				e2->LBSimp[j].effectid=0;
				e2->LBSimp[j].boundsim=0;
			}
		}
    	if(ff1==0 && e2->LBSimp[K].boundsim < sim)//----------Update e2.LBSimp 亦降序.从i开始，LBSimp[i]<sim
		{
		    for(i=1; e2->LBSimp[i].boundsim>=sim && i<=K; i++); 
	        if(K-i+1<e1->objnum)
			{
				for(j=i;j<=K;j++)
				{
					e2->LBSimp[j].boundsim=sim;
					e2->LBSimp[j].effectid=e1->nodeid;
				}
			}
			else
			{
				for(j=i+e1->objnum;j<=K && j-i-e1->objnum<e1->objnum;j++)
				{
				  	e2->LBSimp[j].boundsim=e2->LBSimp[j-e1->objnum].boundsim;
					e2->LBSimp[j].effectid=e2->LBSimp[j-e1->objnum].effectid;
				}
				for(j=i; j<i+e1->objnum; j++)
				{
					e2->LBSimp[j].boundsim=sim;
					e2->LBSimp[j].effectid=e1->nodeid;
				}
			}
		}

		//--------------------------Is a drop ? for e2----------------------------------//

        if(ff1==0 && e2->LBSimp[K].boundsim>e2->UBSimp[0].boundsim)///////////add e2 to pruned                                                                                                                                                                                                                                                                                                  
		{
			ff1=1;
			/*if(e2->nodeid>0)
			{
				AddObjects(answer, e2->nodeid);
			}
			else
			{
				e2->next = answer->front;
				answer->front = e2;
			}*/
			e2->next = answer->front;
			answer->front = e2;
		}
       
		//////////////对 e1 操作///////////////////////
		//-----------------------------update the lower bound of e1-----------------------------//
		for(i=1;i<=K;i++)//---------clean conflict
		{
			while(e1->LBSimp[i].effectid==e2->nodeid || e1->LBSimp[i].effectid==e2->parentid)
			{
				for(j=i;j<K;j++) 
				{
					e1->LBSimp[j].boundsim=e1->LBSimp[j+1].boundsim;
				    e1->LBSimp[j].effectid=e1->LBSimp[j+1].effectid;
				}
				e1->LBSimp[j].effectid=0;
				e1->LBSimp[j].boundsim=0;
			}
		}
		if(ff2==0 && e1->LBSimp[K].boundsim < sim)//Update e1.LBSimp 亦降序.从i开始，LBSimp[i]<sim
		{
			for(i=1; e1->LBSimp[i].boundsim>=sim && i<=K; i++); 
			if(K-i+1<e2->objnum)
			{
				for(j=i;j<=K;j++)
				{
					e1->LBSimp[j].boundsim=sim;
					e1->LBSimp[j].effectid=e2->nodeid;
				}
			}
			else
			{
				for(j=i+e2->objnum;j<=K && j-i-e2->objnum<e1->objnum;j++)
				{
					e1->LBSimp[j].boundsim=e1->LBSimp[j-e2->objnum].boundsim;
					e1->LBSimp[j].effectid=e1->LBSimp[j-e2->objnum].effectid;
				}
				for(j=i; j<i+e2->objnum; j++)
				{
					e1->LBSimp[j].boundsim=sim;
					e1->LBSimp[j].effectid=e2->nodeid;
				}
			}
		}

		//------------------------Is a drop ? for e1------------------------------//

        if(ff2==0 && e1->LBSimp[K].boundsim>e1->UBSimp[0].boundsim)///////////add to pruned                                                                                                                                                                                                                                                                                                  
		{
			ff2 = 2;
		}
	}//end if(ff1==0 || ff2==0)
}


//Apply_prune_list(rearfou, plist, travelpp, answer, prunedo, prunedn,f1)
void CIUR_tree::Apply_prune_list(int rearfou, PRINODE* listp, PRINODEp e2, PRIQUEUEp answer, PRIQUEUEp prunedo, PRIQUEUEp prunedn, int &f)
{
	if(f==1) return;
	int i,j;
	
	REALTYPE sim=MostSim(listp, e2);
	for(i=1; i<=K; i++)//消重，更新
	{
		while(e2->UBSimp[i].effectid==listp->parentid || e2->UBSimp[i].effectid==listp->nodeid)
		{
			for(j=i;j<K;j++) 
			{
				e2->UBSimp[j].boundsim=e2->UBSimp[j+1].boundsim;
				e2->UBSimp[j].effectid=e2->UBSimp[j+1].effectid;
			}
			e2->UBSimp[j].boundsim=0;
			e2->UBSimp[j].effectid=0;
		}
	}
    if(e2->UBSimp[K].boundsim < sim)////////////////////Update e2.UBSimp 降序，MostSim(e,Q)<all the e2.UBSimp[1...K]
	{
		for(i=1; e2->UBSimp[i].boundsim>=sim && i<=K; i++); ///////Update e2.LBSimp 亦降序 
	    for(j=K;j>i;j--)
		{
			e2->UBSimp[j].boundsim=e2->UBSimp[j-1].boundsim;
			e2->UBSimp[j].effectid=e2->UBSimp[j-1].effectid;
		}
	    e2->UBSimp[i].boundsim=sim;
		e2->UBSimp[i].effectid=listp->nodeid;
	}

	if(rearfou && e2->UBSimp[K].boundsim<e2->LBSimp[0].boundsim)/////////add to answer 插入到队列的尾部比插入到头部更有效
	{
		f=1; 
		return;
	}
	sim=LeastSim(listp, e2);
	
	for(i=1; i<=K; i++)//消重，更新
		{
			while(e2->LBSimp[i].effectid==listp->parentid || e2->LBSimp[i].effectid==listp->nodeid)
			{
				for(j=i;j<K;j++) 
				{
					e2->LBSimp[j].boundsim=e2->LBSimp[j+1].boundsim;
					e2->LBSimp[j].effectid=e2->LBSimp[j+1].effectid;
				}
				e2->LBSimp[j].boundsim=0;
				e2->LBSimp[j].effectid=0;
			}
		}
	if(e2->LBSimp[K].boundsim < sim)
    {
		for(i=1; e2->LBSimp[i].boundsim>=sim && i<=K; i++); ///////Update e2.LBSimp 亦降序 Least(e,Q)>其中一个就够了
		 ////找到的位置为DeQueueInfo->LBSimp[i]<sim, i
	    for(j=K;j>i;j--)
		{
			e2->LBSimp[j].boundsim=e2->LBSimp[j-1].boundsim;
			e2->LBSimp[j].effectid=e2->LBSimp[j-1].effectid;
		}
	    e2->LBSimp[i].boundsim=sim;
		e2->LBSimp[i].effectid=listp->nodeid;
	}

    if(e2->LBSimp[K].boundsim>e2->UBSimp[0].boundsim)///////////add to pruned                                                                                                                                                                                                                                                                                                  
	{
		if(e2->nodeid < 0)// e2(entry dequeued from U) is an object, add to prunedo
		{
			e2->next = prunedo->front;
			prunedo->front = e2;
		}
		else
		{
			e2->next = prunedn->front;
			prunedn->front = e2;
		}
		f=1;
	}
}


void CIUR_tree::QuickSort(pVectorCell numberstrp, int low, int high)
{
    int i, j, pivot;
	WEIGHT pivotweight;
       if (low < high)
       {
		   pivot=numberstrp[low].position;
           pivotweight=numberstrp[low].weight;
           i=low; 
           j=high;
              
           while(i<j)
           {
			   while (i<j && numberstrp[j].position>=pivot)
				   j--;
               if(i<j)
			   {
				   numberstrp[i].position=numberstrp[j].position;   //将比枢轴记录小的记录移到低端
				   numberstrp[i++].weight=numberstrp[j].weight;
			   }
                     
               while (i<j && numberstrp[i].position<=pivot)
				   i++;
			   if(i<j) 
			   {
				   numberstrp[j].position=numberstrp[i].position;       //将比枢轴记录大的记录移到高端
				   numberstrp[j--].weight=numberstrp[i].weight;
			   }
           }
              
           numberstrp[i].position=pivot;         //枢轴记录移到最终位置
		   numberstrp[i].weight=pivotweight;
              
           QuickSort(numberstrp,low,i-1);
           QuickSort(numberstrp,i+1,high);
       }
}

void CIUR_tree::Signature(pVectorCell shuzichuan, CString str, int & duan) //输入<string1,weight1>...<string2,weight2>,签名成一串有序数字
{
	int pos,p1=0,p2=0;
	duan=0;
	char ss1[MAXKEYNUMPEROBJ][MAXKEYWORDSIZE]={0};
	char ss2[MAXKEYNUMPEROBJ][15]={0};
	char *s=new char[str.GetLength()+1];
	strcpy(s,str);
	for(pos=0; pos<(int)strlen(s); pos++)
	{   
		if(s[pos]=='<') continue;
		if(s[pos]==',' || s[pos]=='>' || pos==(int)strlen(s))
		{
			duan++;
			p1=0;
			p2=0;
		}
		else 
		{
			if(duan%2==0)
		    	 ss1[duan/2][p1++]=s[pos];
			else ss2[duan/2][p2++]=s[pos];
		}
	}//endfor
    
	delete[] s;
	duan++;
	
    for(int i=0;i<duan/2;i++)
	{
	  //shuzichuan[i].position=atoi(ss1[i]+1); //WEB
		shuzichuan[i].position=atoi(ss1[i]);
	  shuzichuan[i].weight=(REALTYPE)atof(ss2[i]);
	}

	QuickSort(shuzichuan,0,duan/2-1);//按position的大小进行排序
}



void CIUR_tree::BuildTree() 
{
    fp=fopen("Index Files_queries_output\\iur_tree.index","rb+");
	fseek(fp,0,SEEK_END);
    if(ftell(fp)==0) fwrite(bitmap_index_main, MAIN_BITMAPSIZE, 1, fp);
	fclose(fp);

	fp=fopen("Index Files_queries_output\\textvector.index","rb+");
	fseek(fp,0,SEEK_END);
	if(ftell(fp)==0) fwrite(bitmap_index_minor, MINOR_BITMAPSIZE, 1, fp);
	fclose(fp);


   ORGDATA orgdata;
   FILE* fporgdata=fopen("Index Files_queries_output\\100K_GNbanary.txt","rb+"); //读取原始数据

   //fseek(fporgdata,0,SEEK_END);
   //long orgdata_len=ftell(fporgdata);

   //for(long orgdatai=0; orgdatai<orgdata_len; orgdatai+=(sizeof(int)+2*sizeof(float)+MaxTextStrLength*sizeof(char)))//WEB
   for(long orgdatai=0; !feof(fporgdata); orgdatai+=(sizeof(int)+2*sizeof(float)+MaxTextStrLength*sizeof(char)))
   {   //sizeof(ORGDATA) = (sizeof(int)+3*sizeof(char)+2*sizeof(float)+MaxTextStrLength*sizeof(char)
	   fseek(fporgdata, orgdatai, 0);
	   fread(&(orgdata.nodeid),sizeof(int),1,fporgdata);
	   //fread(&(orgdata.c),3*sizeof(char),1,fporgdata);//WEB
	   fread(&(orgdata.weidu),sizeof(float),1,fporgdata);
	   fread(&(orgdata.jingdu),sizeof(float),1,fporgdata);
	   fread(&(orgdata.textstr),MaxTextStrLength*sizeof(char),1,fporgdata);

	   
	   m_lat=orgdata.jingdu;
	   m_lon=orgdata.weidu;
	   m_textinfo = orgdata.textstr;

	    ///---------------------省去插入数据文件-------------------------///


        ////////////////////////////////////////////插入索引文件/////////////////////////////////////////////////////////

		//初始化节点
		int j;
		RTREEROOT root;//root为根结点
		root.root_node=new RTREENODE;
		InitNode(root.root_node);

		RTREEBRANCH branch;
		InitBranch(&branch);//////////////////////初始化branch
		//从1开始计//WEB
		//branch.m_data=1 + orgdatai/(sizeof(int)+3*sizeof(char)+2*sizeof(float)+MaxTextStrLength*sizeof(char)); ////branch.childid=0
		branch.m_data=1 + orgdatai/(sizeof(int)+2*sizeof(float)+MaxTextStrLength*sizeof(char)); ////branch.childid=0
		branch.objnum=1;

		for(j=0;j<=DIMS_NUMB;j=j+DIMS_NUMB)
		{
		  branch.mbr.bound[j]=m_lat;
		  branch.mbr.bound[j+1]=m_lon;
		}
        
		pVectorCell vctcellpp, vctcellp;
		vctcellpp=new VectorCell[MAXKEYNUMPEROBJ];
		int factlen;
		Signature(vctcellpp, m_textinfo, factlen); //将<string,weight>字符串转换成<position,weight>数字串

		if(factlen/2 <= 0) vctcellp=0;
		else vctcellp = new VectorCell[factlen/2];
		for(int t=0; t<factlen/2; t++)
		{
			vctcellp[t].position=vctcellpp[t].position;
			vctcellp[t].weight=vctcellpp[t].weight;
		}
		delete[] vctcellpp;
		//!!!notice here, the intersection and union vector of object are the same. but not stored twice.
		
        /////////////!!!!!!!!!!!以上完成了branch的初始化 but have not store the text vector!!!!!!!!!!!!!!//////////////////


	  ////Insert the branch into the IUR-tree
	  fp=fopen("Index Files_queries_output\\Web Sets\\iur_tree.index","rb+");
	  fpminor=fopen("Index Files_queries_output\\Web Sets\\textvector.index","rb+");
  
	  fseek(fp,0L,SEEK_END);
	  long leng=ftell(fp);

	  if(leng==MAIN_BITMAPSIZE) //只有头文件，正文为空
	  {   
		  root.root_node->nodeid=1;  //////////////////////////////////////////////////////////////////////////////赋值
		  root.root_node->parentid=-1;
		  root.root_node->level=0;
		  root.root_node->count=1;

		  if(factlen/2>0) 
		  {
			  branch.intersectionlen=0;
			  branch.unionlen=factlen/2;
			  branch.childid = factlen/2;
		  }

		  branch.vectorid=StoreVector(vctcellp, factlen/2);
		  CopyBranch(&(root.root_node->branch[0]),&branch);

		 ////////////////////////////////////////////////////////////////存储节点
		 fseek(fp,0,0);
		 if(!fread(bitmap_index_main,MAIN_BITMAPSIZE,1,fp))
		 {
			 AfxMessageBox("main bitmap read fail!");
		 }
		 bitmap_index_main[0]|=1<<(8*sizeof(int)-1);
		 fseek(fp,0,0);
		 if(!fwrite(bitmap_index_main,MAIN_BITMAPSIZE,1,fp))//更改主索引文件的头文件
		 {
			 AfxMessageBox("main bitmap write fail!");
		 }
		 fwrite(root.root_node,sizeof(RTREENODE),1,fp);
		 fflush(fp);
	  }//end if
	 else
	  {
		  
		  FindRoot(root.root_node);
		  
		  ////////////////////////////////////insert/////////////////////////////////////////////////

		  RTREENODE *newroot;
		  RTREENODE *newnode;
		  newnode=new RTREENODE;
		  InitNode(newnode);
		  
		  RTREEBRANCH b;//以备新增加树的高度而生成的两个子树作为两个branch插入新的根节点
		  InitBranch(&b);
      
		 //Recursively insert the branch (its text vector vctcellp with length of factlen/2) into root.root_node
		 //if root.root_node need split, then splited two nodes are the updated root.root_node and newnode, the info 
		 //    of which are stored in the disk already when returning back;
		 //if not, then newnode is a null.
		 if (_RTreeInsertRect(&root, &branch, vctcellp, factlen/2, root.root_node, &newnode, 0)) 
		 {   // root split 
			 newroot=new RTREENODE; //grow a new root, & tree taller 
			 InitNode(newroot);
			 newroot->parentid=-1; 
			 newroot->level = root.root_node->level + 1;
				
			 fseek(fp,0,0);
			 if(!fread(bitmap_index_main,MAIN_BITMAPSIZE,1,fp))
			 {
				 AfxMessageBox("main bitmap read fail!");
			 }
			 for(countdataid=0;countdataid<MAIN_BITMAPSIZE*8;countdataid++)//countdataid表示第几位
			 {
				 if( !( bitmap_index_main[countdataid/(8*sizeof(int))] & (1<<   ( 8*sizeof(int)-1-countdataid%(sizeof(int)*8))   ) ) )
				  break;
			 }

			 bitmap_index_main[countdataid/(8*sizeof(int))]|=1<<(8*sizeof(int)-1-countdataid%(sizeof(int)*8));
			 fseek(fp,0,0);
			 if(!fwrite(bitmap_index_main,MAIN_BITMAPSIZE,1,fp))//更改次索引文件的头文件
			 {
				 AfxMessageBox("main bitmap write fail!");
			 }

			 newroot->nodeid=countdataid+1;//定位新插入根节点的位置

			 b.mbr = RTreeNodeCover(root.root_node);
			 b.childid = root.root_node->nodeid;
			 b.vectorid=VectorCover(root.root_node, newroot->branch[0].intersectionlen, newroot->branch[0].unionlen);
			 b.objnum=ObjnumCover(root.root_node);
			 AddBranch(&root, &b, newroot, NULL);

			 b.mbr = RTreeNodeCover(newnode);
			 b.childid = newnode->nodeid;
   			 b.vectorid=VectorCover(newnode, newroot->branch[1].intersectionlen, newroot->branch[1].unionlen);    
			 b.objnum=ObjnumCover(newnode);
			 AddBranch(&root, &b, newroot, NULL);
             
			 root.root_node->parentid=newroot->nodeid;//父亲节点项值改变，更改该两节点(覆盖写)
			 newnode->parentid=newroot->nodeid;
			 fseek(fp,((root.root_node->nodeid)-1)*sizeof(RTREENODE)+MAIN_BITMAPSIZE,0);
			 fwrite(root.root_node,sizeof(RTREENODE),1,fp);
			 fflush(fp);
			 fseek(fp,((newnode->nodeid)-1)*sizeof(RTREENODE) + MAIN_BITMAPSIZE,0);
			 fwrite(newnode,sizeof(RTREENODE),1,fp);
			 fflush(fp);
             
			 Copy(root.root_node , newroot);
			 delete newroot;
		 }//END IF
		 delete newnode;
	 }//end else
 fclose(fp);
 fclose(fpminor);
 delete root.root_node;
 if(vctcellp) delete[] vctcellp;

 /*QueryPerformanceCounter(&liCount2);
 QueryPerformanceFrequency(&liFrequency);
 fSeconds=(double)(liCount2.QuadPart-liCount1.QuadPart)/(double)liFrequency.QuadPart;
 timecount+=fSeconds;*/

 }//end each inserted orginal data
 fclose(fporgdata);

    
	/*CString strtime;
	strtime.Format("%f",timecount);
    AfxMessageBox(strtime);*/

}


void CIUR_tree::OnOKQuery() 
{
	::diskaccess=0;
	::querytime=0;
	//UpdateData(TRUE);
	

  
	LARGE_INTEGER liCount1;
	LARGE_INTEGER liCount2;
	LARGE_INTEGER liFrequency;
	QueryPerformanceCounter(&liCount1);
    double fSeconds;

    /////////////////////!!!!!!!!!!!得到正式查询格式mbrq, vctcellQ!!!!!!!!!!!//////////////////////
	PRINODE * query = new PRINODE;
	query->LBSimp = 0;
	query->UBSimp = 0;
	query->vectorp=0;
	query->intersectionlen = 0;
	query->mbr.bound[0]=m_qlat;
	query->mbr.bound[1]=m_qlon;
	query->mbr.bound[2]=m_qlat;
	query->mbr.bound[3]=m_qlon;
	query->next=0;
	query->parentid=0;
	query->nodeid=-1;
	query->objnum=0;

    pVectorCell vctcellpp;
    vctcellpp=new VectorCell[MAXKEYNUMPEROBJ];
	Signature(vctcellpp, m_qkeywords, query->unionlen);
	//将<string1,weight1><string2,weight2>字符串转换成<position1,weight1><position2,weight2>数字串
	query->unionlen = (query->unionlen)/2 - 1;

    if(query->unionlen - query->intersectionlen + 1 > 0)
	{
		query->vectorp=new VectorCell[query->unionlen - query->intersectionlen + 1];
	}
	for(int t=0; t<query->unionlen-query->intersectionlen+1; t++)
	{
		query->vectorp[t].position=vctcellpp[t].position;
	    query->vectorp[t].weight=vctcellpp[t].weight;
	}
	if(vctcellpp)
	{
		delete[] vctcellpp;
		vctcellpp=0;
	}

	


	
	//*********************开始************************//

	

	/////////////////////!!!!!!!!!!!!!!!初始化各优先队列及列表!!!!!!!!!!!!!!//////////////////////
    PRIQUEUEp U, candidate, prunedo, prunedn, answer;//遍历优先队列：候选优先队列
	U=new PRIQUEUE;
    InitQueue(U);
	candidate=new PRIQUEUE;
	InitQueue(candidate);
    prunedo=new PRIQUEUE;
    InitQueue(prunedo);
	prunedn=new PRIQUEUE;
    InitQueue(prunedn);
	answer=new PRIQUEUE;
    InitQueue(answer);
    
    RTREENODE root;
	
	//char filetest[30]={0};
	//strcpy(filetest, mainindex);

	if(!(fp=fopen(mainindex,"rb+")))
	{
		AfxMessageBox("Fail to open index file!");
	}
	if(!(fpminor=fopen(minorindex, "rb+")))
	{
		AfxMessageBox("Fail to open index file!");
	}
	FindRoot(&root);
	RTREEMBR mbrroot;
	for(int mbri=0; mbri<root.count; mbri++)//为了求MAXLS
	{
		if(mbri==0)
		{
			for(int mbrj=0; mbrj<SIDES_NUMB; mbrj++)
			{
				mbrroot.bound[mbrj]=root.branch[mbri].mbr.bound[mbrj];
			}
		}
		else mbrroot=RTreeCombineRect(&mbrroot, &(root.branch[mbri].mbr));
	}
	mbrroot = RTreeCombineRect(&mbrroot, &(query->mbr));

	MAXLS=(REALTYPE)sqrt(pow(mbrroot.bound[2]-mbrroot.bound[0],2)+pow(mbrroot.bound[3]-mbrroot.bound[1],2));
    MINLS=0;
	MAXTS=1;
	MINTS=0;
	alfa=(REALTYPE)0.6;
	PRINODE *travelp=new PRINODE; //entry being processed so far.
	travelp->LBSimp = new BOUNDSIM[K+1];
	travelp->UBSimp = new BOUNDSIM[K+1];
	travelp->nodeid=root.nodeid;//!!!!!
	travelp->parentid=-1;
	travelp->objnum=0;
	EnQueue(U, travelp);//根结点入队列
    
	DEQI DeQueueInfo; //DeQueue的返回结果
	RTREENODE node;
	pVectorCell LeafVctIntUnip=0;
	





	
	/////////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!正式进入查询阶段!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//////////////////////
    diskaccess=0;
	
	while(U->front!=0)//当优先队列不为空
	{
		DeQueue(U, &DeQueueInfo);
		fseek(fp,MAIN_BITMAPSIZE+(DeQueueInfo.nodeid-1)*sizeof(RTREENODE),0);
		fread(&node,sizeof(RTREENODE),1,fp);

		diskaccess++;

		//注意实际上考虑每个孩子节点是否确定为answer, 需要考虑完node的所有孩子节点方可。其确定直观上应放在此函数里头，但是
		//需要记录所访问的孩子节点，造成大量中间空间代价。同理队列当中的元素亦如此，但为了节省中间空间代价，其确定放在
		//Apply_plune里头,当且仅当处理最后一个孩子节点时，travel者和被travel者同时考虑是否确定为answer.
		for(int chi=0;chi<node.count;chi++) //expand
		{
	    	travelp=new PRINODE;///////////候选元素分配空间之处
			for(int i=0;i<SIDES_NUMB;i++)
			{
				travelp->mbr.bound[i]=node.branch[chi].mbr.bound[i];
			}
			if(node.level>0) 
			{
				travelp->nodeid=node.branch[chi].childid;
			}
			else {travelp->nodeid=0-node.branch[chi].m_data;}

			travelp->parentid=DeQueueInfo.nodeid; //记住DeQueueInfo.nodeid需和travelp形影不离
			travelp->objnum=node.branch[chi].objnum;
			travelp->next=0;
			if(ifinherit)
			{
				//------------------------继承---------------------------------//
				travelp->LBSimp = new BOUNDSIM[K+1];//传数据值
				travelp->UBSimp = new BOUNDSIM[K+1];
				for(int lui=1; lui<K+1; lui++)
				{
					travelp->LBSimp[lui].boundsim = DeQueueInfo.LBSimp[lui].boundsim;
					travelp->UBSimp[lui].boundsim = DeQueueInfo.UBSimp[lui].boundsim;
					travelp->LBSimp[lui].effectid = DeQueueInfo.LBSimp[lui].effectid;
					travelp->UBSimp[lui].effectid = DeQueueInfo.UBSimp[lui].effectid;
				}
			}
			else
			{
				//------------------------不继承，则重新计算---------------------------------//
				travelp->LBSimp = new BOUNDSIM[K+1];//传数据值
				travelp->UBSimp = new BOUNDSIM[K+1];
				for(int lui=1; lui<K+1; lui++)
				{
					travelp->LBSimp[lui].boundsim = 0;
					travelp->UBSimp[lui].boundsim = 0;
					travelp->LBSimp[lui].effectid = 0;
					travelp->UBSimp[lui].effectid = 0;
				}
			}


            /////////////////////!!!!!!!!!!!通过vectorid从文件中获取vector串!!!!!!!!!!!!!!!!!///////////////////
			if(node.level>0)
			{
				if(node.branch[chi].intersectionlen+node.branch[chi].unionlen>0)
				{
					travelp->intersectionlen = node.branch[chi].intersectionlen;
					travelp->unionlen = node.branch[chi].unionlen;
					travelp->vectorp = new VectorCell[travelp->intersectionlen+travelp->unionlen];
					ReadVector(node.branch[chi].vectorid, travelp->vectorp, travelp->intersectionlen+travelp->unionlen);
				}
				else 
				{
					travelp->intersectionlen = 0;
					travelp->unionlen = 0;
					travelp->vectorp = 0;
				}
				travelp->LBSimp[0].boundsim = LeastSim(travelp, query);
				travelp->UBSimp[0].boundsim = MostSim(travelp, query);
			}
			else
			{
				if(node.branch[0].childid>0)
				{
					if(chi==0)
					{
						LeafVctIntUnip = new VectorCell[node.branch[0].childid];
						ReadVector(node.branch[0].vectorid, LeafVctIntUnip, node.branch[0].childid);
						travelp->intersectionlen = 0; //notice this! use nodeid<0 to identify. 为零且nodeid<0表示开头，要删则删此
						travelp->unionlen = node.branch[0].unionlen - node.branch[0].intersectionlen + 1;
						travelp->vectorp = LeafVctIntUnip;
					}
					else//chi>0 && chi<node.count
					{
						travelp->intersectionlen = node.branch[chi].unionlen - node.branch[chi].intersectionlen + 1;
						travelp->unionlen = travelp->intersectionlen;
						travelp->vectorp = LeafVctIntUnip + node.branch[chi].intersectionlen;
					}
				}
				else
				{
					travelp->vectorp = 0;
					travelp->intersectionlen = 0;
					travelp->unionlen = 0;
				}
				travelp->LBSimp[0].boundsim = LeastSim(travelp, query);
				travelp->UBSimp[0].boundsim = MostSim(travelp, query);
			}
            /////////////////////!!!!!!!!!!!通过vectorid从文件中获取vector串!!!!!!!!!!!!!!!!!///////////////////


			if(travelp->LBSimp[K].boundsim >= travelp->UBSimp[0].boundsim) //-------看看继承是否带来效果
			{
				if(travelp->nodeid > 0)
				{
					travelp->next = prunedn->front;
					prunedn->front = travelp;
				}
				else
				{
					travelp->next = prunedo->front;
					prunedo->front = travelp;
				}
				continue;
			}
			else if((travelp->UBSimp[K].boundsim < travelp->LBSimp[0].boundsim) && chi==node.count-1)//------看看继承是否带来效果
			{
				travelp->next = answer->front;
				answer->front = travelp;
				continue;
			}

			
			if(chi<node.count-1)
			{
				ScanProcess(travelp, 0, U, candidate, answer, prunedo, prunedn);//未到尾部
			}
			else 
			{
				ScanProcess(travelp, 1, U, candidate, answer, prunedo, prunedn);//到了尾部
			}
		}//endfor

		if(DeQueueInfo.LBSimp)
		{
			delete[] DeQueueInfo.LBSimp;
			DeQueueInfo.LBSimp = 0;
		}
		if(DeQueueInfo.UBSimp)
		{
			delete[] DeQueueInfo.UBSimp;
			DeQueueInfo.LBSimp = 0;
		}
	}//endwhile





    



////----------------------------------------------------Step 2-----------------------------------------------------////

	PRINODE* polist=0;
	PRINODE* pnlist=0;
	while(candidate->front!=0&&iflazytraveldown)
	{
		int i, j;
		REALTYPE sim=0;
		
		
        polist=0;
		pnlist=0;
		if(prunedo->front!=0) //get E from PEL (remove from PEL)
		{
			polist=prunedo->front;
			prunedo->front=prunedo->front->next;
			::diskaccess++;
		}
		else
		{
			pnlist=prunedn->front;
			prunedn->front=prunedn->front->next;
			::diskaccess++;
		}

		if(polist==0 && pnlist==0) break;

		for(PRINODE* cand=candidate->front; cand!=0; )
		{
			if(polist!=0) //用prunedo中的objects **先用prundo中的object进行作用***
			{
				sim=LeastSim(polist, cand);
				for(i=1; i<=K; i++)//消重
				{
					while(cand->LBSimp[i].effectid==polist->parentid || cand->LBSimp[i].effectid==polist->nodeid)
					{
						for(j=i;j<K;j++) 
						{
							cand->LBSimp[j].boundsim=cand->LBSimp[j+1].boundsim;
							cand->LBSimp[j].effectid=cand->LBSimp[j+1].effectid;
						}
						cand->LBSimp[j].boundsim=0;
						cand->LBSimp[j].effectid=0;
					}
				}
				if(cand->LBSimp[K].boundsim < sim)//更新
				{
					for(i=1; cand->LBSimp[i].boundsim>=sim && i<=K; i++); ///////Update e2.LBSimp 亦降序 Least(e,Q)>其中一个就够了
					 ////找到的位置为DeQueueInfo->LBSimp[i]<sim, i
					for(j=K;j>i;j--)
					{
						cand->LBSimp[j].boundsim=cand->LBSimp[j-1].boundsim;
						cand->LBSimp[j].effectid=cand->LBSimp[j-1].effectid;
					}
					cand->LBSimp[i].boundsim=sim;
					cand->LBSimp[i].effectid=polist->nodeid;
				}

			   if(cand->LBSimp[K].boundsim > cand->UBSimp[0].boundsim)//prune                                                                                                                                                                                                                                                                                              
			   {
				   PRINODE * tempcand;
				   for(tempcand=candidate->front; ; tempcand=tempcand->next)
				   {
					   if(tempcand==cand)
					   {
						   candidate->front = cand->next;
						   cand = cand->next;
						   break;
					   }
					   if(tempcand->next==cand)
					   {
						   tempcand->next = cand->next;
						   tempcand = cand;
						   cand = cand->next;
						   break;
					   }
				   }
				   delete tempcand;
				   tempcand = 0;
				   continue;
			   }
			   sim=MostSim(polist, cand);
			   if(cand->UBSimp[K].boundsim < sim)////////////////////Update e2.UBSimp 降序，MostSim(e,Q)<all the e2.UBSimp[1...K]
				{
					for(i=1; cand->UBSimp[i].boundsim>=sim && i<=K; i++); ///////Update e2.LBSimp 亦降序.从i开始，LBSimp[i]<sim
					if(K-i+1<1)
					{
						for(j=i;j<=K;j++)
						{
							cand->UBSimp[j].boundsim=sim;
							cand->UBSimp[j].effectid=polist->nodeid;
						}
					}
					else
					{
						for(j=i+1;j<=K && j-i-1<1; j++)
						{
							cand->UBSimp[j].boundsim=cand->UBSimp[j-1].boundsim;
							cand->UBSimp[j].effectid=cand->UBSimp[j-1].effectid;
						}
						for(j=i; j<i+1; j++)
						{
							cand->UBSimp[j].boundsim=sim;
							cand->UBSimp[j].effectid=polist->nodeid;
						}
					}
				}
				if(cand->UBSimp[K].boundsim<cand->LBSimp[0].boundsim)/////////add to answer 插入到队列的尾部比插入到头部更有效
				{
					PRINODE * tempcand;
				    for(tempcand=candidate->front; ; tempcand=tempcand->next)
					{
					   if(tempcand==cand)
					   {
						   candidate->front = cand->next;
						   cand = cand->next;
						   break;
					   }
					   if(tempcand->next==cand)
					   {
						   tempcand->next = cand->next;
						   tempcand = cand;
						   cand = cand->next;
						   break;
					   }
					}
				    tempcand->next = answer->front;
					answer->front = tempcand;
				    continue;
				}

			}//end if(polist)

		    else if(pnlist!=0) //(pnlist)
			{
				/*sim = LeastSim(pnlist, cand);
				for(i=1; i<=K; i++)//消重
				{
					while(cand->LBSimp[i].effectid==pnlist->parentid || cand->LBSimp[i].effectid==pnlist->nodeid)
					{
						for(j=i;j<K;j++) 
						{
							cand->LBSimp[j].boundsim=cand->LBSimp[j+1].boundsim;
							cand->LBSimp[j].effectid=cand->LBSimp[j+1].effectid;
						}
						cand->LBSimp[j].boundsim=0;
						cand->LBSimp[j].effectid=0;
					}
				}

				//lower bound contribution list
				if(cand->LBSimp[K].boundsim < sim)
				{
					for(i=1; cand->LBSimp[i].boundsim>=sim && i<=K; i++); ///////Update e2.LBSimp 亦降序.从i开始，LBSimp[i]<sim
					if(K-i+1<pnlist->objnum)
					{
						for(j=i;j<=K;j++)
						{
							cand->LBSimp[j].boundsim=sim;
							cand->LBSimp[j].effectid=pnlist->nodeid;
						}
					}
					else
					{
						for(j=i+pnlist->objnum;j<=K && j-i-pnlist->objnum<pnlist->objnum;j++)
						{
							cand->LBSimp[j].boundsim=cand->LBSimp[j-pnlist->objnum].boundsim;
							cand->LBSimp[j].effectid=cand->LBSimp[j-pnlist->objnum].effectid;
						}
						for(j=i; j<i+pnlist->objnum; j++)
						{
							cand->LBSimp[j].boundsim=sim;
							cand->LBSimp[j].effectid=pnlist->nodeid;
						}
					}
				}//end if

		    	if(cand->LBSimp[K].boundsim > cand->UBSimp[0].boundsim)///////////prune but not add to pruned                                                                                                                                                                                                                                                                                                  
				{
					PRINODE * tempcand;
				    for(tempcand=candidate->front; ; tempcand=tempcand->next)
					{
					   if(tempcand==cand)
					   {
						   candidate->front = cand->next;
						   cand = cand->next;
						   break;
					   }
					   if(tempcand->next==cand)
					   {
						   tempcand->next = cand->next;
						   tempcand = cand;
						   cand = cand->next;
				    	   break;
					   }
					}
				    delete tempcand;
					tempcand = 0;
				    continue; //consider the next candidate
				}


				//upper bound contribution list
				sim=MostSim(pnlist, cand);
				if(cand->UBSimp[K].boundsim < sim)////////////////////Update e2.UBSimp 降序，MostSim(e,Q)<all the e2.UBSimp[1...K]
				{
					for(i=1; cand->UBSimp[i].boundsim>=sim && i<=K; i++); ///////Update e2.LBSimp 亦降序.从i开始，LBSimp[i]<sim
					if(K-i+1<pnlist->objnum)
					{
						for(j=i;j<=K;j++)
						{
							cand->UBSimp[j].boundsim=sim;
							cand->UBSimp[j].effectid=pnlist->nodeid;
						}
					}
					else
					{
						for(j=i+pnlist->objnum;j<=K && j-i-pnlist->objnum<pnlist->objnum; j++)
						{
							cand->UBSimp[j].boundsim=cand->UBSimp[j-pnlist->objnum].boundsim;
							cand->UBSimp[j].effectid=cand->UBSimp[j-pnlist->objnum].effectid;
						}
						for(j=i; j<i+pnlist->objnum; j++)
						{
							cand->UBSimp[j].boundsim=sim;
							cand->UBSimp[j].effectid=pnlist->nodeid;
						}
					}
				}
				if(cand->UBSimp[K].boundsim<cand->LBSimp[0].boundsim)/////////add to answer 插入到队列的尾部比插入到头部更有效
				{
					PRINODE * tempcand;
				    for(tempcand=candidate->front; ; tempcand=tempcand->next)
					{
					   if(tempcand==cand)
					   {
						   candidate->front = cand->next;
						   cand = cand->next;
						   break;
					   }
					   if(tempcand->next==cand)
					   {
						   tempcand->next = cand->next;
						   tempcand = cand;
						   cand = cand->next;
						   break;
					   }
					}
					tempcand->next = answer->front;
					answer->front = tempcand;
					continue;
				}*/
			}//end else (pnlist)

			cand=cand->next;
		}//end for each cand

//------------------下面不得不对prunedn中的index nodes进行展开*以更新作用candidate objects的contribution list----------------//
		if(pnlist==0)
		{
			/*if(polist->vectorp   &&    (polist->nodeid>0  ||  (polist->nodeid<0 && polist->intersectionlen==0)) )
			{
			   delete[] polist->vectorp;
			   polist->vectorp = 0;
			}*/
			if(polist) 
			{
				delete polist; //删去PEL中的object
				polist = 0;
			}
		}
        else
		{
            fseek(fp,MAIN_BITMAPSIZE+(pnlist->nodeid-1)*sizeof(RTREENODE),0);
		    RTREENODE node;
		    fread(&node,sizeof(RTREENODE),1,fp);
			diskaccess++;
			

			for(int chi=0; chi<node.count; chi++)
			{
				PRINODEp pn = new PRINODE;

				if(node.level>0) pn->nodeid = node.branch[chi].childid;
				else pn->nodeid = 0 - node.branch[chi].m_data;

				for(int i=0;i<SIDES_NUMB;i++)
				{
					pn->mbr.bound[i]=node.branch[chi].mbr.bound[i];
				}

				pn->next = 0;

				pn->objnum=node.branch[chi].objnum;

				pn->parentid = node.nodeid;

				pn->LBSimp = 0;

				pn->UBSimp = 0;

				/////////////////////!!!!!!!!!!!通过vectorid从文件中获取vector串!!!!!!!!!!!!!!!!!///////////////////
				if(node.level>0)
				{
					if(node.branch[chi].intersectionlen+node.branch[chi].unionlen>0)
					{
						pn->intersectionlen = node.branch[chi].intersectionlen;
						pn->unionlen = node.branch[chi].unionlen;
						pn->vectorp = new VectorCell[pn->intersectionlen+pn->unionlen];
						ReadVector(node.branch[chi].vectorid, pn->vectorp, pn->intersectionlen+pn->unionlen);
					}
					else 
					{
						travelp->intersectionlen = 0;
						travelp->unionlen = 0;
						travelp->vectorp = 0;
					}
				}
				else
				{
					if(node.branch[0].childid>0)
					{
						if(chi==0)
						{
							LeafVctIntUnip = new VectorCell[node.branch[0].childid];
							ReadVector(node.branch[0].vectorid, LeafVctIntUnip, node.branch[0].childid);
							pn->intersectionlen = 0; //notice this! use nodeid<0 to identify.
							pn->unionlen = node.branch[0].unionlen - node.branch[0].intersectionlen + 1;
							pn->vectorp = LeafVctIntUnip;
						}
						else//chi>0 && chi<node.count
						{
							pn->intersectionlen = node.branch[chi].unionlen - node.branch[chi].intersectionlen + 1;
							pn->unionlen = pn->intersectionlen;
							pn->vectorp = LeafVctIntUnip + node.branch[chi].intersectionlen;
						}
					}
					else
					{
						pn->vectorp = 0;
						pn->intersectionlen = 0;
						pn->unionlen = 0;
					}
				}
				/////////////////////!!!!!!!!!!!通过vectorid从文件中获取vector串!!!!!!!!!!!!!!!!!///////////////////
				if(node.level>0)
				{
					pn->next=prunedn->front;
					prunedn->front=pn;
				}
				else
				{
					pn->next=prunedo->front;
					prunedo->front=pn;
				}
			}//end for each chi
	 }//end else ----->(prunedn!=0)

  }//end while candidate is not empty

  fclose(fp);
  fclose(fpminor);

  //FILE * fpoutput = fopen("Index Files_queries_output\\output.txt", "a+");
  //fprintf(fpoutput,"IUR-tree Query results:\n");
  //int ansnum = 0;
	PRINODE* temp=0;
	for(temp=answer->front; temp!=0; )
	{
		//ansnum++;
		//fprintf(fpoutput, "o%d, ", temp->nodeid);
		//if(ansnum%50==0) fprintf(fpoutput, "\n");

		answer->front = answer->front->next;
		if(temp->vectorp   &&    (temp->nodeid>0  ||  (temp->nodeid<0 && temp->intersectionlen==0)) )
		{
		   delete[] temp->vectorp;
		   temp->vectorp = 0;
		}
		if(temp) 
		{
			delete temp; 
			temp = 0;
		}
		temp = answer->front;
	}
	//fprintf(fpoutput, "\n");
	//fclose(fpoutput);
	
	for(temp=prunedo->front; temp!=0; )
	{
		prunedo->front = prunedo->front->next;
		if(temp->vectorp   &&    (temp->nodeid>0  ||  (temp->nodeid<0 && temp->intersectionlen==0)) )
		{
		   delete[] temp->vectorp;
		   temp->vectorp = 0;
		}
		if(temp) 
		{
			delete temp; 
			temp = 0;
		}
		temp = prunedo->front;
	}

	for(temp=prunedn->front; temp!=0; )
	{
		prunedn->front = prunedn->front->next;
		if(temp->vectorp   &&    (temp->nodeid>0  ||  (temp->nodeid<0 && temp->intersectionlen==0)) )
		{
		   delete[] temp->vectorp;
		   temp->vectorp = 0;
		}
		if(temp) 
		{
			delete temp; 
			temp = 0;
		}
		temp = prunedn->front;
	}

	QueryPerformanceCounter(&liCount2);
	QueryPerformanceFrequency(&liFrequency);
	fSeconds=(double)(liCount2.QuadPart-liCount1.QuadPart)/(double)liFrequency.QuadPart;
	::querytime = fSeconds;
}


BOOL CIUR_tree::OnInitDialog() 
{
	CDialog::OnInitDialog();
		// TODO: Add extra initialization here
    CRect rect;
	rect.left = 10;
	rect.top =100;
    rect.bottom = 160;
	rect.right = 120;
	m_StaticCtrl.Create("", WS_VISIBLE, rect, this, 1);	
	// TODO: Add your specialized code here and/or call the base class
	//CRect rect;
	GetClientRect(rect);
	rect.top+=120;
	m_ListCtrl.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT,rect,this,1);
	//设置颜色
	m_ListCtrl.SetBkColor(RGB(54,176,120));
	m_ListCtrl.SetTextColor(RGB(0,0,0));
	m_ListCtrl.SetTextBkColor(RGB(177,151,240));
	//Set EX_Style
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);
	//创建标题
	m_ListCtrl.InsertColumn(0,_T("号码"),LVCFMT_LEFT,100);
	m_ListCtrl.InsertColumn(1,_T("名称"),LVCFMT_LEFT,100);
	m_ListCtrl.InsertColumn(2,_T("纬度"),LVCFMT_LEFT,100);
	m_ListCtrl.InsertColumn(3,_T("经度"),LVCFMT_LEFT,100);
    m_ListCtrl.InsertColumn(4,_T("属性信息"),LVCFMT_LEFT,300);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
