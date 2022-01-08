#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 50000 // Size of the Hash Table

static int currScope = 0;
typedef struct LinkedList LinkedList;
typedef struct Declerations Declerations;
typedef struct Properties Properties;
typedef struct Ht_item Ht_item;
typedef struct Scope Scope;
typedef struct HashTable HashTable;
typedef struct StackNode StackNode;



//maybe add value later... Avi!!!!!
struct Properties{
    char* kind;
    char* type;
};

struct Declerations{
    int scopeNum;
    Properties* p;
    Declerations* next;
};

unsigned long hash_function(char* str) {
    unsigned long i = 0;
    for (int j=0; str[j]; j++)
        i += str[j];
    return i % CAPACITY;
}

// Define the Hash Table Item here
struct Ht_item {
    char* name;
    Declerations* d;
};

Properties* create_properties(char* type, char* kind){
    Properties* item = (Properties*) malloc (sizeof(Properties));
    item->type = (char*) calloc (strlen(type) + 1, sizeof(char));
    strcpy(item->type, type);
    item->kind = (char*) calloc (strlen(kind) + 1, sizeof(char));
    strcpy(item->kind, kind);
    return item;
}

void print_prop(Properties* p){
    printf("Kind:%s Type:%s \n",p->kind,p->type);
}

Declerations * create_dec(int scopeNum,char* type,char* kind){
    Declerations* item = (Declerations*) malloc (sizeof(Declerations));
    item->scopeNum = scopeNum;
    item->p = create_properties(type,kind);
    item->next = NULL;
    return item;
}

void print_dec(Declerations* d){
    Declerations* temp = d;
    while(temp){
        printf("Scope num:%d ",temp->scopeNum);
        print_prop(temp->p);
        temp = temp->next;    
    }
}

Ht_item* create_item(char* name, int scopeNum ,char* type,char* kind) {
    // Creates a pointer to a new hash table item
    Ht_item* item = (Ht_item*) malloc (sizeof(Ht_item));
    item->name = (char*) calloc (strlen(name) + 1, sizeof(char));
    strcpy(item->name, name);
    item->d = create_dec(scopeNum,type,kind);
    return item;
}

Ht_item* add_dec(Ht_item* item,int scopeNum,char* type,char* kind){
    Declerations* temp = item->d;
    while(temp->next){
        temp = temp->next;
    }
    temp->next = create_dec(scopeNum,type,kind);
    return item;
}

void print_item(Ht_item* item){
    printf("Name is:%s\n",item->name);
    print_dec(item->d);
}

void free_properties(Properties* p){
    free(p->kind);
    free(p->type);
    free(p);
}

void free_declerations(Declerations* d){
    Declerations* temp =d;
    while(temp){
        free_properties(temp->p);
        temp = temp->next;
        d->next = NULL;
        free(d);
    }
}

void free_item(Ht_item* item) {
    // Frees an item
    free(item->name);
    free(item->d);
    free(item);
}

// Define the Linkedlist here
struct LinkedList {
    Ht_item* item; 
    LinkedList* next;
};

// Define the Hash Table here
struct HashTable {
    // Contains an array of pointers
    // to items
    Ht_item** items;
    LinkedList** overflow_buckets;
    int size;
    int count;
};

static LinkedList* allocate_list () {
    // Allocates memory for a Linkedlist pointer
    LinkedList* list = (LinkedList*) calloc (1, sizeof(LinkedList));
    return list;
}

static LinkedList* linkedlist_insert(LinkedList* list, Ht_item* item) {
    // Inserts the item onto the Linked List
    if (!list) {
        LinkedList* head = allocate_list();
        head->item = item;
        head->next = NULL;
        list = head;
        return list;
    } 
    
    else if (list->next == NULL) {
        LinkedList* node = allocate_list();
        node->item = item;
        node->next = NULL;
        list->next = node;
        return list;
    }

    LinkedList* temp = list;
    while (temp->next) {
        temp = temp->next;
    }
    
    LinkedList* node = allocate_list();
    node->item = item;
    node->next = NULL;
    temp->next = node;
    
    return list;
}

