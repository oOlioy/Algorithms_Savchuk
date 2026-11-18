#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <random>
#include <algorithm>
#include <cmath>

using namespace std;

struct Record {
    int id, key, x, y;
};

// 1. B-TREE (t = 2)
struct BNode {
    bool leaf;
    vector<Record> keys;
    vector<BNode*> child;
    BNode(bool l) : leaf(l) {}
};

class BTree {
    BNode* root = nullptr;
    const int t = 2; // Мінімальний степінь

    void splitChild(BNode* parent, int i) {
        BNode* y = parent->child[i];
        BNode* z = new BNode(y->leaf);

        z->keys.push_back(y->keys[2]); // Останній (t-1 = 1) ключ іде в z
        if (!y->leaf) {
            z->child = {y->child[2], y->child[3]}; // Останні t = 2 нащадки
            y->child.resize(2);
        }

        parent->child.insert(parent->child.begin() + i + 1, z);
        parent->keys.insert(parent->keys.begin() + i, y->keys[1]); // Середній ключ вгору
        y->keys.resize(1);
    }

    void insertNonFull(BNode* node, Record r) {
        int i = (int)node->keys.size() - 1;
        if (node->leaf) {
            node->keys.push_back(r);
            while (i >= 0 && node->keys[i].key > r.key) {
                node->keys[i + 1] = node->keys[i];
                i--;
            }
            node->keys[i + 1] = r;
        } else {
            while (i >= 0 && node->keys[i].key > r.key) i--;
            i++;
            if (node->child[i]->keys.size() == 3) { // 2t - 1 = 3
                splitChild(node, i);
                if (node->keys[i].key < r.key) i++;
            }
            insertNonFull(node->child[i], r);
        }
    }

public:
    void insert(Record r) {
        if (!root) {
            root = new BNode(true);
            root->keys.push_back(r);
            return;
        }
        if (root->keys.size() == 3) {
            BNode* s = new BNode(false);
            s->child.push_back(root);
            splitChild(s, 0);
            int i = (s->keys[0].key < r.key) ? 1 : 0;
            insertNonFull(s->child[i], r);
            root = s;
        } else {
            insertNonFull(root, r);
        }
    }

    void print() {
        cout << "\n--- B-TREE (t = 2) ---\n";
        queue<BNode*> q;
        q.push(root);
        for (int lvl = 0; !q.empty(); lvl++) {
            cout << "Рівень " << lvl << ": ";
            for (int sz = q.size(); sz--; ) {
                BNode* cur = q.front(); q.pop();
                cout << "[";
                for (size_t j = 0; j < cur->keys.size(); j++)
                    cout << cur->keys[j].key << "(#" << cur->keys[j].id << ")" << (j + 1 < cur->keys.size() ? "|" : "");
                cout << "] ";
                for (auto c : cur->child) q.push(c);
            }
            cout << "\n";
        }
    }

    // Завдання А: найменший ключ >= K
    bool taskA(int K, Record& res) {
        BNode* cur = root;
        bool ok = false;
        while (cur) {
            int i = 0;
            while (i < (int)cur->keys.size() && cur->keys[i].key < K) i++;
            if (i < (int)cur->keys.size() && (!ok || cur->keys[i].key < res.key)) {
                res = cur->keys[i];
                ok = true;
            }
            cur = cur->leaf ? nullptr : cur->child[i];
        }
        return ok;
    }
};

// 2. R-TREE (M = 3, m = 1)
struct Rect {
    int x1, y1, x2, y2;
    long long area() const { return 1LL * (x2 - x1) * (y2 - y1); }
    Rect merge(Rect b) const { return {min(x1, b.x1), min(y1, b.y1), max(x2, b.x2), max(y2, b.y2)}; }
    bool overlap(Rect b) const { return !(x2 < b.x1 || x1 > b.x2 || y2 < b.y1 || y1 > b.y2); }
};

struct RNode {
    bool leaf;
    vector<Rect> mbr;
    vector<Record> recs;   // Для листків
    vector<RNode*> child;  // Для внутрішніх вузлів
    RNode(bool l) : leaf(l) {}

    Rect totalMBR() const {
        Rect r = mbr[0];
        for (size_t i = 1; i < mbr.size(); i++) r = r.merge(mbr[i]);
        return r;
    }
};

class RTree {
    RNode* root = new RNode(true);
    const int M = 3; // Максимум 3 записи, мінімум 1

    // Спрощене розщеплення (Linear Split) для 4 елементів
    RNode* split(RNode* node) {
        int n = node->mbr.size(); // Тут завжди 4 елементи
        int s1 = 0, s2 = 1;
        long long maxDist = -1;

        // 1. Знаходимо два найдальші елементи
        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++) {
                long long d = abs(node->mbr[i].x1 - node->mbr[j].x1) + abs(node->mbr[i].y1 - node->mbr[j].y1);
                if (d > maxDist) { maxDist = d; s1 = i; s2 = j; }
            }

        auto oldMBR = node->mbr;
        auto oldRecs = node->recs;
        auto oldChild = node->child;

        RNode* brother = new RNode(node->leaf);
        node->mbr.clear(); node->recs.clear(); node->child.clear();

        auto addItem = [&](RNode* target, int idx) {
            target->mbr.push_back(oldMBR[idx]);
            if (target->leaf) target->recs.push_back(oldRecs[idx]);
            else target->child.push_back(oldChild[idx]);
        };

        addItem(node, s1);
        addItem(brother, s2);
        Rect r1 = oldMBR[s1], r2 = oldMBR[s2];

