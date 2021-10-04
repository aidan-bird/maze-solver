/* aidan bird 2021 */
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "png_utils.h"
#include "graph.h"

#define WHITE 0xffffffff
#define RED 0xff0000ff
#define GET_GOALS(VAR, STARTPTR, SAVEPTR) \
errno = 0; \
VAR = strtoul(STARTPTR, SAVEPTR, 10); \
if (errno) \
    return 1;

char *progName;

void
DrawSolution(VecPath *vp, Img* bmp, uint32_t colour)
{
    size_t i;
    int32_t wx;
    int32_t wy;
    int j;
    int k;

    for (i = 1; i < vp->count; i++) {
        j = vp->start[i]->node->x - vp->start[i - 1]->node->x;
        k = vp->start[i]->node->y - vp->start[i - 1]->node->y;
        if (j)
            j /= abs(j);
        if (k)
            k /= abs(k);
        wx = vp->start[i - 1]->node->x;
        wy = vp->start[i - 1]->node->y;
        while (wx - vp->start[i]->node->x + wy - vp->start[i]->node->y) {
            ((uint32_t **)bmp->grid)[wy][wx] = RED;
            wx += j;
            wy += k;
        }
    }
    ((uint32_t **)bmp->grid)[vp->start[i - 2]->parent->y][vp->start[i - 2]->parent->x] = RED;
}

void
emitError(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    va_end(ap);
}

void
die(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    exit(1);
}

void
emitUsage()
{
    emitError("usage: %s [-f file] [-p startx,starty:endx,endy] [-o file]\n", progName);
}

int
main(int argc, char **argv)
{
    FILE *inputFp;
    FILE *outputFp;
    png_struct *pStruct;
    png_info *pInfo;
    size_t i;
    int oflag;
    int stdoutOutputFlag;
    int pflag;
    int fflag;
    int indexofInputFile;
    int indexofOutputFile;
    int exitState;
    uint32_t startx;
    uint32_t starty;
    uint32_t endx;
    uint32_t endy;
    MazeGraph *mg;
    Maze m;
    VecPath *vp;
    char *saveptr;

    progName = strrchr(argv[0], '/') + 1;
    if (argc <= 1) {
        emitUsage();
        return 1;
    }
    pflag = 0;
    fflag = 0;
    oflag = 0;
    stdoutOutputFlag = 0;
    exitState = 1;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-f")) {
            if (fflag)
                die("Only one maze file can be submitted.\n");
            if (i + 1 >= argc)
                die("No maze file submitted.\n");
            fflag = 1;
            i++;
            indexofInputFile = i;
        }
        else if (!strcmp(argv[i], "-p")) {
            if (pflag)
                die("Only one start-end coordinate can be submitted.\n");
            if (i + 1 >= argc)
                die("No start-end coordinate submitted.\n");
            GET_GOALS(startx, argv[i + 1], &saveptr);
            GET_GOALS(starty, saveptr + 1, &saveptr);
            GET_GOALS(endx, saveptr + 1, &saveptr);
            GET_GOALS(endy, saveptr + 1, &saveptr);
            pflag = 1;
            i++;
        }
        else if (!strcmp(argv[i], "-o")) {
            if (oflag)
                die("Only one output file can be submitted.\n");
            if (i + 1 >= argc)
                die("No output file submitted.\n");
            oflag = 1;
            i++;
            if (!strcmp(argv[i], "-"))
                stdoutOutputFlag = 1;
            else
                indexofOutputFile = i;
        }
    }
    if (!pflag || !fflag || !oflag)
        die("A file, start-end coordinates, and a output file are required.\n");
    if (stdoutOutputFlag)
        outputFp = stdout;
    else if (!(outputFp = fopen(argv[indexofOutputFile], "wb")))
        die("Cannot open output file.\n");
    if (!(m.grid = openPng(argv[indexofInputFile], &inputFp, &pStruct, &pInfo))) {
        emitError("Cannot open file.\n");
        goto error0;
    }
    m.w = png_get_image_width(pStruct, pInfo);
    m.h = png_get_image_height(pStruct, pInfo);
    if (!(mg = mkGraph(&m, startx, starty))) {
        emitError("Cannot build graph.\n");
        goto error1;
    }
    if (!(vp = dijkstra(&mg->nodemat[endy][endx],
        mg, &m))) {
        emitError("Cannot find solution.\n");
        goto error2;
    }
    DrawSolution(vp, &m, RED);
    if ((writePng(8, &m, outputFp))) {
        emitError("Cannot write solution.\n");
        goto error3;
    }
    exitState = 0;
error3:;
    free(vp);
error2:;
    free(mg);
error1:;
    fclose(inputFp);
    png_free(pStruct, m.grid);
    png_destroy_read_struct(&pStruct, &pInfo, NULL);
error0:;
    if (!stdoutOutputFlag)
        fclose(outputFp);
    return exitState;
}