static Ht_item* linkedlist_remove(LinkedList* list) {
    // Removes the head from the linked list
    // and returns the item of the popped element
    if (!list)
        return NULL;
    if (!list->next)
        return NULL;
    LinkedList* node = list->next;
    LinkedList* temp = list;
    temp->next = NULL;
    list = node;
    Ht_item* it = NULL;
    memcpy(temp->item, it, sizeof(Ht_item));
    free_item(temp->item);
    free(temp);
    return it;
}

static void free_linkedlist(LinkedList* list) {
    LinkedList* temp = list;
    if (!list)
        return;
    while (list) {
        temp = list;
        list = list->next;
        free_item(temp->item);
        free(temp);
    }
}

static LinkedList** create_overflow_buckets(HashTable* table) {
    // Create the overflow buckets; an array of linkedlists
    LinkedList** buckets = (LinkedList**) calloc (table->size, sizeof(LinkedList*));
    for (int i=0; i<table->size; i++)
        buckets[i] = NULL;
    return buckets;
}

static void free_overflow_buckets(HashTable* table) {
    // Free all the overflow bucket lists
    LinkedList** buckets = table->overflow_buckets;
    for (int i=0; i<table->size; i++)
        free_linkedlist(buckets[i]);
    free(buckets);
}

HashTable* create_table(int size) {
    // Creates a new HashTable
    HashTable* table = (HashTable*) malloc (sizeof(HashTable));
    table->size = size;
    table->count = 0;
    table->items = (Ht_item**) calloc (table->size, sizeof(Ht_item*));
    for (int i=0; i<table->size; i++)
        table->items[i] = NULL;
    table->overflow_buckets = create_overflow_buckets(table);
    return table;
}

void free_hashtable(HashTable* table) {
    // Frees the table
    for (int i=0; i<table->size; i++) {
        Ht_item* item = table->items[i];
        if (item != NULL)
            free_item(item);
    }

    free_overflow_buckets(table);
    free(table->items);
    free(table);
}

void handle_collision(HashTable* table, unsigned long index, Ht_item* item) {
    LinkedList* head = table->overflow_buckets[index];

    if (head == NULL) {
        // We need to create the list
        head = allocate_list();
        head->item = item;
        table->overflow_buckets[index] = head;
        return;
    }
    else {
        // Insert to the list
        table->overflow_buckets[index] = linkedlist_insert(head, item);
        return;
    }
 }

void ht_insert(HashTable* table,Ht_item* item) {

    // Compute the index
    int index = hash_function(item->name);

    Ht_item* current_item = table->items[index];
    if (current_item == NULL) {
        // Key does not exist.
        if (table->count == table->size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            // Remove the create item
            free_item(item);
            return;
        }
        
        // Insert directly
        table->items[index] = item; 
        table->count++;
    }

    else {
            // Scenario 1: We only need to update value
            if (strcmp(current_item->name, item->name) == 0) {
                Declerations* prv_dec = current_item->d;
                Declerations* curr_dec = prv_dec;
                while(curr_dec){
                    if (curr_dec->scopeNum == item->d->scopeNum){
                        printf("id Same scope ERROR");
                        exit(1);
                    }
                    prv_dec = curr_dec;
                    curr_dec = prv_dec->next;
                }
                prv_dec->next = item->d;
                item = current_item;
                return;
            }
    
        else {
            // Scenario 2: Collision
            handle_collision(table, index, item);
            return;
        }
    }
}

Ht_item* ht_search(HashTable* table, char* name) {
    // Searches the key in the hashtable
    // and returns NULL if it doesn't exist
    int index = hash_function(name);
    Ht_item* item = table->items[index];
    LinkedList* head = table->overflow_buckets[index];

    // Ensure that we move to items which are not NULL
    while (item != NULL) {
        if (strcmp(item->name, name) == 0)
            return item;
        if (head == NULL)
            return NULL;
        item = head->item;
        head = head->next;
    }
    return NULL;
}
//dont actually need, keep for later

