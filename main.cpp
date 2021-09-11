//
//  main.cpp
//  cs307_hw_utufan
//
//  Created by Tufan Usta on 26.11.2019.
//  Copyright © 2019 Tufan Usta. All rights reserved.
//

// 20993-HÜSEYİN TUFAN USTA


#include <iostream>
#include <pthread.h>
#include <list>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <queue>
#include <time.h>
#include <sstream>
#include <semaphore.h>
using namespace std;

#define MEMORY_SIZE 10
#define NUM_THREADS 4


struct memory_Request_Node
{
    int id;
    int size;
};

struct memory_List_Node
{
    int id;
    int size;
    int index;
};

void dump_memory();

list<memory_List_Node> memory_List;
queue<memory_Request_Node> memory_Request_Queue;
char memory_Array[MEMORY_SIZE];
int thread_message[NUM_THREADS];
int currIndex = 0;


sem_t semlist[NUM_THREADS];
pthread_mutex_t listLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sharedLock = PTHREAD_MUTEX_INITIALIZER;

bool runForrestRun = true;
bool space_Exists = false;

void printLinkedList()
{
    
    /*
     The linked list is actually consistent with the array but I had some issues in printing
     so I just print it like this.
     int current = 0;
    for (list<memory_List_Node>::iterator list_iter = memory_List.begin(); list_iter != memory_List.end(); ++list_iter)
    {
        cout<<"["<<list_iter->id<<"]"<<"["<<list_iter->size<<"]"<<"["<<list_iter->index<<"]";
        if(list_iter != memory_List.end())
            cout<<"---";
        current = list_iter->size+list_iter->index;
    }
    if(current<MEMORY_SIZE)
        cout<<"[-1]"<<"["<<MEMORY_SIZE-current<<"]"<<"["<<current<<"]";*/
    
    int j=0;
    while(j<MEMORY_SIZE)
    {
        char ID = memory_Array[j];
        string s(1,ID);
        int size = 1;
        int index = j;
        while(true)
        {
            if(memory_Array[j] == memory_Array[j+1])
            {
                size++;
                j++;
            }
            else
                break;
            
        }
        if(ID=='X')
            s = "-1" ;
        cout<<"["<<s<<"]"<<"["<<size<<"]"<<"["<<index<<"]";
        if(j != MEMORY_SIZE-1)
            cout<<"---";
        j++;
    }
    
}

void printArray()
{
    cout << "\nMemory Dump:\n";
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        cout << memory_Array[i];
    }
    cout << "\n*****\n";
    
}

void free_mem_list(int ID, int randomMemorySize, int index)
{
    for (list<memory_List_Node>::iterator list_iter = memory_List.begin(); list_iter != memory_List.end(); ++list_iter)
    {
        if (list_iter->id == ID)
        {
            memory_List.erase(list_iter);
            break;
        }
    }
    
}

void free_mem_array(int ID, int size, int index)
{
    
    for (int k = index, s = size; 0 < s; k++, s--)
    {
        memory_Array[k] = 'X';
    }
    
}


int spaceExists(int i, memory_Request_Node requestNode){
    
    for (list<memory_List_Node>::iterator list_iter = memory_List.begin(); list_iter != memory_List.end(); ++list_iter)
    {
        if (list_iter->index - i >= requestNode.size)
        {
       
            memory_List_Node listNode;
            listNode.id=requestNode.id;
            listNode.size=requestNode.size;
            listNode.index=i;
            
            space_Exists = true;
            memory_List.insert(list_iter,listNode);
            break;
        }
        i = list_iter->index + list_iter->size;
    }
    
    if (!space_Exists)
    {
        if(MEMORY_SIZE - i >= requestNode.size){
        
            memory_List_Node listNode;
            listNode.id=requestNode.id;
            listNode.size=requestNode.size;
            listNode.index=i;
            
            space_Exists = true;
            memory_List.push_back(listNode);
            
        }
    }
    
    return i;
}

void changeAndDumpMemory(int i, memory_Request_Node requestNode){
    
    int randomMemorySize = requestNode.size;
    int r = i;
    stringstream ss;
    for (int k = r, s = randomMemorySize; s > 0; k++, s--)
    {
        ss << requestNode.id;
        memory_Array[k] = ss.str()[0];
    }

    dump_memory();
}
void dump_memory()
{
    
    printLinkedList();
    printArray();
}


