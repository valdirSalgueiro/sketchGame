#ifndef _TREENODE_H_INCLUDED_
#define _TREENODE_H_INCLUDED_

#include "Allocator.h"

class FTBitmapChar;

class TreeNode
{
public:
	TreeNode();
	TreeNode(int x, int y, int width, int height);
	~TreeNode();
	void Set(int x, int y, int width, int height);
	bool Add(FTBitmapChar* pFTBitmapChar);
	bool IsEmpty() const
	{
		return m_pLeaf1 == NULL && m_pLeaf2 == NULL;
	}
	static Allocator<TreeNode>& GetAllocator() 
	{
		return m_allocator;
	}
private:
	void CreateBranches(FTBitmapChar* pFTBitmapChar);
	bool Fits(FTBitmapChar* pFTBitmapChar);
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	TreeNode* m_pLeaf1;
	TreeNode* m_pLeaf2;
	static Allocator<TreeNode> m_allocator;
};





#endif // _TREENODE_H_INCLUDED_
