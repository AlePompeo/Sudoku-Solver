//*********************************************************************************************
//Nel seguente programma è stato usato un algoritmo di Depth First Search (DFS).
//L'algoritmo può essere visto anche come una macchina a stati, dove vengono creati
//dei nodi in maniera ricorsiva che esplorano tutte le possibili e successive soluzioni 
//in maniera sequenziale dopo la prima iterazione del ciclo.
//Si prega di compilare il codice nel seguente modo per usufruire delle massime performance
//gcc -fopenmp <nomefile>
//Il flag usato appartiene alla libreria multithread Open MP.
//Run più veloce registrata su CPU Ryzen Mobile (6 core): 0.054s
//*********************************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#pragma GCC optimize("Ofast")
#define LEN 9
#define C3 3
#define C10 10

static unsigned char isAllowed(unsigned char y, unsigned char x, unsigned char n, unsigned char (*mat)[LEN][LEN]) {
    for(unsigned char i=0;i<LEN;++i) {
        if((*mat)[y][i] == n) {
            return 0;
        }
        if((*mat)[i][x] == n) {
            return 0;
        }
        if((*mat)[y/C3*C3+i/C3][x/C3*C3+(i >= C3 ? i % C3 : i)] == n) {   //fast mod operation implemented
            return 0;
        }
    }
    return 1;
}

static unsigned char mySolver(unsigned char (*mat)[LEN][LEN]) {
    for(unsigned char i=0;i<LEN;++i) {
        for(unsigned char j=0;j<LEN;++j) {
            if((*mat)[j][i] == 0) {
                for(unsigned char n=1;n<C10;++n) {
                    if(isAllowed(j,i,n,mat)) {   //assign the first possible value allowed
                        (*mat)[j][i] = n;
                        if(mySolver(mat)) {  //call another node in depth and finds the next solution
                            return 1;
                        }
                        (*mat)[j][i] = 0;
                    }
                }
                return 0; //recursion stops if none is allowed ---> the function backtracks to the previous cell and finds the first possibile value
            }      
        }
    }
    return 1;
}

struct runnerStruct {
    unsigned char matrix[LEN][LEN];
    int i, j;
    enum STATE { 
        RUNNING,
        FAILED,
        SUCCEDED
    } state;
    pthread_t handle;
};

void* runner(void* ptr) 
{
    struct runnerStruct* aRunnerStruct = (struct runnerStruct*) ptr; 
//    printf("Thread i=%i j=%i started\n",aRunnerStruct->i,aRunnerStruct->j); 
    for(unsigned char n=1;n<C10;++n) {
        if(isAllowed(aRunnerStruct->j,aRunnerStruct->i,n,&aRunnerStruct->matrix)) {   //assign the first possible value allowed
            aRunnerStruct->matrix[aRunnerStruct->j][aRunnerStruct->i] = n;
            if(mySolver(&aRunnerStruct->matrix)) {  //call another node in depth and finds the next solution
                aRunnerStruct->state=SUCCEDED;
//                printf("Thread i=%i j=%i succeded\n",aRunnerStruct->i,aRunnerStruct->j); 
                return NULL;
            }
        }
    }
    aRunnerStruct->state=FAILED;
//    printf("Thread i=%i j=%i failed\n",aRunnerStruct->i,aRunnerStruct->j); 
    return NULL;
}

void solve(unsigned char (*mat)[LEN][LEN]) {

    struct runnerStruct runners[LEN*LEN];
    int runnerMaxIndex=0;

    for(unsigned char i=0;i<LEN;++i) {
        for(unsigned char j=0;j<LEN;++j) {
            if((*mat)[j][i] == 0) {               
                memcpy(&runners[runnerMaxIndex].matrix,mat,sizeof(runners[0].matrix));
                runners[runnerMaxIndex].i=i;
                runners[runnerMaxIndex].j=j;
                runners[runnerMaxIndex].state=RUNNING;
                pthread_create(&runners[runnerMaxIndex].handle,NULL,runner,(void*)&runners[runnerMaxIndex]);
                runnerMaxIndex++;                
            }      
        }
    }

//    printf("%i runners started\n",runnerMaxIndex); 

    //wait for first successful runners completion

    int runnersCompleted;
    do
    {
        runnersCompleted=0;
        for(int r=0;r<runnerMaxIndex;r++) {
            if(runners[r].state==SUCCEDED) {
                memcpy(mat,&runners[r].matrix,sizeof(runners[r].matrix));                
//                printf("Found a solution\n"); 
                return;
            } else if (runners[r].state==FAILED) {
                runnersCompleted++;
            }
        }
    } while (runnersCompleted<runnerMaxIndex);
    printf("All threads failed\n"); 
}

