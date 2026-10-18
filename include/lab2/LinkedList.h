#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdexcept>

template <typename T>
class LinkedList{
    struct Node{
        T data;
        Node* next;
    };
    Node* head;
    Node* tail;
    int length;
public:
    LinkedList(const T* items, int count) : length(count){
        if ((items == nullptr && count > 0) || count < 0) throw std::invalid_argument("неправильные данные");
        if (count == 0) {
            head = tail = nullptr;
            return;
        }
        head = tail = new Node{items[0], nullptr};
        for (int i = 1; i < count; i++){
            tail->next = new Node{items[i], nullptr};
            tail = tail->next;
        }
    }

    LinkedList() : head(nullptr), tail(nullptr), length(0) {}

    LinkedList(const LinkedList<T>& list){
        if (list.length == 0){
            head = tail = nullptr;
            length = 0;
            return;
        }
        Node* curl = list.head;
        head = tail = new Node{curl->data, nullptr};
        curl = curl->next;
        while (curl != nullptr){
            tail->next = new Node{curl->data, nullptr};
            tail = tail->next;
            curl = curl->next;
        }
        length = list.length;
    }

    ~LinkedList(){
        while (head != nullptr) {
            Node* curl = head->next;
            delete head;
            head = curl;
        }
    }

    LinkedList<T>& operator=(const LinkedList<T>& other){
        if (this == &other) return *this;

        while (head != nullptr){
            Node* next = head->next;
            delete head;
            head = next;
        }

        tail = nullptr;
        length = 0;

        if (other.head == nullptr) return *this;

        Node* curl = other.head;
        head = tail = new Node{curl->data,nullptr};
        curl = curl->next;
        while (curl != nullptr){
            tail->next = new Node{curl->data, nullptr};
            tail = tail->next;
            curl = curl->next;
        }
        length = other.length;

        return *this;
    }
 
    T& operator[](int index) {
        if (index < 0 || index >= this->length) {
            throw std::out_of_range("Индекс вне диапазона");
        }
        Node* current = this->head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->data;
    }

    const T& GetFirst() const {
        if (head == nullptr) throw std::out_of_range("список пуст");
        return head->data;
    }

    const T& GetLast() const {
        if (tail == nullptr) throw std::out_of_range("список пуст");
        return tail->data;
    }

    const T& Get(int index) const{
        if (index < 0 || index >= length) throw std::out_of_range("неправильный индекс");
        Node* curl = head;
        for (int i = 0; i < index; i++){
            curl = curl->next;
        }
        return curl->data;
    }

    int GetLength() const {
        return length;
    }

    LinkedList<T>* GetSubList(int stIndex, int endIndex) const{
        if (stIndex >= length || stIndex < 0 || endIndex >= length || endIndex < 0 || stIndex > endIndex) throw std::out_of_range("неправильный индекс");
        const Node* curl = head;
        for (int i = 0; i < stIndex; i++){
            curl = curl->next;
        }

        LinkedList<T>* result = new LinkedList<T>();

        result->head = result->tail = new Node{curl->data, nullptr};
        curl = curl->next;
        for (int i = stIndex + 1; i <=endIndex; i++){
            result->tail->next = new Node{curl->data, nullptr};
            result->tail = result->tail->next;
            curl = curl->next;
        }
        result->length = endIndex - stIndex + 1;
        return result;
        
    }

    void Append(const T& item ){
        Node* newNode = new Node{item, nullptr};
        if (head == nullptr) {
            head = tail = newNode;
        }else {
            tail->next = newNode;
            tail = tail->next;
        }
        length++;
    }

    void Prepend(const T& item ) {
        Node* newNode = new Node{item, head};
        head = newNode;
        if (tail == nullptr){
            tail = newNode;
        }
        length++;
    }

    void InsertAt(const T& item , int index){
        if (index < 0 || index >= length) throw std::out_of_range("неправильный индекс");
        if (index == 0) {
            Prepend(item);
            return;
        }

        Node* curl = head;
        for (int i = 0; i < index - 1; i++){
            curl = curl->next;
        }
        Node* newNode = new Node{item, curl->next};
        curl->next = newNode;

        length++;
    }    

    LinkedList<T>* Concat(LinkedList<T>* list) const{
        if (list == nullptr) throw std::invalid_argument("переданный указатель равен nullptr");
        
        LinkedList<T>* result = new LinkedList<T>(*this);

        const Node* curr = list->head;
        while (curr != nullptr) {
            result->Append(curr->data);
            curr = curr->next;
        }

        return result;
    }
        
};







#endif 