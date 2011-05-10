//
// Element.h
//
//			2008.12.12
//			2008.11.05
//			k.Takeda
#ifndef ELEMENT_HH_05B90007_6581_437f_AD9D_7C9227A8D7B6
#define ELEMENT_HH_05B90007_6581_437f_AD9D_7C9227A8D7B6


#include "CommonStd.h"
#include "TypeDef.h"
#include <map>
#include <utility> //pair

#include "Node.h"
#include "ElementType.h"

#include "EdgeTree.h"    // node0,node1   -> edge
#include "FaceTree.h"    // <node_indexs> -> face
#include "EdgeFaceTree.h"// edge0,edge1   -> face
#include "NodeConnectNodeTree.h"// node   -> node

#include "Logger.h"


typedef std::pair<pmw::CNode*,pmw::CNode*> PairNode;

namespace pmw{
class CElement 
{
public:
    CElement(void);
    virtual ~CElement(void);

protected:
    uint mnID;// Meshの要素Index番号
    vector<CNode*> mvNode;

    map<uint, uint, less<uint> > mmIDLocal;//key:NodeID => val:局所頂点番号
    
    uint mMGLevel;//MultiGrid Level

    
    // prolongation準備 (ノード周囲の要素集合は、ノードがIndex番号で所有)
    // ---------------
    // 辺の要素集合、辺の中間ノード
    //
    vector<CNode*> mvEdgeInterNode;// prolongation用の,Edge中間ノード（要素のノード数には含めない)
    vector<vector<CElement*> > mvvEdgeElement;//辺(Edge)に隣接する要素
    bool*   mvb_edge;// Edgeに要素が既にセットされているか?
    // 面に隣接する要素
    // ---
    vector<CElement*> mvFaceElement;// 面に隣接する要素(面-隣接は1個の要素に限定)
    vector<CNode*> mvFaceNode;// 面中心のノード
    bool*   mvb_face;  // Faceにノードが既にセットされているか？
    // 要素中心のノード
    CNode* mpCenterNode;



    // Property Check
    virtual bool IndexCheck(const uint& propType, const uint& index, string& method_name)=0;

    // ノードから要素内の局所番号に変換
    vuint getLocalNodeNum(CNode* pNode0, CNode* pNode1);

    // 局所ノード番号から、面の局所番号に変換
    virtual uint& getLocalFaceNum(const vuint& vLocalNodeNum)=0;
    
    
    // 子要素:CommElementでのprolongation対応
    // --
    vector<CElement*> mvProgElement;//再分割した要素:頂点番号(局所ノード番号)順に配置

    bool mbComm; //CommElementか？(通信に使用されるComm)
    bool mbDComm;//DCommElementか？(prolongationにより,計算領域に属さなくなった要素)
    bool mbRComm;//CommMeshに含まれるが通信はしない(全てのランクがmyRank)
    int mCommID; //CommElementのID(DCommElementも含む)



    // MPCに使用する要素かどうか判定する属性
    // --
    bool mbMaster;
    bool mbSlave;
    bool* mvbMPCFace;//面番号のどれがMPCするのか


    // CommMesh2の属性 : 節点共有-通信テーブル
    // --
    bool mbCommMesh2;   //通信界面に使用される要素か
    bool* mvbCommEntity;//{Entity == 面番号(Solid),辺番号(Shell),点(Beam)}のどれがCommFaceに対応するのか
    uint mnCommEntity;  //Entity番号{Solidの場合=面番号, Shellの場合=辺番号, Beamの場合=頂点番号}


public:
    virtual void initialize()=0;

    // Element ID
    void setID(const uint& id){ mnID = id;}
    uint& getID(){ return mnID;}
    

    // CommElementのprolongation対応:(pyramid以外は,頂点番号順でProgElemを配置),(pyramidは,頂点順にHexa,面番号順にPyramid)
    // --
    void setProgElem(CElement* pProgElem, const uint& ivert){ mvProgElement[ivert]= pProgElem;}
    CElement* getProgElem(const uint& ivert){ return mvProgElement[ivert];}//ProgElementは,頂点番号順に配列に入っている.

    // ContactMeshのprolongation対応
    // "CommMesh2"でもCommFace::refineで利用
    // --
    CElement* getProgElem_NodeID(const uint& nodeID);//CSkinFace::refine()で利用
    uint& getLocalVertNum(const uint& nodeID){ return mmIDLocal[nodeID]; }//NodeID -> 頂点番号(局所番号)


    // CommElementとの接続:CommElementに入っているか？,入っている場合のCommElementのIDは？
    // --
    void interCommElem(){ mbComm= true;}    //CommElementに所有されていることをスタンプ
    bool& isInterCommElem(){ return mbComm;}//CommElementに所有されているか？
    void setCommID(const int& comID){ mCommID= comID;}//IDは全てのCommElementで番号を連番
    int& getCommID(){ return mCommID;}

    // DCommElementに入っているか？(計算に供しない要素)
    // --
    void interDCommElem(){ mbDComm= true;}
    bool& isInterDCommElem(){ return mbDComm;}

    // RCommElement
    void interRCommElem(){ mbRComm= true;}
    bool& isInterRCommElem(){ return mbRComm;}


