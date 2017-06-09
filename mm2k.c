#include <stdio.h>
#include <stdlib.h> 
#include <math.h> 
#include <time.h>
#include <string.h> 
#include <sys/stat.h>
#include <sys/types.h>

struct State {
    int arrivalCount;
    int clientsServedA;
    int clientsServedB;
    char name[50];
    double prob;
};
typedef struct State State;

int getRandomEvent (int l, int ma, int mb)  {
    double random = ((double) rand() / (RAND_MAX));
    if (random < (double)l / (l + ma + mb)) return 0;
    else if (random < (double)(l + ma) / (l + ma + mb)) return 1;
    else return 2;
}


int main(void)
{
    
    int l, ma = 2, mb = 2, N = 10;
    double res[10][4];
    srand(time(NULL));
    rand();
    fprintf(stdout, "Please insert arrival rate λ (only integers from 1 to 9 please): ");
    fscanf(stdin, "%d", &l);

    char output[30] = {0};
    strcpy(output, "outputs/output__.txt");
    output[15] = l+48;

    int result = mkdir("outputs", 0777);
    FILE *fp = fopen(output, "w");
    strcpy(output, "outputs/excel__.csv");
    output[14] = l+48;
    FILE *excel = fopen(output, "w");
    strcpy(output, "outputs/excel_mac__.csv");
    output[18] = l+48;
    FILE *excel_mac = fopen(output, "w");
    fprintf(fp, "Arrival rate λ = %d\n", l);

    for (int k = 1; k <= 9; k++)
    {
        int losses = 0, total_arrivals = 0, total_clientsServedA = 0, total_clientsServedB = 0;
        int reps = 0, iter = 0, event;
        double average_clients = 0, average_clients_aux = 0, throughputA, throughputB, total_throughput;
        State states[10+1][2];
        strcpy(states[0][0].name, "0");
        strcpy(states[0][1].name, "ERROR: State should not be reached");
        states[0][0].arrivalCount = 0;
            states[0][0].clientsServedA = 0;
            states[0][0].clientsServedB = 0;
            states[0][1].arrivalCount = 0;
            states[0][1].clientsServedA = 0;
            states[0][1].clientsServedB = 0;
        for (int i = 1; i <= k; i++) {
            states[i][0].name[0] = i+48;
            states[i][0].name[1] = 'a';
            states[i][1].name[0] = i+48;
            states[i][1].name[1] = 'b';
            states[i][0].arrivalCount = 0;
            states[i][0].clientsServedA = 0;
            states[i][0].clientsServedB = 0;
            states[i][1].arrivalCount = 0;
            states[i][1].clientsServedA = 0;
            states[i][1].clientsServedB = 0;
        }
        for (int i = k+1; i <= N; i++) {
            states[i][0].name[0] = i+48;
            strcpy(states[i][1].name, "ERROR: State should not be reached");
            states[i][0].arrivalCount = 0;
            states[i][0].clientsServedA = 0;
            states[i][0].clientsServedB = 0;
            states[i][1].arrivalCount = 0;
            states[i][1].clientsServedA = 0;
            states[i][1].clientsServedB = 0;
        }
        
        int currState_i = 0;
        int currState_j = 0;
        fprintf(excel, "k = %d,", k);
        fprintf(excel_mac, "k = %d;", k);
        while (1) {         //is it really ok with 1??
            
            iter++;
            while (reps < 10000*iter) {
                if (currState_i == 0) event = getRandomEvent(l, 0, 0);
                else if ((currState_i <= k) && (currState_j == 0)) event = getRandomEvent(l, ma, 0);
                else if ((currState_i == 1) && (currState_j == 1)) event = getRandomEvent(l, 0, mb);
                else event = getRandomEvent(l, ma, mb);
                if (event == 0) {
                    total_arrivals++;
                    states[currState_i][currState_j].arrivalCount++;
                    if (currState_i == 10) losses++;
                    else {
                        currState_i++;
                        if (currState_i == k+1) currState_j = 0;
                    }
                }
                else if (event == 1) {
                    total_clientsServedA++;
                    states[currState_i][currState_j].clientsServedA++;
                    currState_i--;
                    if (currState_i == k) currState_j = 1;
                }
                else {
                    total_clientsServedB++;
                    states[currState_i][currState_j].clientsServedB++;
                    currState_i--;
                    currState_j = 0;
                }
                reps++;
            }

            states[0][0].prob = states[0][0].arrivalCount / (double)total_arrivals;
            //states[10][0].arrivalCount = 0;
            for (int i=1; i<=k; i++){
                states[i][0].prob = states[i][0].arrivalCount / (double)total_arrivals;
                states[i][1].prob = states[i][1].arrivalCount / (double)total_arrivals;
            }
            for (int i=k+1; i<=N; i++){
                states[i][0].prob = states[i][0].arrivalCount / (double)total_arrivals;
            }
            
            
            average_clients_aux = average_clients;
            average_clients = 0;
            
            for (int i=1; i<=k; i++){
                average_clients += i * states[i][0].prob;
                average_clients += i * states[i][1].prob;
            }
            for (int i=k+1; i<=N; i++){
                average_clients += i * states[i][0].prob;
            }
            fprintf(excel, "%f,", average_clients);
            fprintf(excel_mac, "%f;", average_clients);
            
            if ((average_clients < 1.0001*average_clients_aux) && (average_clients > 0.9999*average_clients_aux)) break;
        }
        total_throughput = l * (1 - states[10][0].prob);
        throughputA = ma * (1 - states[0][0].prob - states[1][1].prob);
        throughputB = total_throughput - throughputA;
        fprintf(fp, "For k = %d: %f\n", k, average_clients);
        fprintf(fp, "\tThroughput of server A is %f\n", throughputA);
        fprintf(fp, "\tThroughput of server B is %f\n", throughputB);
        fprintf(fp, "\tThroughput of all servers is %f\n", throughputA / throughputB);
        fprintf(excel, "\n");
        fprintf(excel_mac, "\n");

        res[k][0] = average_clients;
        res[k][1] = throughputA;
        res[k][2] = throughputB;
        res[k][3] = throughputA / throughputB;
    }
    
    fprintf(excel, "\n\nAverage Clients\n");
    fprintf(excel_mac, "\n\nAverage Clients\n");

    for (int i = 1; i <= 9; ++i)
    {
        fprintf(excel, "%f,", res[i][0]);
        fprintf(excel_mac, "%f;", res[i][0]);
    }
    fprintf(excel, "\n\nThroughput of server A\n");
    fprintf(excel_mac, "\n\nThroughput of server A\n");

    for (int i = 1; i <= 9; ++i)
    {
        fprintf(excel, "%f,", res[i][1]);
        fprintf(excel_mac, "%f;", res[i][1]);
    }
    fprintf(excel, "\n\nThroughput of server B\n");
    fprintf(excel_mac, "\n\nThroughput of server B\n");
    for (int i = 1; i <= 9; ++i)
    {
        fprintf(excel, "%f,", res[i][2]);
        fprintf(excel_mac, "%f;", res[i][2]);
    }
    fprintf(excel, "\n\nThroughput Ratio\n");
    fprintf(excel_mac, "\n\nThroughput Ratio\n");
    for (int i = 1; i <= 9; ++i)
    {
        fprintf(excel, "%f,", res[i][2]);
        fprintf(excel_mac, "%f;", res[i][1]);
    }

    return 0;
}

