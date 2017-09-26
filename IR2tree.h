#ifndef  IR2TREE_H_INCLUDED
#define  IR2TREE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#ifndef  TRUE
#define  TRUE  1
#endif

#ifndef  FALSE
#define  FALSE  0
#endif


#define  METHODS  1
#define  PAGE_SIZE    4096 //4KB Bytes
#define  DIMS_NUMB    2       /* number of dimensions 设为两维*/
#define  SIDES_NUMB   2*DIMS_NUMB//min max
#define  INFINITY     2//定义无穷大，而Similarity(,)已经规范化为[0,1]

#define  SIZEDATASET  (2*sizeof(int)+50*sizeof(char)+2*sizeof(REALTYPE)+DocItemNum*sizeof(char))
#define  DocItemNum   450 //一个

//#define K  10 //Reverse k Nearest Neighbor Query

#define  HEADLEN  (int)(PAGE_SIZE/sizeof(int)) //设定管理块的大小




#define ONETIMENUM 100     //一次读取keyword文件中keyword的个数
#define MAXKEYWORDSIZE 100 //最大keyword大小
#define NumNewAddItem  5  //一个插入的object所新增加的keywords的最大个数
#define OPMPHFTIMES    3  //OPMPHF的那个倍数m=xn
#define MAXKEYNUMPEROBJ  500 //每个object的最大keyword数目 
#define MAIN_BITMAPSIZE (PAGE_SIZE*10) //The bitmap size (byte#, bit#=MAIN_BITMAPSIZE*8) of main IUR-tree index (storing IUR-tree nodes), a bit->a node(a page size)

//The bitmap size (byte#, bit#=MAIN_BITMAPSIZE*8) of minor IUR-tree index (storing text vectors of IUR-tree), a bit->a page size (vectors of an index node 
//or all the vectors of objects in a leaf node)
#define MINOR_BITMAPSIZE (PAGE_SIZE*50)


/* max branching factor of a node */
//每个R-Tree的节点占一整个磁盘块页面
//#define MAXCARD (int)((PAGE_SIZE-(4*sizeof(int))) / sizeof(RTREEBRANCH)) //一个节点的最大分支数
//#ifndef MAXCARD
  #define MAXCARD 102 //102 //先假设为2
#define MaxCluster 200 //数据集最大聚类数 
//#endif

//------The maximum number of words in a page, note that the "int" at the end of the page marks 
//------the pointer of the next page for the rest words of the vector if there is, otherwise it is 0
#define MAXWORDS (int)((PAGE_SIZE-sizeof(int))/(sizeof(VectorCell))) 

#ifndef INVALD_RECT
  #define INVALID_RECT(x) ((x)->bound[0] > (x)->bound[DIMS_NUMB])//min>max,则无效矩形
#endif
#ifndef UnitSphereVolume
  #define UnitSphereVolume UnitSphereVolumes[DIMS_NUMB]
#endif



typedef float REALTYPE;
typedef float WEIGHT;


///////////////////////////原始数据结构///////////////////////////////

#define MaxTextStrLength 1024 //文本描述最大长度<position, weight>结构(看做字符串) the format is the same as that of getdata.exe






#ifndef MIN
 #define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
 #define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef NODECARD
  #define NODECARD MAXCARD
#endif
#ifndef LEAFCARD
  #define LEAFCARD MAXCARD
#endif

/* balance criteria for node splitting */
/* NOTE: can be changed if needed. */
#ifndef MINNODEFILL
  #define MINNODEFILL (NODECARD / 2)
#endif

#ifndef MINLEAFFILL
  #define MINLEAFFILL (LEAFCARD / 2)
#endif

#ifndef MAXKIDS
  #define MAXKIDS(n) ((n)->level > 0 ? NODECARD : LEAFCARD)
#endif

#ifndef MINFILL
  #define MINFILL(n) ((n)->level > 0 ? MINNODEFILL : MINLEAFFILL)
#endif


/**
 * Precomputed volumes of the unit spheres for the first few dimensions 
 */
const double UnitSphereVolumes[] = {
 0.000000,  /* dimension   0 */
 2.000000,  /* dimension   1 */
 3.141593,  /* dimension   2 */
 4.188790,  /* dimension   3 */
 4.934802,  /* dimension   4 */
 5.263789,  /* dimension   5 */
 5.167713,  /* dimension   6 */
 4.724766,  /* dimension   7 */
 4.058712,  /* dimension   8 */
 3.298509,  /* dimension   9 */
 2.550164,  /* dimension  10 */
 1.884104,  /* dimension  11 */
 1.335263,  /* dimension  12 */
 0.910629,  /* dimension  13 */
 0.599265,  /* dimension  14 */
 0.381443,  /* dimension  15 */
 0.235331,  /* dimension  16 */
 0.140981,  /* dimension  17 */
 0.082146,  /* dimension  18 */
 0.046622,  /* dimension  19 */
 0.025807,  /* dimension  20 */
};


#ifdef __cplusplus
}
#endif

#endif /* IR2TREE_H_INCLUDED */