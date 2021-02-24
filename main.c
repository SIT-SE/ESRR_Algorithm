/* Program: ESRR (Efficient Shortest Remaining Time Round Robin)
 * About: ESRR consolidate two of the preemptive version of existing scheduling algorithms namely RR and SRTF.
 * ESRR reduces total waiting time and turn around time in compare to RR and reduces the waiting time of
 * the shorter process and it also provides a longer process to execute faster than SRTF.
 * Author: May Madi Aung
 */
#include<stdio.h>
#include<stdlib.h>
/*-------------------------------------------Linked Lists-------------------------------------------------------------*/
/* process using doubly linked list */
struct process {
    int pid;
    int arrival_time;
    int burst_time;
    int completion_time;
    struct process *next; // Pointer to next node in DLL
    struct process *prev; // Pointer to previous node in DLL
} *process;
struct CPU {
    int pid;
    int CPU_time;
    struct CPU *next; // Pointer to next node in DLL
    struct CPU *prev; // Pointer to previous node in DLL
} *CPU;
struct ready_queue {
    int pid;
    int OG_burst_time;
    int remaining_burst_time;
    int arrival_time;
    int remove_flag;
    struct ready_queue *next; // Pointer to next node in DLL
} *ready_queue;

struct Wait_Time {
    int pid;
    int wait_time;
    struct Wait_Time *next; // Pointer to next node in DLL
} *Wait_Time;

struct TA_Time {
    int pid;
    int ta_time;
    struct TA_Time *next; // Pointer to next node in DLL
} *TA_Time;

/*--------------------------------------Methods to deal with list-----------------------------------------------------*/

/* Given a reference (pointer to pointer) to the head of a
   list and a key, deletes the first occurrence of key in
   linked list */
void deleteNode(struct ready_queue **head_ref, int key) {
    // Store head node
    struct ready_queue *temp = *head_ref, *prev;

    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->remove_flag == key) {
        *head_ref = temp->next; // Changed head
        free(temp); // free old head
        return;
    }

    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->remove_flag != key) {
        prev = temp;
        temp = temp->next;
    }

    // If key was not present in linked list
    if (temp == NULL)
        return;

    // Unlink the node from linked list
    prev->next = temp->next;

    free(temp); // Free memory
}

void createNodeList(int n) {
    struct process *fnNode, *tmp;
    int arr_time, exe_time, i;
    process = (struct process *) malloc(sizeof(struct process));

    if (process == NULL) //check whether the fnnode is NULL and if so no memory allocation
    {
        printf(" Memory can not be allocated.");
    } else {
// reads data for the node through keyboard

        printf("\nEnter Arrival Time for process 1: ");
        scanf("%d", &arr_time);
        printf("\nEnter Execution Time for process 1: ");
        scanf("%d", &exe_time);
        process->pid = 1;
        process->burst_time = exe_time;
        process->arrival_time = arr_time;
        process->next = NULL; // links the address field to NULL
        tmp = process;
// Creating n nodes and adding to linked list
        for (i = 2; i <= n; i++) {
            fnNode = (struct process *) malloc(sizeof(struct process));
            if (fnNode == NULL) {
                printf(" Memory can not be allocated.");
                break;
            } else {
                printf("\nEnter Arrival Time for process %d: ", i);
                scanf(" %d", &arr_time);
                printf("\nEnter Execution Time for process %d: ", i);
                scanf(" %d", &exe_time);
                fnNode->pid = i;
                fnNode->burst_time = exe_time;
                fnNode->arrival_time = arr_time;      // links the num field of fnNode with num
                fnNode->next = NULL; // links the address field of fnNode with NULL

                tmp->next = fnNode; // links previous node i.e. tmp to the fnNode
                tmp = tmp->next;
            }
        }
    }
}


