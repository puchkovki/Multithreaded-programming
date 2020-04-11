#include <iostream>
#include <vector>
#include <thread>

using namespace std;

/* Алгоритм Винограда - Штрассена
 
A11 A12     B11 B12     C11 C12
         *           =
A21 A22     B21 B22     C21 C22
 
S1 = A21 + A22
S2 = S1 - A11
S3 = A11 - A21
S4 = A12 - S2
S5 = B12 - B11
S6 = B22 - S5
S7 = B22 - B12
S8 = S6 - B21
 
P1 = S2 * S6
P2 = A11 * B11
P3 = A12 * B21
P4 = S3 * S7
P5 = S1 * S5
P6 = S4 * B22
P7 = A22 * S8
 
T1 = P1 + P2
T2 = T1 + P4
 
C11 = P2 + P3
C12 = T1 + P5 + P6
C21 = T2 - P7
C22 = T2 + P5
*/

typedef vector<vector<double> > Matrix;

struct input {
    Matrix &A, &B, &C;
    size_t left_index, right_index, rank;
};

void Parallel_multiply (input in) {
    for(size_t i = in.left_index; i < in.right_index; i++) {
        for(size_t j = 0; j < in.rank; j++) {
            for(size_t k = 0; k < in.rank; k++) {
                in.C[i][j] += in.A[i][k] * in.B[j][k];
            }
        }
    }
}

Matrix Multiply (Matrix& matrix_A, Matrix& matrix_B, size_t n_threads) {
    size_t rank = matrix_A.size();

    /*vector<size_t> index(n_threads + 1, 0);
	index[n_threads] = rank;
	for(int i = 1; i < n_threads; i++) {
		index[i] = index[i - 1] + (rank / n_threads) + ((i - 1) < ((rank % n_threads) - 2));
	}*/

    Matrix transponent(rank, vector<double>(rank, 0)); 
    for(size_t i = 0; i < rank; i++) {
        for(size_t j = 0; j < rank; j++) {
            transponent[j][i] = matrix_B[i][j];
        }
    }

    Matrix answer(rank, vector<double>(rank, 0));
    
    std::vector<std::thread> threads;
    for(size_t i = 0; i < n_threads; i++) {        
        size_t left = i * (rank / n_threads);
        size_t right = (i == n_threads - 1) ? rank : (i + 1) * (rank / n_threads);
        input in = {matrix_A, transponent, answer, left, right, rank};
        //cout << left << " " << right << endl;
        threads.push_back(std::thread(&Parallel_multiply, in));
    }
    
    for(size_t i = 0; i < n_threads; i++) {
        threads[i].join();
    }

    return answer;
}

int main(int argc, char** argv) {
    size_t n_threads = 1, rank = 1, average = 1;
    
    // Считывание аргументов
    if(argc > 1) {
        n_threads = stoll(argv[1]);
        if(argc > 2) {
            rank = stoll(argv[2]);
            if(argc > 3) {
                average = stoll(argv[3]);
            }
        }
    }
    
    /*size_t auxiliary = 1;
    while(auxiliary < rank) {
        auxiliary <<= 1;
    }
    rank = auxiliary;*/

    Matrix  matrix_A (rank, vector<double>(rank, 1));    

    //matrix[rank / 2][rank / 2] = 2;
    Matrix  matrix_C (rank, vector<double>(rank, 0));

    double time = 0;
    for(size_t i = 0; i < average; i++) {
        auto start = std::chrono::system_clock::now();
        
        matrix_C = Multiply(matrix_A, matrix_A, n_threads);

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> wasted = end - start;
        time += wasted.count();
    }

    /*for(size_t i = 0; i < rank; i++) {
        for(size_t j = 0; j < rank; j++) {
            cout << matrix_C[i][j] << " ";
        }
        cout << endl;
    }*/
    
    std::cout << n_threads << " " << rank << " " << time / average << std::endl;

    return EXIT_SUCCESS;
}