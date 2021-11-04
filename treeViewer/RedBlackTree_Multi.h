#pragma once

template<typename T>
class CObjectFreeList;

class SimpleProfiler;
class CRedBlackTreeLess;

#if defined(SPEED_TEST)
extern SimpleProfiler* sp;
#endif

#if defined(_WINDOWS_)
extern RECT windowRect;
#endif

template<typename T, class C = CRedBlackTreeLess>
class CRedBlackTree_Multi {

//	friend class CJumpPointSearch;

public:


	struct stNode {

		stNode() {}
		stNode(CObjectFreeList<stNode>* freeList, bool isRed = true, bool isNill = false, stNode* parent = nullptr);
		~stNode();

		CLinkedList<T>* _dataList;
		stNode* _left;
		stNode* _right;
		stNode* _parent;
		bool _isRed;
		bool _isNill;

	};

	// stNode�� ���� free list
	CObjectFreeList<stNode>* nodeFreeList;

	// CLinekdList �� ���� free list
	CObjectFreeList<CLinkedList<T>>* listFreeList;

	CRedBlackTree_Multi(C compareFunctor = CRedBlackTreeLess());
	~CRedBlackTree_Multi();

	// ������ ����
	void insert(T data);
	// ������ ����
	void erase(T data);

	// T�� ã�Ƽ� T�� �ݴϴ�
	// ���� tree�� key ������ �ᱹ �ֳĸ� ���� ����
	T find(T data);

	// ������ȸ�� ������� �����͸� �迭�� ��Ƽ� �������ݴϴ�.
	T* inorder();

	// ��ü ����
	void clear();

	// ��� ���� ���
	int getNodeNum();

	// Ʈ���� ����ִ°�
	bool empty();

#ifdef _WINDOWS_

	// �׽�Ʈ ����, addNum ��ŭ�� ���� ����
	static const int addNum = 100;
	// �׽�Ʈ ����, eraseNUm ��ŭ�� ���� ������
	static const int eraseNum = 50;
	// ��������� �׽�Ʈ�� ����Ǹ�, addNum - eraseNum ��ŭ�� ���ڰ� Ʈ�� ���ο� ���ԵǴ� ����

	// �� �� ���� ������ ���� ���ϵǴ� �Լ�
	// 100�� �����ϴ� �׽�Ʈ��� 1�� �����ϰ� �����ϰ�
	// �ٽ� �Լ��� �����ϸ� �� 1�� �����ϰ� �����ϴ� ��
	// ������ ���� ��, �� �ܰ辿 ���� �; ������
	static CRedBlackTree_Multi<int>* singleCaseTest(int seed);

	// ��ü ������ �Ϸ��ϰ� ���ϵǴ� �Լ�
	// 100���� �����ϴ� �׽�Ʈ��� 100���� �����ϰ� ������
	static CRedBlackTree_Multi<int>* test(CRedBlackTree_Multi<int>*);

	// ��� ����
	// treeViewer�� �����
	void print(HDC hdc, int x);

#endif

private:

	// �� ����
	C _compareFunctor;

	// ���� Ʈ���� �� ����� ����
	int _nodeNum;

	// ��Ʈ ���
	stNode* _root;

	// ���������� ��尡 �������� �Լ�
	stNode* eraseNode(stNode** node, bool* isRed);

	// ���� �� �뷱�� ��� �Լ�
	void insertBalance(stNode* node);
	// ���� �� �뷱�� ��� �Լ�
	void eraseBalance(stNode* node);

	void leftRotation(stNode* parent, stNode* right);
	void rightRotation(stNode* parent, stNode* left);

	// ��ü ���ܿ� �Լ�
	// Ʈ���� ���� ������������ �Ǵ���
	void diagnosis(stNode* node = _root);

#if defined(_VECTOR_)
	// Ʈ���� ���ͷ� ��ȯ�ؼ� ��������
	// �׽�Ʈ������ ����
	void treeToList(std::vector<T>* vector, std::vector<T>* blackNumList, stNode* node, int blackNum = 0);
#endif

	// ���������� ������ȸ�ϴ� �Լ�
	void inorderLoop(T* arr, int* index, stNode* node);

#ifdef _WINDOWS_
	HDC _hdc;
	int printLoop(stNode* node, int deepth, int* printCnt, int x);
#endif

};

#ifndef _REDBLACKTREELESS_
#define _REDBLACKTREELESS_

class CRedBlackTreeLess {
public:
	template<typename T>
	bool operator()(T left, T right) {
		return left < right;
	}
};

#endif

template <typename T, class C>
CRedBlackTree_Multi<T, C>::stNode::stNode(CObjectFreeList<stNode>* freeList, bool isRed, bool isNill, stNode* parent) {

	_dataList = listFreeList->allocObject();

	if (isNill == false) {
		_left = freeList->allocObject();
		new (_left) stNode(freeList, false, true, this);

		_right = freeList->allocObject();
		new (_right) stNode(freeList, false, true, this);
	}
	else {
		_left = nullptr;
		_right = nullptr;
	}
	_parent = parent;
	_isRed = isRed;
	_isNill = isNill;

}