void displayRQ() {
    struct ready_queue *tmp;
    if (ready_queue == NULL) {
        printf(" List is empty.");
    } else {
        tmp = ready_queue;
        while (tmp != NULL) {
            printf(" Process %d\n", tmp->pid);
            printf(" Arrival Time = %d\n", tmp->remaining_burst_time);       // prints the data of current node
            tmp = tmp->next;                     // advances the position of current node
        }
    }
}
void displayWT() {
    struct Wait_Time *tmp;
    if (Wait_Time == NULL) {
        printf(" List is empty.");
    } else {
        tmp = Wait_Time;
        while (tmp != NULL) {
            printf("\nProcess %d Wait Time = %d",tmp->pid, tmp->wait_time);       // prints the data of current node
            tmp = tmp->next;                     // advances the position of current node
        }
    }
}
void displayTA() {
    struct TA_Time *tmp;
    if (TA_Time == NULL) {
        printf(" List is empty.");
    } else {
        tmp = TA_Time;
        while (tmp != NULL) {
            printf("\nProcess %d Turn Around Time = %d",tmp->pid, tmp->ta_time);       // prints the data of current node
            tmp = tmp->next;                     // advances the position of current node
        }
    }
}
void insert_endTA(struct TA_Time **head, int pid, int ta_time) {
    struct TA_Time *new_node = NULL;
    struct TA_Time *last = NULL;

    /*Create the new node*/
    new_node = (struct TA_Time *) malloc(sizeof(struct TA_Time));

    if (new_node == NULL) {
        printf("Failed to insert element. Out of memory");
        return;
    }

    new_node->pid = pid;
    new_node->ta_time = ta_time;
    new_node->next = NULL;

    /*No element in the linked list. So point head to the new node*/
    if (*head == NULL) {
        *head = new_node;
        return;
    }

    /*Traverse to the last node*/
    last = *head;
    while (last->next) last = last->next;

    /*Point last node's link (next pointer) to the new node*/
    last->next = new_node;
}

void insert_endWT(struct Wait_Time **head, int pid, int wait_time) {
    struct Wait_Time *new_node = NULL;
    struct Wait_Time *last = NULL;

    /*Create the new node*/
    new_node = (struct Wait_Time *) malloc(sizeof(struct Wait_Time));

    if (new_node == NULL) {
        printf("Failed to insert element. Out of memory");
        return;
    }

    new_node->pid = pid;
    new_node->wait_time = wait_time;
    new_node->next = NULL;

    /*No element in the linked list. So point head to the new node*/
    if (*head == NULL) {
        *head = new_node;
        return;
    }

    /*Traverse to the last node*/
    last = *head;
    while (last->next) last = last->next;

    /*Point last node's link (next pointer) to the new node*/
    last->next = new_node;
}


void insert_endCPU(struct CPU **head, int pid, int CPU_time) {
    struct CPU *new_node = NULL;
    struct CPU *last = NULL;

    /*Create the new node*/
    new_node = (struct CPU *) malloc(sizeof(struct CPU));

    if (new_node == NULL) {
        printf("Failed to insert element. Out of memory");
        return;
    }

    new_node->pid = pid;
    new_node->CPU_time = CPU_time;
    new_node->next = NULL;

    /*No element in the linked list. So point head to the new node*/
    if (*head == NULL) {
        *head = new_node;
        return;
    }

    /*Traverse to the last node*/
    last = *head;
    while (last->next) last = last->next;

    /*Point last node's link (next pointer) to the new node*/
    last->next = new_node;
}

void insert_endRQ(struct ready_queue **head, int pid, int remaining_burst_time, int arrival_time, int OG) {
    struct ready_queue *new_node = NULL;
    struct ready_queue *last = NULL;

    /*Create the new node*/
    new_node = (struct ready_queue *) malloc(sizeof(struct ready_queue));

    if (new_node == NULL) {
        printf("Failed to insert element. Out of memory");
        return;
    }

    new_node->pid = pid;
    new_node->remaining_burst_time = remaining_burst_time;
    new_node->arrival_time = arrival_time;
    new_node->OG_burst_time = OG;
    new_node->next = NULL;

    /*No element in the linked list. So point head to the new node*/
    if (*head == NULL) {
        *head = new_node;
        return;
    }

    /*Traverse to the last node*/
    last = *head;
    while (last->next) last = last->next;

    /*Point last node's link (next pointer) to the new node*/
    last->next = new_node;
}



