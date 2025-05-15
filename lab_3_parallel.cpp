#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <direct.h>

using namespace std;
using namespace chrono;

vector<vector<int>> readCSV(const string& filename, int size) {
    vector<vector<int>> matrix(size, vector<int>(size));
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error, cant open the file " << filename << endl;
        exit(1);
    }
    string line;
    for (int i = 0; i < size; ++i) {
        getline(file, line);
        stringstream ss(line);
        string value;
        for (int j = 0; j < size; ++j) {
            getline(ss, value, ',');
            matrix[i][j] = stoi(value);
        }
    }
    file.close();
    return matrix;
}

void writeCSV(const string& filename, const vector<vector<int>>& matrix, int size) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error, cant open the file " << filename << endl;
        exit(1);
    }

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            file << matrix[i][j];
            if (j != size - 1) file << ",";
        }
        file << "\n";
    }
    file.close();
    cout << "Result saved to: " << filename << endl;
}

void writeTimeToFile(const string& filename, double time_s) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error, cant open the file " << filename << endl;
        exit(1);
    }
    file << "Time of multiplication " << time_s << " с" << endl;
    file.close();
    cout << "Time of multiplication daved to: " << filename << endl;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    const string result_folder = "results";
    if (rank == 0) _mkdir(result_folder.c_str());

    vector<int> sizes = { 100, 200, 500, 750, 1000, 1300, 1500, 1700, 2000 };

    string file1 = "matrix1.csv.";
    string file2 = "matrix2.csv.";

    for (int size : sizes) {
        if (rank == 0)
            cout << "\nSubmatrix processing " << size << "x" << size << "..." << endl;

        vector<vector<int>> A, B, result;
        vector<int> A_flat, B_flat(size * size);

        // распределение строк
        vector<int> sendcounts(num_procs), displs(num_procs);
        int offset = 0;
        for (int i = 0; i < num_procs; ++i) {
            int rows = size / num_procs + (i < size% num_procs ? 1 : 0);
            sendcounts[i] = rows * size;
            displs[i] = offset;
            offset += sendcounts[i];
        }

        if (rank == 0) {
            A = readCSV(file1, size);
            B = readCSV(file2, size);
            A_flat.resize(size * size);
            for (int i = 0; i < size; ++i)
                for (int j = 0; j < size; ++j) {
                    A_flat[i * size + j] = A[i][j];
                    B_flat[i * size + j] = B[i][j];
                }
        }

        MPI_Bcast(B_flat.data(), size * size, MPI_INT, 0, MPI_COMM_WORLD);

        int local_elems = sendcounts[rank];
        int local_rows = local_elems / size;

        vector<int> A_local(local_elems);
        vector<int> C_local(local_rows * size, 0);

        MPI_Scatterv(A_flat.data(), sendcounts.data(), displs.data(), MPI_INT,
            A_local.data(), local_elems, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
        auto start = high_resolution_clock::now();

        // умножение блоков
        for (int i = 0; i < local_rows; ++i)
            for (int j = 0; j < size; ++j)
                for (int k = 0; k < size; ++k)
                    C_local[i * size + j] += A_local[i * size + k] * B_flat[k * size + j];

        vector<int> C_flat;
        if (rank == 0)
            C_flat.resize(size * size);

        MPI_Gatherv(C_local.data(), local_rows * size, MPI_INT,
            C_flat.data(), sendcounts.data(), displs.data(), MPI_INT,
            0, MPI_COMM_WORLD);

        auto stop = high_resolution_clock::now();
        double time = duration_cast<milliseconds>(stop - start).count() / 1000.0;

        if (rank == 0) {
            result.resize(size, vector<int>(size));
            for (int i = 0; i < size; ++i)
                for (int j = 0; j < size; ++j)
                    result[i][j] = C_flat[i * size + j];

            string result_filename = result_folder + "result_" + to_string(size) + "x" + to_string(size) + ".csv";
            string time_filename = result_folder + "time_" + to_string(size) + "x" + to_string(size) + ".txt";

            writeCSV(result_filename, result, size);
            writeTimeToFile(time_filename, time);
            cout << "Done! Time is: " << time << " с\n";
        }
    }

    if (rank == 0)
        cout << "\nAll results saved to: " << result_folder << endl;

    MPI_Finalize();
    return 0;
}
