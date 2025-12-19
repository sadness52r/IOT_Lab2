#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

// #ifdef _OPENMP
// #include <omp.h>
// #else
// static inline int omp_get_thread_num(void) { return 0; }
// static inline int omp_get_num_threads(void) { return 4; }
// #endif

#define MAX_ITER 1000

int is_in_mandelbrot(double x0, double y0) {
    double x = 0.0, y = 0.0;
    
    for (int i = 0; i < MAX_ITER; i++) {
        if (x*x + y*y >= 4.0) {
            return 0;
        }
        
        double x_old = x;
        double y_old = y;
        x = x_old * x_old - y_old * y_old + x0;
        y = 2 * x_old * y_old + y0;
    }
    
    return 1;
}

double x_min = -2.0, x_max = 0.5;
double y_min = -1.25, y_max = 1.25;

void compute_mandelbrot(
    int nthreads,
    int npoints,
    double *points_x,
    double *points_y,
    int write_points
) {
    #pragma omp parallel num_threads(nthreads)
    {
        unsigned int seed = (unsigned int)time(NULL) ^ omp_get_thread_num();

        #pragma omp for schedule(dynamic,1)
        for (int i = 0; i < npoints; i++) {
            double x0, y0;
            int found = 0;

            while (!found) {
                x0 = x_min + (x_max - x_min) * rand_r(&seed) / (double)RAND_MAX;
                y0 = y_min + (y_max - y_min) * rand_r(&seed) / (double)RAND_MAX;
                found = is_in_mandelbrot(x0, y0);
            }

            if (write_points) {
                points_x[i] = x0;
                points_y[i] = y0;
            }
        }
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <nthreads> <npoints>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const int nthreads = atoi(argv[1]);
    if (nthreads <= 0) {
        fprintf(stderr, "Error: nthreads must be a positive integer.\n");
        return EXIT_FAILURE;
    }

    omp_set_num_threads(nthreads);

    const int npoints = atoi(argv[2]);
    if (npoints <= 0) {
        fprintf(stderr, "Error: npoints must be a positive integer.\n");
        return EXIT_FAILURE;
    }
    
    int runs = 5;
    double sum = 0.0;

    for (int r = 0; r < runs; r++) {
        double t0 = omp_get_wtime();
        compute_mandelbrot(nthreads, npoints, NULL, NULL, 0);
        sum += omp_get_wtime() - t0;
    }

    double avg_time = sum / runs;
    fprintf(stdout, "%f\n", avg_time);

    double *points_x = malloc(npoints * sizeof(double));
    double *points_y = malloc(npoints * sizeof(double));

    compute_mandelbrot(nthreads, npoints, points_x, points_y, 1);

    FILE *file = fopen("mandelbrot_points.csv", "w");
    for (int i = 0; i < npoints; i++) {
        fprintf(file, "%f,%f\n", points_x[i], points_y[i]);
    }
    fclose(file);

    free(points_x);
    free(points_y);
    return 0;
}