template <typename T, class C>
CRedBlackTree_Multi<T, C>::stNode::~stNode(){
	listFreeList->freeObject(_dataList);
}

template <typename T, class C>
CRedBlackTree_Multi<T, C>::CRedBlackTree_Multi(C compareFunctor) {

	listFreeList = new CObjectFreeList<CLinkedList<T>>(100);
	nodeFreeList = new CObjectFreeList<stNode>(100);

	_root = nodeFreeList->allocObject();
	new (_root) stNode(nodeFreeList, false, true);

	_compareFunctor = compareFunctor;
	_nodeNum = 0;

}

template <typename T, class C>
CRedBlackTree_Multi<T, C>::~CRedBlackTree_Multi() {

	if (_root->_isNill == true) {
		_root->~stNode();
		nodeFreeList->freeObject(_root);
		return;
	}

	while (_root->_left->_isNill == false || _root->_right->_isNill == false) {
		//_root->_dataList->clear();
		erase(_root->_dataList->begin()->value);
	}

	_root->_left->~stNode();
	nodeFreeList->freeObject(_root->_left);

	_root->_right->~stNode();
	nodeFreeList->freeObject(_root->_right);

	_root->~stNode();
	nodeFreeList->freeObject(_root);

	delete nodeFreeList;
	nodeFreeList = nullptr;
}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::insert(T data) {

#if defined(SPEED_TEST)
	//sp->profileBegin("insert");
#endif

	do {
		stNode* newNode = nodeFreeList->allocObject();
		new (newNode) stNode(nodeFreeList);

		if (_root->_isNill == true) {
			_root->~stNode();
			nodeFreeList->freeObject(_root);
			newNode->_isRed = false;
			_root = newNode;
			newNode->_dataList->push_back(data);
			_nodeNum = 1;
			break;
		}

		stNode* node = _root;
		while (true) {

			CLinkedList<T>* list = node->_dataList;
			T value = list->begin()->value;

			if (_compareFunctor(value, data) == true) {
				if (node->_right->_isNill == true) {

					node->_right->~stNode();
					nodeFreeList->freeObject(node->_right);
					node->_right = newNode;
					newNode->_dataList->push_back(data);
					newNode->_parent = node;
					insertBalance(newNode);

					_nodeNum += 1;

#if defined(LOGIC_TEST)
					diagnosis(_root);
#endif

					// return �ϴ� ��Ȳ. profile ó���� return ������ �ؾ��ؼ� ó���Ϸ� goto �� �̵�
					// break �ϸ� ��尡 delete �Ǿ����
					goto FIN;
				}
				node = node->_right;
			}
			else if (_compareFunctor(data, value) == true) {
				if (node->_left->_isNill == true) {
					
					node->_left->~stNode();
					nodeFreeList->freeObject(node->_left);
					node->_left = newNode;
					newNode->_dataList->push_back(data);
					newNode->_parent = node;
					insertBalance(newNode);

					_nodeNum += 1;

#if defined(LOGIC_TEST)
					diagnosis(_root);
#endif

					// return �ϴ� ��Ȳ. profile ó���� return ������ �ؾ��ؼ� ó���Ϸ� goto �� �̵�
					// break �ϸ� ��尡 delete �Ǿ����
					goto FIN;
				}
				node = node->_left;
			}
			else {
				list->push_back(data);
				break;
			}
		}
		newNode->~stNode();
		nodeFreeList->freeObject(newNode);
	} while (false);

FIN:;

#if defined(SPEED_TEST)
	//sp->profileEnd("insert");
#endif

}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::erase(T data) {

#if defined(SPEED_TEST)
	//sp->profileBegin("erase");
#endif

	stNode** node = &_root;

	while ((*node)->_isNill == false) {

		CLinkedList<T>* list = (*node)->_dataList;
		T value = list->begin()->value;

		if (_compareFunctor(value, data) == true) {
			//right
			node = &(*node)->_right;
		}
		else if (_compareFunctor(data, value) == true) {
			//left
			node = &(*node)->_left;
		}
		else {
			// correct

			// ������ ����Ʈ���� �ش� �׸� ����
			typename CLinkedList<T>::iterator iter = list->begin();
			for (; iter != list->end(); ++iter) {
				if (*iter == data) {
					list->erase(iter);
					break;
				}
			}

			if (list->empty() == false) {
				break;
			}

			// ���� �ߴµ� ����Ʈ�� ����ִٸ� �ش� ��� ����
			bool isRed;
			stNode* erasedNode = eraseNode(node, &isRed);
			_nodeNum -= 1;
			if (isRed == false) {
				eraseBalance(erasedNode);

#if defined(LOGIC_TEST)
				diagnosis(_root);
#endif
			}
			break;
		}
	}

#if defined(SPEED_TEST)
	//sp->profileEnd("erase");
#endif
}

