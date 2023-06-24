#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <limits>

using namespace std;

class Helper
{
    public:
        static float small(float val)
        {
            float result = fabs(val) * numeric_limits<float>::epsilon() * 6;

            return result;
        }

        static vector<float> calR(vector<float> cp, vector<float> cd, float fgxmin, float fgymin, float fgx, float fgy)
        {

            float R1 = -1.0;
            if (cd[0] != 0.0f && fgx-cp[0] != 0.0f)
            {
                float R1T = (fgx-cp[0])/cd[0];
                if (R1T*cd[1] + cp[1] >= fgymin-small(fgymin) && R1T*cd[1] + cp[1] <= fgy+small(fgy) && R1T > 0.0f)
                {
                    R1 = R1T;
                }
            }

            float R2 = -1.0;
            if (cd[0] != 0.0f && 0.0f-cp[0] != 0.0f)
            {
                float R2T = (fgxmin-cp[0])/cd[0];
                if (R2T*cd[1] + cp[1] >= fgymin-small(fgymin) && R2T*cd[1] + cp[1] <= fgy+small(fgy) && R2T > 0.0f)
                {
                    R2 = R2T;
                }
            }

            float R3 = -1.0;
            if (cd[1] != 0.0f && fgy-cp[1] != 0.0f)
            {
                float R3T = (fgy-cp[1])/cd[1];
                if (R3T*cd[0] + cp[0] >= fgxmin-small(fgxmin) && R3T*cd[0] + cp[0] <= fgx+small(fgx) && R3T > 0.0f)
                {
                    R3 = R3T;
                }
            }

            float R4 = -1.0;
            if (cd[1] != 0.0f && fgymin-cp[1] != 0.0f)
            {
                float R4T = (fgymin-cp[1])/cd[1];
                if (R4T*cd[0] + cp[0] >= fgxmin-small(fgxmin) && R4T*cd[0] + cp[0] <= fgx+fgx+small(fgx) && R4T > 0.0f)
                {
                    R4 = R4T;
                }
            }

            float Rin = minNotNeg(R1,minNotNeg(R2,minNotNeg(R3,R4)));
            float Rout = max(R1,max(R2,max(R3,R4)));
            vector<float> result = {Rin, Rout};
            return result;
        }

        static float minNotNeg(float x, float y)
        {
            if (x < 0.0f) 
            { return y; }
            else if (y < 0.0f)
            { return x; }
            else if (x > y)
            { return y; }
            else
            { return x; }
        }

        static float RandomFloat(float a, float b) 
        {
            float randomNumb = ((float)rand()) / (float)RAND_MAX;
            float difference = b - a;
            float result = randomNumb * difference;
            return a + result;
        }

};