//sortReadyQueue() will sort the given list in ascending order
void sortReadyQueue() {
    struct ready_queue *current = NULL, *index = NULL;
    int temp, temp2, temp3, temp4;
    //Check whether list is empty
    if (ready_queue == NULL) {
        return;
    } else {
        //Current will point to head
        for (current = ready_queue; current->next != NULL; current = current->next) {
            //Index will point to node next to current
            for (index = current->next; index != NULL; index = index->next) {
                //If current's data is greater than index's data, swap the data of current and index
                if (current->remaining_burst_time > index->remaining_burst_time) {
                    temp = current->remaining_burst_time;
                    temp2 = current->pid;
                    temp3 = current->arrival_time;
                    temp4 = current->OG_burst_time;
                    current->remaining_burst_time = index->remaining_burst_time;
                    current->pid = index->pid;
                    current->arrival_time = index->arrival_time;
                    current->OG_burst_time = index->OG_burst_time;
                    index->remaining_burst_time = temp;
                    index->pid = temp2;
                    index->arrival_time = temp3;
                    index->OG_burst_time = temp4;
                }
            }
        }
    }
}

//sortReadyQueue() will sort the given list in ascending order
void sortWT() {
    struct Wait_Time *current = NULL, *index = NULL;
    int temp, temp2;
    //Check whether list is empty
    if (Wait_Time == NULL) {
        return;
    } else {
        //Current will point to head
        for (current = Wait_Time; current->next != NULL; current = current->next) {
            //Index will point to node next to current
            for (index = current->next; index != NULL; index = index->next) {
                //If current's data is greater than index's data, swap the data of current and index
                if (current->pid > index->pid) {
                    temp = current->wait_time;
                    temp2 = current->pid;
                    current->wait_time = index->wait_time;
                    current->pid = index->pid;
                    index->wait_time= temp;
                    index->pid = temp2;
                }
            }
        }
    }
}

//sortReadyQueue() will sort the given list in ascending order
void sortTA() {
    struct TA_Time *current = NULL, *index = NULL;
    int temp, temp2;
    //Check whether list is empty
    if (TA_Time == NULL) {
        return;
    } else {
        //Current will point to head
        for (current = TA_Time; current->next != NULL; current = current->next) {
            //Index will point to node next to current
            for (index = current->next; index != NULL; index = index->next) {
                //If current's data is greater than index's data, swap the data of current and index
                if (current->pid > index->pid) {
                    temp = current->ta_time;
                    temp2 = current->pid;
                    current->ta_time = index->ta_time;
                    current->pid = index->pid;
                    index->ta_time= temp;
                    index->pid = temp2;
                }
            }
        }
    }
}

float sumTA() {
    struct TA_Time *tmp;
    float sum=0;
    if (TA_Time == NULL) {
        printf(" List is empty.");
    } else {
        tmp = TA_Time;
        while (tmp != NULL) {
            sum += tmp->ta_time;     // prints the data of current node
            tmp = tmp->next;                     // advances the position of current node
        }
        return sum;
    }
}
float sumWT() {
    struct Wait_Time *tmp;
    float sum=0;
    if (Wait_Time == NULL) {
        printf(" List is empty.");
    } else {
        tmp = Wait_Time;
        while (tmp != NULL) {
            sum += tmp->wait_time;     // prints the data of current node
            tmp = tmp->next;                     // advances the position of current node
        }
        return sum;
    }
}