template <typename T, class C>
T CRedBlackTree_Multi<T, C>::find(T data) {

#if defined(SPEED_TEST)
	sp->profileBegin("find");
#endif

	stNode* node = _root;
	T result = NULL;

	while (node != nullptr) {

		if (node->_isNill == true) {
			break;
		}

		if (_compareFunctor(data, node->_dataList->begin()->value) == true) {
			// data�� �� ����
			node = node->_left;
		}
		else if (_compareFunctor(node->_dataList->begin()->value, data) == true) {
			// data�� �� ŭ
			node = node->_right;
		}
		else {
			// ����
			result = node->_dataList->begin()->value;
			break;
		}

	}

#if defined(SPEED_TEST)
	sp->profileEnd("find");
#endif

	return result;

}

#if defined(_WINDOWS_)
template <typename T, class C>
CRedBlackTree_Multi<int>* CRedBlackTree_Multi<T, C>::singleCaseTest(int seed) {

	static CRedBlackTree_Multi<int>* tree = nullptr;

	static std::vector<int>* addNumList = nullptr;
	static std::vector<int>* eraseIndexList = nullptr;

	static std::vector<int>* ableNumList = nullptr;

	static std::vector<int>::iterator addNumListIter;
	static std::vector<int>::iterator eraseIndexListIter;

	if (addNumList == nullptr) {

		printf("seed : %d\n", seed);
		srand(seed);

		delete(tree);
		tree = new CRedBlackTree();
		addNumList = new std::vector<int>();
		eraseIndexList = new std::vector<int>();
		ableNumList = new std::vector<int>();

		std::vector<int> answerList;

		for (int numCnt = 0; numCnt < addNum; numCnt++) {
			ableNumList->push_back(numCnt);
		}

		printf("add num: ");
		for (int numCnt = 0; numCnt < addNum; numCnt++) {
			int index = rand() % ableNumList->size();
			addNumList->push_back(ableNumList->at(index));
			printf("%d ", ableNumList->at(index));
			answerList.push_back(ableNumList->at(index));
			ableNumList->erase(ableNumList->begin() + index);
		}
		printf("\n");

		for (int indexCnt = 0; indexCnt < eraseNum; ++indexCnt) {
			int index = rand() % (addNum - indexCnt);
			eraseIndexList->push_back(index);
			answerList.erase(answerList.begin() + index);
		}

		std::sort(answerList.begin(), answerList.end());

		addNumListIter = addNumList->begin();
		eraseIndexListIter = eraseIndexList->begin();

		printf("answer: ");
		for (std::vector<int>::iterator answerIter = answerList.begin(); answerIter != answerList.end(); ++answerIter) {

			printf("%d ", *answerIter);

		}
		printf("\n");

	}

	if (addNumListIter != addNumList->end()) {


		tree->insert(*addNumListIter);
		++addNumListIter;
		singleCaseTest(seed);
		return tree;

	}

	if (eraseIndexListIter != eraseIndexList->end()) {

		tree->erase(addNumList->at(*eraseIndexListIter));
		addNumList->erase(addNumList->begin() + *eraseIndexListIter);
		++eraseIndexListIter;
		addNumListIter = addNumList->end();
		return tree;

	}

	std::vector<int> treeData;
	std::vector<int> blackNodeNum;
	tree->treeToList(&treeData, &blackNodeNum, tree->_root);

	std::vector<int>::iterator treeIter;
	std::vector<int>::iterator numIter;
	sort(addNumList->begin(), addNumList->end());

	for (treeIter = treeData.begin(), numIter = addNumList->begin(); treeIter != treeData.end(); ++treeIter, ++numIter) {
		if (*treeIter != *numIter) {
			break;
		}
	}
	if (treeIter != treeData.end()) {
		printf("Ʈ���� �����Ͱ� ������ �����Ͱ� �ƴ�\n");
		system("PAUSE>NUL");
	}

	std::vector<int>::iterator blackCntIter;
	for (blackCntIter = blackNodeNum.begin() + 1; blackCntIter != blackNodeNum.end(); ++blackCntIter) {
		if (*blackCntIter != blackNodeNum[0]) {
			break;
		}
	}

	if (blackCntIter != blackNodeNum.end()) {
		printf("�� ����� ���� ��� ���� ��ġ���� �ʽ��ϴ�.");
		system("PAUSE>NUL");
	}


	delete(addNumList);
	addNumList = nullptr;
	delete(eraseIndexList);
	eraseIndexList = nullptr;
	delete(ableNumList);
	ableNumList = nullptr;

	tree = test();

	return tree;
}

