#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <chrono>
#include <fstream>

#define forn(i, n) for(size_t i = 0; i < size_t(n); ++i)
#define forf(i, n1, n2) for(size_t i = size_t(n1); i < size_t(n2); ++i)
#define NUM_THR 100

typedef std::vector<double> __vector;
typedef std::vector<__vector> __matrix;
typedef std::pair<size_t, size_t> __m_size_t; 

double operator*(const __vector &left, const __vector &right) {
    if (left.size() != right.size()) {
        throw "__vector: operator*: unappropriate arguments";
    }

    double res = 0.0;
    forn (i, left.size()) {
        res += left[i] * right[i];
    }
    return res;
}

std::ostream& operator<<(std::ostream &os, __vector to_print) {
    os << "[ ";
    forn (i, to_print.size()) {
        os << to_print[i] << " ";
    }
    os << "]";
    return os;
}

class Matrix {
    private:
        __matrix val;
        size_t rows;
        size_t cols;

        size_t num_threads;

        Matrix(void);

    public:
        struct __thr_m_j_input {
            const Matrix &left;
            const Matrix &tright;
            const size_t lefti;
            const size_t righti;
            __matrix &res;
        }; 

        Matrix(size_t rows, size_t cols = 0, size_t num_threads = 1) : val(__matrix(rows, __vector(cols, 0))), rows(rows), cols(cols), num_threads(num_threads) {}

        Matrix(const __matrix &val) : val(val), rows(val.size()) {
            if (rows != 0) {
                cols = val[0].size();
            } else {
                cols = 0;
            }
        }

        Matrix(const Matrix &other) : val(other.val), rows(other.rows), cols(other.cols) {}

        Matrix operator=(Matrix other) {
            this->val = other.val;
            return *this;
        }

        __m_size_t Size() const { return __m_size_t(rows, cols); }
        size_t Rows() const { return rows; }
        size_t Cols() const { return cols; }

        __vector& operator[](size_t i) { return val[i]; }
        __vector operator[](size_t i) const { return val[i]; }

        Matrix T() const {
            __matrix res(cols, __vector(rows, 0));
            forn (i, rows) {
                forn (j, cols) {
                    res[j][i] = val[i][j];
                }
            }
            return Matrix(res);
        }

        friend Matrix operator*(const Matrix &left, const Matrix &right);

        ~Matrix() {}
};

void thread_multiply_job(Matrix::__thr_m_j_input in) {
            forf (i, in.lefti, in.righti) {
                forn (j, in.tright.Rows()) {
                    in.res[i][j] = in.left[i] * in.tright[j];
                }
            }
        }

Matrix operator*(const Matrix &left, const Matrix &right) {
    __matrix res(left.rows, __vector(right.cols, 0));
    const Matrix tright = right.T();
    std::vector<std::thread> threads;
    forn (i, left.num_threads) {
        size_t lefti = i * (res.size() / left.num_threads);
        size_t righti = i == left.num_threads - 1 ? res.size() : (i + 1) * (res.size() / left.num_threads);
        Matrix::__thr_m_j_input in = {left, tright, lefti, righti, res};
        threads.push_back(std::thread(thread_multiply_job, in));
    }
    /*forn (i, left.rows) {
        forn (j, tright.rows) {
            res[i][j] = left[i] * tright[j];
        }
    }*/
    forn (i, left.num_threads) {
        threads[i].join();
    }
    return Matrix(res);
}

std::ostream& operator<<(std::ostream &os, Matrix to_print) {
    os << "[" << std::endl;
    forn (i, to_print.Rows()) {
        os << to_print[i] << std::endl;
    }
    os << "]";
    return os;
}

int main(int argc, char *argv[]) {
    size_t num_threads;
    size_t m_size;
    size_t many;
    if (argc < 2) {
        num_threads = 1;
    } else {
        num_threads = std::stoull(argv[1]);
    }
    if (argc < 3) {
        m_size = 1;
    } else {
        m_size = std::stoull(argv[2]);
    }
    if (argc < 4) {
        many = 1;
    } else {
        many = std::stoull(argv[3]);
    }

    Matrix a(m_size, m_size, num_threads);
    forn (i, a.Rows()) {
        forn (j, a.Cols()) {
            if (i == j) {
                a[i][j] = 1;
            }
        }
    }

    double res = 0.0;
    forn (i, many) {
        auto start = std::chrono::system_clock::now();
        a = a * a;
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        res += elapsed.count();
    }
    
    std::cout << num_threads << " " << m_size << " " << res / many << std::endl;

    return 0;
}