void ht_delete(HashTable* table, char* name) {
    // Deletes an item from the table
    int index = hash_function(name);
    Ht_item* item = table->items[index];
    LinkedList* head = table->overflow_buckets[index];

    if (item == NULL) {
        // Does not exist. Return
        return;
    }
    else {
        if (head == NULL && strcmp(item->name, name) == 0) {
            // No collision chain. Remove the item
            table->items[index] = NULL;
            free_item(item);
            table->count--;
            return;
        }
        else if (head != NULL) {
            // Collision Chain exists
            if (strcmp(item->name, name) == 0) {
                // Remove this item and set the head of the list
                // as the new item
                
                free_item(item);
                LinkedList* node = head;
                head = head->next;
                node->next = NULL;
                table->items[index] = create_item(node->item->name, node->item->d->scopeNum,node->item->d->p->type,node->item->d->p->kind);
                free_linkedlist(node);
                table->overflow_buckets[index] = head;
                return;
            }

            LinkedList* curr = head;
            LinkedList* prev = NULL;
            
            while (curr) {
                if (strcmp(curr->item->name, name) == 0) {
                    if (prev == NULL) {
                        // First element of the chain. Remove the chain
                        free_linkedlist(head);
                        table->overflow_buckets[index] = NULL;
                        return;
                    }
                    else {
                        // This is somewhere in the chain
                        prev->next = curr->next;
                        curr->next = NULL;
                        free_linkedlist(curr);
                        table->overflow_buckets[index] = head;
                        return;
                    }
                }
                curr = curr->next;
                prev = curr;
            }

        }
    }

}

//improve print search function
void print_search(HashTable* table, char* name) {
    Ht_item* val = ht_search(table, name);
    if (val == NULL) {
        printf("%s does not exist\n", name);
        return;
    }
    else {
        printf("Key:%s\n",val->name );
    }
}

//improve print hashtable the part of dec need print dec
void print_hashtable(HashTable* table) {
    printf("\n-------------------\n");
    for (int i=0; i<table->size; i++) {
        if (table->items[i]) {
            print_item(table->items[i]);
            if (table->overflow_buckets[i]) {
                printf(" => Overflow Bucket => ");
                LinkedList* head = table->overflow_buckets[i];
                while (head) {
                    print_item(table->items[i]);
                    head = head->next;
                }
            }
            printf("\n");
        }
    }
    printf("-------------------\n");
}

struct Scope{
    Ht_item* item;
    Scope* next;
};

static Scope* allocate_scope () {
    // Allocates memory for a Scope pointer
    Scope* scope = (Scope*) calloc (1, sizeof(Scope));
    return scope;
}

void scope_insert(Scope** scope, Ht_item* item) {
    if( item == NULL){
        printf("why you sent null item not good\n");
    }
    // Inserts the item onto the Scope
    if (!(*scope) || !(*scope)->item) {
        Scope* head = allocate_scope();
        head->item = item;
        head->next = NULL;
        *scope = head;
        return;
    } 
    
    else if ((*scope)->next == NULL) {
        if(strcmp((*scope)->item->name,item->name) != 0){
            Scope* node = allocate_scope();
            node->item = item;
            node->next = NULL;
            (*scope)->next = node;
            return;
        }else
        printf("Already exist in scope!!");
    }

    Scope* temp = *scope;
    while (temp->next) {
        temp = temp->next;
    }
    
    Scope* node = allocate_scope();
    node->item = item;
    node->next = NULL;
    temp->next = node;
    return ;
}

static Ht_item* scope_remove(Scope* scope,char* name) {
    Scope* prev = scope;
    Scope* curr = scope->next;
    if (prev == NULL) {
        printf("scope is empty dude\n");
        return NULL;
    }
    else if (strcmp(prev->item->name,name)){
        scope = NULL;
        return prev->item;
    }
    while (curr) {
        if (strcmp(curr->item->name, name) == 0) {
            // This is somewhere in the chain
            prev->next = curr->next;
            curr->next = NULL;
            return curr->item;
        }
        curr = curr->next;
        prev = curr;
    }
    printf("why you sent me something that is not in the scope?!\n");
    return NULL;
}

