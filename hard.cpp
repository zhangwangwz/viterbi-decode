/***************************************************
Channel Coding Course Work: conolutional codes
This program template has given the message generator, BPSK modulation, AWGN channel model and BPSK demodulation,
you should first determine the encoder structure, then define the message and codeword length, generate the state table, write the convolutional encoder and decoder.

If you have any question, please contact me via e-mail: wuchy28@mail2.sysu.edu.cn
***************************************************/

#define  _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include <fstream>
#include<ctime>
using namespace std;

#define message_length 10 //the length of message
#define codeword_length 20 //the length of codeword
float code_rate = (float)message_length / (float)codeword_length;

// channel coefficient
#define pi 3.1415926
double N0, sgm;

int state_table[1][1];//state table, the size should be defined yourself
int state_num = 2;//the number of the state of encoder structure

static int message[message_length], codeword[codeword_length];//message and codeword
static int re_codeword[codeword_length];//the received codeword
static int de_message[message_length];//the decoding message

static double tx_symbol[codeword_length][2];//the transmitted symbols
static double rx_symbol[codeword_length][2];//the received symbols

void statetable();
void encoder();
void modulation();
void demodulation();
void channel();
void decoder();
int findmin(float a, float b, float c, float d);

int main()
{
    long long i;
    float SNR, start, finish;
    long long bit_error, seq, seq_num;
    double BER;
    double progress;
    double tresult = 0;
    clock_t tstart, tfinish;




    //generate state table
    //statetable();

    //random seed
    srand((int)time(0));

    //input the SNR and frame number
    //printf("\nEnter start SNR: ");
    //cin >> start;
    //printf("\nEnter finish SNR: ");
    //cin >> finish;
    //printf("\nPlease input the number of message: ");
    //cin >> seq_num;
    start = 0;
    finish = 9;
    seq_num = 1000;


    ofstream outfile1, outfile2;
    outfile1.open("data.txt");
    outfile2.open("timedata.txt");
    for (SNR = start; SNR <= finish; SNR = SNR + 0.3)
    {
        tstart = clock();
        //channel noise
        N0 = (1.0 / code_rate) / pow(10.0, (float)(SNR) / 10.0);
        sgm = sqrt(N0 / 2);

        bit_error = 0;
        seq = 0;

        while (bit_error <= 100)
        {
            //generate binary message randomly
            /****************
            Pay attention that message is appended by 0 whose number is equal to the state of encoder structure.
            ****************/
            //cout << "message[i] = ";
            for (i = 0; i < message_length - state_num; i++)
            {
                message[i] = rand() % 2;
                //cout << message[i];
            }
            for (i = message_length - state_num; i < message_length; i++)
            {
                message[i] = 0;
                //cout << message[i];
            }
            //cout << endl;
            //convolutional encoder
            encoder();

            //BPSK modulation
            modulation();

            //AWGN channel
            channel();

            //BPSK demodulation, it's needed in hard-decision Viterbi decoder
            demodulation();

            //convolutional decoder
            decoder();

            //calculate the number of bit error
            for (i = 0; i < message_length; i++)
            {
                if (message[i] != de_message[i])
                    bit_error++;
            }
            seq = seq + 1;
            //progress = (double)(seq * 100) / (double)seq_num;

            //calculate the BER
            BER = (double)bit_error / (double)(message_length * seq);

            //print the intermediate result
            printf("SNR=%2.1f, Bit Errors=%2.1d, BER=%E\r", SNR, bit_error, BER);
        }

        //calculate the BER
        BER = (double)bit_error / (double)(message_length * seq);
        tfinish = clock();
        tresult = (double)(tfinish - tstart) / CLOCKS_PER_SEC;

        //print the final result
        printf("SNR=%2.1f, Bit Errors=%2.1d, BER=%E", SNR, bit_error, BER);
        cout << ", Time consuming: " << tresult << endl;
        tstart = 0;
        tfinish = 0;

        outfile1 << BER << " ";
        outfile2 << tresult << " ";
        tresult = 0;
    }
    //system("pause");
    outfile1.close();
    outfile2.close();
    return 0;
}
void statetable()
{

}

