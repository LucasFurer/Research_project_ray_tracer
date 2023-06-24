#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <limits>
#include "Helper.cpp"
#include "VecMeth.cpp"
#include <iomanip>

using namespace std;

//---------------------------settings---------------------------------------
int width = 1980/4;
int height = 1080/4;

//float POV = 0.025;
float POV = 0.004;

int tessellationAmount = 2;//2 means no tessellation

bool smooth = false;

bool perspectiveCam = true;

bool renderBounceAmount = false;

bool renderNormal = false;

int bounceAmount = 8;

const int gridx = 2;
const int gridy = 2;

int degrees = 100;//resolution of the angle output

//vector<float> camPosOriginal = vector<float>{10.0f,4.0f,6.5f};
vector<float> camPosOriginal = vector<float>{1.0f,-1.0f,3.25f};
//vector<float> camDirOriginal = VecMeth::normalize(vector<float>{-2.3f,0.0f,-2.0f});
vector<float> camDirOriginal = VecMeth::normalize(vector<float>{0.0f,1.0f,-1.0f});

vector<float> lightPos = vector<float>{15,15,20};
vector<float> lightCol = vector<float>{1,1,1};

//----------------------------------------------------------------------------------------------------

vector<float> U = VecMeth::normalize(VecMeth::crossProduct(camDirOriginal, vector<float>{0.0f,0.0f,1.0f}));
vector<float> V = VecMeth::normalize(VecMeth::crossProduct(camDirOriginal, U));

float fgridx = (float) gridx;
float fgridy = (float) gridy;

float heightField[gridx+1][gridy+1] = {};
vector<float> heightFieldNormals[gridx+1][gridy+1] = {};
vector<float> fieldColor = vector<float>{1,1,1};

float minRandom = 0.5;
float maxRandom = 2;

int bounceFailure = 0;

int xTestV = 0;
int yTestV = 0;

vector<vector<float>> castRay(vector<float>, vector<float>, int, bool);

const float pie = 3.14159265358979323846f;

