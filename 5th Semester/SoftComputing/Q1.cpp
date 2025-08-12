#include <bits/stdc++.h>
using namespace std;

double FuzzySetA(double x)
{
    if (x < 30)
        return 1.0;
    else if (x > 30 && x < 50)
        return ((x - 30) / (50 - 30));
    else if (x > 50 && x < 70)
        return ((x - 50) / (70 - 50));
    return 0.0;
}

double FuzzySetB(double x)
{
    if (x < 60)
        return 0.0;
    else if (x > 60 && x < 80)
        return ((x - 60) / (80 - 60));
    else if (x > 80 && x < 100)
        return ((x - 80) / (100 - 80));
    return 1.0;
}

double Union(double x)
{
    double A = FuzzySetA(x);
    double B = FuzzySetB(x);
    return max(A, B);
}

double Intersection(double x)
{
    double A = FuzzySetA(x);
    double B = FuzzySetB(x);
    return min(A, B);
}

double complement_A(double x)
{
    double A = FuzzySetA(x);
    return 1 - A;
}

double complement_B(double x)
{
    double B = FuzzySetB(x);
    return 1 - B;
}

double difference_(double x)
{
    double A = FuzzySetA(x);
    double B = FuzzySetB(x);
    return A - B;
}
int main()
{
    double x;
    cout << "Enter Your Speed \n";
    cin >> x;

    cout << "Union: " << Union(x) << endl;
    cout << "Intersection: " << Intersection(x) << endl;
    cout << "Complement of A: " << complement_A(x) << endl;
    cout << "Complement of B: " << complement_B(x) << endl;
    cout << "Difference of A and B: " << difference_(x) << endl;

    return 0;
}