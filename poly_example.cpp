#include "poly.h"

using namespace std;

int main() {
    poly<int, 2> p(2137, 6969);
    cout << "p done" << endl;
    poly<poly<int, 2>, 1> q(p);
    cout << "q done" << endl;
    for (size_t i = 0; i < q.size(); i++) {
        cout << q[i][0] << " " << q[i][1] << endl;
    }

    for (size_t i = 0; i < p.size(); i++) {
        cout << p[i] << endl;
    }

    // poly<int, 2> q(3, 4);
    // poly<int, 2> r = p + q;
    // return 0;
}