int main()
{
    float angles_output_array[degrees][degrees] = {}; 

    for (int x = 0; x < degrees; x++)
    {
        for (int y = 0; y < degrees; y++)
        {
            angles_output_array[x][y] = 0.0f;
        }
    }


    vector<float> camOpposite = {
        -1.0f*camDirOriginal[0],
        -1.0f*camDirOriginal[1],
        -1.0f*camDirOriginal[2]
    };

    float rCamTemp = sqrt(camOpposite[0] * camOpposite[0] + camOpposite[1] * camOpposite[1] + camOpposite[2] * camOpposite[2]);
    float angle1CamTemp = acos(camOpposite[0] / sqrt(camOpposite[0] * camOpposite[0] + camOpposite[1] * camOpposite[1])) * (camOpposite[1] < 0 ? -1 : 1);
    float angle2CamTemp = acos(camOpposite[2] / rCamTemp);

    angle1CamTemp = degrees * (angle1CamTemp + pie) / (2.0f*pie);
    angle2CamTemp = degrees * (angle2CamTemp) / (pie);

    int camxRemember = (int)floor(angle1CamTemp);
    int camyRemember = (int)floor(angle2CamTemp);

    

    for (int x = 0; x < gridx+1; x++)
    {
        for (int y = 0; y < gridy+1; y++)
        {
            heightField[x][y] = Helper::RandomFloat(minRandom, maxRandom);
        }
    }
    /*
    heightField[0][0] = 0.3f;
    heightField[0][1] = 1.2f;
    heightField[0][2] = 2.0f;

    heightField[1][0] = 1.2f;
    heightField[1][1] = 1.2f;
    heightField[1][2] = 0.8f;

    heightField[2][0] = 1.2f;
    heightField[2][1] = 1.2f;
    heightField[2][2] = 2.0f;
    */
    for (int x = 0; x < gridx+1; x++)
    {
        for (int y = 0; y < gridy+1; y++)
        {
            vector<float> N1 = {0.0f,0.0f,0.0f};
            vector<float> N2 = {0.0f,0.0f,0.0f};
            vector<float> N3 = {0.0f,0.0f,0.0f};
            vector<float> N4 = {0.0f,0.0f,0.0f};
            vector<float> N5 = {0.0f,0.0f,0.0f};
            vector<float> N6 = {0.0f,0.0f,0.0f};


            if (x-1 >= 0 && y-1 >= 0)
            {
                N1 = VecMeth::calcNormal(vector<float>{float(x),float(y),heightField[x][y]},vector<float>{float(x-1),float(y),heightField[x-1][y]},vector<float>{float(x),float(y-1),heightField[x][y-1]});
            }

            if (x+1 <= gridx && y-1 >= 0)
            {
                N2 = VecMeth::calcNormal(vector<float>{float(x+1),float(y),heightField[x+1][y]},vector<float>{float(x+1),float(y-1),heightField[x+1][y-1]},vector<float>{float(x),float(y),heightField[x][y]});
                N3 = VecMeth::calcNormal(vector<float>{float(x),float(y-1),heightField[x][y-1]},vector<float>{float(x+1),float(y-1),heightField[x+1][y-1]},vector<float>{float(x),float(y),heightField[x][y]});
            }

            if (x-1 >= 0 && y+1 <= gridy)
            {
                N4 = VecMeth::calcNormal(vector<float>{float(x-1),float(y),heightField[x-1][y]},vector<float>{float(x-1),float(y+1),heightField[x-1][y+1]},vector<float>{float(x),float(y),heightField[x][y]});
                N5 = VecMeth::calcNormal(vector<float>{float(x),float(y+1),heightField[x][y+1]},vector<float>{float(x-1),float(y+1),heightField[x-1][y+1]},vector<float>{float(x),float(y),heightField[x][y]});
            }

            if (x+1 <= gridx && y+1 <= gridy)
            {
                N6 = VecMeth::calcNormal(vector<float>{float(x),float(y),heightField[x][y]},vector<float>{float(x+1),float(y),heightField[x+1][y]},vector<float>{float(x),float(y+1),heightField[x][y+1]});
            }
            
            vector<float> sum = vector<float>{N1[0]+N2[0]+N3[0]+N4[0]+N5[0]+N6[0], N1[1]+N2[1]+N3[1]+N4[1]+N5[1]+N6[1], N1[2]+N2[2]+N3[2]+N4[2]+N5[2]+N6[2]};
            vector<float> normalSum = VecMeth::normalize(sum);
            heightFieldNormals[x][y] = normalSum;
        }
    }


    
    ofstream render;
    render.open("render.ppm");




    if(render.is_open())
    {
        string res = to_string(width) + " " + to_string(height);
        render << "P3" << endl;
        render << res << endl;
        render << "255" << endl;

        float rCount = 0;
        for (int y = 0; y < height; y++)//width
        {
            for (int x = 0; x < width; x++)//height
            {
                xTestV = x;
                yTestV = y;

                if (xTestV == 302 && yTestV == 197)
                {
                    //cout << "hello" << endl;
                }




                //cout << "x: " << x << endl;
                //cout << "y: " << y << endl;


                float xoff = 0;
                float yoff = 0;
                if (width % 2 == 0) { xoff = 0.5; }
                if (height % 2 == 0) { yoff = 0.5; }

                float centrex = xoff+x-width/2;
                float centrey = yoff+y-height/2;

                vector<float> camPos;
                vector<float> camDir;

                if (perspectiveCam)
                {
                    camPos = {camPosOriginal[0],camPosOriginal[1],camPosOriginal[2]};
                    camDir = VecMeth::normalize(vector<float>{camDirOriginal[0] + centrex*U[0]*POV + centrey*V[0]*POV, camDirOriginal[1] + centrex*U[1]*POV + centrey*V[1]*POV, camDirOriginal[2] + centrex*U[2]*POV + centrey*V[2]*POV});
                }
                else
                {
                    camPos = {camPosOriginal[0] + centrex*U[0]*POV + centrey*V[0]*POV, camPosOriginal[1] + centrex*U[1]*POV + centrey*V[1]*POV, camPosOriginal[2] + centrex*U[2]*POV + centrey*V[2]*POV};
                    camDir = {camDirOriginal[0], camDirOriginal[1], camDirOriginal[2]};   
                }


                vector<vector<float>> hitData = castRay(camDir, camPos, bounceAmount, renderNormal);



                if (hitData[3][0] != 9.0f && hitData[3][0] != 8.0f && hitData[3][0] != 0.0f && hitData[3][0] != (float)bounceAmount)
                {



                    if (hitData[1][2] < 0.0f)
                    {
                        //cout << "x: " << x << endl;
                        //cout << "y: " << y << endl;
                        //cout << "bounce goes down" << endl;
                    }



                    float r = sqrt(hitData[1][0] * hitData[1][0] + hitData[1][1] * hitData[1][1] + hitData[1][2] * hitData[1][2]);



                    float angle1 = acos(hitData[1][0] / sqrt(hitData[1][0] * hitData[1][0] + hitData[1][1] * hitData[1][1])) * (hitData[1][1] < 0 ? -1 : 1);
                    float angle2 = acos(hitData[1][2] / r);




                    angle1 = degrees * (angle1 + pie) / (2.0f*pie);
                    angle2 = degrees * (angle2) / (pie);

                    if (xTestV == 302 && yTestV == 197)
                    {
                        //cout << "hello" << endl;
                        //cout << (int)floor(angle1) << endl;
                        //cout << (int)floor(angle2) << endl;
                        //cout << angles_output_array[(int)floor(angle1)][(int)floor(angle2)] << endl;
                        
                        //cout << sizeof(angles_output_array)/sizeof(angles_output_array[0]) << endl;
                        //cout << sizeof(angles_output_array[99])/sizeof(angles_output_array[99][0]) << endl;
                    }
                    //cout << "angles: " << angles_output_array[(int)floor(angle1)][(int)floor(angle2)] << endl;
                    //angles_output_array[(int)floor(angle1)][(int)floor(angle2)];
                    //float storage = angles_output_array[(int)floor(angle1)][(int)floor(angle2)];
                    //cout << storage << endl;
                    angles_output_array[(int)floor(angle1)][(int)floor(angle2)] += 5.0f; 


                }


                
                if (renderBounceAmount == true)
                {
                    hitData[0][0] = hitData[3][0]/bounceAmount;
                    hitData[0][1] = hitData[3][0]/bounceAmount;
                    hitData[0][2] = hitData[3][0]/bounceAmount;
                }
                render << floor(max(hitData[0][0],0.0f)*255) << " " << floor(max(hitData[0][1],0.0f)*255) << " " << floor(max(hitData[0][2],0.0f)*255) << endl;


            }
        }
        //cout << rCount << endl;
    }

    render.close();

    
    ofstream angles_output;
    angles_output.open("angles_output.ppm");

    if (angles_output.is_open())
    {
        string resAngles = to_string(degrees) + " " + to_string(degrees);
        angles_output << "P3" << endl;
        angles_output << resAngles << endl;
        angles_output << "255" << endl;

        for (int y = 0; y < degrees; y++)
        {
            for (int x = 0; x < degrees; x++)
            {
                float output = floor(angles_output_array[x][y] < 255 ? angles_output_array[x][y] : 255);
                output = floor((output*-1.0f) + 255.0f);

                if (camxRemember == x && camyRemember == y)
                {
                    angles_output << 255 << " " << 0 << " " << 0 << endl;
                }
                else
                {
                    angles_output << output << " " << output << " " << 255 << endl;
                }

                
            }
        }
    }
    angles_output.close();


    cout << "bounce failure: " << bounceFailure << endl;
    return 0;
}








