#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define G 6.67430e-11
#define DT 50.0

typedef struct {
    double m;
    double x, y;
    double vx, vy;
} Body;

Body* read_input(const char* filename, int* n) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Ошибка открытия входного файла");
        exit(1);
    }

    fscanf(f, "%d", n);
    Body* bodies = (Body*)malloc((*n) * sizeof(Body));

    for (int i = 0; i < *n; i++) {
        fscanf(f, "%lf %lf %lf %lf %lf",
               &bodies[i].m,
               &bodies[i].x, &bodies[i].y,
               &bodies[i].vx, &bodies[i].vy);
    }

    fclose(f);
    return bodies;
}

double simulate(double tend, Body* bodies, int n,
                const char* out_csv, int write_output)
{
    int steps = (int)(tend / DT);

    double* fx = (double*)malloc(n * sizeof(double));
    double* fy = (double*)malloc(n * sizeof(double));

    FILE* out = NULL;
    if (write_output) {
        out = fopen(out_csv, "w");
        fprintf(out, "t");
        for (int i = 1; i <= n; i++) {
            fprintf(out, ",x%d,y%d", i, i);
        }
        fprintf(out, "\n");
    }

    double t_start = omp_get_wtime();

    for (int step = 0; step <= steps; step++) {
        double t = step * DT;

        if (write_output) {
            fprintf(out, "%.10f", t);
            for (int i = 0; i < n; i++)
                fprintf(out, ",%.10f,%.10f", bodies[i].x, bodies[i].y);
            fprintf(out, "\n");
        }

        #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            fx[i] = 0.0;
            fy[i] = 0.0;
        }

        #pragma omp parallel for schedule(static)
        for (int i = 0; i < n; i++) {
            double fxq = 0.0, fyq = 0.0;
            for (int j = i + 1; j < n; j++) {
                double dx = bodies[j].x - bodies[i].x;
                double dy = bodies[j].y - bodies[i].y;
                double r2 = dx*dx + dy*dy + 1e-12;
                double r = sqrt(r2);

                double f = G * bodies[i].m * bodies[j].m / (r2 * r);

                double fx_ij = f * dx;
                double fy_ij = f * dy;

                fxq += fx_ij;
                fyq += fy_ij;

                #pragma omp atomic
                fx[j] -= fx_ij;
                #pragma omp atomic
                fy[j] -= fy_ij;
            }
            fx[i] += fxq;
            fy[i] += fyq;
        }

        #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            double vx_old = bodies[i].vx;
            double vy_old = bodies[i].vy;

            bodies[i].x += vx_old * DT;
            bodies[i].y += vy_old * DT;

            bodies[i].vx += (fx[i] / bodies[i].m) * DT;
            bodies[i].vy += (fy[i] / bodies[i].m) * DT;
        }
    }

    double t_end = omp_get_wtime();

    if (write_output) fclose(out);
    free(fx);
    free(fy);

    return t_end - t_start;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Использование: ./a.out nthreads tend input.txt\n");
        return 1;
    }

    double nthreads = atoi(argv[1]);
    double tend = atof(argv[2]);
    const char* input = argv[3];
    const char* output = "output.csv";
    int runs = 5;

    omp_set_num_threads(nthreads);

    int n;
    Body* bodies_orig = read_input(input, &n);

    double total_time = 0.0;

    for (int r = 0; r < runs; r++) {
        Body* bodies = (Body*)malloc(n * sizeof(Body));
        for (int i = 0; i < n; i++)
            bodies[i] = bodies_orig[i];

        double t = simulate(tend, bodies, n, output, r == 0);
        total_time += t;

        free(bodies);
    }

    printf("Среднее время выполнения: %.6f секунд\n",
           total_time / runs);

    free(bodies_orig);
    return 0;
}
