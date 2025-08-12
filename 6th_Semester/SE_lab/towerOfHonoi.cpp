#include <iostream>
#include <chrono>
#include <fstream>
using namespace std;
using namespace std::chrono;

void towerOfHonai(int n, char from, char to, char usin)
{
    if (n == 1)
        return;

    towerOfHonai(n - 1, from, usin, to);
    towerOfHonai(n - 1, usin, to, from);
}
int main()
{
    ofstream myFile("output.csv");
    myFile << "Input Size" << " " << " Running time\n";
    for (int i = 1; i < 32; i++)
    {
        auto start = high_resolution_clock::now();
        towerOfHonai(i, 'A', 'C', 'B');
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        myFile << i << "          " << duration.count() << endl;
    }

    return 0;
}