template <typename T, class C>
CRedBlackTree_Multi<int>* CRedBlackTree_Multi<T, C>::test(CRedBlackTree_Multi<int>* tree) {

	//CRedBlackTree<int>* tree = nullptr;

	std::vector<int>* addNumList = nullptr;
	std::vector<int>* eraseIndexList = nullptr;

	std::vector<int>* ableNumList = nullptr;

	std::vector<int>::iterator addNumListIter;
	std::vector<int>::iterator eraseIndexListIter;

	int seed = rand() % 10000;
	//int seed = 6699;
	printf("seed : %d\n", seed);
	srand(seed);

	//tree = new CRedBlackTree();
	addNumList = new std::vector<int>();
	eraseIndexList = new std::vector<int>();
	ableNumList = new std::vector<int>();

	std::vector<int> answerList;

	for (int numCnt = 0; numCnt < addNum; numCnt++) {
		ableNumList->push_back(numCnt);
	}

	printf("add num: ");
	for (int numCnt = 0; numCnt < addNum; numCnt++) {
		int index = rand() % ableNumList->size();
		addNumList->push_back(ableNumList->at(index));
		printf("%d ", ableNumList->at(index));
		answerList.push_back(ableNumList->at(index));
		ableNumList->erase(ableNumList->begin() + index);
	}
	printf("\n");

	for (int indexCnt = 0; indexCnt < eraseNum; ++indexCnt) {
		int index = rand() % (addNum - indexCnt);
		eraseIndexList->push_back(index);
		answerList.erase(answerList.begin() + index);
	}

	std::sort(answerList.begin(), answerList.end());

	printf("answer: ");
	for (std::vector<int>::iterator answerIter = answerList.begin(); answerIter != answerList.end(); ++answerIter) {

		printf("%d ", *answerIter);

	}
	printf("\n");


	addNumListIter = addNumList->begin();
	for (; addNumListIter != addNumList->end(); ++addNumListIter) {
		tree->insert(*addNumListIter);
	}


	eraseIndexListIter = eraseIndexList->begin();
	for (; eraseIndexListIter != eraseIndexList->end(); ++eraseIndexListIter) {
		tree->erase(addNumList->at(*eraseIndexListIter));
		addNumList->erase(addNumList->begin() + *eraseIndexListIter);
	}


	std::vector<int> treeData;
	std::vector<int> blackNodeNum;
	tree->treeToList(&treeData, &blackNodeNum, tree->_root);

	std::vector<int>::iterator treeIter;
	std::vector<int>::iterator numIter;
	sort(addNumList->begin(), addNumList->end());

	for (treeIter = treeData.begin(), numIter = addNumList->begin(); treeIter != treeData.end(); ++treeIter, ++numIter) {
		if (*treeIter != *numIter) {
			break;
		}
	}

	if (treeIter != treeData.end()) {
		printf("Ʈ���� �����Ͱ� ������ �����Ͱ� �ƴ�\n");
		system("PAUSE>NUL");
	}

	std::vector<int>::iterator blackCntIter;
	for (blackCntIter = blackNodeNum.begin() + 1; blackCntIter != blackNodeNum.end(); ++blackCntIter) {
		if (*blackCntIter != blackNodeNum[0]) {
			break;
		}
	}

	if (blackCntIter != blackNodeNum.end()) {
		printf("�� ����� ���� ��� ���� ��ġ���� �ʽ��ϴ�.");
		system("PAUSE>NUL");
	}



	delete(addNumList);
	addNumList = nullptr;
	delete(eraseIndexList);
	eraseIndexList = nullptr;
	delete(ableNumList);
	ableNumList = nullptr;


	return tree;
}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::print(HDC hdc, int x) {
	do {
		if (_root->_isNill == true) {
			break;
		}

		_hdc = hdc;
		int printCnt = 0;
		printLoop(_root, 0, &printCnt, x);
	} while (false);
	{
		// ���� ��� ����Ű ����

		int left = windowRect.right - 200;
		int top = 0;
		int right = windowRect.right - 10;
		int bottom = 125;

		Rectangle(hdc, left, top, right, bottom);

		TextOutW(hdc, left + 10, top + 10, L"�����̵� : a", 8);
		TextOutW(hdc, left + 10, top + 25, L"�����̵� : d", 8);
		TextOutW(hdc, left + 10, top + 40, L"�����߰� : z", 8);
		TextOutW(hdc, left + 10, top + 55, L"�������� : x", 8);
		TextOutW(hdc, left + 10, top + 70, L"�׽�Ʈ�ڵ� ���� : c", 12);
		TextOutW(hdc, left + 10, top + 85, L"���ð� ���� : q", 10);
		TextOutW(hdc, left + 10, top + 100, L"���ð� ���� : e", 10);

	}

}
#endif