        // 2. Решту 2 елементи кидаємо туди, де менший приріст площі
        for (int i = 0; i < n; i++) {
            if (i == s1 || i == s2) continue;
            long long inc1 = r1.merge(oldMBR[i]).area() - r1.area();
            long long inc2 = r2.merge(oldMBR[i]).area() - r2.area();
            if (inc1 <= inc2) { addItem(node, i); r1 = r1.merge(oldMBR[i]); }
            else              { addItem(brother, i); r2 = r2.merge(oldMBR[i]); }
        }
        return brother;
    }

    RNode* insertRec(RNode* cur, Record r, Rect box) {
        if (cur->leaf) {
            cur->mbr.push_back(box);
            cur->recs.push_back(r);
            return (int)cur->mbr.size() > M ? split(cur) : nullptr;
        }
        // Вибираємо піддерево з мінімальним розширенням площі
        int best = 0;
        long long minInc = 1e18;
        for (size_t i = 0; i < cur->mbr.size(); i++) {
            long long inc = cur->mbr[i].merge(box).area() - cur->mbr[i].area();
            if (inc < minInc) { minInc = inc; best = i; }
        }

        RNode* newChild = insertRec(cur->child[best], r, box);
        cur->mbr[best] = cur->child[best]->totalMBR();

        if (newChild) {
            cur->mbr.push_back(newChild->totalMBR());
            cur->child.push_back(newChild);
            return (int)cur->mbr.size() > M ? split(cur) : nullptr;
        }
        return nullptr;
    }

    void searchBox(RNode* cur, Rect win, vector<Record>& out) {
        for (size_t i = 0; i < cur->mbr.size(); i++) {
            if (cur->mbr[i].overlap(win)) {
                if (cur->leaf) out.push_back(cur->recs[i]);
                else searchBox(cur->child[i], win, out);
            }
        }
    }

public:
    void insert(Record r) {
        Rect box = {r.x, r.y, r.x, r.y};
        RNode* brother = insertRec(root, r, box);
        if (brother) {
            RNode* newRoot = new RNode(false);
            newRoot->mbr = {root->totalMBR(), brother->totalMBR()};
            newRoot->child = {root, brother};
            root = newRoot;
        }
    }

    void print() {
        cout << "\n--- R-TREE (M = 3, m = 1) ---\n";
        queue<RNode*> q;
        q.push(root);
        for (int lvl = 0; !q.empty(); lvl++) {
            cout << "Рівень " << lvl << ": ";
            for (int sz = q.size(); sz--; ) {
                RNode* cur = q.front(); q.pop();
                cout << "{ ";
                for (size_t i = 0; i < cur->mbr.size(); i++) {
                    if (cur->leaf)
                        cout << "#" << cur->recs[i].id << "(" << cur->recs[i].x << "," << cur->recs[i].y << ") ";
                    else {
                        cout << "[" << cur->mbr[i].x1 << ".." << cur->mbr[i].x2 << ","
                             << cur->mbr[i].y1 << ".." << cur->mbr[i].y2 << "] ";
                        q.push(cur->child[i]);
                    }
                }
                cout << "} ";
            }
            cout << "\n";
        }
    }

    // Завдання Б: точки у вікні [x, x+20] x [y, y+20]
    vector<Record> taskB(int x, int y) {
        vector<Record> res;
        searchBox(root, {x, y, x + 20, y + 20}, res);
        return res;
    }

    // Завдання В: точний пошук точки q(x, y)
    bool taskC(int x, int y, Record& out) {
        vector<Record> res;
        searchBox(root, {x, y, x, y}, res); // Вікно 0x0 шукає точну точку
        if (res.empty()) return false;
        out = res[0];
        return true;
    }
};

int main() {
    setlocale(LC_ALL, "");
    const int N = 25;
    vector<Record> data;
    set<pair<int, int>> used;

    cout << "1 - Згенерувати 25 записів | 2 - Ввести вручну: ";
    int mode; cin >> mode;

    mt19937 rng(42);
    for (int i = 1; i <= N; i++) {
        int k, x, y;
        if (mode == 2) {
            cout << "Запис #" << i << " (ключ x y): ";
            cin >> k >> x >> y;
            if (used.count({x, y})) { cout << "Точка вже існує!\n"; i--; continue; }
        } else {
            k = rng() % 100 + 1;
            do { x = rng() % 101; y = rng() % 101; } while (used.count({x, y}));
        }
        used.insert({x, y});
        data.push_back({i, k, x, y});
    }

    cout << "\nЗаписи:\n";
    for (auto& r : data)
        cout << "#" << r.id << ": key=" << r.key << ", (" << r.x << ", " << r.y << ")\n";

    BTree btree;
    RTree rtree;
    for (auto& r : data) {
        btree.insert(r);
        rtree.insert(r);
    }

    btree.print();
    rtree.print();

    // Завдання А
    int K;
    cout << "\n[А] Введіть K: "; cin >> K;
    Record ansA;
    if (btree.taskA(K, ansA))
        cout << "Відповідь А: #" << ansA.id << ", key = " << ansA.key << "\n";
    else
        cout << "Відповідь А: не знайдено\n";

    // Завдання Б і В
    int qx, qy;
    cout << "\n[Б, В] Введіть точку q (x y): "; cin >> qx >> qy;

    auto ansB = rtree.taskB(qx, qy);
    cout << "Відповідь Б (точки у вікні [" << qx << ".." << qx + 20 << "] x [" << qy << ".." << qy + 20 << "]):\n";
    for (auto& r : ansB)
        cout << "  #" << r.id << " (" << r.x << ", " << r.y << "), key = " << r.key << "\n";
    if (ansB.empty()) cout << "  немає\n";

    Record ansC;
    if (rtree.taskC(qx, qy, ansC))
        cout << "Відповідь В: #" << ansC.id << ", key = " << ansC.key << "\n";
    else
        cout << "Відповідь В: не знайдено\n";

    return 0;
}