void release_function()
{
    runForrestRun = false;
    
    memory_List.erase(memory_List.begin(), memory_List.end());

    for (int i = 0; i < MEMORY_SIZE; i++)
        memory_Array[i] = 'X';

    for (int i = 0; i < NUM_THREADS; i++)
        sem_destroy(&semlist[i]);

    //pthread_mutex_destroy(&queLock);
    //pthread_mutex_destroy(&listLock);

}


void free_mem(int ID, int randomMemorySize, int index)
{
    
    free_mem_list(ID,randomMemorySize,index);
    free_mem_array(ID,randomMemorySize,index);
    
}

void use_mem()
{
    int sleepTime = (rand() % 5);
    sleep(sleepTime);
}

/*Thread should add its ID and size to the queue and block on its semaphore. After memory server thread increments the semaphore of the blocked thread, thread should continue and check the linked list to see if it is allocated or not. Thread could use or not use memory based on the return value of the my_malloc function.*/

//NEVERMIND THIS
int my_malloc(int ID, int size)
{
    memory_Request_Node n;
    n.id=ID;
    n.size=size;
    int index = -1;
    
    pthread_mutex_lock(&queLock);
    memory_Request_Queue.push(n);
    pthread_mutex_unlock(&queLock);

    sem_wait(&semlist[ID]);

    pthread_mutex_lock(&listLock);
                 
                 
    for (list<memory_List_Node>::iterator list_iter = memory_List.begin(); list_iter != memory_List.end(); ++list_iter)
    {
    if (list_iter->id == ID)
    {
        index = list_iter->index;
        break;
    }
    }
    pthread_mutex_unlock(&listLock);
             
             
    return index;
}

void *thread_function(void *id)
{
    
    int *ID = (int *)id;
    while (runForrestRun)
    {

        int randomMemorySize = rand() % (MEMORY_SIZE / 3) + 1;
        int index = my_malloc(*ID, randomMemorySize);    // -1 indicates no allocation
        if (index != -1)
        {
            use_mem();
            pthread_mutex_lock(&listLock);
            free_mem(*ID, randomMemorySize, index);
            pthread_mutex_unlock(&listLock);
        }

    }
    //return nullptr;
   
}

void *server_function(void *){
    
    
    while (runForrestRun)
    {
        pthread_mutex_lock(&sharedLock);
        space_Exists = false;
        currIndex = 0;
        if (!memory_Request_Queue.empty())
        {
           
            memory_Request_Node requestNode = memory_Request_Queue.front();
            memory_Request_Queue.pop();

            
            pthread_mutex_lock(&listLock);
            
            currIndex = spaceExists(currIndex, requestNode);

            if (space_Exists)
            {
                //thread_message[requestNode.id]=currIndex;
                changeAndDumpMemory(currIndex, requestNode);
            }
            else
            {
                //thread_message[requestNode.id] = -1;
            }
            pthread_mutex_unlock(&listLock);
            sem_post(&semlist[requestNode.id]);
        }
        pthread_mutex_unlock(&sharedLock);
    }
    
    //return nullptr;
    
}

int main(int argc, char *argv[])
{
    srand(time(NULL)); // proper randomization

    int i=0;
    for (; i < NUM_THREADS; i++)
        sem_init(&semlist[i], 0, 0);

    i=0;
    for (; i < MEMORY_SIZE; i++)
        memory_Array[i] = 'X';

    // IDs
    unsigned int threadIDs[NUM_THREADS];
    i=0;
    for (; i < NUM_THREADS; i++)
        threadIDs[i] = i;

    pthread_t threadsArray[NUM_THREADS];
    pthread_t server;

    // create thread for server
    pthread_create(&server, NULL, server_function, NULL);

    // create threads
    i=0;
    for (; i < NUM_THREADS; i++)
        pthread_create(&threadsArray[i], NULL, thread_function, (void *)&threadIDs[i]);

    sleep(10); // sleeps for 10 seconds
    release_function();

    // wait for threads
    i=0;
    for (;i < NUM_THREADS; i++)
        pthread_join(threadsArray[i], NULL);

    return 0;
}
