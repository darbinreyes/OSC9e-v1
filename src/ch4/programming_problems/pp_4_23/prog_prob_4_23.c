#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <omp.h>
/*


libomp has the following notes:
    To use this OpenMP library:
     * For clang-3.8+, or clang-3.7 with +openmp variant:
        add "-fopenmp" during compilation / linking.
     * For clang-3.7 without +openmp variant, use:
        "-I/opt/local/include/libomp -L/opt/local/lib/libomp -fopenmp"


*/
/*

    The unit circle described in the problem is has a radius of 1 and is
    inscribed in a square whose sides equal 2. Therefore, to generate a
    random x coordinate, we can multiply a random fraction by 2 (the size of the
    square's side) and add the product to the lesser x coordinate, which is -1.
    We can generate a random y coordinate in exactly the same way.

    Next, we must determine if the randomly generated point falls within the
    circle. Since the radius of the circle is 1, all we need to do is compute
    the distance of the random point from the the origin (x,y) = (0,0), we can
    use Pythagoras theorem for that, finally, if that distance is less than 1
    we know that the point has fallen within the circle. We shall consider
    points that are exactly a distance 1 from the origin to be outside of the
    circle.

    Finally, we compute the estimation of pi with:

    4.0*num_inside/num_outside.


*/


double get_rand_fraction(void) {
    double frn;
    double frd;

    frn = (double) arc4random_uniform(UINT32_MAX);
    frd = (double) UINT32_MAX;


    frn = (frn/frd);

    return frn;
}

typedef struct _point_t {
    double x;
    double y;
} point_t;

void get_rand_point(point_t *p) {
    p->x = -1.0 + 2.0 * get_rand_fraction();
    p->y = -1.0 + 2.0 * get_rand_fraction();
}

int is_inside_circle(point_t *p){
    double xs, ys, s;

    xs = p->x * p->x;
    ys = p->y * p->y;

    s = sqrt(xs + ys);

    return s < 1.0;
}

double inside = 0.0;
double t = 0.0;

void *inside_count(long const * const np) {
    point_t p;
    long n;

    n = *np;
    t += (double) n;
    while (n-- > 0) {
        get_rand_point(&p);
        if (is_inside_circle(&p))
            inside++;
    }

    return NULL;
}



int main(int argc, char **argv) {
    long num_points;
    int i;

    if (argc <= 1 || argc > 2) {
        printf("Usage: Provide 1 positive integer. Example: ./a.out 7.\n4 times as many points will be generated, since 4 threads are created e.g. if you enter 10, then 40 points are generated.\n");
        return -1;
    }

    num_points = strtol(argv[1], NULL, 10);

    if (num_points < 1) {
        printf("Failed to convert \"%s\" to a positive integer.\n", argv[1]);
        return -1;
    }

    #pragma omp parallel
    {
        inside_count(&num_points);
    }

    printf("num_points = %.12f.\n", t);

/*
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_attr_init(&attr[i]);
        pthread_create(&tid[i], &attr[i], ( void * (*)(void *)  )inside_count, &num_points);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(tid[i], NULL);
    }
*/
    t = 4.0 * inside / t;

    printf("Pi ~= %.12f. It should be close to 3.141592653589793.\n", t);

    return 0;
}