template <typename T, class C>
typename CRedBlackTree_Multi<T, C>::stNode* CRedBlackTree_Multi<T, C>::eraseNode(stNode** node, bool* isRed) {

	stNode* left = (*node)->_left;
	stNode* right = (*node)->_right;

	bool isLeftNill = left->_isNill;
	bool isRightNill = right->_isNill;

	if (isLeftNill == true) {

		// ��� ���� ���� ��¥�� ���⼭ �θ𿡼� �� ��尡 ����Ǿ� �ִ� ���� �ҷ� �����
		// ����� ���� �ڽ� ����

		right->_parent = (*node)->_parent;
		left->~stNode();
		nodeFreeList->freeObject(left);
		//delete(left); // ���� �� ����
		*isRed = (*node)->_isRed;
		(*node)->~stNode();
		nodeFreeList->freeObject(*node);
		//delete(*node); // ���� ��� ����
		*node = right; // �θ� ��尡 �ٶ󺸰� �ִ� ��带 ���� ���� ����
		return *node;

	}
	else if (isRightNill == true) {

		// ����� ���� �ڽ� ����

		left->_parent = (*node)->_parent;
		right->~stNode();
		nodeFreeList->freeObject(right);
		//delete(right); // ���� �� ����
		*isRed = (*node)->_isRed;
		(*node)->~stNode();
		nodeFreeList->freeObject(*node);
		//delete(*node); // ���� ��� ����
		*node = left;  // �θ� �ٶ󺸰� �ִ� ��带 ���� ���� ����
		return *node;

	}
	else {

		// ����� ���� �ڽ� ����
		// ����� ���� �ڽ��� �ֿ��� �ڽİ� ������ ��ü

		if (left->_right->_isNill == true) {

			left->_right->~stNode();
			nodeFreeList->freeObject(left->_right);
			//delete(left->_right); // �� ����

			// ����� ���� �ڽ��� ���� �ڽ��� ���� ��

			// ����� ���� �ڽİ� ����� �����͸� ��ü�ϰ�
			// ����� ���� �ڽ��� ���� �ڽ��� ���� ����

			(*node)->_dataList = left->_dataList; // ����� �����͸� ���� �ڽ��� �����ͷ� ����
			(*node)->_left = left->_left; // ����� ���� �ڽ��� ���� �ڽ��� ���� �ڽ����� ����
			left->_left->_parent = *node;
			*isRed = left->_isRed;

			left->~stNode();
			nodeFreeList->freeObject(left);
			//delete(left); // ���� ��� ����
			return (*node)->_left;

		}
		else {

			// ����� ���� �ڽ��� ���� �ڽ��� ���� ��

			// �ֿ��� �ڽ��� ã�� ���� ����
			stNode** lastRightNode = &left; // ���� ������ ������ �ڽ�
			while ((*lastRightNode)->_right->_isNill == false) { // ���� �ڽ��� ���� �ƴϸ�
				lastRightNode = &((*lastRightNode)->_right); // ���� �ڽ����� ��ü
			}
			(*lastRightNode)->_right->~stNode();
			nodeFreeList->freeObject((*lastRightNode)->_right);
			//delete((*lastRightNode)->_right); // ������

			(*node)->_dataList = (*lastRightNode)->_dataList; // ����� ���� �ֿ�������� ������ ����

			// �ֿ��� ����� ���� ��� ���� ����
			// �ش� ���� �ֿ��� ����� �θ�� ����
			// ���̸� �� �����

			stNode* lastNodeLeft = (*lastRightNode)->_left; // �ֿ��� ����� ���� ���
			lastNodeLeft->_parent = (*lastRightNode)->_parent;
			*isRed = (*lastRightNode)->_isRed;
			(*lastRightNode)->~stNode();
			nodeFreeList->freeObject(*lastRightNode);
			//delete(*lastRightNode); // �ֿ��� ��� ����
			*lastRightNode = lastNodeLeft; // �ֿ��� ����� �θ� ���� �ֿ��� ����� ���� ��� ����


			return *lastRightNode;
		}

	}

}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::insertBalance(stNode* node) {

	// ��尡 ��Ʈ�� Ż��
	if (node == _root) {
		node->_isRed = false;
		return;
	}

	stNode* parent = node->_parent;

	// ����� �θ� �����̸� Ż��
	if (parent->_isRed == false) {
		return;
	}

	stNode* grand = parent->_parent;
	stNode* uncle;
	bool isNodeLeftGrand;
	if (grand->_left == parent) {
		uncle = grand->_right;
		isNodeLeftGrand = true;
	}
	else {
		uncle = grand->_left;
		isNodeLeftGrand = false;
	}

	stNode* sibling;
	bool isSiblingLeftParent;
	if (parent->_left == node) {
		sibling = parent->_right;
		isSiblingLeftParent = false;
	}
	else {
		sibling = parent->_left;
		isSiblingLeftParent = true;
	}

	if (parent->_isRed == true) {

		// �θ�� ������ ������ ���
		if (uncle->_isRed == true) {
			grand->_isRed = true; // ���θ� �������� ��
			parent->_isRed = false; // �θ� �������� ��
			uncle->_isRed = false; // ������ �������� ��
			insertBalance(grand); // ���η� ���
			return;
		}

		// �θ� ������ ���

		if (isSiblingLeftParent == true) {
			// ������ ���ʿ� ���� ���
			if (isNodeLeftGrand == true) {
				// ���� ������ ���

				// ��� - �θ� - ������ ȸ��

				// ���� �������� ��带 �̵�
				grand->_left = node;
				node->_parent = grand;


				// �θ� �������� �ҳ��� ����
				parent->_right = node->_left;
				node->_left->_parent = parent;

				// ��� �������� �θ� �̵�
				node->_left = parent;
				parent->_parent = node;


				// ��� ��Ī ����
				stNode* temp = node;
				node = parent;
				parent = temp;
				sibling = parent->_right; // �� ���

			}
		}
		else {

			// ������ �����ʿ� ���� ���
			if (isNodeLeftGrand == false) {
				// ���� �������� ���

				// ��� - �θ� - ������ ȸ��

				// ���� ���������� ��带 �̵�
				grand->_right = node;
				node->_parent = grand;

				// �θ� ������ �ҳ��� ����
				parent->_left = node->_right;
				node->_right->_parent = parent;

				// ��� ���������� �θ� �̵�
				node->_right = parent;
				parent->_parent = node;

				// ��� ��Ī ����
				stNode* temp = node;
				node = parent;
				parent = temp;
				sibling = parent->_left; // �� ���
			}

		}

		// ���α��� ������ ȸ��

		if (grand->_parent != nullptr) {
			if (grand->_parent->_left == grand) {
				grand->_parent->_left = parent;
				parent->_parent = grand->_parent;
			}
			else {
				grand->_parent->_right = parent;
				parent->_parent = grand->_parent;
			}
		}
		else {
			parent->_parent = nullptr;
			_root = parent;
		}


		if (isNodeLeftGrand == true) {
			// ��尡 ���� ���ʿ� ���� ���
			grand->_left = sibling;
			sibling->_parent = grand;
			grand->_parent = parent;
			parent->_right = grand;

		}
		else {
			// ��尡 ���� �����ʿ� ���� ���
			grand->_right = sibling;
			sibling->_parent = grand;
			grand->_parent = parent;
			parent->_left = grand;

		}

		grand->_isRed = true;
		parent->_isRed = false;

		return;
	}
}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::leftRotation(stNode* parent, stNode* right) {

	right->_parent = parent->_parent;
	if (parent->_parent != nullptr) {
		if (parent->_parent->_left == parent) {
			parent->_parent->_left = right;
		}
		else {
			parent->_parent->_right = right;
		}
	}
	else {
		_root = right;
	}

	parent->_right = right->_left;
	right->_left->_parent = parent;

	right->_left = parent;
	parent->_parent = right;

}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::rightRotation(stNode* parent, stNode* left) {

	left->_parent = parent->_parent;
	if (parent->_parent != nullptr) {
		if (parent->_parent->_left == parent) {
			parent->_parent->_left = left;
		}
		else {
			parent->_parent->_right = left;
		}
	}
	else {
		_root = left;
	}

	parent->_left = left->_right;
	left->_right->_parent = parent;

	left->_right = parent;
	parent->_parent = left;


}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::eraseBalance(stNode* node) {

	if (node == _root) {
		_root->_isRed = false;
		return;

	}

	if (node->_isRed == true) {
		node->_isRed = false;
		return;
	}

	stNode* parent = node->_parent;
	stNode* sibling;
	bool _isNodeLeftParent;

	if (parent->_left == node) {
		sibling = parent->_right;
		_isNodeLeftParent = true;
	}
	else {
		sibling = parent->_left;
		_isNodeLeftParent = false;
	}

	if (parent->_isRed == true) {
		// �θ� ������ ���

		if (sibling->_isNill == true) {
			return;
		}

		if (sibling->_isRed == false) {
			// ������ ������ �� ��
			if (sibling->_left->_isRed == false && sibling->_right->_isRed == false) {
				// ������ �ڽĵ� ��� ������ ��
				if (_isNodeLeftParent == true) {
					leftRotation(parent, sibling);
				}
				else {
					rightRotation(parent, sibling);
				}
				return;
			}

			if (sibling->_left->_isRed == true && sibling->_right->_isRed == true) {
				// ���� �ڽ� ��� ������ ��
				sibling->_isRed = true;
				parent->_isRed = false;
				if (_isNodeLeftParent == true) {
					sibling->_right->_isRed = false;
					leftRotation(parent, sibling);
				}
				else {
					sibling->_left->_isRed = false;
					rightRotation(parent, sibling);
				}
				return;
			}

			stNode* left = sibling->_left;
			stNode* right = sibling->_right;

			if (_isNodeLeftParent == true) {

				// ��尡 �θ��� ���ʿ� ���� ��
				if (left->_isRed == true) {
					// ������ ���� ��尡 �����̸�
					sibling->_isRed = true;
					left->_isRed = false;
					rightRotation(sibling, left);
					sibling = left;
				}

				// ������ ���� �ڽ��� �����̸�
				leftRotation(parent, sibling);

			}
			else {
				// ��尡 �θ��� �����ʿ� ���� ��
				if (right->_isRed == true) {
					// ������ ���� ��尡 �����̸�
					sibling->_isRed = true;
					right->_isRed = false;
					leftRotation(sibling, right);
					sibling = right;
				}

				// ������ ���� ��尡 �����̸�
				rightRotation(parent, sibling);
			}
			return;
		}




		return;

	}

	// �θ� ������ ���

	if (sibling->_isRed == false) {
		// ������ ������ ���

		if (sibling->_isNill == true) {
			return;
		}

		if (_isNodeLeftParent == true) {

			// ������ ������ ���� ���
			if (sibling->_left->_isRed == true) {

				// ������ ���� �ڽ��� ������ ���

				// ���� - ������ �ڽ� �� ��ȸ��
				stNode* left = sibling->_left;
				stNode* right = sibling->_right;

				sibling->_isRed = true;
				left->_isRed = false;
				rightRotation(sibling, left);
				sibling = parent->_right;

			}

			if (sibling->_right->_isRed == true) {

				// ������ ���� �ڽ��� ������ ���
				sibling->_right->_isRed = false;
				leftRotation(parent, sibling);
				//eraseBalance(parent);

				return;
			}
		}
		else {
			// ������ ������ ���� ���
			if (sibling->_right->_isRed == true) {
				// ������ ���� �ڽ��� ������ ���

				// ���� - ������ �ڽ� �� ��ȸ��
				stNode* left = sibling->_left;
				stNode* right = sibling->_right;

				sibling->_isRed = true;
				right->_isRed = false;
				leftRotation(sibling, right);
				sibling = parent->_left;
			}
			if (sibling->_left->_isRed == true) {

				// ������ ���� �ڽ��� ������ ���
				sibling->_left->_isRed = false;
				rightRotation(parent, sibling);
				//eraseBalance(parent);

				return;
			}


		}
		sibling->_isRed = true;
		eraseBalance(parent);
		return;
	}

	if (sibling->_isRed == true) {
		// ������ ������ ���

		sibling->_isRed = false;
		parent->_isRed = true;


		if (_isNodeLeftParent == true) {
			// �θ� ������ ��ȸ��
			sibling->_isRed = false;
			leftRotation(parent, sibling);
		}
		else {

			// �θ� ������ ��ȸ��
			rightRotation(parent, sibling);
		}

		eraseBalance(node);

	}


}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::diagnosis(stNode* node) {

	if (node->_left != nullptr) {

		if (node->_left->_parent != node) {
			// left�� �θ� ���� �ƴ� ���
			printf("{\n");
			printf("\t���� ����� �θ� ���� ��尡 �ƴմϴ�.\n");
			printf("\t���� ��� �� : %d\n", node->_dataList->begin()->value);
			printf("\t���� ��� �� : %d\n", node->_left->_dataList->begin()->value);
			printf("\t���� ��� �θ� �� : %d\n", node->_left->_parent->_dataList->begin()->value);
			printf("}\n");
			system("PAUSE>NUL");
		}

		if (node->_isRed == true) {
			if (node->_left->_isRed == true) {

				printf("{\n");
				printf("\t���� �� ���� ��尡 �����Դϴ�.\n");
				printf("\tnode data: %d\n", node->_dataList->begin()->value);
				printf("}\n");
				system("PAUSE>NUL");
			}
		}

		diagnosis(node->_left);
	}
	else if (node->_isNill == false) {
		// left�� ���ε�, ���� �� ��尡 �ƴϸ� ����
		printf("{\n");
		printf("\t���� ��尡 Null ������ ���� �� ��尡 �ƴմϴ�.\n");
		printf("\tnode data: %d\n", node->_dataList->begin()->value);
		printf("}\n");
		system("PAUSE>NUL");
	}

	//printf("���� ��ȸ, ���� ��� ��: %d\n", node->_data);

	if (node->_isNill == true) {
		if (node->_isRed == true) {
			// �ҳ�� �� ����
			printf("{\n");
			printf("\tNill ��尡 �������Դϴ�.\n");
			printf("\tnode data: %d\n", node->_dataList->begin()->value);
			printf("}\n");
			system("PAUSE>NUL");
		}
	}

	if (node->_right != nullptr) {

		if (node->_right->_parent != node) {
			// right�� �θ� ���� �ƴ� ���
			printf("{\n");
			printf("\t���� ����� �θ� ���� ��尡 �ƴմϴ�.\n");
			printf("\t���� ��� �� : %d\n", node->_dataList->begin()->value);
			printf("\t���� ��� �� : %d\n", node->_right->_dataList->begin()->value);
			printf("\t���� ��� �θ� �� : %d\n", node->_right->_parent->_dataList->begin()->value);
			printf("}\n");
			system("PAUSE>NUL");
		}

		if (node->_isRed == true) {
			if (node->_right->_isRed == true) {

				printf("{\n");
				printf("\t���� �� ���� ��尡 �����Դϴ�.\n");
				printf("\tnode data: %d\n", node->_dataList->begin()->value);
				printf("}\n");
				system("PAUSE>NUL");
			}
		}

		diagnosis(node->_right);
	}
	else if (node->_isNill == false) {
		// right�� ���ε�, ���� �� ��尡 �ƴϸ� ����
		printf("{\n");
		printf("\t���� ��尡 Null ������ ���� �� ��尡 �ƴմϴ�.\n");
		printf("\tnode data: %d\n", node->_dataList->begin()->value);
		printf("}\n");
		system("PAUSE>NUL");
	}

}

