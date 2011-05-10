//
//  Tetra.h
//					2009.06.19
//					2008.11.27
//					k.Takeda
#ifndef TETRA_CF92E37B_646D_4183_81F8_DEFC108C09A2
#define TETRA_CF92E37B_646D_4183_81F8_DEFC108C09A2

#include "SolidBase.h"
//#include "Element.h"

namespace pmw{
class CTetra :public CSolidBase
{
public:
    CTetra(void);
    virtual ~CTetra(void);

private:
    static uint mnElemType;
    static uint mnElemOrder;
    static uint mNumOfFace;
    static uint mNumOfEdge;
    static uint mNumOfNode;
    static uint mNumOfVert;

    ////API 形状関数・導関数
    //pmw::CShapeTetra    *mpShapeTetra;

protected:
    virtual bool IndexCheck(const uint& propType, const uint& index, string& method_name);
    virtual uint& getLocalFaceNum(const vuint& vLocalNodeNum);

public:
    virtual void initialize();

public:
    // Property
    virtual const uint& getType();
    virtual const uint& getOrder();
    virtual const uint& getNumOfFace(){ return mNumOfFace;}
    virtual const uint& getNumOfEdge(){ return mNumOfEdge;}
    virtual const uint& getNumOfNode(){ return mNumOfNode;}
    virtual const uint& getNumOfVert(){ return mNumOfVert;}

    // EdgeNode(Pair Node)
    // --
    virtual PairNode getPairNode(const uint& iedge);
    virtual void getPairNode(vint& pairNodeIndex, const uint& iedge);
    virtual bool isEdgeElem(CNode* pNode0, CNode* pNode1);
    virtual void setBoolEdgeElem(CNode* pNode0, CNode* pNode1);
    //
    // (局所ノード番号,局所ノード番号)に対応した辺の、Index番号
    virtual uint& getEdgeIndex(CNode* pNode0, CNode* pNode1);
    virtual uint& getEdgeIndex(const uint& nodeID_0, const uint& nodeID_1);

    // Edge -> Face Index
    virtual uint& getFaceIndex(const uint& edge0, const uint& edge1);
    // 3 Nodes -> Face Index
    virtual uint& getFaceIndex(CNode* pNode0, CNode* pNode1, CNode* pNode2);

    // Face構成Node
    //
    virtual vector<CNode*> getFaceCnvNodes(const uint& iface);

    // Node周囲の接続先Nodeを配列で返す. (係数行列 作成用途, CMesh::setupAggregate)
    // 
    virtual vector<CNode*> getConnectNode(CNode* pNode);


    // Refine後
    // 1. 辺-面 Element*配列を解放
    // 2. 辺-面 Node*   配列を解放 (2次要素は辺ノードを残す)
    // --
    virtual void deleteProgData();
};
}
#endif