//---------------method-----------------------------------------








vector<vector<float>>castRay(vector<float> camDir, vector<float> camPos, int maxBounce, bool renderNormalVar)
{


    vector<vector<float>> result = {
        vector<float>{-1,-1,-1},
        vector<float>{camDir[0],camDir[1],camDir[2]},
        vector<float>{camPos[0],camPos[1],camPos[2]},
        vector<float>{0,0,0}
    };


    if (maxBounce <= 0)
    {
        bounceFailure++;

        return result;
    }

    float xsign;
    if (camDir[0] >= 0.0) { xsign = 1; } else { xsign = -1; }
    float ysign;
    if (camDir[1] >= 0.0) { ysign = 1; } else { ysign = -1; }

    vector<float> color = {-1.0f,-1.0f,-1.0f};//set return color
    vector<float> hitOrigin = {camPos[0],camPos[1],camPos[2]};
    vector<float> hitDirection = {camDir[0],camDir[1],camDir[2]};
    vector<float> enter = {};


    bool inside = camPos[0] >= 0 && camPos[0] <= fgridx && camPos[1] >= 0 && camPos[1] <= fgridy;//is the camera inside the global bounding box
    float Rin = Helper::calR(camPos, camDir, 0.0, 0.0, gridx, gridy)[0];//get intersection R with the global bounding box


    if (Rin != -1.0f || inside) //if there is a hit then start checking hit cells triangles
    { 
        if (inside) 
        { 
            enter = {camPos[0],camPos[1],camPos[2]}; 
            
        } 
        else 
        { 
            enter = {camPos[0] + Rin*camDir[0],camPos[1] + Rin*camDir[1],camPos[2] + Rin*camDir[2]};//new origin of the ray 
        }



        while (true)//until end of box is reached
        {

            //float h1x = floor(enter[0] + (xsign * fabs(enter[0]) * numeric_limits<float>::epsilon() * 6));//lower left corner of the current cell
            //float h1y = floor(enter[1] + (ysign * fabs(enter[1]) * numeric_limits<float>::epsilon() * 6));

            float h1x = floor(enter[0] + (xsign * 0.000001f));//lower left corner of the current cell
            float h1y = floor(enter[1] + (ysign * 0.000001f));


            if (h1x < 0 || h1x > fgridx-1 || h1y < 0 || h1y > fgridy-1) { break; }//check if we are on the exit of the global bounding box



            float Rout = Helper::calR(camPos, camDir, h1x, h1y, h1x+1.0f, h1y+1.0f)[1];

            vector<float> exit = {camPos[0] + Rout*camDir[0], camPos[1] + Rout*camDir[1], camPos[2] + Rout*camDir[2]};

            float smallAmount = 0.000001f;
            vector<float> a = {h1x - smallAmount, h1y - smallAmount, heightField[(int)h1x][(int)h1y]};//the four heightfields of the cell
            vector<float> aN = heightFieldNormals[(int)h1x][(int)h1y];
            vector<float> b = {h1x+1 + smallAmount, h1y - smallAmount, heightField[(int)(h1x+1)][(int)h1y]};
            vector<float> bN = heightFieldNormals[(int)(h1x+1)][(int)h1y];
            vector<float> c = {h1x - smallAmount, h1y+1 + smallAmount, heightField[(int)h1x][(int)(h1y+1)]};
            vector<float> cN = heightFieldNormals[(int)h1x][(int)(h1y+1)];
            vector<float> d = {h1x+1 + smallAmount, h1y+1 + smallAmount, heightField[(int)(h1x+1)][(int)(h1y+1)]};
            vector<float> dN = heightFieldNormals[(int)(h1x+1)][(int)(h1y+1)];

            float cellZmin = min(a[2],min(b[2],min(c[2],d[2])));//the min and max of the four corners
            float cellZmax = max(a[2],max(b[2],max(c[2],d[2])));

            if (exit[2] < cellZmax + 0.25f)//if the ray touches the box defined by the cell and the min/max heights
            {
                bool hit = false;
                float tempT = numeric_limits<float>::infinity();
                vector<vector<float>> tempResult = VecMeth::tesselate(camPos, camDir, a,aN,b,bN,c,cN, lightPos, lightCol, fieldColor, tessellationAmount, smooth, renderNormalVar, xTestV, yTestV);
                if (tempResult[0][0] > -0.5f && tempResult[3][0] > 0.0f) 
                {
                    color = tempResult[0];
                    hitDirection = tempResult[1];
                    hitOrigin = tempResult[2];

                    tempT = tempResult[3][0];
                    hit = true;
                }
                tempResult = VecMeth::tesselate(camPos, camDir, d,dN,b,bN,c,cN, lightPos, lightCol, fieldColor, tessellationAmount, smooth, renderNormalVar, xTestV, yTestV);
                if (tempResult[0][0] > -0.5f && tempResult[3][0] < tempT && tempResult[3][0] > 0.0f) 
                {
                    color = tempResult[0];
                    hitDirection = tempResult[1];
                    hitOrigin = tempResult[2];

                    hit = true;
                }
                
                if (hit && xTestV == 368 && yTestV == 101) 
                { 

                    //cout << "hit!!!!!" << endl;

                    //VecMeth::printVec(color);
                }
                if (hit) { break; }
            }
            enter = vector<float>{exit[0],exit[1],exit[2]};//no triangle has been hit so check the next cell'
        }
    }
    
    result[0] = color;
    result[1] = hitDirection;
    float xS = hitDirection[0] > 0.0f ? 1.0f : -1.0f;
    float yS = hitDirection[1] > 0.0f ? 1.0f : -1.0f;
    float zS = hitDirection[2] > 0.0f ? 1.0f : -1.0f;
    
    result[2] = vector<float>{
        hitOrigin[0] + hitDirection[0]*0.00001f,
        hitOrigin[1] + hitDirection[1]*0.00001f,
        hitOrigin[2] + hitDirection[2]*0.00001f
        };

    //result[2] = vector<float>{
        //hitOrigin[0] + (xS * fabs(hitOrigin[0]) * numeric_limits<float>::epsilon() * 6),
        //hitOrigin[1] + (yS * fabs(hitOrigin[1]) * numeric_limits<float>::epsilon() * 6),
        //hitOrigin[2] + (zS * fabs(hitOrigin[2]) * numeric_limits<float>::epsilon() * 6)
        //};
    
    if (xTestV == 368 && yTestV == 101)
    {
        //cout << "we are at the end" << endl;
        //cout << "colorx: " << color[0] << endl;
    }

    if (color[0] > -0.5f)
    {
        
        if (xTestV == 368 && yTestV == 101)
        {
            //cout << "end in the if" << endl;
        }

        vector<vector<float>> bounceResult = castRay(result[1], result[2], maxBounce-1, renderNormalVar);
        //if (bounceResult[0][0] > 0.0f)
        //{
        //if (VecMeth::length(result[1]) == 0.0f) { cout << "return zero direction second" << endl; }
        result[1] = bounceResult[1];
        result[2] = bounceResult[2];
        result[3][0] += 1.0f + bounceResult[3][0]; 
        //}
    }
    

    //if (VecMeth::length(result[1]) == 0.0f) { cout << "return zero direction second" << endl; }
    return result;
}