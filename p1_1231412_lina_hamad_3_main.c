// lina hamad    1231412   sec:3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node node;
typedef struct node* Stack;

struct node {
    int ID;
    char name[100];
    char date[15];
    double duration;
    struct node* next;
};

typedef struct {
    node* head;       // unperformed tasks list
    node* top;        // performed stack top (for undo)
    node* performed;  // performed tasks list
    node* undoneTop;  // undone stack top
} System;

/* ---------- Helpers for safe input ---------- */
static void readLine(char* buf, size_t sz) {
    if (!fgets(buf, (int)sz, stdin)) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = '\0'; // remove trailing newline
}

/* ---------- Memory cleanup ---------- */
void freeList(node** head) {
    node* curr = *head;
    while (curr != NULL) {
        node* next = curr->next;
        free(curr);
        curr = next;
    }
    *head = NULL;
}

void freeStack(node** top) {
    node* curr = *top;
    while (curr != NULL) {
        node* next = curr->next;
        free(curr);
        curr = next;
    }
    *top = NULL;
}

/* ---------- Core functions ---------- */
node* createNode(int ID, const char name[100], const char date[15], double duration) {
    node* newNode = (node*)malloc(sizeof(node));
    if (newNode == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    newNode->ID = ID;
    strcpy(newNode->name, name);
    strcpy(newNode->date, date);
    newNode->duration = duration;
    newNode->next = NULL;
    return newNode;
}

void push(node* x, System* sys, Stack s) {
    if (x == NULL || sys == NULL) {
        printf("ERROR 404\n");
        return;
    }

    node* newNode = createNode(x->ID, x->name, x->date, x->duration);
    if (newNode == NULL) {
        printf("out of space\n");
        return;
    }

    if (s == sys->top) {
        newNode->next = sys->top;
        sys->top = newNode;
    } else if (s == sys->undoneTop) {
        newNode->next = sys->undoneTop;
        sys->undoneTop = newNode;
    }
}

/* NOTE: old pop implementation assumed dummy head; we will NOT use it in cleanup anymore.
   Keeping it for compatibility with your current logic (used in UndoLastPerformedTask) */
void pop(Stack s) {
    if (s == NULL || s->next == NULL) {
        printf("Stack is empty\n");
        return;
    }
    node* temp = s->next;
    s->next = s->next->next;
    free(temp);
}

void LoadTasksFile(System* sys) {
    FILE* in = fopen("tasks.txt", "r");
    if (in == NULL) {
        printf("There is no such file\n");
        return;
    }

    char sent[300];
    while (fgets(sent, sizeof(sent), in)) {
        char* token;
        char* tmp = sent;

        token = strtok(tmp, "#");
        if (!token) continue;
        int ID = atoi(token);

        token = strtok(NULL, "#");
        if (!token) continue;
        char name[100] = {0};
        strncpy(name, token, sizeof(name) - 1);

        token = strtok(NULL, "#");
        if (!token) continue;
        char date[15] = {0};
        strncpy(date, token, sizeof(date) - 1);

        token = strtok(NULL, "#");
        if (!token) continue;
        double duration = atof(token);

        node* newNode = createNode(ID, name, date, duration);
        if (newNode == NULL) {
            printf("Memory allocation failed\n");
            fclose(in);
            return;
        }

        if (sys->head == NULL) {
            sys->head = newNode;
        } else {
            node* p = sys->head;
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = newNode;
        }
    }

    fclose(in);
    printf("Tasks loaded successfully :)\n");
}

node* findPrev(int x, System* sys) {
    if (sys == NULL || sys->head == NULL) {
        return NULL;
    }
    if (sys->head->ID == x) {
        return NULL;
    }

    node* p = sys->head;
    while (p->next != NULL && p->next->ID != x) {
        p = p->next;
    }
    if (p->next == NULL) {
        return NULL;
    }
    return p;
}

void DeleteTask(int x, System* sys) {
    if (sys == NULL || sys->head == NULL) {
        printf("List is empty\n");
        return;
    }
    if (sys->head->ID == x) {
        node* temp = sys->head;
        sys->head = sys->head->next;
        free(temp);
        return;
    }

    node* p = findPrev(x, sys);
    if (p == NULL || p->next == NULL) {
        printf("Node with ID %d not found\n", x);
        return;
    }

    node* temp = p->next;
    p->next = temp->next;
    free(temp);
}

node* SearchById(int x, System* sys) {
    if (sys == NULL || sys->head == NULL) {
        return NULL;
    }
    node* p = sys->head;
    while (p != NULL && p->ID != x) {
        p = p->next;
    }
    return p;
}

node* SearchByName(const char c[100], System* sys) {
    if (sys == NULL || sys->head == NULL) {
        return NULL;
    }
    node* p = sys->head;
    while (p != NULL && strcmp(p->name, c) != 0) {
        p = p->next;
    }
    return p;
}

void AddNewTask(node* x, System* sys) {
    if (x == NULL || sys == NULL) {
        printf("Invalid input\n");
        return;
    }
    x->next = NULL;

    node* p = SearchById(x->ID, sys);
    if (p != NULL) {
        printf("Error: Task with ID %d already exists in the list please enter a new task with another ID\n", x->ID);
        free(x);
        return;
    }

    if (sys->head == NULL) {
        sys->head = x;
    } else {
        node* p2 = sys->head;
        while (p2->next != NULL) {
            p2 = p2->next;
        }
        p2->next = x;
    }
    printf("Task added successfully!\n");
}

void ViewUnperformedTasks(System* sys) {
    if (sys == NULL || sys->head == NULL) {
        printf("No Unperformed Tasks.\n");
        return;
    }
    node* p = sys->head;
    while (p != NULL) {
        printf("ID: %-4d  Name: %-20s  Date: %-10s  Duration: %5.1f hours\n",
               p->ID, p->name, p->date, p->duration);
        p = p->next;
    }
}

void PerformTask(int x, System* sys) {
    node* p = SearchById(x, sys);
    if (p == NULL) {
        printf("there is no such task\n");
        return;
    }

    node* copy = createNode(p->ID, p->name, p->date, p->duration);
    if (copy == NULL) {
        printf("memory allocation failed.\n");
        return;
    }

    // push onto performed stack (top)
    push(copy, sys, sys->top);

    // add to performed list
    if (sys->performed == NULL) {
        sys->performed = copy;
        copy->next = NULL;
    } else {
        node* p2 = sys->performed;
        while (p2->next != NULL) {
            p2 = p2->next;
        }
        p2->next = copy;
    }

    // remove from unperformed list
    DeleteTask(x, sys);

    printf("task %d performed successfully\n", x);
}

void UndoLastPerformedTask(System* sys) {
    if (sys == NULL || sys->top == NULL) {
        printf("No performed tasks to undo\n");
        return;
    }

    node* p = sys->top;

    node* copy = createNode(p->ID, p->name, p->date, p->duration);
    if (copy == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    // add back to unperformed list
    if (sys->head == NULL) {
        sys->head = copy;
    } else {
        AddNewTask(copy, sys);
    }

    // remove from performed list
    node* prev = NULL;
    node* current = sys->performed;
    while (current != NULL && current->ID != p->ID) {
        prev = current;
        current = current->next;
    }
    if (current != NULL) {
        if (prev == NULL) {
            sys->performed = current->next;
        } else {
            prev->next = current->next;
        }
        free(current);
    }

    // push onto undone stack
    push(copy, sys, sys->undoneTop);

    // pop from performed stack
    pop(sys->top);

    printf("Undo successful: Task %d undone\n", copy->ID);
}

void ViewPerformedTasks(System* sys) {
    if (sys == NULL || sys->performed == NULL) {
        printf("No performed tasks.\n");
        return;
    }

    printf("Performed Tasks:\n");
    node* current = sys->performed;
    while (current != NULL) {
        printf("ID: %-4d  Name: %-20s  Date: %-10s  Duration: %5.1f hours\n",
               current->ID, current->name, current->date, current->duration);
        current = current->next;
    }
}

void GenerateSummaryReport(System* sys) {
    FILE* out = fopen("Report.txt", "w");
    if (out == NULL) {
        printf("Error 404: Could not create report file\n");
        return;
    }

    fprintf(out, "TASK MANAGEMENT SYSTEM REPORT\n\n");

    fprintf(out, "UNPERFORMED TASKS:\n");
    node* current = sys->head;
    int unperformedCount = 0;
    while (current != NULL) {
        fprintf(out, "ID: %-4d  Name: %-20s  Date: %-10s  Duration: %5.1f hours\n",
                current->ID, current->name, current->date, current->duration);
        current = current->next;
        unperformedCount++;
    }
    if (unperformedCount == 0) {
        fprintf(out, "There is no unperformed tasks\n\n");
    }
    fprintf(out, "\n");

    fprintf(out, "UNDONE TASKS:\n");
    if (sys->undoneTop == NULL) {
        fprintf(out, "There are no undone tasks.\n\n");
    } else {
        node* u = sys->undoneTop; // IMPORTANT: do NOT free or modify sys->undoneTop here
        while (u != NULL) {
            fprintf(out, "ID: %-4d  Name: %-20s  Date: %-10s  Duration: %5.1f hours\n",
                    u->ID, u->name, u->date, u->duration);
            u = u->next;
        }
        fprintf(out, "\n");
    }

    fprintf(out, "PERFORMED TASKS:\n");
    int performedCount = 0;
    node* performedCurrent = sys->performed;
    while (performedCurrent != NULL) {
        fprintf(out, "ID: %-4d  Name: %-20s  Date: %-10s  Duration: %5.1f hours\n",
                performedCurrent->ID, performedCurrent->name, performedCurrent->date, performedCurrent->duration);
        performedCurrent = performedCurrent->next;
        performedCount++;
    }
    if (performedCount == 0) {
        fprintf(out, "There are no performed tasks\n\n");
    }

    fclose(out);
    printf("Generated successfully\n");
}

int main() {
    System sys = {NULL, NULL, NULL, NULL};

    int choice;
    char searchName[100];
    int searchID;
    node* foundTask = NULL;

    printf("\nTASK MANAGEMENT SYSTEM :)  \n\n");

    do {
        printf("\n1. Load Tasks File\n");
        printf("2. Add a New Task\n");
        printf("3. Delete a Task\n");
        printf("4. Search for a Task\n");
        printf("5. View Unperformed Tasks\n");
        printf("6. Perform a Task\n");
        printf("7. Undo Last Performed Task\n");
        printf("8. View Performed Tasks\n");
        printf("9. Generate Summary Report\n");
        printf("10. Exit\n");
        printf("Enter your choice (1-10): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            // clear input buffer
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            continue;
        }
        getchar(); // consume newline

        switch (choice) {
            case 1:
                LoadTasksFile(&sys);
                break;

            case 2: {
                int newID;
                char newName[100];
                char newDate[15];
                double newDuration;

                printf("Enter Task ID: ");
                scanf("%d", &newID);
                getchar();

                printf("Enter Task Name: ");
                readLine(newName, sizeof(newName));

                printf("Enter Task Date: ");
                readLine(newDate, sizeof(newDate));

                printf("Enter Task Duration: ");
                scanf("%lf", &newDuration);
                getchar();

                node* newNode = createNode(newID, newName, newDate, newDuration);
                if (newNode != NULL) {
                    AddNewTask(newNode, &sys);
                }
                break;
            }

            case 3:
                printf("Enter Task ID to delete: ");
                scanf("%d", &searchID);
                getchar();
                DeleteTask(searchID, &sys);
                break;

            case 4: {
                printf("Search by:\n1. ID\n2. Name\nChoice: ");
                int searchType;
                scanf("%d", &searchType);
                getchar();

                if (searchType == 1) {
                    printf("Enter Task ID: ");
                    scanf("%d", &searchID);
                    getchar();
                    foundTask = SearchById(searchID, &sys);
                } else if (searchType == 2) {
                    printf("Enter Task Name: ");
                    readLine(searchName, sizeof(searchName));
                    foundTask = SearchByName(searchName, &sys);
                } else {
                    printf("Invalid search type.\n");
                    foundTask = NULL;
                }

                if (foundTask != NULL) {
                    printf("\nTask Found:\n");
                    printf("ID: %d\nName: %s\nDate: %s\nDuration: %.1f hours\n",
                           foundTask->ID, foundTask->name, foundTask->date, foundTask->duration);
                } else {
                    printf("Task not found.\n");
                }
                break;
            }

            case 5:
                ViewUnperformedTasks(&sys);
                break;

            case 6:
                printf("Enter Task ID to perform: ");
                scanf("%d", &searchID);
                getchar();
                PerformTask(searchID, &sys);
                break;

            case 7:
                UndoLastPerformedTask(&sys);
                break;

            case 8:
                ViewPerformedTasks(&sys);
                break;

            case 9:
                GenerateSummaryReport(&sys);
                break;

            case 10:
                printf("Exiting the program. Goodbye!\n");
                break;

            default:
                printf("Invalid choice. Please enter a number between 1-10.\n");
        }

    } while (choice != 10);

    freeList(&sys.head);
    freeList(&sys.performed);
    freeStack(&sys.top);
    freeStack(&sys.undoneTop);

    return 0;
}