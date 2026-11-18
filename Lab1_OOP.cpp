
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <stdexcept>
#include <cassert>
#include <algorithm>

using namespace std;

// Класс комплексного числа
class Complex {
private:
    double re, im;

public:
    Complex(double r = 0, double i = 0) : re(r), im(i) {}

    double real() const { return re; }
    double imag() const { return im; }

    Complex operator+(const Complex& other) const {
        return Complex(re + other.re, im + other.im);
    }

    Complex operator-(const Complex& other) const {
        return Complex(re - other.re, im - other.im);
    }

    Complex operator*(const Complex& other) const {
        return Complex(
            re * other.re - im * other.im,
            re * other.im + im * other.re
        );
    }

    Complex operator/(const Complex& other) const {
        double d = other.re * other.re + other.im * other.im;

        if (d == 0)
            throw invalid_argument("Division by zero");

        return Complex(
            (re * other.re + im * other.im) / d,
            (im * other.re - re * other.im) / d
        );
    }

    bool operator==(const Complex& other) const {
        return re == other.re && im == other.im;
    }

    bool operator!=(const Complex& other) const {
        return !(*this == other);
    }

    double abs() const {
        return sqrt(re * re + im * im);
    }

    void print() const {
        cout << re;

        if (im >= 0) cout << " + " << im << "i";
        else cout << " - " << -im << "i";
    }
};

// Квадратный корень комплексного числа
Complex complexSqrt(const Complex& z) {
    double r = sqrt(z.real() * z.real() +
        z.imag() * z.imag());

    double a = sqrt(max(0.0, (r + z.real()) / 2));
    double b = sqrt(max(0.0, (r - z.real()) / 2));

    if (z.imag() < 0) b = -b;

    return Complex(a, b);
}

// Абстрактный класс уравнения
class Equation {
public:
    virtual Complex operator()(const Complex& x) const = 0;

    virtual vector<Complex> solve() const = 0;

    virtual ~Equation() = default;
};

// Линейное уравнение ax + b = 0
class LinearEquation : public Equation {
private:
    double a, b;

public:
    LinearEquation(double a, double b) : a(a), b(b) {
        if (a == 0)
            throw invalid_argument("a cannot be zero");
    }

    Complex operator()(const Complex& x) const override {
        return Complex(a) * x + Complex(b);
    }

    vector<Complex> solve() const override {
        return { Complex(-b / a) };
    }
};

// Квадратное уравнение ax^2 + bx + c = 0
class QuadraticEquation : public Equation {
private:
    double a, b, c;

public:
    QuadraticEquation(double a, double b, double c)
        : a(a), b(b), c(c) {

        if (a == 0)
            throw invalid_argument("a cannot be zero");
    }

    Complex operator()(const Complex& x) const override {
        return Complex(a) * x * x +
            Complex(b) * x +
            Complex(c);
    }

    vector<Complex> solve() const override {
        double d = b * b - 4 * a * c;

        Complex discriminant(d);

        Complex sqrtD = complexSqrt(discriminant);

        Complex x1 = (Complex(-b) + sqrtD) / Complex(2 * a);
        Complex x2 = (Complex(-b) - sqrtD) / Complex(2 * a);

        if (d == 0)
            return { x1 };

        return { x1, x2 };
    }
};

// Коллекция уравнений
class EquationCollection {
private:
    vector<unique_ptr<Equation>> equations;

public:
    void add(unique_ptr<Equation> equation) {
        equations.push_back(move(equation));
    }

    void solveAll() const {
        for (size_t i = 0; i < equations.size(); i++) {
            cout << "\nEquation " << i + 1 << ":\n";

            vector<Complex> roots = equations[i]->solve();

            for (const auto& root : roots) {
                root.print();
                cout << endl;
            }
        }
    }

    void calculateAll(const Complex& x) const {
        cout << "\nValues at x = ";
        x.print();
        cout << endl;

        for (size_t i = 0; i < equations.size(); i++) {
            cout << "Equation " << i + 1 << ": ";

            (*equations[i])(x).print();
            cout << endl;
        }
    }

    void findRoot(const Complex& x) const {
        cout << "\nChecking root ";
        x.print();
        cout << ":\n";

        for (size_t i = 0; i < equations.size(); i++) {
            Complex result = (*equations[i])(x);

            if (result.abs() < 1e-9)
                cout << "Equation " << i + 1
                << " has this root\n";
        }
    }

    size_t size() const {
        return equations.size();
    }
};

// Проверки программы
int main() {
    try {
        EquationCollection collection;

        collection.add(make_unique<LinearEquation>(2, -4));

        collection.add(make_unique<QuadraticEquation>(1, -5, 6));

        collection.add(make_unique<QuadraticEquation>(1, 0, 1));

        cout << "Number of equations: "
            << collection.size() << endl;

        collection.solveAll();

        collection.calculateAll(Complex(2));

        collection.findRoot(Complex(2));

        // Проверка операций комплексных чисел
        Complex z1(2, 3);
        Complex z2(1, 2);

        assert(z1 + z2 == Complex(3, 5));
        assert(z1 - z2 == Complex(1, 1));
        assert(z1 * z2 == Complex(-4, 7));
        assert(z1 / z2 == Complex(1.6, -0.2));
        assert(z1 != z2);

        // Проверка некорректного создания уравнения
        bool error = false;

        try {
            LinearEquation invalid(0, 5);
        }
        catch (const invalid_argument&) {
            error = true;
        }

        assert(error);

        error = false;

        try {
            QuadraticEquation invalid(0, 2, 3);
        }
        catch (const invalid_argument&) {
            error = true;
        }

        assert(error);

        // Проверка деления на ноль
        error = false;

        try {
            Complex result = z1 / Complex(0);
        }
        catch (const invalid_argument&) {
            error = true;
        }

        assert(error);

        cout << "\nAll tests passed!" << endl;
    }
    catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}
