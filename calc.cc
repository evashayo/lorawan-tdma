#include <stdio.h>
#include <math.h>
#include <iostream>

using namespace std;

double calc_pl(double d);
int main()
{
    /*P(RX) = P(TX) - L - F + G(TX) + G(RX)*/
    for (int i = 0; i < 20; i++)
    {
        double d = i * i;
        double PL = calc_pl(d);
        double Ptx = 14; //14dB
        double Xg = 0;
        double L = PL + Xg;
        double F = 0.93;
        double Gtx = 0; //0dBi
        double Grx = 5; //5dBi
        double Prx = Ptx - L - F + Gtx + Grx;
        cout << "Distance(m)=" << d << "; PL=" << PL << "; Prx(dBm)=" << Prx << endl;
    }
    return 0;
}

double calc_pl(double d)
{
    /*PL(d<d_0) = 0 || PL(d_0 <= d < d_1) = PL_0 + 10n_0*log(d/d_0) || PL(d >= d_1) = PL_0 + 10n_0*log(d_1/d_0) + 10n_1*log(d/d_1)*/
    double d0 = 2; //2 metres
    if (d < d0)
        return 0;

    double pl0 = 25, n0 = 2;
    double d1 = 100; //100 metres
    if (d0 <= d && d < d1)
        return pl0 + 10 * n0 * log10(d / d0);

    double n1 = 4;
    return pl0 + 10 * n0 * log10(d1 / d0) + 10 * n1 * log10(d / d1);
}
