/*
Отлично! Тогда отправляю тестовое задание:

Случайным образом задаются сто точек точки внутри окружности. 
Соединить их таким образом, чтобы каждая точка была соединена как минимум с двумя соседними, как максимум - с шестью.

Теперь представим, что мы путешествуем из первой точки в какую-то (задаётся пользователем). 
Но мы хотим посетить все точки и потратить как можно меньше денег. 
Скажем, единица расстояния будет стоить 10 USD. Найти путь, который будет стоить минимум (или близкое к нему количество) денег.

В качестве результата хотелось бы видеть граф, построенный из точек. 
Стоимости переходов. Найденный путь. Будет плюсом, если сможете доказать правильность решения

На тестовое выделяется неделя (5 рабочих дней или 7 календарных)
*/



#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <stdlib.h>
#include <list>
#include <math.h>

using namespace std;

//#define TESTING

class PathFinder {

private:
// structs  --------------------
    struct Dot {
        double x;
        double y;
        Dot() :x(0),y(0){}

        double distance(Dot &other){
            return std::sqrt( (x-other.x)*(x-other.x) + (y-other.y)*(y-other.y) );
        }
        double distance() {
            return std::sqrt( (x)*(x) + (y)*(y) );
        }
    };
    struct Node {
        Node* prev;
        Node* next;
        int st;
        int end;
        double dist;
        Node():prev(NULL),next(NULL),st(-1),end(-1),dist(-1){}
        Node( int sp, int ep, double d ):prev(NULL),next(NULL), st(sp),end(ep),dist(d) {}
        Node(Node* s, Node* e, int sp, int ep, double d) :prev(s),next(e),st(sp),end(ep),dist(d) {}

    };
    
// values   --------------------
    int dotNumber;
    double radius;
    std::vector<Dot> data;

    int destId;
    double pathSum = 0;

    unordered_map<int, set<int>> conns;
    Node* result = NULL;

// funcs    --------------------
    void generatePoints() {

        for (int i = 0; i < dotNumber; ++i) {

            int angle = rand() % 360;
            int dist = rand() * 1.0 / RAND_MAX * radius;

            data[i].x = cos(angle) * dist;
            data[i].y = sin(angle) * dist;
        }
    }

    Node* replace(Node* a, Node*b, Node* replaced, double &dist) {
        replaced->prev->next = a;
        a->prev = replaced->prev;
        a->next = b;
        b->prev = a;
        b->next = replaced->next;
        replaced->next->prev = b;
        dist += a->dist + b->dist - replaced->dist;
        delete replaced;
        return b;
    }

public:
    PathFinder(): dotNumber(0), data(0),destId(-1), radius(0) {}
    ~PathFinder() {
        if (result == NULL) { return; }
        auto last = result->prev;
        while (result != last) {
            Node* tmp = result;
            result = result->next;
            delete tmp;
        }
        delete last;
    }

    void fill(int dn, double _radius) {
        radius = _radius;
        dotNumber = dn;
        data = std::vector<Dot>(100);
        generatePoints();
    }

    void calculate(double cpm, int dest) {
        int startId = 0;
        pathSum = 0;
        destId = dest;
        vector<bool> used(dotNumber);
        
        double cost = cpm;

        Node* pathPart = NULL;

        { // add dest to path
            Dot &d = data[dest];
            pathPart = new Node(startId, destId, data[startId].distance(data[destId]));
            pathPart->next = pathPart;
            pathPart->prev = pathPart;
            pathSum += pathPart->dist;
        }
        
        
        used[startId] = true;
        used[destId] = true;

#ifdef TESTING
    printf("In calculate: main loop starting\n");
#endif

        for (int i = 0; i < dotNumber; ++i) { // iterate through unused points
            if (used[i]) continue;
            
            // search for the closest consequtive points on the path.
            Node* best = pathPart;
            double minDist = radius * 3;
            bool isReplace = true;

            do {
                pathPart = pathPart->next;
                double distSt = data[pathPart->st].distance(data[i]);
                double distEn = data[pathPart->end].distance(data[i]);
#ifdef TESTING
                printf("\tChecking for connection: [%d]->[%d]\n", pathPart->st, pathPart->end);
#endif
                if (minDist < distSt + distEn){
                    minDist = distSt + distEn;
                    best = pathPart;
                    isReplace = true;
                }
            }
            while (pathPart->end != destId);
#ifdef TESTING
            printf("while loop exited.\n");
#endif

            best = replace(
                new Node(best->st, i, data[best->st].distance(data[i])),
                new Node(i, best->end, data[i].distance(data[best->end])),
                best, 
                pathSum
            );
            if (best->end == dest) { pathPart = best; }
            used[i] = true;

#ifdef TESTING
            printf("Connection found for dotId: %d", i);
#endif
        }
        result = pathPart->next;
        
        pathPart = pathPart->next;

        printf("Full cost: %.2lf\n", pathSum * cost);

        do {
            printf("Connection: dot(%d) at [%.2lf, %.2lf] with dot(%d) at [%.2lf, %.2lf]. Cost: %.2lf\n", 
                    pathPart->st, data[pathPart->st].x, data[pathPart->st].y,
                    pathPart->end, data[pathPart->end].x, data[pathPart->end].y,
                    pathPart->dist * cost);
            pathPart = pathPart->next;
        } while (pathPart->end != dest);

    }

};





int main() {

    // srand(0);

    PathFinder pf;

    double radius = 5.0;
    int dotNum = 100;
    double costPerMeter = 10;
    int idOfDest = rand() % dotNum;

    pf.fill(dotNum, radius);

#ifdef TESTING
    printf("pf.fill completed\n");
#endif
    pf.calculate(costPerMeter, idOfDest);

    return 0;
}

