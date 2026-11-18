#include <iostream>

// ==========================================
// 1. SPLAY TREE
// ==========================================
struct SplayNode {
    int key;
    SplayNode* left;
    SplayNode* right;
    SplayNode(int k) : key(k), left(nullptr), right(nullptr) {}
};

SplayNode* rightRotate(SplayNode* x) {
    SplayNode* y = x->left;
    x->left = y->right;
    y->right = x;
    return y;
}

SplayNode* leftRotate(SplayNode* x) {
    SplayNode* y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

SplayNode* splay(SplayNode* root, int key) {
    if (!root || root->key == key) return root;

    if (root->key > key) {
        if (!root->left) return root;
        if (root->left->key > key) {
            root->left->left = splay(root->left->left, key);
            root = rightRotate(root);
        } else if (root->left->key < key) {
            root->left->right = splay(root->left->right, key);
            if (root->left->right) root->left = leftRotate(root->left);
        }
        return (root->left == nullptr) ? root : rightRotate(root);
    } else {
        if (!root->right) return root;
        if (root->right->key > key) {
            root->right->left = splay(root->right->left, key);
            if (root->right->left) root->right = rightRotate(root->right);
        } else if (root->right->key < key) {
            root->right->right = splay(root->right->right, key);
            root = leftRotate(root);
        }
        return (root->right == nullptr) ? root : leftRotate(root);
    }
}

SplayNode* insertSplay(SplayNode* root, int key) {
    if (!root) return new SplayNode(key);
    root = splay(root, key);
    if (root->key == key) return root;

    SplayNode* newNode = new SplayNode(key);
    if (root->key > key) {
        newNode->right = root;
        newNode->left = root->left;
        root->left = nullptr;
    } else {
        newNode->left = root;
        newNode->right = root->right;
        root->right = nullptr;
    }
    return newNode;
}

SplayNode* deleteSplay(SplayNode* root, int key) {
    if (!root) return nullptr;
    root = splay(root, key);
    if (key != root->key) return root;

    SplayNode* temp;
    if (!root->left) {
        temp = root;
        root = root->right;
    } else {
        temp = root;
        root = splay(root->left, key);
        root->right = temp->right;
    }
    delete temp;
    return root;
}

void clearSplay(SplayNode* root) {
    if (!root) return;
    clearSplay(root->left);
    clearSplay(root->right);
    delete root;
}

// ==========================================
// 2. LEFT-LEANING RED-BLACK TREE (LLRB)
// ==========================================
enum Color { RED, BLACK };

struct LLRBNode {
    int key;
    Color color;
    LLRBNode* left;
    LLRBNode* right;
    LLRBNode(int k) : key(k), color(RED), left(nullptr), right(nullptr) {}
};

bool isRed(LLRBNode* node) {
    if (!node) return false;
    return node->color == RED;
}

LLRBNode* rotateLeftLLRB(LLRBNode* h) {
    LLRBNode* x = h->right;
    h->right = x->left;
    x->left = h;
    x->color = h->color;
    h->color = RED;
    return x;
}

LLRBNode* rotateRightLLRB(LLRBNode* h) {
    LLRBNode* x = h->left;
    h->left = x->right;
    x->right = h;
    x->color = h->color;
    h->color = RED;
    return x;
}

void flipColors(LLRBNode* h) {
    h->color = (h->color == RED) ? BLACK : RED;
    if (h->left) h->left->color = (h->left->color == RED) ? BLACK : RED;
    if (h->right) h->right->color = (h->right->color == RED) ? BLACK : RED;
}

LLRBNode* insertLLRB(LLRBNode* h, int key) {
    if (!h) return new LLRBNode(key);

    if (key < h->key) h->left = insertLLRB(h->left, key);
    else if (key > h->key) h->right = insertLLRB(h->right, key);
    else return h;

    if (isRed(h->right) && !isRed(h->left)) h = rotateLeftLLRB(h);
    if (isRed(h->left) && isRed(h->left->left)) h = rotateRightLLRB(h);
    if (isRed(h->left) && isRed(h->right)) flipColors(h);

    return h;
}

LLRBNode* moveRedLeft(LLRBNode* h) {
    flipColors(h);
    if (h->right && isRed(h->right->left)) {
        h->right = rotateRightLLRB(h->right);
        h = rotateLeftLLRB(h);
        flipColors(h);
    }
    return h;
}

LLRBNode* moveRedRight(LLRBNode* h) {
    flipColors(h);
    if (h->left && isRed(h->left->left)) {
        h = rotateRightLLRB(h);
        flipColors(h);
    }
    return h;
}

LLRBNode* balanceLLRB(LLRBNode* h) {
    if (isRed(h->right)) h = rotateLeftLLRB(h);
    if (isRed(h->left) && isRed(h->left->left)) h = rotateRightLLRB(h);
    if (isRed(h->left) && isRed(h->right)) flipColors(h);
    return h;
}

LLRBNode* minLLRB(LLRBNode* h) {
    if (!h->left) return h;
    return minLLRB(h->left);
}

LLRBNode* deleteMinLLRB(LLRBNode* h) {
    if (!h->left) {
        delete h;
        return nullptr;
    }
    if (!isRed(h->left) && !isRed(h->left->left)) h = moveRedLeft(h);
    h->left = deleteMinLLRB(h->left);
    return balanceLLRB(h);
}

LLRBNode* deleteLLRB(LLRBNode* h, int key) {
    if (!h) return nullptr;

    if (key < h->key) {
        if (h->left && !isRed(h->left) && !isRed(h->left->left)) h = moveRedLeft(h);
        h->left = deleteLLRB(h->left, key);
    } else {
        if (isRed(h->left)) h = rotateRightLLRB(h);
        if (key == h->key && !h->right) {
            delete h;
            return nullptr;
        }
        if (h->right && !isRed(h->right) && !isRed(h->right->left)) h = moveRedRight(h);
        if (key == h->key) {
            LLRBNode* x = minLLRB(h->right);
            h->key = x->key;
            h->right = deleteMinLLRB(h->right);
        } else {
            h->right = deleteLLRB(h->right, key);
        }
    }
    return balanceLLRB(h);
}

void clearLLRB(LLRBNode* root) {
    if (!root) return;
    clearLLRB(root->left);
    clearLLRB(root->right);
    delete root;
}

// ==========================================
// Допоміжні функції
// ==========================================
template <typename T>
int getHeight(T* root) {
    if (!root) return 0;
    int lh = getHeight(root->left);
    int rh = getHeight(root->right);
    return 1 + (lh > rh ? lh : rh);
}

template <typename T>
T* searchNode(T* root, int key) {
    if (!root || root->key == key) return root;
    if (key < root->key) return searchNode(root->left, key);
    return searchNode(root->right, key);
}

LLRBNode* copyLLRB(LLRBNode* root) {
    if (!root) return nullptr;
    LLRBNode* newRoot = new LLRBNode(root->key);
    newRoot->color = root->color;
    newRoot->left = copyLLRB(root->left);
    newRoot->right = copyLLRB(root->right);
    return newRoot;
}

// Перевантаження для друку кольору без вживання std::is_same_v / constexpr if
inline void printColorColumnHeader(SplayNode*) {}
inline void printColorColumnHeader(LLRBNode*) { std::cout << "\tколір"; }

inline void printColorValue(SplayNode*) {}
inline void printColorValue(LLRBNode* node) {
    std::cout << "\t" << (node->color == RED ? "червоний" : "чорний");
}

template <typename T>
void printBinaryTreeTable(T* root) {
    int h = getHeight(root);
    if (h == 0) return;

    T* queue[1000];
    int level[1000];
    int parentIdx[1000];
    char side[1000];

    int head = 0, tail = 0;

    queue[tail] = root;
    level[tail] = 1;
    parentIdx[tail] = 0;
    side[tail] = '-';
    tail++;

    while (head < tail) {
        T* curr = queue[head];
        int currLvl = level[head];

        if (curr->left) {
            queue[tail] = curr->left;
            level[tail] = currLvl + 1;
            parentIdx[tail] = head + 1;
            side[tail] = 'L';
            tail++;
        }
        if (curr->right) {
            queue[tail] = curr->right;
            level[tail] = currLvl + 1;
            parentIdx[tail] = head + 1;
            side[tail] = 'R';
            tail++;
        }
        head++;
    }

    std::cout << "вузол\tрівень\tбатько\tбік\tключ";
    printColorColumnHeader((T*)nullptr);
    std::cout << "\n";

    for (int i = 0; i < tail; i++) {
        std::cout << (i + 1) << "\t" << level[i] << "\t";
        if (parentIdx[i] == 0) std::cout << "-\t-";
        else std::cout << parentIdx[i] << "\t" << (side[i] == 'L' ? "лівий" : "правий");

        std::cout << "\t" << queue[i]->key;
        printColorValue(queue[i]);
        std::cout << "\n";
    }
    std::cout << "висота: " << h << "\n";
}

// ==========================================
// 2-3 TREE ТА ЙОГО ДРУК
// ==========================================
struct Node23 {
    int keys[2];
    int numKeys;
    Node23* children[3];
    Node23() : numKeys(0) {
        children[0] = children[1] = children[2] = nullptr;
    }
};

Node23* convertLLRBTo23(LLRBNode* root) {
    if (!root) return nullptr;

    Node23* node23 = new Node23();

    if (isRed(root->left)) {
        node23->numKeys = 2;
        node23->keys[0] = root->left->key;
        node23->keys[1] = root->key;

        node23->children[0] = convertLLRBTo23(root->left->left);
        node23->children[1] = convertLLRBTo23(root->left->right);
        node23->children[2] = convertLLRBTo23(root->right);
    } else {
        node23->numKeys = 1;
        node23->keys[0] = root->key;

        node23->children[0] = convertLLRBTo23(root->left);
        node23->children[1] = convertLLRBTo23(root->right);
    }

    return node23;
}

int getHeight23(Node23* root) {
    if (!root) return 0;
    return 1 + getHeight23(root->children[0]);
}

void print23TreeTable(Node23* root) {
    int h = getHeight23(root);
    if (h == 0) return;

    Node23* queue[1000];
    int level[1000];
    int parentIdx[1000];
    int childNum[1000];

    int head = 0, tail = 0;

    queue[tail] = root;
    level[tail] = 1;
    parentIdx[tail] = 0;
    childNum[tail] = 0;
    tail++;

    while (head < tail) {
        Node23* curr = queue[head];
        int currLvl = level[head];

        for (int i = 0; i <= curr->numKeys; i++) {
            if (curr->children[i]) {
                queue[tail] = curr->children[i];
                level[tail] = currLvl + 1;
                parentIdx[tail] = head + 1;
                childNum[tail] = i + 1;
                tail++;
            }
        }
        head++;
    }

    std::cout << "вузол\tрівень\tбатько\tщадок\tключі\n";
    for (int i = 0; i < tail; i++) {
        std::cout << (i + 1) << "\t" << level[i] << "\t";
        if (parentIdx[i] == 0) std::cout << "-\t-";
        else std::cout << parentIdx[i] << "\t" << childNum[i];

        std::cout << "\t" << queue[i]->keys[0];
        if (queue[i]->numKeys == 2) std::cout << " " << queue[i]->keys[1];
        std::cout << "\n";
    }
    std::cout << "висота: " << h << "\n";
}

void clear23Tree(Node23* root) {
    if (!root) return;
    for (int i = 0; i <= root->numKeys; i++) clear23Tree(root->children[i]);
    delete root;
}

// ==========================================
// ЛОГІКА ЗАВДАНЬ A, Б, В, Г
// ==========================================
template <typename T>
void inOrderGreater(T* root, int K, bool& found) {
    if (!root) return;
    inOrderGreater(root->left, K, found);
    if (root->key > K) {
        std::cout << root->key << " ";
        found = true;
    }
    inOrderGreater(root->right, K, found);
}

SplayNode* replaceKeySplay(SplayNode* root, int K1, int K2) {
    if (!searchNode(root, K1)) return root;
    root = deleteSplay(root, K1);
    root = insertSplay(root, K2);
    return root;
}

LLRBNode* replaceKeyLLRB(LLRBNode* root, int K1, int K2) {
    if (!searchNode(root, K1)) return root;
    root = deleteLLRB(root, K1);
    root = insertLLRB(root, K2);
    if (root) root->color = BLACK;
    return root;
}

template <typename T>
void findNth(T* root, int N, int& counter, int& result) {
    if (!root || counter >= N) return;
    findNth(root->left, N, counter, result);
    counter++;
    if (counter == N) {
        result = root->key;
        return;
    }
    findNth(root->right, N, counter, result);
}


// MAIN
int main() {
    int n1, n2;
    int k1_arr[500], k2_arr[500];

    std::cout << "Введіть кількість елементів T1 та самі ключі:\n";
    if (!(std::cin >> n1)) return 0;
    for (int i = 0; i < n1; i++) std::cin >> k1_arr[i];

    std::cout << "\nВведіть кількість елементів T2 та самі ключі:\n";
    if (!(std::cin >> n2)) return 0;
    for (int i = 0; i < n2; i++) std::cin >> k2_arr[i];

    int K, K1, K2, M, N, a, b;
    std::cout << "\nВведіть параметри K, K1, K2, M, N, a, b:\n";
    std::cin >> K >> K1 >> K2 >> M >> N >> a >> b;

    // Побудова дерев
    SplayNode* splayT1 = nullptr;
    LLRBNode* llrbT1 = nullptr;
    for (int i = 0; i < n1; i++) {
        splayT1 = insertSplay(splayT1, k1_arr[i]);
        llrbT1 = insertLLRB(llrbT1, k1_arr[i]);
        if (llrbT1) llrbT1->color = BLACK;
    }

    SplayNode* splayT2 = nullptr;
    LLRBNode* llrbT2 = nullptr;
    for (int i = 0; i < n2; i++) {
        splayT2 = insertSplay(splayT2, k2_arr[i]);
        llrbT2 = insertLLRB(llrbT2, k2_arr[i]);
        if (llrbT2) llrbT2->color = BLACK;
    }

    // --- ЗАВДАННЯ A ---
    std::cout << "\n--- ЗАВДАННЯ A ---\n";
    
    std::cout << "Splay Tree (T1) ключі > " << K << ": ";
    bool found1 = false;
    inOrderGreater(splayT1, K, found1);
    if (!found1) std::cout << "немає";
    std::cout << "\n\nSplay tree T1:\n";
    printBinaryTreeTable(splayT1);
    std::cout << "\nSplay tree T2:\n";
    printBinaryTreeTable(splayT2);

    std::cout << "\nLLRB Tree (T1) ключі > " << K << ": ";
    bool found2 = false;
    inOrderGreater(llrbT1, K, found2);
    if (!found2) std::cout << "немає";
    std::cout << "\n\nLLRB tree T1:\n";
    printBinaryTreeTable(llrbT1);
    std::cout << "\nLLRB tree T2:\n";
    printBinaryTreeTable(llrbT2);

    // --- ЗАВДАННЯ Б ---
    std::cout << "\n--- ЗАВДАННЯ Б ---\n";
    splayT1 = replaceKeySplay(splayT1, K1, K2);
    llrbT1 = replaceKeyLLRB(llrbT1, K1, K2);

    std::cout << "Splay tree T1 після заміни " << K1 << " -> " << K2 << ":\n";
    printBinaryTreeTable(splayT1);
    std::cout << "\nSplay tree T2:\n";
    printBinaryTreeTable(splayT2);

    std::cout << "\nLLRB tree T1 після заміни " << K1 << " -> " << K2 << ":\n";
    printBinaryTreeTable(llrbT1);
    std::cout << "\nLLRB tree T2:\n";
    printBinaryTreeTable(llrbT2);

    // --- ЗАВДАННЯ В ---
    std::cout << "\n--- ЗАВДАННЯ В ---\n";
    int counter1 = 0, res1 = -1;
    findNth(splayT1, N, counter1, res1);
    std::cout << "Splay Tree T1: " << N << "-й ключ = ";
    if (counter1 < N) std::cout << "немає"; else std::cout << res1;
    std::cout << "\n";

    int counter2 = 0, res2 = -1;
    findNth(llrbT1, N, counter2, res2);
    std::cout << "LLRB Tree T1: " << N << "-й ключ = ";
    if (counter2 < N) std::cout << "немає"; else std::cout << res2;
    std::cout << "\n";

    LLRBNode* llrbT1_copy = copyLLRB(llrbT1);

    std::cout << "\nSplay tree T1:\n";
    printBinaryTreeTable(splayT1);
    std::cout << "\nSplay tree T2:\n";
    printBinaryTreeTable(splayT2);
    std::cout << "\nLLRB tree T1:\n";
    printBinaryTreeTable(llrbT1);
    std::cout << "\nLLRB tree T1 (копія):\n";
    printBinaryTreeTable(llrbT1_copy);
    std::cout << "\nLLRB tree T2:\n";
    printBinaryTreeTable(llrbT2);

    // --- ЗАВДАННЯ Г ---
    std::cout << "\n--- ЗАВДАННЯ Г ---\n";
    std::cout << "Перетворення LLRB Tree (T2) на 2-3 Tree:\n\n";
    
    std::cout << "Вихідне LLRB Tree T2 до перетворення:\n";
    printBinaryTreeTable(llrbT2);

    Node23* tree23 = convertLLRBTo23(llrbT2);

    std::cout << "\nОтримане 2-3 tree T2 після перетворення:\n";
    print23TreeTable(tree23);

    clearSplay(splayT1);
    clearSplay(splayT2);
    clearLLRB(llrbT1);
    clearLLRB(llrbT1_copy);
    clearLLRB(llrbT2);
    clear23Tree(tree23);

    return 0;
}

// Умови для завдання 
// 1. 10 21 12 8 13 29 1 4 9 7 25
// 2. 10 29 25 7 22 2 16 23 17 20 15
// 3. 5 12 99 0 3 0 0
