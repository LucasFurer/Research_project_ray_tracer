#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <limits>

using namespace std;

class VecMeth
{
    public:

        static float dotProduct3(vector<float> v1,vector<float> v2)
        {
            return v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2];
        }

        static vector<float> calcNormal(vector<float> a, vector<float> b, vector<float> c)
        {
            vector<float> N = normalize(crossProduct(vector<float>{b[0]-a[0],b[1]-a[1],b[2]-a[2]},vector<float>{c[0]-a[0],c[1]-a[1],c[2]-a[2]}));
            if (N[2] < 0) 
            { 
                N[0] = -N[0];
                N[1] = -N[1];
                N[2] = -N[2];
            }
            return N;
        }

        static vector<float> bisection(vector<float> v1, vector<float> v2)
        {
            float v1l = length(v1);
            float v2l = length(v2);
            return vector<float>{v2l*v1[0] + v1l*v2[0], v2l*v1[1] + v1l*v2[1], v2l*v1[2] + v1l*v2[2]};
        }

        static float det3(vector<float> v1,vector<float> v2,vector<float> v3)
        {
            return (v1[0] * det2(vector<float>{v2[1], v2[2]}, vector<float>{v3[1], v3[2]})) + (-1.0f * v2[0] * det2(vector<float>{v1[1], v2[2]}, vector<float>{v3[1], v3[2]})) + (v3[0] * det2(vector<float>{v1[1], v2[2]}, vector<float>{v2[1], v3[2]}));
        }

        static float det2(vector<float> v1,vector<float> v2)
        {
            return (v1[0]*v2[1])-(v2[0]*v1[1]);
        }

        static vector<float> crossProduct(vector<float> a, vector<float> b)
        {
            return vector<float>{a[1]*b[2]-(a[2]*b[1]), -1*(a[0]*b[2]-(a[2]*b[0])), a[0]*b[1]-(a[1]*b[0])};
        }

        static vector<float> normalize(vector<float> a)
        {
            float L = length(a);
            return vector<float>{a[0]/L, a[1]/L, a[2]/L};
        }

        static float length(vector<float> a)
        {
            return sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
        }

        static void printVec(vector<float> a)
        {
            cout << "x: " << a[0] << endl;
            cout << "y: " << a[1] << endl;
            cout << "z: " << a[2] << endl;
        }



        static vector<vector<float>> tesselate(vector<float> o,vector<float> d, vector<float> a, vector<float> aN, vector<float> b, vector<float> bN, vector<float> c, vector<float> cN, vector<float> lightPos, vector<float> lightCol, vector<float> fieldCol, int tessAmount, bool smooth, bool renderNormalVar, int xTestV, int yTestV)
        {
            if (xTestV == 368 && yTestV == 101)
            {
                //cout << "tesselate start-----------" << endl;
            }


            vector<vector<float>> result = {
                vector<float>{-1,-1,-1},
                vector<float>{0,0,0},
                vector<float>{0,0,0},
                vector<float>{0,0,0}
            };

            vector<float> betaGammaT = calcBetaGammaT(o,d,a,b,c);
            float beta = betaGammaT[0];
            float gamma = betaGammaT[1];
            float t = betaGammaT[2];

            float alpha = 1-beta-gamma;

            //int totalSize = (tessAmount*tessAmount+tessAmount)/2;

            //vector<vector<vector<float>>> newVertices = {};
            //vector<vector<vector<float>>> newVerticesNormals = {};

            vector<float> newVertices[tessAmount][tessAmount] = {};
            vector<float> newVerticesNormals[tessAmount][tessAmount] = {};


            for (int y = 0; y < tessAmount; y++)
            {
                for (int x = 0; x < tessAmount-y; x++)
                {
                    float betaTemp = (float)x/(tessAmount-1);
                    float gammaTemp = (float)y/(tessAmount-1);
                    float alphaTemp = 1-betaTemp-gammaTemp;

                    vector<float> newVertex = {
                        a[0] + betaTemp*(b[0]-a[0]) + gammaTemp*(c[0]-a[0]),
                        a[1] + betaTemp*(b[1]-a[1]) + gammaTemp*(c[1]-a[1]),
                        a[2] + betaTemp*(b[2]-a[2]) + gammaTemp*(c[2]-a[2])
                        };


                    vector<float> aToNewVer = {newVertex[0]-a[0],newVertex[1]-a[1],newVertex[2]-a[2]};
                    float distanceA = dotProduct3(aToNewVer, aN);
                    vector<float> newVertexA = vector<float>{newVertex[0]-distanceA*aN[0],newVertex[1]-distanceA*aN[1],newVertex[2]-distanceA*aN[2]};

                    vector<float> bToNewVer = {newVertex[0]-b[0],newVertex[1]-b[1],newVertex[2]-b[2]};
                    float distanceB = dotProduct3(bToNewVer, bN);
                    vector<float> newVertexB = vector<float>{newVertex[0]-distanceB*bN[0],newVertex[1]-distanceB*bN[1],newVertex[2]-distanceB*bN[2]};

                    vector<float> cToNewVer = {newVertex[0]-c[0],newVertex[1]-c[1],newVertex[2]-c[2]};
                    float distanceC = dotProduct3(cToNewVer, cN);
                    vector<float> newVertexC = vector<float>{newVertex[0]-distanceC*cN[0],newVertex[1]-distanceC*cN[1],newVertex[2]-distanceC*cN[2]};


                    if (x == 0 || y == 0 || x == tessAmount-y-1)
                    {
                        newVertices[x][y] = vector<float>{
                            newVertex[0],
                            newVertex[1],
                            alphaTemp*newVertexA[2] + betaTemp*newVertexB[2] + gammaTemp*newVertexC[2]
                        };

                        newVerticesNormals[x][y] = vector<float>{
                            alphaTemp*aN[0] + betaTemp*bN[0] + gammaTemp*cN[0],
                            alphaTemp*aN[1] + betaTemp*bN[1] + gammaTemp*cN[1],
                            alphaTemp*aN[2] + betaTemp*bN[2] + gammaTemp*cN[2]
                        };

                        
                    }
                    else
                    {
                        newVertices[x][y] = vector<float>{
                            alphaTemp*newVertexA[0] + betaTemp*newVertexB[0] + gammaTemp*newVertexC[0],
                            alphaTemp*newVertexA[1] + betaTemp*newVertexB[1] + gammaTemp*newVertexC[1],
                            alphaTemp*newVertexA[2] + betaTemp*newVertexB[2] + gammaTemp*newVertexC[2]
                        };

                        newVerticesNormals[x][y] = vector<float>{
                            alphaTemp*aN[0] + betaTemp*bN[0] + gammaTemp*cN[0],
                            alphaTemp*aN[1] + betaTemp*bN[1] + gammaTemp*cN[1],
                            alphaTemp*aN[2] + betaTemp*bN[2] + gammaTemp*cN[2]
                        };
                    }

                    
                }
            }


            float closestT = numeric_limits<float>::infinity();

            for (int y = 0; y < tessAmount; y++)
            {
                for (int x = 0; x < tessAmount-y; x++)
                {
                    if (x+1 < tessAmount-y && y+1 < tessAmount-x)
                    {
                        betaGammaT = calcBetaGammaT(o,d,newVertices[x][y],newVertices[x+1][y],newVertices[x][y+1]);
                        beta = betaGammaT[0];
                        gamma = betaGammaT[1];
                        t = betaGammaT[2];

                        if (beta >= 0.0f && gamma >= 0.0f && beta + gamma <= 1.0f && t < closestT && t > 0.0f)
                        {
                            result = phong(o,d,newVertices[x][y],newVerticesNormals[x][y],newVertices[x+1][y],newVerticesNormals[x+1][y],newVertices[x][y+1],newVerticesNormals[x][y+1],lightPos,lightCol,fieldCol,beta,gamma,smooth,renderNormalVar, xTestV, yTestV);
                            closestT = t;
                        }
                    }

                    if (x+1 < tessAmount-y && y+1 < tessAmount-(x+1))
                    {
                        betaGammaT = calcBetaGammaT(o,d,newVertices[x+1][y+1],newVertices[x+1][y],newVertices[x][y+1]);
                        beta = betaGammaT[0];
                        gamma = betaGammaT[1];
                        t = betaGammaT[2];

                        if (beta >= 0.0f && gamma >= 0.0f && beta + gamma <= 1.0f && t < closestT &&  t > 0.0f)
                        {
                            result = phong(o,d,newVertices[x+1][y+1],newVerticesNormals[x+1][y+1],newVertices[x+1][y],newVerticesNormals[x+1][y],newVertices[x][y+1],newVerticesNormals[x][y+1],lightPos,lightCol,fieldCol,beta,gamma,smooth,renderNormalVar, xTestV, yTestV);
                            closestT = t;
                        }
                    }
                }
            }

            result[3] = vector<float>{closestT,closestT,closestT};

            if (xTestV == 368 && yTestV == 101)
            {
                //printVec(result[0]);
                //cout << "tesselate end-----------" << endl;
            }

            return result;
        }

        static vector<float> calcBetaGammaT(vector<float> o, vector<float> d, vector<float> a, vector<float> b, vector<float> c)
        {
            float eihf = (a[1]-c[1])*(d[2])-(d[1])*(a[2]-c[2]);
            float gfdi = (d[0])*(a[2]-c[2])-(a[0]-c[0])*(d[2]);
            float dheg = (a[0]-c[0])*(d[1])-(a[1]-c[1])*(d[0]);

            float akjb = (a[0]-b[0])*(a[1]-o[1])-(a[0]-o[0])*(a[1]-b[1]);
            float jcal = (a[0]-o[0])*(a[2]-b[2])-(a[0]-b[0])*(a[2]-o[2]);
            float blkc = (a[1]-b[1])*(a[2]-o[2])-(a[1]-o[1])*(a[2]-b[2]);

            float M = (a[0]-b[0])*eihf + (a[1]-b[1])*gfdi + (a[2]-b[2])*dheg;
            if (M == 0) {return vector<float>{-1,-1,-1};}
            float beta = ((a[0]-o[0])*eihf + (a[1]-o[1])*gfdi + (a[2]-o[2])*dheg)/M;
            float gamma = ((d[2])*akjb + (d[1])*jcal + (d[0])*blkc)/M;
            float t = -1.0f*((a[2]-c[2])*akjb + (a[1]-c[1])*jcal + (a[0]-c[0])*blkc)/M;

            return vector<float>{beta, gamma, t};
        }

        static vector<vector<float>> phong(vector<float> o,vector<float> d, vector<float> a, vector<float> aN, vector<float> b, vector<float> bN, vector<float> c, vector<float> cN, vector<float> lightPos, vector<float> lightCol, vector<float> fieldCol, float beta, float gamma, bool smooth, bool renderNormalVar, int xTestV, int yTestV)
        {
            vector<vector<float>> result = {
                vector<float>{1,1,1},
                vector<float>{0,0,0},
                vector<float>{0,0,0},
                vector<float>{0,0,0}
            };

            //ambient----------------------------------------------------------------------------------------------------------------
            float ambient = 0.05f;

            //diffuse--------------------------------------------------------------------------------------------------------------------
            // n = normal at point of surface      l = normalized vector from point to light source
            //diffuse = max(dot(N,L),0)*lightcolor;
            float alpha = 1-beta-gamma;
            vector<float> N;// = calcNormal(a,b,c);
            if (smooth) 
            { 
                N = vector<float>{
                    alpha*aN[0] + beta*bN[0] + gamma*cN[0],
                    alpha*aN[1] + beta*bN[1] + gamma*cN[1],
                    alpha*aN[2] + beta*bN[2] + gamma*cN[2]
                }; 
            } 
            else 
            { 
                N = calcNormal(a,b,c); 
            }

            if (xTestV == 250 && yTestV == 90)
            {
                //cout << "N: " << endl;
                //printVec(N);
            }
            
            vector<float> hitPoint = vector<float>{a[0] + beta*(b[0]-a[0]) + gamma*(c[0]-a[0]),a[1] + beta*(b[1]-a[1]) + gamma*(c[1]-a[1]),a[2] + beta*(b[2]-a[2]) + gamma*(c[2]-a[2])};
            vector<float> L = normalize(vector<float>{lightPos[0]-hitPoint[0],lightPos[1]-hitPoint[1],lightPos[2]-hitPoint[2]});
            float diffuse = max(dotProduct3(N,L),0.0f);

            //specular--------------------------------------------------------------------------------------------
            //n = normal at point of surface        h = normalized vector between (bisecting) camera and light      k = constant (32, 64 128) makes highlight bigger or smaller
            //specular = exp(max(dot(n,h),0), k) * lightcolor
            float specularConst = 256.0f;
            vector<float> hitToLight = vector<float>{lightPos[0]-hitPoint[0], lightPos[1]-hitPoint[1], lightPos[2]-hitPoint[2]};
            vector<float> hitToCam = vector<float>{o[0]-hitPoint[0], o[1]-hitPoint[1], o[2]-hitPoint[2]};
            vector<float> H = normalize(bisection(hitToLight, hitToCam));
            float specular = powf(max(dotProduct3(N,H),0.0f), specularConst);
            if (isnan(specular)) { specular = 0; }
            specular = 0.0f;

            //phong----------------------------------------------------------------------------------------------------
            vector<float> color = vector<float>{
                min((ambient*lightCol[0] + diffuse*lightCol[0] + specular*lightCol[0])*fieldCol[0],1.0f),
                min((ambient*lightCol[1] + diffuse*lightCol[1] + specular*lightCol[1])*fieldCol[1],1.0f),
                min((ambient*lightCol[2] + diffuse*lightCol[2] + specular*lightCol[2])*fieldCol[2],1.0f)
                };

            //vector<float> nonSmoothN = calcNormal(a,b,c);
            
            N = normalize(N);
            if (renderNormalVar) 
            {
                result[0] = vector<float>{(N[0]+1.0f)/2.0f,(N[1]+1.0f)/2.0f,(N[2]+1.0f)/2.0f}; 
            } 
            else 
            { 
                result[0] = color; 
            }
            
            float dotDN = dotProduct3(d,N);
            result[1] = normalize(vector<float>{
                d[0] - (2*dotDN*N[0]),
                d[1] - (2*dotDN*N[1]),
                d[2] - (2*dotDN*N[2])
            });
            result[2] = hitPoint;

            if (xTestV == 368 && yTestV == 101)
            {
                //printVec(N);
                //printVec(result[0]);
                //printVec(result[1]);
                //printVec(result[2]);
                //cout << "tesselate end-----------" << endl;
            }

            return result;
        }
    
};