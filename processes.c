//By Travelle Barrett
//Partner: Emily Macleod

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>

pid_t pid;
int parentToChild[5][2];
int childToParent[5][2];
int id =0, greater = 0;
int numberArray[5];
int numinArray = 5 ;
char fileName[16];
int n = 25;
int pivotfound = 0; 

//define codes
int message = 0, element = 0, reply =0, ranEle =0;

#define REQUEST 100
#define PIVOT 200
#define LARGE 300
#define SMALL 400
#define READY 500
#define KILL 0

int ready = 0, value =0;
int readycount = 0;
int pivot;

int readFile(int arr[], int child_id);
int main()
{
    setbuf(stdout, NULL);
    for(int i=0; i<5; i++) 
    {
        pipe(childToParent[i]); //child to parent
        pipe(parentToChild[i]); //parent to child

        pid = fork(); //fork here
        //children processes
        if(pid == 0)
        {
            id = i; 
            printf("Child %d here\n", id);
            if (readFile(numberArray, id) == -1 ){
                printf("Close File");
                exit(NULL);
            }
            else {

                printf("Child %i: %i %i %i %i %i",
                       id,numberArray[0],numberArray[1],numberArray[2],numberArray[3],numberArray[4]);
                numinArray = 5;
            }

            ready = READY;
            printf("\nChild %d sends ready\n", id);
            close(childToParent[id][0]);
            write(childToParent[id][1], &ready, sizeof(ready));
            while(1)
            {  
                close(parentToChild[id][1]); //waiting for parent response
                read(parentToChild[id][0], &message, sizeof(message));
                switch(message)
                {
                    case REQUEST:
                    {
                        printf("in request %d\n", id);
                        if(numinArray == 0)
                        {
                            int empty = -1;
                            close(childToParent[id][0]);
                            write(childToParent[id][1], &empty, sizeof(empty));
                        }
                        else
                        {
                            int r = rand() % numinArray; //select a random element
                            ranEle = numberArray[r];
                            printf("Random element of an array %d, r is: %d\n", ranEle, r);
                            close(childToParent[id][0]); //send elements to oparents
                            write(childToParent[id][1], &ranEle, sizeof(ranEle));
                        }
                        break;
                    }
                    case PIVOT:
                    {
                        //read the element from parent
                        close(parentToChild[id][1]);
                        read(parentToChild[id][0], &element,sizeof(element));
                        pivot = element;
                        printf("Pivot = %d\n", pivot);
                        int greater = 0;
                        for(int i =0; i < numinArray; i++)
                        {
                            if(numberArray[i] > pivot)
                            {
                                greater++;
                            }
                        }
                        //send greater to parent
                        close(childToParent[id][0]);
                        write(childToParent[id][1], &greater, sizeof(greater));

                        break;
                    }
                    case LARGE:
                    {
                        if (numinArray == 0);
                        else{
                            printf("In large %d\n", id);
                            for(int i =0; i < numinArray; i++)
                            {
                                if (numberArray[i] > pivot){
                                    int temp = numberArray[numinArray-1];
                                    numberArray[numinArray -1] = numberArray[i];
                                    numberArray[i] = temp;
                                    numinArray--;
                                }
                            }
                        }
                        break;
                    }
                    case SMALL:
                    {
                    if (numinArray == 0);
                    else{
                        printf("In large %d\n", id);
                        for(int i =0; i < numinArray; i++)
                        {
                            if (numberArray[i] < pivot){
                                int temp = numberArray[numinArray-1];
                                numberArray[numinArray -1] = numberArray[i];
                                numberArray[i] = temp;
                                numinArray--;
                            }
                        }
                    }
                        printf("In small %d\n", id);
                        break;
                    }
                    case KILL:
                    {
                        //kill children processes
                        kill(0, SIGKILL);
                        break;
                    }
                    default:
                    {
                        printf("Default case\n");
                    }
                }
            }
        }

    if (pid > 0)  //parent code
    {
        close(childToParent[i][1]);
        read(childToParent[i][0], &ready, sizeof(ready));
        if (ready == READY)
        {
            printf("---\nChild %d is ready\n", i+1);
            readycount++;
            printf("Ready count is %d\n", readycount);
        }
        else {
            printf("KILL CHILDREN HERE");
        }
        if(readycount == 5)
        {
            //pivot process
            printf("Start the pivot process\n");

            //use loop to send request
            while(1)
            {
                int k = n/2; 
                int m = 0;
                int request = REQUEST, pivot = PIVOT, large = LARGE, small = SMALL, kill = KILL;
                int cur_child = (rand() % 5); //random numbers
                printf("Current child is %d\n", cur_child);
                //send request for element to random child
                close(parentToChild[cur_child][0]);
                write(parentToChild[cur_child][1], &request, sizeof(request));
                close(childToParent[cur_child][1]);
                read(childToParent[cur_child][0], &element, sizeof(element));
                printf("Child %i sends %i\n",cur_child,element);
                //if -1 is the respnse
                if(element == -1)
                {
                    printf("Array is Empty!\n");
                }
                else
                {
                        write(parentToChild[i][1], &pivot, sizeof(pivot));
                        close(parentToChild[i][0]);
                        write(parentToChild[i][1], &element, sizeof(element));
                        //read response
                        close(childToParent[i][1]);
                        read(childToParent[i][0], &greater, sizeof(greater));
                        m += greater;
                    }
                    printf("m is :%d\n", m);
                    printf("n is :%d\n", n);
                    printf("k is :%d\n", k);

                    //carry on with the process
                    //write pivot to ALL the children, then send element

                    for(int i =0; i<5; i++)
                    {
                        printf("Writing pivot\n");
                        close(parentToChild[i][0]);                   
                    if(k > m)//means less than half of the array is greater than pivot - send large
                    {
                        for(int i =0; i<5; i++)
                        {
                            close(parentToChild[i][0]);
                            write(parentToChild[i][1], &large, sizeof(large));
                        }
                        k=k-m;
                    }
                    if (k < m) // send small
                    {
                        for(int i =0; i<5; i++)
                        {
                            close(parentToChild[i][0]);
                            write(parentToChild[i][1], &small, sizeof(small));
                        }
                    }
                    if(k == m)
                    {
                        printf("Found pivot", element);
                        exit(NULL);
                        for(int i =0; i<5; i++)
                        {
                            //close(parentToChild[i][0]);
                            //write(parentToChild[i][1], &kill, sizeof(kill));
                        }
                    }
                }
            }
        }

    }

    else
    {

    }

    printf("exit %d\n", id);
    }
}

int readFile(int arr[], int id){
    //readin array
    char fileName[100];
    FILE *file = NULL ;
    sprintf(fileName, "input_%i.txt", id);
    file =fopen(fileName,"r");
    if (file == NULL){
        printf("Error Reading File: %s\n",fileName);
        return -1;

    }
    else {
        for (int i = 0 ; i < 5; i++)
        {
            fscanf(file,"%d",&arr[i]);
        }
        fclose(file);
        return 1;
    }

}