#if defined(_VECTOR_)
template <typename T, class C>
void CRedBlackTree_Multi<T, C>::treeToList(std::vector<T>* vector, std::vector<T>* blackNumList, stNode* node, int blackNum) {
	blackNum += (node->_isRed == false);
	if (node->_left->_isNill == false) {
		treeToList(vector, blackNumList, node->_left, blackNum);
	}
	else {
		blackNumList->push_back(blackNum);
	}

	T value = node->_dataList->begin()->value;
	vector->push_back(value);

	if (node->_right->_isNill == false) {
		treeToList(vector, blackNumList, node->_right, blackNum);
	}
	else {
		blackNumList->push_back(blackNum);
	}

}
#endif

#if defined(_WINDOWS_)
template <typename T, class C>
int CRedBlackTree_Multi<T, C>::printLoop(stNode* node, int deepth, int* printCnt, int x) {

	if (node->_left->_isNill == false) {
		int leftPrintCnt = printLoop(node->_left, deepth + 1, printCnt, x);

		// ���� ����� ���� ������
		int leftLineX = leftPrintCnt * 80 + 80 - (80 * 0.14); // right
		int leftLineY = (deepth + 1) * 80 + (80 * 0.14); // top

		// ���� ����� ���� ������
		int nodeLineX = *printCnt * 80 + (80 * 0.14); // left
		int nodeLineY = deepth * 80 + 80 - (80 * 0.14); // bottom

		MoveToEx(_hdc, leftLineX - x, leftLineY, nullptr);
		LineTo(_hdc, nodeLineX - x, nodeLineY);

	}

	RECT ellipseRect;
	ZeroMemory(&ellipseRect, sizeof(RECT));

	ellipseRect.left = *printCnt * 80;
	ellipseRect.top = deepth * 80;
	ellipseRect.right = ellipseRect.left + 80;
	ellipseRect.bottom = ellipseRect.top + 80;

	HBRUSH ellipseBrush;
	if (node->_isRed == true) {
		ellipseBrush = CreateSolidBrush(RGB(240, 70, 80));
		SetBkColor(_hdc, RGB(240, 70, 80));
		SetTextColor(_hdc, RGB(0, 0, 0));
	}
	else {
		ellipseBrush = CreateSolidBrush(RGB(43, 43, 43));
		SetBkColor(_hdc, RGB(43, 43, 43));
		SetTextColor(_hdc, RGB(255, 255, 255));
	}

	HBRUSH oldEllipseBrush = (HBRUSH)SelectObject(_hdc, ellipseBrush);
	Ellipse(_hdc, ellipseRect.left - x, ellipseRect.top, ellipseRect.right - x, ellipseRect.bottom);
	SelectObject(_hdc, oldEllipseBrush);
	DeleteObject(ellipseBrush);


	WCHAR text[10] = { 0, };
	_itow_s(node->_dataList->begin()->value, text, 10, 10);
	TextOutW(_hdc, ellipseRect.left + 40 - x, ellipseRect.top + 40, text, wcslen(text));

	SetTextColor(_hdc, RGB(0, 0, 0));
	SetBkColor(_hdc, RGB(255, 255, 255));
	int nodePrintCnt = *printCnt;
	*printCnt += 1;

	if (node->_right->_isNill == false) {
		int rightPrintCnt = printLoop(node->_right, deepth + 1, printCnt, x);

		// ���� ����� ���� ������
		int rightLineX = rightPrintCnt * 80 + (80 * 0.14); // left
		int rightLineY = (deepth + 1) * 80 + (80 * 0.14); // top

		// ����� ���� ������
		int nodeLineX = nodePrintCnt * 80 + 80 - (80 * 0.14); // right
		int nodeLineY = deepth * 80 + 80 - (80 * 0.14); // bottom

		MoveToEx(_hdc, rightLineX - x, rightLineY, nullptr);
		LineTo(_hdc, nodeLineX - x, nodeLineY);

	}

	return nodePrintCnt;

}
#endif

