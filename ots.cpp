/*
 * algorithm: odd-even transposition sort
 * author: jakub zak, Willaschek Tomas (xwilla00)
 *
 */

#include <mpi.h>
#include <iostream>
#include <fstream>

/*
 * if true: return code je celkovy pocet iteraci, zadny output
 * else:    return code je 0 pokud program nespadne
 */
//#define TESTING

#define TAG 0

using namespace std;

bool test_end(int numprocs, MPI_Status stat, int mynumber, int myid, bool to_print){
    /*
     * Posle vysledky na procesor "0", sekvencne je porovna a zjisti, jestli ma byt sort ukoncen nebo ne
     * Funkce je pritomna jen z duvodu ziskani informaci pro graf
     */
    int neighnumber;
    int *final = new int [numprocs-1];
    for(int i = 1; i < numprocs; i++){
        if(myid == i)
            MPI_Send(&mynumber, 1, MPI_INT, 0, TAG,  MPI_COMM_WORLD);
        if(myid == 0){
            MPI_Recv(&neighnumber, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &stat);
            final[i] = neighnumber;
        }
    }
    int can_end = 1;
    // procesor "0" sekvencne porovna cisla a preda vsem procesorum informaci o ukonceni iterace
    if(myid == 0){
        final[0] = mynumber;
        for(int i = 1; i < numprocs; i++){
            if(final[i-1] > final[i] && can_end)
                can_end = 0;

        }
        // zaslani informace o ukonceni a pripadny vypis
        #ifndef TESTING
        if(to_print)
            can_end = 1;
        if(can_end)
            cout << final[0] << endl;
        #endif
        for(int i = 1; i < numprocs; i++){
            MPI_Send(&can_end, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
            #ifndef TESTING
            if(can_end)
                cout << final[i] << endl;
            #endif
        }
    }
    else {
        MPI_Recv(&can_end, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
    }
    return can_end == 1;
}

int main(int argc, char *argv[])
{
    int iterations = 0;
    int numprocs, myid, neighnumber, mynumber;
    MPI_Status stat;

    // inicializace MPI a ridicich hodnot
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    // nacteni vstupniho souroru procesem "0" a rozeslani cisel jednotlivym procesum, vcetne "0"
    // vypis cisel ze souboru na stdout
    if(myid == 0){
        int number, receiver = 0;
        ifstream stream;
        stream.open("numbers", ios::in);
        while(stream.good()){
            number = stream.get();
            #ifndef TESTING
            if(receiver)
                cout << " ";
            #endif
            if(!stream.good())
                break;
            #ifndef TESTING
            cout << number;
            #endif
            MPI_Send(&number, 1, MPI_INT, receiver++, TAG, MPI_COMM_WORLD);
        }
        #ifndef TESTING
        cout << endl;
        #endif
        stream.close();
    }

    MPI_Recv(&mynumber, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);

    int odd_max = 2 * (numprocs / 2) - 1;
    int even_max = 2 * ((numprocs - 1) / 2);

    bool printed = false;
    double start;
    if(myid == 0)
        start = MPI_Wtime();
    // zacatek razeni, maximalne -- pocet_cisel / 2
    for(int k = 0; k < numprocs/2; k++){
//        if(test_end(numprocs, stat, mynumber, myid, false)){
//            printed = true;
//            break;
//        }
        iterations++;

        // sude odeslou lichym a cekaji na vysledek
        // ve skutecnosti indexujeme procesory od "0", takze se vlastne jedna o liche procesory pri indexaci od "1"
        if(myid < odd_max && !(myid % 2)){
            MPI_Send(&mynumber, 1, MPI_INT, myid + 1, TAG, MPI_COMM_WORLD);
            MPI_Recv(&mynumber, 1, MPI_INT, myid + 1, TAG, MPI_COMM_WORLD, &stat);
        }
        else if(myid <= odd_max){ // liche porovnaji a vrati mensi
            MPI_Recv(&neighnumber, 1, MPI_INT, myid - 1, TAG, MPI_COMM_WORLD, &stat);
            if(mynumber < neighnumber){
                MPI_Send(&mynumber, 1, MPI_INT, myid - 1, TAG, MPI_COMM_WORLD);
                mynumber = neighnumber;
            }
            else
                MPI_Send(&neighnumber, 1, MPI_INT, myid - 1, TAG, MPI_COMM_WORLD);
        }

        // liche odeslou sudym a cekaji na vysledek
        if(myid < even_max && myid % 2){
            MPI_Send(&mynumber, 1, MPI_INT, myid + 1, TAG, MPI_COMM_WORLD);
            MPI_Recv(&mynumber, 1, MPI_INT, myid + 1, TAG, MPI_COMM_WORLD, &stat);
        }
        else if(myid <= even_max && myid != 0){ // sude porovnaji a vrati mensi, 0 nic, protoze se posila na +1
            MPI_Recv(&neighnumber, 1, MPI_INT, myid - 1, TAG, MPI_COMM_WORLD, &stat);
            if(mynumber < neighnumber){
                MPI_Send(&mynumber, 1, MPI_INT, myid - 1, TAG, MPI_COMM_WORLD);
                mynumber = neighnumber;
            }
            else
                MPI_Send(&neighnumber, 1, MPI_INT, myid - 1, TAG, MPI_COMM_WORLD);
        }
    }
    double end;
    if(myid == 0)
        end = MPI_Wtime();
    if(!printed)
        test_end(numprocs, stat, mynumber, myid, true);

    #ifdef TESTING
    if(myid == 0){
        cout << "Iterations: " << iterations << "\nTime: " << end - start << endl;
        ofstream f;
        f.open("output", ios::app);
        f << ";" << end - start;
        f.close();
    }
    #endif

    MPI_Finalize();


    #ifdef TESTING
    return iterations;
    #else
    return 0;
    #endif
}
