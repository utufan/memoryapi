//
//  LinkedList.h
//  cs307_hw4_utufan
//
//  Created by Tufan Usta on 26.11.2019.
//  Copyright © 2019 Tufan Usta. All rights reserved.
//

#ifndef LinkedList_h
#define LinkedList_h

class Node
{
public:
    Node* next;
    int ID;
    int size;
    int index;
};

using namespace std;

class LinkedList
{
public:
    int length;
    Node* head;

    LinkedList();
    ~LinkedList();
    void add(int ID, int size , int index);
    void print();
    bool enoughspace(int size);
};

LinkedList::LinkedList(){
    this->length = 0;
    this->head = NULL;
}

LinkedList::~LinkedList(){
    std::cout << "LIST DELETED";
}

void LinkedList::add(int ID, int size, int index){
    Node* node = new Node();
    node->ID = ID;
    node->size = size;
    node->index = index;
    node->next = this->head;
    this->head = node;
    this->length++;
}

void LinkedList::print(){
    Node* head = this->head;
    int i = 1;
    while(head){
        std::cout << i << ": " << head->ID << std::endl;
        head = head->next;
        i++;
    }
}

bool LinkedList::enoughspace(int temp)
{
    Node * ptr = head;
    while(ptr != NULL)
    {
        if((ptr->ID) == -1)
        {
            if(ptr->size >= temp)
            {
                return true;
            }
        }
        ptr = ptr -> next;
    }
    return false;
}

#endif /* LinkedList_h */
