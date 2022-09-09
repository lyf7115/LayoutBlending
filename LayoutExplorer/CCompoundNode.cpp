#include "CCompoundNode.h"
#include <queue>

void CompoundNode::DeleteNode(CompoundNode* node)
{
	if (node == nullptr)
	{
		return;
	}
	if (node->parent != nullptr)
	{
		node->parent->children.erase(std::find(node->parent->children.begin(), node->parent->children.end(), node));
		node->parent = nullptr;
	}

	auto pointer = node;

	while (true)
	{	
		while (!pointer->children.empty())
		{
			pointer = pointer->children.back();
		}
		auto pointer_parent = pointer->parent;

		delete pointer;
		pointer = nullptr;
		pointer = pointer_parent;

		if (pointer == nullptr)
		{
			break;
		}

		auto back_iterator = pointer->children.end() - 1;
		pointer->children.erase(back_iterator);
	}

	node = nullptr;
}

//Deep Copy
CompoundNode* CompoundNode::DeepCopy()
{	
	CompoundNode* newRoot = new CompoundNode();
	*newRoot = *this;

	std::queue<CompoundNode*> nodeQueue;
	nodeQueue.push(newRoot);

	while (!nodeQueue.empty())
	{
		CompoundNode* thisNode = nodeQueue.front();
		nodeQueue.pop();

		std::vector<CompoundNode*> newChildren;

		for (int i = 0; i < thisNode->children.size(); ++i)
		{
			CompoundNode* newChild = new CompoundNode();
			*newChild = *(thisNode->children[i]);

			//note this(new address)
			newChild->parent = thisNode;

			newChildren.push_back(newChild);
			nodeQueue.push(newChild);
		}

		thisNode->children = newChildren;
		newChildren.clear();
	}
	return newRoot;
}

void CompoundNode::DeepDestroy(CompoundNode* aNode)
{
	if (aNode != nullptr)
	{
		std::queue<CompoundNode*> nodeQueue;
		nodeQueue.push(aNode);

		while (!nodeQueue.empty())
		{
			CompoundNode* thisNode = nodeQueue.front();
			nodeQueue.pop();			

			for (int i = 0; i < thisNode->children.size(); ++i)
			{
				nodeQueue.push(thisNode->children[i]);
			}

			delete thisNode;
			thisNode = nullptr;
		}
	}	

	return;
}