void free_scope(Scope* scope){
    Scope* temp = scope;
    while(temp){
        //delete_dec
        temp->item = NULL;
        temp= temp->next;
        scope->next= NULL;
        free(scope);
    }
}

void print_scope(Scope* scope,int ScopeNum){
    Scope* temp = scope;
    while(temp){
        printf("Name is:%s\n",temp->item->name);
        Declerations* temp_dec = temp->item->d;
        while(temp_dec){
            if(temp_dec->scopeNum == ScopeNum){
                printf("Kind:%s Type:%s\n",temp_dec->p->kind,temp_dec->p->type);
            }
            temp_dec = temp_dec->next;
        }
        temp = temp->next;
    }
}

struct StackNode {
    int scopeNum;
    Scope* scope;
    struct StackNode* next;
};
 
struct StackNode* newNode(Scope* scope)
{
    struct StackNode* stackNode = (struct StackNode*)malloc(sizeof(struct StackNode));
    stackNode->scopeNum = currScope;
    stackNode->scope = scope;
    stackNode->next = NULL;
    return stackNode;
}
 
int isEmpty(struct StackNode* root)
{
    return !root;
}
 
void push(struct StackNode** root, Scope* scope)
{
    if(*root == NULL){
        *root = newNode(scope);
        (*root)->next = NULL;
        printf("stack num %d pushed to stack\n",currScope);
    }else{
        currScope++;
        struct StackNode* stackNode = newNode(scope);
        stackNode->next = *root;
        *root = stackNode;
        printf("stack num %d pushed to stack\n",currScope);
        
    }
}
 
Scope* pop(struct StackNode** root)
{
    if (isEmpty(*root))
        return NULL;
    struct StackNode* temp = *root;
    *root = (*root)->next;
    Scope* popped = temp->scope;
    temp->scope = NULL;
    free(temp);
    printf("stack %d popped\n",currScope);
    currScope--;
    return popped;
}
 
Scope** peek(struct StackNode* root)
{
    if (isEmpty(root))
        return NULL;
    return &(root->scope);
}
void print_stack(StackNode* stack){
    StackNode* temp = stack;
    printf("\n");
    while(temp){
        printf("Scope num:%d\n",temp->scopeNum);
        print_scope(temp->scope,temp->scopeNum);
        temp = temp->next;
        printf("\n");
    }
}

void free_stack(StackNode** stack){
    while(isEmpty(*stack))
        free_scope(pop(stack));
    free(*stack);
}

void addItem(HashTable* hash,StackNode* stack,char* id,int scopeNum,char* type, char* kind){
    Ht_item* temp = create_item(id,scopeNum,type,kind);
    ht_insert(hash,temp);
    Scope** curr_scope = peek(stack);
    scope_insert(curr_scope,ht_search(hash,temp->name));
}

void removeScope(HashTable* hash,StackNode** stack){
    Scope* scope = pop(stack);
    Scope* temp_scope = scope;
    while(temp_scope){
        Ht_item* temp = temp_scope->item;
        if(temp->d->next == NULL)
            ht_delete(hash,temp->name);
        else{
            Declerations* curr_dec = temp->d;
            Declerations* prev_dec = curr_dec;
            while(curr_dec){
                if(curr_dec->scopeNum == currScope +1){
                    prev_dec->next = NULL;
                    free_properties(curr_dec->p);
                    free(curr_dec);
                    break;
                }
                prev_dec = curr_dec;
                curr_dec = prev_dec->next;
            }
        }
        temp_scope = temp_scope->next;
    }
    free_scope(scope);
    free(scope);
}


void free_stack_and_hashtable(StackNode** stack,HashTable* hash){
    free_hashtable(hash);
    free_stack(stack);
}



int main(){
    HashTable* hash = create_table(CAPACITY);
    StackNode* stack = newNode(NULL);
    addItem(hash,stack,"x",currScope,"var","int");
    push(&stack,NULL);
    addItem(hash,stack,"x",currScope,"var","string");
    addItem(hash,stack,"hamishim",currScope,"var","real");

    print_hashtable(hash);
    removeScope(hash,&stack);
    print_stack(stack);
    print_hashtable(hash);
    return 0;
}