/*void solve(unsigned char (*mat)[LEN][LEN]) {
    mySolver(mat);
}*/

static unsigned char problem1[9][9] =
    {{0, 5, 0, 0, 0, 0, 4, 0, 0},
     {0, 0, 0, 4, 0, 0, 0, 3, 0},
     {4, 0, 0, 0, 9, 0, 0, 0, 6},
     {7, 0, 0, 0, 0, 0, 9, 0, 8},
     {9, 0, 0, 7, 0, 0, 0, 5, 0},
     {0, 8, 3, 0, 5, 0, 6, 2, 0},
     {0, 0, 0, 0, 2, 8, 0, 4, 0},
     {0, 0, 2, 0, 0, 0, 0, 0, 5},
     {6, 0, 0, 0, 0, 3, 0, 0, 0}};

static unsigned char problem2[9][9] =
    {{0, 4, 9, 7, 0, 5, 0, 0, 0},
     {0, 0, 0, 0, 0, 4, 0, 0, 3},
     {6, 0, 1, 2, 0, 0, 0, 7, 0},
     {0, 0, 0, 0, 9, 1, 0, 0, 5},
     {0, 2, 4, 0, 6, 8, 7, 3, 1},
     {1, 5, 8, 0, 2, 7, 4, 9, 0},
     {0, 0, 0, 0, 0, 2, 6, 4, 0},
     {0, 6, 0, 1, 0, 0, 0, 0, 0},
     {4, 0, 5, 0, 0, 0, 3, 0, 2}};

static unsigned char problem3[9][9] =
    {{0, 9, 0, 0, 0, 3, 0, 2, 0},
     {0, 7, 0, 0, 0, 6, 0, 0, 0},
     {0, 1, 0, 8, 0, 0, 0, 7, 9},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 3, 0, 0, 8, 0, 9, 0},
     {0, 2, 0, 0, 1, 0, 0, 5, 0},
     {0, 0, 4, 0, 0, 5, 0, 8, 0},
     {0, 0, 2, 6, 0, 0, 5, 0, 0},
     {0, 0, 0, 0, 0, 0, 7, 0, 6}};

static unsigned char problem4[9][9] =
    {{8, 1, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 5, 0},
     {6, 0, 0, 1, 0, 0, 0, 3, 8},
     {0, 0, 4, 0, 0, 9, 0, 0, 0},
     {3, 0, 0, 2, 0, 0, 0, 1, 6},
     {0, 0, 0, 0, 0, 0, 7, 0, 0},
     {0, 0, 0, 8, 0, 0, 5, 0, 0},
     {7, 0, 0, 0, 0, 0, 2, 0, 0},
     {0, 0, 3, 0, 4, 0, 0, 8, 7}};

int main(int argc, char **argv) {
    //clock_t start, end;
    //solve(&problem1);
    //solve(&problem2);
    //start = clock();
    //solve(&problem3);
    //end = clock();
    solve(&problem4);
    
    //double duration = ((double)end - start)/CLOCKS_PER_SEC; 
    //printf("Time taken to execute in seconds : %f", duration);
    /*
    printf("\n+-----+-----+-----+\n");
    for (int x = 0; x < 9; ++x) {
      for (int y = 0; y < 9; ++y) printf("|%d", problem1[x][y]);
      printf("|\n");
      if (x % 3 == 2) printf("\n+-----+-----+-----+\n");
    }
    printf("|\n");
    printf("|\n");
    printf("|\n");
    printf("|\n");
    for (int x = 0; x < 9; ++x) {
      for (int y = 0; y < 9; ++y) printf("|%d", problem2[x][y]);
      printf("|\n");
      if (x % 3 == 2) printf("\n+-----+-----+-----+\n");
    }
    printf("|\n");
    printf("|\n");
    printf("|\n");
    printf("|\n");
    for (int x = 0; x < 9; ++x) {
      for (int y = 0; y < 9; ++y) printf("|%d", problem3[x][y]);
      printf("|\n");
      if (x % 3 == 2) printf("\n+-----+-----+-----+\n");
    }
    printf("|\n");
    printf("|\n");
    printf("|\n");
    printf("|\n");
    */
    for (int x = 0; x < 9; ++x) {
      for (int y = 0; y < 9; ++y) printf("|%d", problem4[x][y]);
      printf("|\n");
      if (x % 3 == 2) printf("\n+-----+-----+-----+\n");
    }

    return 0;
}