/*---------------------------------------------------MAIN-------------------------------------------------------------*/
void main() {
    int n, tq;
    printf("\n\n Efficient Shortest Remaining Time Round Robin\n");
    printf("---------------------------------------------------\n");

    printf("\nEnter the no of processes : ");
    scanf("%d", &n);

    printf("\nEnter the time quantum : ");
    scanf("%d", &tq);


    createNodeList(n);
    int current_time = 0;
    int completed = 0;
    int OG = 0;


    struct ready_queue *next_process;
    //int Flag_ready_queue [100];
    int index = 0;
    //new process arrives
    //while (completed != n) {
    //new process arrives
    struct process *tmp;
    //no process
    if (process == NULL) {
        printf(" List is empty.");
    } else {
        tmp = process;
        //New process arrives
        while (tmp != NULL) {
            //if execution time less than time slice
            if (tmp->burst_time <= tq) {
                //no process exists in ready queue
                if (ready_queue == NULL) {
                    //No process in  CPU
                    if (CPU == NULL) {

                        //Allocate CPU to process
                        insert_endCPU(&CPU, tmp->pid, tmp->burst_time);
                        current_time += tmp->burst_time; //Update current time
                        tmp->burst_time = 0; //Process execute finish
                        tmp->completion_time = current_time;
                        completed += 1; //Process execute finish
                    }//No process in  CPU end
                    else {
                        OG = tmp->burst_time;
                        //Insert process into the ready queue
                        insert_endRQ(&ready_queue, tmp->pid, tmp->burst_time, tmp->arrival_time, OG);
                    }
                }//No process in RQ END
                else {
                    OG = tmp->burst_time;
                    //Insert process into the ready queue
                    insert_endRQ(&ready_queue, tmp->pid, tmp->burst_time, tmp->arrival_time, OG);
                }
            }//if execution time less than time slice END
                //if execution time more than time slice
            else if (tmp->burst_time > tq) {
                //no process exists in ready queue
                if (ready_queue == NULL) {
                    //No process in CPU
                    if (CPU == NULL) {
                        OG = tmp->burst_time;
                        //Allocate CPU to process up to quantum
                        insert_endCPU(&CPU, tmp->pid, tq);
                        //Insert process into the ready queue, with exec_time -= quantum
                        tmp->burst_time -= tq;
                        insert_endRQ(&ready_queue, tmp->pid, tmp->burst_time, tmp->arrival_time, OG);
                        current_time += tq; //Update current time
                    }//No process in CPU END
                        // CPU Serving
                    else {
                        OG = tmp->burst_time;
                        //Insert process into the ready queue
                        insert_endRQ(&ready_queue, tmp->pid, tmp->burst_time, tmp->arrival_time, OG);
                    } // CPU Serving END
                }//No process in RQ END
                else {
                    OG = tmp->burst_time;
                    //Insert process into the ready queue
                    insert_endRQ(&ready_queue, tmp->pid, tmp->burst_time, tmp->arrival_time, OG);
                }
            }//if execution time more than time slice END
            //Sort ready queue according to exec_time, from shortest to longest
            sortReadyQueue();

            // Set next_process = first item in ready queue
            next_process = ready_queue;
            tmp = tmp->next;

        }
    }//Check if have process END
    //}//Check complete END
    int count = 1;
    struct CPU *tamp;
    //CPU not serving process
    if (tamp == NULL) {
        printf(" List is empty.");
    } else {
        tamp = CPU;
        //CPU is serving someone
        while (completed != n) {


            //if allocated CPU time for process is up
            if (next_process->arrival_time <= current_time & next_process->remaining_burst_time >= 0) {
                //process exist
                if (next_process != NULL) {
                    //ready queue not empty
                    if (ready_queue == NULL) {
                        printf(" List is empty.");
                    } //RQ not empty
                    else {

                        //Allocate CPU to process up to quantum
                        insert_endCPU(&CPU, next_process->pid, tq);
                        int burst_time = next_process->remaining_burst_time;
                        next_process->remaining_burst_time -= tq;
                        current_time += tq; //Update current time

                        //If execution time less than equal to 0
                        if (next_process->remaining_burst_time <= 0) {
                            current_time -= tq;
                            current_time += burst_time;
                            process->completion_time = current_time;
                            next_process->remaining_burst_time = 0;
                            //Flag next_process for removal from ready queue
                            next_process->remove_flag = 1;
                            int turnaround_time = current_time - next_process->arrival_time;
                            int wait_time = turnaround_time - next_process->OG_burst_time;
                            insert_endTA(&TA_Time,next_process->pid, turnaround_time);
                            insert_endWT(&Wait_Time,next_process->pid, wait_time);
                            completed += 1;
                            //Set next_process = next item in ready queue
                            deleteNode(&ready_queue, 1);
                            next_process = ready_queue;
                        } else {

                            if (next_process->next == NULL) {
                                next_process = ready_queue;
                            } else {
                                next_process = next_process->next;
                            }
                        }
                    }//RQ not empty END
                }//process exist END
                else {
                    //Terminate
                    break;
                }
            }//if allocated CPU time for process is up END
            else {
                next_process = next_process->next;
            }
        }//CPU is serving someone END
    }

    sortTA();
    sortWT();
    printf("\n----------Process Turn Around Time-----------\n");
    displayTA();
    printf("\nTotal Process Turn Around Time:%.2f\n", sumTA());
    printf("Average Process Turn Around Time:%.2f\n", sumTA()/n);
    printf("\n-------------Process Wait Time---------------\n");
    displayWT();
    printf("\nTotal Process Wait Time:%.2f\n", sumWT());
    printf("Average Process Wait Time:%.2f\n", sumWT()/n);




}