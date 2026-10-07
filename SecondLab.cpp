#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* next;

    Node(int val) : data(val), next(nullptr) {}
};

struct SinglyLinkedList {
    Node* head;
    Node* tail;

    SinglyLinkedList() : head(nullptr), tail(nullptr) {}

    ~SinglyLinkedList() {
        clear();
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    
    void pushHead(int val) {
        Node* newNode = new Node(val);
        if (isEmpty()) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head = newNode;
        }
    }

    
    void pushTail(int val) {
        Node* newNode = new Node(val);
        if (isEmpty()) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    
    int getLength() const { 
        int length = 0;
        Node* curr = head;
        while (curr != nullptr) {
            length++;
            curr = curr->next;
        }
        return length;
    }

    void print() const {
        if (isEmpty()) {
            cout << "[Порожній список]";
        } else {
            Node* curr = head;
            while (curr != nullptr) {
                cout << curr->data;
                if (curr->next != nullptr) {
                    cout << " -> ";
                }
                curr = curr->next;
            }
        }
        cout << " (Довжина: " << getLength() << ")\n";
    }

    void clear() {
        Node* curr = head;
        while (curr != nullptr) {
            Node* nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }
        head = tail = nullptr;
    }

    // Завдання А
    int taskA_removeNegatives() {
        int pointerHops = 0;

        while (head != nullptr && head->data < 0) {
            Node* temp = head;
            head = head->next;
            pointerHops++;
            delete temp;
        }

        if (head == nullptr) {
            tail = nullptr;
            return pointerHops;
        }

        Node* curr = head;
        while (curr->next != nullptr) {
            pointerHops++; 
            if (curr->next->data < 0) {
                Node* temp = curr->next;
                curr->next = temp->next;
                pointerHops++; 
                if (temp == tail) {
                    tail = curr;
                }
                delete temp;
            } else {
                curr = curr->next;
                pointerHops++; 
            }
        }
        return pointerHops;
    }

    // Завдання Б 
    int taskB_moveFirstToTail() {
        int pointerHops = 0;

        if (head == nullptr || head == tail) {
            return pointerHops;
        }

        Node* oldHead = head;
        head = head->next;
        pointerHops++;

        tail->next = oldHead;
        pointerHops++;

        oldHead->next = nullptr;
        tail = oldHead;

        return pointerHops;
    }

    // Завдання В
    int taskC_findKthFromEnd(int k, int& resultValue, bool& found) {
        int pointerHops = 0;
        found = false;

        if (k <= 0 || isEmpty()) {
            return pointerHops;
        }

        Node* fast = head;
        Node* slow = head;

        for (int i = 0; i < k - 1; ++i) {
            if (fast->next == nullptr) {
                return pointerHops;
            }
            fast = fast->next;
            pointerHops++;
        }

        while (fast->next != nullptr) {
            fast = fast->next;
            pointerHops++;
            slow = slow->next;
            pointerHops++;
        }

        resultValue = slow->data;
        found = true;
        return pointerHops;
    }
};

int main() {
    SinglyLinkedList list;

    int initialData[] = { 12, -7, 45, -3, 89, -15, 23, 6 };
    int initialSize = sizeof(initialData) / sizeof(initialData[0]);

    for (int i = 0; i < initialSize; ++i) {
        list.pushTail(initialData[i]);
    }

    cout << "ПОЧАТКОВИЙ СТАН СПИСКУ\n";
    cout << "Список: ";
    list.print();
    cout << "-----------------------------------------------------------------\n\n";

    int hopsA = list.taskA_removeNegatives();
    cout << "[Завдання А] Видалення від'ємних елементів:\n";
    cout << "Вміст списку: ";
    list.print();
    cout << "Кількість переходів за покажчиком: " << hopsA << "\n\n";

    int hopsB = list.taskB_moveFirstToTail();
    cout << "[Завдання Б] Перенесення першого елемента в tail:\n";
    cout << "Вміст списку: ";
    list.print();
    cout << "Кількість переходів за покажчиком: " << hopsB << "\n\n";

    int k = 3;
    int kthValue = 0;
    bool found = false;
    int hopsC = list.taskC_findKthFromEnd(k, kthValue, found);
    cout << "[Завдання В] Пошук " << k << "-го елемента з кінця:\n";
    if (found) {
        cout << "Знайдений елемент: " << kthValue << "\n";
    } else {
        cout << "Помилка: елемент не знайдено (k перевищує довжину списку).\n";
    }
    cout << "Вміст списку: ";
    list.print();
    cout << "Кількість переходів за покажчиком: " << hopsC << "\n\n";

    cout << "\nТАБЛИЦЯ ПЕРЕХОДІВ ЗА ПОКАЖЧИКОМ\n";
    cout << "--------------------------------------------------------------------\n";
    cout << "  Завдання      Опис операції                            Переходів  \n";
    cout << "--------------------------------------------------------------------\n";
    cout << "  Завдання А    Видалення від'ємних елементів            " << hopsA;
    if (hopsA < 10) cout << "          \n";
    else if (hopsA < 100) cout << "         \n";
    else cout << "        \n";

    cout << "  Завдання Б    Перенесення першого елемента в tail      " << hopsB;
    if (hopsB < 10) cout << "          \n";
    else if (hopsB < 100) cout << "         \n";
    else cout << "       |\n";

    cout << "  Завдання В    Пошук k-го елемента з кінця              " << hopsC;
    if (hopsC < 10) cout << "          \n";
    else if (hopsC < 100) cout << "         \n";
    else cout << "        \n";
    return 0;
}