    // MultiGrid Level <== Meshで管理するように改変予定,Elementに持たせる値ではない.
    void setMGLevel(const uint& mgLevel){ mMGLevel= mgLevel;}
    uint& getMGLevel(){ return mMGLevel;}


    // Node*
    void  setNode(CNode* pNode,const uint& local_id);
    vector<CNode*>& getNode() { return mvNode;}
    CNode* getNode(const uint& local_id){ return mvNode[local_id];}

    // mmIDLocal mapの付け直し
    //
    void ReInit_IDLocal();



    // Property
    virtual const uint& getType()=0;
    virtual const uint& getOrder()=0;//1次-2次要素判定
    virtual const uint& getNumOfFace()=0;
    virtual const uint& getNumOfEdge()=0;
    virtual const uint& getNumOfNode()=0;
    virtual const uint& getNumOfVert()=0;
    virtual const uint& getEntityType()=0;


    // prolongation準備
    // ---------------
    //
    // EdgeElement
    //
    virtual PairNode getPairNode(const uint& iedge)=0;
    virtual void     getPairNode(vint& pairNodeIndex, const uint& iedge)=0;
    void reserveEdgeElement(const uint& edgeIndex, const uint& numOfElem);
    void setEdgeElement(const uint& edgeIndex, CElement* pElem);
    void setEdgeAggElement(const uint& edgeIndex, vector<CElement*> vElement);
    vector<CElement*>& getEdgeElement(const uint& edgeIndex){ return mvvEdgeElement[edgeIndex];}
    //
    // EdgeElement boolスタンプ
    //
    virtual bool isEdgeElem(CNode* pNode0, CNode* pNode1)=0;
    virtual void setBoolEdgeElem(CNode* pNode0, CNode* pNode1)=0;
    //
    // (局所ノード番号,局所ノード番号)に対応した辺の,Index番号
    //
    virtual uint& getEdgeIndex(CNode* pNode0, CNode* pNode1)=0;
    virtual uint& getEdgeIndex(const uint& nodeID_0, const uint& nodeID_1)=0;
    //
    // EdgeNode
    // set Intermediate Node for prolongation
    // (prolongation 辺ノード)
    //
    void setEdgeInterNode(CNode* pNode, const uint& edgeIndex);
    CNode* getEdgeInterNode(const uint& edgeIndex){ return mvEdgeInterNode[edgeIndex];}
    uint getEdgeInterNodeSize(){ mvEdgeInterNode.size();}//deleteProgData処理後の確認用
    //
    // 2次要素において、辺NodeをmvNodeに移し替える && 1次要素では何もしない.
    //
    virtual void replaseEdgeNode(){ ;}

    //
    // FaceElement
    //
    void setFaceElement(CElement* pElem, const uint& faceIndex){ mvFaceElement[faceIndex]= pElem;}

    // 面構成ノード配列
    //
    virtual vector<CNode*> getFaceCnvNodes(const uint& iface)=0;


    //
    // FaceNode
    //
    void setFaceNode(CNode* pNode, const uint& faceIndex){ mvFaceNode[faceIndex]= pNode;}
    CNode* getFaceNode(const uint& faceIndex){ return mvFaceNode[faceIndex];}
    virtual bool isFaceElem(CNode* pNode0, CNode* pNode1, CNode* pNode2);     //Faceにノードがセットされているか？
    virtual void setBoolFaceElem(CNode* pNode0, CNode* pNode1, CNode* pNode2);//Faceノードがセットされたことをスタンプ
    uint getFaceNodeSize(){ mvFaceNode.size();}//deleteProgDataの確認用

    // Face index
    // 
    virtual uint& getFaceIndex(const uint& edge0, const uint& edge1)=0;       // iFace  (2 Edges)
    virtual uint& getFaceIndex(CNode* pNode0, CNode* pNode1, CNode* pNode2)=0;// iFace  (3 Nodes)


    // CenterNode(Volume Center)
    //
    void setVolumeNode(CNode* pNode){ mpCenterNode= pNode;}
    CNode* getVolumeNode(){return mpCenterNode;}


    // Node周囲の接続先Nodeを配列で返す.
    // (係数行列 作成用途, CMesh::setupAggregate)
    //
    virtual vector<CNode*> getConnectNode(CNode* pNode)=0;



    // MPCに使用する要素か判定する属性
    //
    void markingMPCMaster(){ mbMaster= true;}
    bool& isMPCMaster(){ return mbMaster;}

    void markingMPCSlave(){ mbSlave= true;}
    bool& isMPCSlave(){ return mbSlave;}

    void markingMPCFace(const uint& iface){ mvbMPCFace[iface]= true;}
    bool isMPCFace(const uint& iface){ return mvbMPCFace[iface];}


    // CommMesh2に使用する要素か判定する属性
    //
    void markingCommMesh2(){ mbCommMesh2= true;}
    bool& isCommMesh2(){ return mbCommMesh2;}

    void markingCommEntity(const uint& ient){ mnCommEntity= ient;  mvbCommEntity[ient]= true;}
    bool isCommEntity(const uint& ient){ return mvbCommEntity[ient];}
    uint& getCommEntityID(){ return  mnCommEntity;}


    // Refine後
    // 1. 辺-面 Element*配列を解放
    // 2. 辺-面 Node*   配列を解放 (2次要素は辺ノードを残す)
    // 
    virtual void deleteProgData()=0;

};
}
#endif