#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// 樹的節點類別，適用於 M-way 樹和 B-tree
class TreeNode {
public:
    vector<int> keys;  // 儲存節點中的鍵
    vector<TreeNode*> children;  // 儲存節點的子節點
    bool isLeaf;  // 是否是葉節點

    TreeNode(bool leaf) : isLeaf(leaf) {}

    // 顯示樹的結構
    void display(int level = 0) const {
        cout << string(level * 2, ' ') << "[ ";
        for (int key : keys) {
            cout << key << " ";
        }
        cout << "]" << endl;

        if (!isLeaf) {
            // 遞歸顯示子節點
            for (TreeNode* child : children) {
                child->display(level + 1);
            }
        }
    }

    // 清理子節點記憶體
    ~TreeNode() {
        for (TreeNode* child : children) {
            delete child;
        }
    }
};

// M-way 樹和 B-tree 類別的基底類
template <typename T>
class Tree {
protected:
    TreeNode* root;  // 樹的根節點
    int t;  // 樹的階數

    // 插入鍵值到非滿節點
    void insertNonFull(TreeNode* node, T key) {
        int i = node->keys.size() - 1;

        if (node->isLeaf) {
            node->keys.push_back(0);  // 增加空間
            while (i >= 0 && node->keys[i] > key) {
                node->keys[i + 1] = node->keys[i];  // 向右移動元素
                i--;
            }
            node->keys[i + 1] = key;  // 插入新鍵
        }
        else {
            while (i >= 0 && node->keys[i] > key) {
                i--;
            }
            i++;
            if (node->children[i]->keys.size() == 2 * t - 1) {
                splitChild(node, i);  // 如果子節點滿了，進行分裂
                if (key > node->keys[i]) {
                    i++;  // 確保插入正確的子節點
                }
            }
            insertNonFull(node->children[i], key);  // 進行遞歸插入
        }
    }

    // 分裂子節點
    void splitChild(TreeNode* parent, int i) {
        TreeNode* fullNode = parent->children[i];
        TreeNode* newNode = new TreeNode(fullNode->isLeaf);
        parent->children.insert(parent->children.begin() + i + 1, newNode);
        parent->keys.insert(parent->keys.begin() + i, fullNode->keys[t - 1]);

        newNode->keys.assign(fullNode->keys.begin() + t, fullNode->keys.end());
        fullNode->keys.resize(t - 1);

        if (!fullNode->isLeaf) {
            newNode->children.assign(fullNode->children.begin() + t, fullNode->children.end());
            fullNode->children.resize(t);
        }
    }

    // 從節點中刪除鍵
    void deleteKey(TreeNode* node, T key) {
        int i = 0;
        while (i < node->keys.size() && node->keys[i] < key) {
            i++;
        }

        // 鍵在當前節點中
        if (i < node->keys.size() && node->keys[i] == key) {
            if (node->isLeaf) {
                node->keys.erase(node->keys.begin() + i);
            }
            else {
                TreeNode* leftChild = node->children[i];
                TreeNode* rightChild = node->children[i + 1];
                if (leftChild->keys.size() >= t) {
                    T predecessor = leftChild->keys.back();
                    deleteKey(leftChild, predecessor);
                    node->keys[i] = predecessor;
                }
                else if (rightChild->keys.size() >= t) {
                    T successor = rightChild->keys.front();
                    deleteKey(rightChild, successor);
                    node->keys[i] = successor;
                }
                else {
                    mergeNodes(node, i);
                    deleteKey(leftChild, key);
                }
            }
        }
        else if (!node->isLeaf) {
            deleteKey(node->children[i], key);
        }
    }

    // 合併子節點
    void mergeNodes(TreeNode* parent, int i) {
        TreeNode* leftChild = parent->children[i];
        TreeNode* rightChild = parent->children[i + 1];

        leftChild->keys.push_back(parent->keys[i]);
        leftChild->keys.insert(leftChild->keys.end(), rightChild->keys.begin(), rightChild->keys.end());
        leftChild->children.insert(leftChild->children.end(), rightChild->children.begin(), rightChild->children.end());

        parent->keys.erase(parent->keys.begin() + i);
        parent->children.erase(parent->children.begin() + i + 1);
        delete rightChild;
    }

public:
    Tree(int degree) : t(degree), root(new TreeNode(true)) {}

    // 插入鍵值
    void insert(T key) {
        if (root->keys.size() == 2 * t - 1) {
            TreeNode* newNode = new TreeNode(false);
            newNode->children.push_back(root);
            root = newNode;
            splitChild(root, 0);
        }
        insertNonFull(root, key);
    }

    // 刪除鍵值
    void deleteKey(T key) {
        deleteKey(root, key);
        if (root->keys.empty()) {
            TreeNode* temp = root;
            root = root->children[0];
            delete temp;
        }
    }

    // 顯示樹的結構
    void display() const {
        root->display();
    }

    // 清理記憶體
    ~Tree() {
        delete root;
    }
};

// 主程式
int main() {
    cout << "Enter the degree of the tree: ";
    int degree;
    cin >> degree;

    cout << "Enter the number of elements: ";
    int numElements;
    cin >> numElements;

    vector<int> values(numElements);
    cout << "Enter the values: ";
    for (int& val : values) {
        cin >> val;
    }

    // 針對 M-way 樹
    Tree<int> mTree(degree);
    for (int val : values) {
        mTree.insert(val);
    }

    cout << "\nM-way Tree structure after insertion:\n";
    mTree.display();

    // 插入操作
    cout << "\nEnter the value to insert into M-way Tree: ";
    int key;
    cin >> key;
    mTree.insert(key);
    mTree.display();

    // 刪除操作
    cout << "\nEnter the value to delete from M-way Tree: ";
    cin >> key;
    mTree.deleteKey(key);
    mTree.display();

    // 針對 B-tree
    Tree<int> bTree(degree);
    for (int val : values) {
        bTree.insert(val);
    }

    cout << "\nB-tree structure after insertion:\n";
    bTree.display();

    // 插入操作
    cout << "\nEnter the value to insert into B-tree: ";
    cin >> key;
    bTree.insert(key);
    bTree.display();

    // 刪除操作
    cout << "\nEnter the value to delete from B-tree: ";
    cin >> key;
    bTree.deleteKey(key);
    bTree.display();

    return 0;
}