template <typename T, class C>
T* CRedBlackTree_Multi<T, C>::inorder() {

#if defined(SPEED_TEST)
	sp->profileBegin("Inorder");
#endif

	T* arr = new T[_nodeNum];
	int index = 0;

	inorderLoop(arr, &index, _root);

#if defined(SPEED_TEST)
	sp->profileEnd("Inorder");
#endif

	return arr;

}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::inorderLoop(T* arr, int* index, stNode* node) {

	stNode* left = node->_left;
	stNode* right = node->_right;

	if (left->_isNill == false) {

		inorderLoop(arr, index, left);

	}

	arr[*index] = node->_dataList->begin()->value;
	*index += 1;

	if (right->_isNill == false) {

		inorderLoop(arr, index, right);

	}

}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::clear() {

	if (_root->_isNill == true) {
		_root->~stNode();
		nodeFreeList->freeObject(_root);
		//delete(_root);
		return;
	}

	while (_root->_left->_isNill == false || _root->_right->_isNill == false) {
		erase(_root->_dataList->begin()->value);
	}

	_root->_left->~stNode();
	nodeFreeList->freeObject(_root->_left);

	_root->_right->~stNode();
	nodeFreeList->freeObject(_root->_right);

	_root->~stNode();
	nodeFreeList->freeObject(_root);

	//delete(_root->_left);
	//delete(_root->_right);
	//delete(_root);

	_root = nodeFreeList->allocObject();// new stNode(0, false, true);
	new (_root) stNode(nodeFreeList, false, true);

	_nodeNum = 0;

}

template <typename T, class C>
int CRedBlackTree_Multi<T, C>::getNodeNum() {
	return _nodeNum;
}

template <typename T, class C>
bool CRedBlackTree_Multi<T, C>::empty() {
	return _nodeNum == 0;
}