void encoder()
{
    //convolution encoder, the input is message[] and the output is codeword[]
    int S0 = 0;
    int S1 = 0;
    long long i;
    for (i = 0; i < message_length; i++)
    {
        codeword[2 * i] = (message[i] + S0 + S1) % 2;
        codeword[2 * i + 1] = (message[i] + S1) % 2;
        S1 = S0;
        S0 = message[i];
    }
    //cout << "codeword =    ";
    //for (i = 0; i < codeword_length; i++)
    //{
            //cout << codeword[i];
    //}
    //cout << endl;
}

void modulation()
{
    //BPSK modulation
    long long i;

    //0 is mapped to (1,0) and 1 is mapped tp (-1,0)
    for (i = 0; i < codeword_length; i++)
    {
        tx_symbol[i][0] = -1 * (2 * codeword[i] - 1);
        tx_symbol[i][1] = 0;
    }
}
void channel()
{
    //AWGN channel
    long long i, j;
    double u, r, g;

    for (i = 0; i < codeword_length; i++)
    {
        for (j = 0; j < 2; j++)
        {
            u = (float)rand() / (float)RAND_MAX;
            if (u == 1.0)
                u = 0.999999;
            r = sgm * sqrt(2.0 * log(1.0 / (1.0 - u)));

            u = (float)rand() / (float)RAND_MAX;
            if (u == 1.0)
                u = 0.999999;
            g = (float)r * cos(2 * pi * u);

            rx_symbol[i][j] = tx_symbol[i][j] + g;
        }
    }
}
void demodulation()
{
    long long i;
    double d1, d2;
    for (i = 0; i < codeword_length; i++)
    {
        d1 = (rx_symbol[i][0] - 1) * (rx_symbol[i][0] - 1) + rx_symbol[i][1] * rx_symbol[i][1];
        d2 = (rx_symbol[i][0] + 1) * (rx_symbol[i][0] + 1) + rx_symbol[i][1] * rx_symbol[i][1];
        if (d1 < d2)
            re_codeword[i] = 0;
        else
            re_codeword[i] = 1;
    }
}
void decoder()
{
    static int trellis[8][message_length] = { 0 };
    static float node[4][message_length + 1] = { 0 };
    static int node_record[4][message_length + 1] = { -1 };
    float temp1 = 0;
    float temp2 = 0;
    long long i, j;

    trellis[0][0] = abs(re_codeword[0]) + abs(re_codeword[1]);
    trellis[1][0] = abs(re_codeword[0] - 1) + abs(re_codeword[1] - 1);
    for (j = 2; j < 8; j++)
    {
        trellis[j][0] = 25535;
        trellis[j][1] = 25535;
    }
    trellis[0][1] = abs(re_codeword[2]) + abs(re_codeword[3]);
    trellis[1][1] = abs(re_codeword[2] - 1) + abs(re_codeword[3] - 1);
    trellis[4][1] = abs(re_codeword[2] - 1) + abs(re_codeword[3]);
    trellis[5][1] = abs(re_codeword[2]) + abs(re_codeword[3] - 1);

    for (i = 2; i < message_length - 2; i++)
    {
        trellis[0][i] = abs(re_codeword[2 * i]) + abs(re_codeword[2 * i + 1]);
        trellis[1][i] = abs(re_codeword[2 * i] - 1) + abs(re_codeword[2 * i + 1] - 1);
        trellis[2][i] = abs(re_codeword[2 * i] - 1) + abs(re_codeword[2 * i + 1] - 1);
        trellis[3][i] = abs(re_codeword[2 * i]) + abs(re_codeword[2 * i + 1]);
        trellis[4][i] = abs(re_codeword[2 * i] - 1) + abs(re_codeword[2 * i + 1]);
        trellis[5][i] = abs(re_codeword[2 * i]) + abs(re_codeword[2 * i + 1] - 1);
        trellis[6][i] = abs(re_codeword[2 * i]) + abs(re_codeword[2 * i + 1] - 1);
        trellis[7][i] = abs(re_codeword[2 * i] - 1) + abs(re_codeword[2 * i + 1]);
    }
    trellis[0][i] = abs(re_codeword[2 * i]) + abs(re_codeword[2 * i + 1]);
    trellis[1][i] = 25535;
    trellis[2][i] = abs(re_codeword[2 * i] - 1) + abs(re_codeword[2 * i + 1] - 1);
    trellis[3][i] = 25535;
    trellis[4][i] = abs(re_codeword[2 * i] - 1) + abs(re_codeword[2 * i + 1]);
    trellis[5][i] = 25535;
    trellis[6][i] = abs(re_codeword[2 * i]) + abs(re_codeword[2 * i + 1] - 1);
    trellis[7][i] = 25535;
    i++;
    trellis[0][i] = abs(re_codeword[2 * i]) + abs(re_codeword[2 * i + 1]);
    trellis[1][i] = 25535;
    trellis[2][i] = abs(re_codeword[2 * i] - 1) + abs(re_codeword[2 * i + 1] - 1);
    trellis[3][i] = 25535;
    trellis[4][i] = 25535;
    trellis[5][i] = 25535;
    trellis[6][i] = 25535;
    trellis[7][i] = 25535;

    node[0][1] = trellis[0][0];
    node[1][1] = 25535;
    node[2][1] = trellis[1][0];
    node[3][1] = 25535;
    node_record[0][1] = 0;
    node_record[2][1] = 0;
    node[0][2] = node[0][1] + trellis[0][1];
    node[1][2] = node[2][1] + trellis[4][1];
    node[2][2] = node[0][1] + trellis[1][1];
    node[3][2] = node[2][1] + trellis[5][1];
    node_record[0][2] = 0;
    node_record[1][2] = 2;
    node_record[2][2] = 0;
    node_record[3][2] = 2;
    for (i = 3; i <= message_length; i++)
    {
        temp1 = node[0][i - 1] + trellis[0][i - 1];
        temp2 = node[1][i - 1] + trellis[2][i - 1];
        if (temp1 > temp2)
        {
            node[0][i] = temp2;
            node_record[0][i] = 1;
        }
        else
        {
            node[0][i] = temp1;
            node_record[0][i] = 0;
        }

        temp1 = node[2][i - 1] + trellis[4][i - 1];
        temp2 = node[3][i - 1] + trellis[6][i - 1];
        if (temp1 > temp2)
        {
            node[1][i] = temp2;
            node_record[1][i] = 3;
        }
        else
        {
            node[1][i] = temp1;
            node_record[1][i] = 2;
        }

        temp1 = node[0][i - 1] + trellis[1][i - 1];
        temp2 = node[1][i - 1] + trellis[3][i - 1];
        if (temp1 > temp2)
        {
            node[2][i] = temp2;
            node_record[2][i] = 1;
        }
        else
        {
            node[2][i] = temp1;
            node_record[2][i] = 0;
        }

        temp1 = node[2][i - 1] + trellis[5][i - 1];
        temp2 = node[3][i - 1] + trellis[7][i - 1];
        if (temp1 > temp2)
        {
            node[3][i] = temp2;
            node_record[3][i] = 3;
        }
        else
        {
            node[3][i] = temp1;
            node_record[3][i] = 2;
        }
    }

    int current_node;
    temp1 = findmin(node[0][message_length], node[1][message_length], node[2][message_length], node[3][message_length]);
    current_node = temp1;
    for (i = message_length; i > 0; i--)
    {
        if (current_node == 0 || current_node == 1)
        {
            de_message[i - 1] = 0;
        }
        if (current_node == 2 || current_node == 3)
        {
            de_message[i - 1] = 1;
        }
        current_node = node_record[current_node][i];
    }
    //cout << "re_codeword = ";
    //for (i = 0; i < codeword_length; i++)
    //{
            //cout << re_codeword[i];
    //}
    //cout << endl;
    //cout << "de_message = ";
    //for (i = 0; i < message_length; i++)
    //{
            //cout << de_message[i];
    //}
    //cout << endl;
}
int findmin(float a, float b, float c, float d)
{
    int temp1 = -1;
    int temp2 = -1;
    float temp[4] = { 0 };
    temp[0] = a;
    temp[1] = b;
    temp[2] = c;
    temp[3] = d;
    if (a >= b)
    {
        temp1 = 1;
    }
    else
    {
        temp1 = 0;
    }
    if (c >= d)
    {
        temp2 = 3;
    }
    else
    {
        temp2 = 2;
    }
    if (temp[temp1] > temp[temp2])
    {
        return temp2;
    }
    else
    {
        return temp1;
    }
}
