
#include <string.h>

void byte_swap_int(int *v)
{
    int new_val;
    char *in = (char*)v;
    char *out = (char*)&new_val;
    out[0] = in[3];
    out[1] = in[2];
    out[2] = in[1];
    out[3] = in[0];
    memcpy(in, out, 4);
}

void byte_swap_float(float *v)
{
    float new_val;
    char *in = (char*)v;
    char *out = (char*)&new_val;
    out[0] = in[3];
    out[1] = in[2];
    out[2] = in[1];
    out[3] = in[0];
    memcpy(in, out, 4);
}

void byte_swap_double(double *v)
{
    double new_val;
    char *in = (char*)v;
    char *out = (char*)&new_val;
    out[0] = in[7];
    out[1] = in[6];
    out[2] = in[5];
    out[3] = in[4];
    out[4] = in[3];
    out[5] = in[2];
    out[6] = in[1];
    out[7] = in[0];
    memcpy(in, out, 8);
}
