#include <iostream>
#include <thread>
#include <fstream>
#include <thread>

#define USED -1

using namespace std; 
const int LIST[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }; 
int VALIDLIST[3] = {0, 0, 0}; /*int[0]: Rows, int[1]: Columns, int[2]: Subgrids  */

int** populateMatrix(string file) {
    ifstream input; input.open(file, ios::in);
    int** matrix = new int*[9];
    string tp; int count = 0;

    if(input.is_open()){
        while(getline(input, tp)){
            matrix[count] = new int[9]; 
            for(int x = 0; x < 18; x+=2) matrix[count][x/2] = int(tp[x]) - 48;
            count++;
        } input.close();
    }
    return matrix;
}

void rowSudoku(int *matrix, int threadnum, int row, string file){
    ofstream output; output.open(file, ios::out | ofstream::app);
    int list[9];
    copy(begin(LIST), end(LIST), begin(list));

    for(int x = 0; x < 9; x++){
        if(list[matrix[x] - 1] != USED) list[matrix[x] - 1] = USED;
        else { output << "[Thread " << threadnum << "] Row: " << row + 1 << " Ivalid\n"; return; }
    } 
    VALIDLIST[0] ++;
    output << "[Thread " << threadnum << "] Row: " << row  + 1 << " Valid\n";
    output.close();
    return;
}

void colSudoku(int **matrix, int threadnum, int col, string file) {
    ofstream output; output.open(file, ios::out | ofstream::app);
    int list[9], column[9];
    copy(begin(LIST), end(LIST), begin(list));

    for(int x = 0; x < 9; x++) column[x] = matrix[x][col]; 
    for(int x = 0; x < 9; x++){
        if(list[column[x] - 1] != USED) list[column[x] - 1] = USED;
        else { output << "[Thread " << threadnum << "] Column: " << col + 1 << " Ivalid\n"; return; }
    } 
    VALIDLIST[1] ++; 
    output << "[Thread " << threadnum << "] Column: " << col + 1 << " Valid\n";
    output.close();
    return;
}

void gridSudoku(int **matrix, int threadnum, int zone, string file) {
    ofstream output; output.open(file, ios::out | ofstream::app);
    int list[9], grid[9];
    copy(begin(LIST), end(LIST), begin(list));
    int rub, rlb, cub, clb, count = 0;
    string RCS = "R";

    switch(zone){
        case 1: rlb = 0; rub = 2; clb = 0; cub = 2; break;
        case 2: rlb = 3; rub = 5; clb = 0; cub = 2; break;
        case 3: rlb = 6; rub = 8; clb = 0; cub = 2; break;
        case 4: rlb = 0; rub = 2; clb = 3; cub = 5; break;
        case 5: rlb = 3; rub = 5; clb = 3; cub = 5; break;
        case 6: rlb = 6; rub = 8; clb = 3; cub = 5; break;
        case 7: rlb = 0; rub = 2; clb = 6; cub = 8; break;
        case 8: rlb = 3; rub = 5; clb = 6; cub = 8; break;
        case 9: rlb = 6; rub = 8; clb = 6; cub = 8; break;
    }

    for(int x = rlb; x <= rub; x++) RCS += to_string(x);
    RCS += 'C';
    for(int x = clb; x <= cub; x++) RCS += to_string(x);

    for(int x = rlb; x <= rub; x++) {
        for(int y = clb; y <= cub; y++) {
            grid[count] = matrix[x][y];
            count++;
        }
    }

    for(int x = 0; x < 9; x++){
        if(list[grid[x] - 1] != USED) list[grid[x] - 1] = USED;
        else { 
            output << "[Thread " << threadnum << "] Subgrid: " << RCS << " Ivalid\n"; 
            return; 
        }
    } 
    VALIDLIST[2] ++;
    output << "[Thread " << threadnum << "] Subgrid: " << RCS << " Valid\n";
    output.close();
    return;
}

int main(int argc, char** argv) {
    remove(argv[2]);
    fstream output;
    int** matrix = populateMatrix(argv[1]);
    int sum = 0;

    auto rows =  [](int** matrix, int threadNum, string file){ for(int x = 0; x < 9; x++) rowSudoku(matrix[x], threadNum, x, file); };
    auto columns = [](int** matrix, int threadNum, string file){ for(int x = 0; x < 9; x++) colSudoku(matrix, threadNum, x, file); };
    auto subgrids = [](int** matrix, int threadNum, string file){ for(int x = 1; x <= 9; x++) gridSudoku(matrix, threadNum, x, file); };

    thread t1(rows, matrix, 1, argv[2]);
    t1.join();

    thread t2(columns, matrix, 2, argv[2]);
    t2.join();

    thread t3(subgrids, matrix, 3, argv[2]);
    t3.join();

    output.open(argv[2], ios::out | ofstream::app);
    output << "\nValid rows: " << VALIDLIST[0] << "\n";
    output << "Valid columns: " << VALIDLIST[1] << "\n";
    output << "Valid subgrids: " << VALIDLIST[2] << "\n";
    output << "This Sudoku solution is: ";

    for(int x = 0; x < 3; x++) sum += VALIDLIST[x];
    if(sum == 27) output << "Valid\n";
    else output << "Invalid\n";
    output.close();
    

    return 0;
}