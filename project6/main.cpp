#include <GL/glut.h>
#include<GL/gl.h>

#include <GL/glu.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <bits/stdc++.h>
using namespace std;


GLfloat width = 700;
GLfloat height = 700;

GLfloat floor_width = 40;
GLfloat floor_height = 50;
int day=0;

float rotation = 0;
double Txval=0,Tyval=0,Tzval=0;

double A_Txval=0,A_Tyval=0,A_Tzval=0;
GLfloat leftLightHight = 40;
GLfloat rightLightHight = 40;
GLfloat spotLightHight = 40;

bool light1_on = true;
bool light2_on = true;
bool leftLight_on = true;
bool rightLight_on = true;
bool spotLight_on = true;
bool amb_on = true;
bool dif_on = true;
bool spe_on = true;
float spt_cutoff = 30;

bool redSignal = false;
GLfloat carStep = 0.2;
GLfloat car1Initial = 0;
GLfloat car2Initial = 0;
GLfloat car3Initial = 0;

GLfloat alpha = 0.0, theta = 0.0,zeta=0.0, gamma = 0.0, axis_x=0.0, axis_y=0.0, axis_z=0.0, eye_x = 0.0, eye_z = 10.0, eye_y = 6.0, look_x = 0.0, look_y = 6.0, look_z = 0.0;
GLfloat A_alpha = 0.0, A_theta = 0.0, A_zeta=0.0;
GLfloat ux = 0, uy = 1, uz = 0;

GLfloat nearPLane = 2, farPlane = 50;

GLfloat dx, dy, dz, dxyz;
GLboolean bRotate = false, uRotate = false, kRotate = false;


double roll_value = 0.2, pi = acos(-1), cs_angle=cos(pi/180), sn_angle=sin(pi/180);

double scale_x = 1;
double scale_y = 1;
double scale_z = 1;
GLfloat eyeX = 0;
GLfloat eyeY = 5;
GLfloat eyeZ = -27;

GLfloat lookX = 0;
GLfloat lookY = 5;
GLfloat lookZ = 30;

vector<int> v;
unsigned int ID;

static void res(int width, int height)
{
    glViewport(0, 0, width, height);
}

static GLfloat v_cube[8][3] =
{
    {0,0,0},
    {0,0,1},
    {0,1,0},
    {0,1,1},

    {1,0,0},
    {1,0,1},
    {1,1,0},
    {1,1,1}
};

static GLubyte c_ind[6][4] =
{
    {3,1,5,7},  //front
    {6,4,0,2},  //back
    {2,3,7,6},  //top
    {1,0,4,5},  //bottom
    {7,5,4,6},  //right
    {2,0,1,3}   //left
};

static void getNormal3p(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2-x1;
    Uy = y2-y1;
    Uz = z2-z1;

    Vx = x3-x1;
    Vy = y3-y1;
    Vz = z3-z1;

    Nx = Uy*Vz - Uz*Vy;
    Ny = Uz*Vx - Ux*Vz;
    Nz = Ux*Vy - Uy*Vx;

    glNormal3f(Nx,Ny,Nz);
}

void cube(float R=0.5, float G=0.5, float B=0.5, float val=1)
{
    GLfloat m_no[] = {0, 0, 0, 1.0};
    GLfloat m_amb[] = {R,G,B,1};
    GLfloat m_diff[] = {R,G,B,1};
    GLfloat m_spec[] = {1,1,1,1};
    GLfloat m_sh[] = {30};

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    glBegin(GL_QUADS);
    for (GLint i = 0; i <6; i++)
    {
        getNormal3p(v_cube[c_ind[i][0]][0], v_cube[c_ind[i][0]][1], v_cube[c_ind[i][0]][2],
                    v_cube[c_ind[i][1]][0], v_cube[c_ind[i][1]][1], v_cube[c_ind[i][1]][2],
                    v_cube[c_ind[i][2]][0], v_cube[c_ind[i][2]][1], v_cube[c_ind[i][2]][2]);
        glTexCoord2f(0,val);
        glVertex3fv(&v_cube[c_ind[i][0]][0]);
        glTexCoord2f(0,0);
        glVertex3fv(&v_cube[c_ind[i][1]][0]);
        glTexCoord2f(val,0);
        glVertex3fv(&v_cube[c_ind[i][2]][0]);
        glTexCoord2f(val,val);
        glVertex3fv(&v_cube[c_ind[i][3]][0]);
    }
    glEnd();
}


/// Texture
class BmpLoader
{
public:
    unsigned char* textureData;
    int iWidth, iHeight;

    BmpLoader(const char*);
    ~BmpLoader();

private:
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
};

BmpLoader::BmpLoader(const char* filename)
{
    FILE *file=0;
    file=fopen(filename, "rb");
    if(!file)
        cout<<"File not found"<<endl;
    fread(&bfh, sizeof(BITMAPFILEHEADER),1,file);
    if(bfh.bfType != 0x4D42)
        cout<<"Not a valid bitmap"<<endl;
    fread(&bih, sizeof(BITMAPINFOHEADER),1,file);
    if(bih.biSizeImage==0)
        bih.biSizeImage=bih.biHeight*bih.biWidth*3;
    textureData = new unsigned char[bih.biSizeImage];
    fseek(file, bfh.bfOffBits, SEEK_SET);
    fread(textureData, 1, bih.biSizeImage, file);
    unsigned char temp;
    for(int i=0; i<bih.biSizeImage; i+=3)
    {
        temp = textureData[i];
        textureData[i] = textureData[i+2];
        textureData[i+2] = temp;
    }

    iWidth = bih.biWidth;
    iHeight = bih.biHeight;
    fclose(file);
}

BmpLoader::~BmpLoader()
{
    delete [] textureData;
}

void LoadTexture(const char*filename)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    BmpLoader bl(filename);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData );
}

void texture_image()
{
    /// road in z-direction
      LoadTexture("F:\\graphics_lab\\lab1\\project5\\images\\road.bmp");
    v.push_back(ID);

    /// road in x-direction
    LoadTexture("F:\\graphics_lab\\lab1\\project6\\images\\grass1.bmp");
    v.push_back(ID);
     LoadTexture("F:\\graphics_lab\\lab1\\project6\\images\\grass.bmp");
    v.push_back(ID);
      LoadTexture("F:\\graphics_lab\\lab1\\project6\\images\\car.bmp");
    v.push_back(ID);
        LoadTexture("F:\\graphics_lab\\lab1\\project6\\images\\tree_body.bmp");
    v.push_back(ID);
        LoadTexture("F:\\graphics_lab\\lab1\\project6\\images\\tree_leaf2.bmp");
    v.push_back(ID);
       LoadTexture("F:\\graphics_lab\\lab1\\project6\\images\\house.bmp");
    v.push_back(ID);


}

/// Lighting
void leftLight()
{
    //light
    GLfloat l_no[] = {0, 0, 0, 1.0};
    GLfloat l_amb[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat l_dif[] = {1,1,1,1};
    GLfloat l_spec[] = {1,1,1,1};
    GLfloat l_pos[] = {10, leftLightHight, 0};

    glEnable(GL_LIGHT0);

    if(leftLight_on)
    {
        glLightfv(GL_LIGHT0, GL_AMBIENT, l_amb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, l_dif);
        glLightfv(GL_LIGHT0, GL_SPECULAR, l_spec);
    }
    else
    {
        glLightfv(GL_LIGHT0, GL_AMBIENT, l_no);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, l_no);
        glLightfv(GL_LIGHT0, GL_SPECULAR, l_no);
    }

    if(amb_on && leftLight_on) glLightfv(GL_LIGHT0, GL_AMBIENT, l_amb);
    else glLightfv(GL_LIGHT0, GL_AMBIENT, l_no);

    if(dif_on && leftLight_on) glLightfv(GL_LIGHT0, GL_DIFFUSE, l_dif);
    else glLightfv(GL_LIGHT0, GL_DIFFUSE, l_no);

    if(spe_on && leftLight_on) glLightfv(GL_LIGHT0, GL_SPECULAR, l_spec);
    else glLightfv(GL_LIGHT0, GL_SPECULAR, l_no);

    glLightfv(GL_LIGHT0, GL_POSITION, l_pos);
}

void leftLightIndicator()
{
    glPushMatrix();
        glTranslatef(10, leftLightHight+1, 0);
        glScalef(1, 1, 1);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
}

void rightLight()
{
    //light
    GLfloat l_no[] = {0, 0, 0, 1.0};
    GLfloat l_amb[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat l_dif[] = {1,1,1,1};
    GLfloat l_spec[] = {1,1,1,1};
    GLfloat l_pos[] = {-10, rightLightHight, 0};

    glEnable(GL_LIGHT1);

    if(rightLight_on)
    {
        glLightfv(GL_LIGHT1, GL_AMBIENT, l_amb);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, l_dif);
        glLightfv(GL_LIGHT1, GL_SPECULAR, l_spec);
    }
    else
    {
        glLightfv(GL_LIGHT1, GL_AMBIENT, l_no);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, l_no);
        glLightfv(GL_LIGHT1, GL_SPECULAR, l_no);
    }

    if(amb_on && rightLight_on) glLightfv(GL_LIGHT1, GL_AMBIENT, l_amb);
    else glLightfv(GL_LIGHT1, GL_AMBIENT, l_no);

    if(dif_on && rightLight_on) glLightfv(GL_LIGHT1, GL_DIFFUSE, l_dif);
    else glLightfv(GL_LIGHT1, GL_DIFFUSE, l_no);

    if(spe_on && rightLight_on) glLightfv(GL_LIGHT1, GL_SPECULAR, l_spec);
    else glLightfv(GL_LIGHT1, GL_SPECULAR, l_no);

    glLightfv(GL_LIGHT1, GL_POSITION, l_pos);
}

void rightLightIndicator()
{
    glPushMatrix();
        glTranslatef(-10, rightLightHight+1, 0);
        glScalef(1, 1, 1);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.000, 1.0, 1.0, 1.0);
    glPopMatrix();
}

void spotLight()
{
    //light
    GLfloat l_no[] = {0, 0, 0, 1.0};
    GLfloat l_amb[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat l_dif[] = {1,1,1,1};
    GLfloat l_spec[] = {1,1,1,1};
    GLfloat l_pos[] = {0, spotLightHight, 0};

    glEnable(GL_LIGHT2);

    if(spotLight_on)
    {
        glLightfv(GL_LIGHT2, GL_AMBIENT, l_amb);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, l_dif);
        glLightfv(GL_LIGHT2, GL_SPECULAR, l_spec);
    }
    else
    {
        glLightfv(GL_LIGHT2, GL_AMBIENT, l_no);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, l_no);
        glLightfv(GL_LIGHT2, GL_SPECULAR, l_no);
    }

    if(amb_on && spotLight_on) glLightfv(GL_LIGHT2, GL_AMBIENT, l_amb);
    else glLightfv(GL_LIGHT2, GL_AMBIENT, l_no);

    if(dif_on && spotLight_on) glLightfv(GL_LIGHT2, GL_DIFFUSE, l_dif);
    else glLightfv(GL_LIGHT2, GL_DIFFUSE, l_no);

    if(spe_on && spotLight_on) glLightfv(GL_LIGHT2, GL_SPECULAR, l_spec);
    else glLightfv(GL_LIGHT2, GL_SPECULAR, l_no);

    glLightfv(GL_LIGHT2, GL_POSITION, l_pos);

    GLfloat l_spt[] = {0, -1, 0, 1};
    GLfloat spt_ct[] = {spt_cutoff};
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, l_spt);
    glLightfv(GL_LIGHT2, GL_SPOT_CUTOFF, spt_ct);
}

void spotLightIndicator()
{
    glPushMatrix();
        glTranslatef(0, spotLightHight+1, 0);
        glScalef(1, 1, 1);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.000, 1.0, 1.0, 1.0);
    glPopMatrix();
}

void eyeIndicator()
{
    glPushMatrix();
        glTranslatef(eyeX, eyeY, eyeZ);
        glScalef(1, 1, 1);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.000, 0, 0, 1.0);
    glPopMatrix();
}

void lookAtPointIndicator()
{
    glPushMatrix();
        glTranslatef(lookX, lookY, lookZ);
        glScalef(1, 1, 1);
        glTranslatef(-0.5, 0, -0.5);
        cube(0, 1.0, 0, 1.0);
    glPopMatrix();
}

/// Grounding
void axes()
{
    float length = 0.10;
    float width = 0.3;

    // X-axis
    glPushMatrix();
        glTranslatef(length/2, 0, 0);
        glScalef(length, width, width);
        glTranslatef(-0.5, -0.5, -0.5);
        cube(0.8, 0.1, 0.1);
    glPopMatrix();

    // Y-axis
    glPushMatrix();
        glTranslatef(0, length/2, 0);
        glScalef(width, length, width);
        glTranslatef(-0.5, -0.5, -0.5);
        cube(0.1, 0.8, 0.1);
    glPopMatrix();

    // Z-axis
    glPushMatrix();
        glTranslatef(0, 0, length/2);
        glScalef(width, width, length);
        glTranslatef(-0.5, -0.5, -0.5);
        cube(0.1, 0.1, 0.8);
    glPopMatrix();
}

void flr()
{
    glPushMatrix();
        glScalef(floor_width, 1, floor_height);
        glTranslatef(-0.5, -1, -0.5);
        cube(1, 1, 1);
    glPopMatrix();
}

void ground()
{
    /// left Front floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[2]);
        glPushMatrix();
            glTranslatef(10, 0, -12.5);
            glScalef(20.0, 0.1, 25.0);
            glTranslatef(-0.5, -0.95, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// left Back floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[2]);
        glPushMatrix();
            glTranslatef(10, 0, 12.5);
            glScalef(20.0, 0.1, 25.0);
            glTranslatef(-0.5, -0.95, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// right Front floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[2]);
        glPushMatrix();
            glTranslatef(-10, 0, -12.5);
            glScalef(20.0, 0.1, 25.0);
            glTranslatef(-0.5, -0.95, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// right Back floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[2]);
        glPushMatrix();
            glTranslatef(-10, 0, 12.5);
            glScalef(20.0, 0.1, 25.0);
            glTranslatef(-0.5, -0.95, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void road()
{
    /// road in z-direction
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[0]);
        glPushMatrix();
            glScalef(15, 1, floor_height);
            glTranslatef(-0.5, -0.9, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// road in x-direction
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[1]);
        glPushMatrix();
            glScalef(floor_width, 1, 15);
            glTranslatef(-0.5, -0.9, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// tow roads intersection
    glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D,v[2]);
        glPushMatrix();
            glScalef(15, 1, 15);
            glTranslatef(-0.5, -0.8, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    //glDisable(GL_TEXTURE_2D);
}



/// Trees
void tree()
{
    GLfloat m_amb[] = {0.000, 0.392, 0.000,1};
    GLfloat m_diff[] = {0.000, 0.392, 0.000,1};
    GLfloat m_spec[] = {0.000, 0.392, 0.000,1};
    GLfloat m_sh[] = {30};

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    /// root
    glPushMatrix();
        glTranslated(11, 0, -17);
        glRotated(90, -1, 0, 0);
        glBegin(GL_POLYGON);
            GLUquadricObj *roo_obj = gluNewQuadric();
            gluCylinder(roo_obj, 0.3, 0.3, 9.0, 300.0, 300.0);
        glEnd();
    glPopMatrix();
    /// branch
    glPushMatrix();
        glTranslated(11, 8.0, -17);
        glRotated(90, -1, 0, 0);
        glutSolidCone(2.5, 4.0, 300, 300);
    glPopMatrix();
    glPushMatrix();
        glTranslated(11, 10.5, -17);
        glRotated(90, -1, 0, 0);
        glutSolidCone(2.5, 4.0, 300, 300);
    glPopMatrix();
}

void trees()
{
     glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[6]);
    glPushMatrix();
     glTranslated(10, 0, -15.0);
    glScalef(7.2,15.2,8.2);
    cube();
        //tree();
    glPopMatrix();
      glDisable(GL_TEXTURE_2D);
    glPushMatrix();
        glTranslated(0, 0, 34.0);
        tree();
    glPopMatrix();
      glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[6]);
    glPushMatrix();
        glTranslated(-17, 0, -15.0);
    glScalef(7.2,15.2,8.2);
        cube();
        //tree();
    glPopMatrix();
     glDisable(GL_TEXTURE_2D);
    glPushMatrix();
        glTranslated(-22.0, 0, 34.0);
        tree();
    glPopMatrix();
}





/// Traffic System
void trafficSystem()
{
    /// traffic light base
    glPushMatrix();
        glScalef(1.5, 1.5, 1.5);
        glTranslatef(-0.5, 0, -0.5);
        cube(0.941, 0.502, 0.502, 1.0);
    glPopMatrix();
    /// traffic light root

    GLfloat m_amb[] = {0.941, 0.502, 0.502, 1.0};
    GLfloat m_diff[] = {0.941, 0.502, 0.502, 1.0};
    GLfloat m_spec[] = {0.941, 0.502, 0.502, 1.0};
    GLfloat m_sh[] = {30};
    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    glPushMatrix();
        glRotated(90, -1, 0, 0);
        glBegin(GL_POLYGON);
            GLUquadricObj *clock_obj = gluNewQuadric();
            gluCylinder(clock_obj, 0.5, 0.5, 25.0, 300.0, 300.0);
        glEnd();
    glPopMatrix();

    /// traffic light holder
    glPushMatrix();
        glTranslatef(0, 25.5, 0);
        glScalef(4, 8, 4);
        glTranslatef(-0.5, 0, -0.5);
        cube(0.0, 0.0, 1.0, 1.0);
    glPopMatrix();

    /// traffic lights
    ///front in z
    glPushMatrix();
        glTranslatef(0, 30.0, -2.0);
        glScalef(1.0, 1.5, 0.1);
        glTranslatef(-0.5, 0, -0.5);
            if(!redSignal) cube(1.0, 1.000, 1.000, 1.0);
            else cube(1.0, 0.000, 0.000, 1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 28.0, -2.0);
        glScalef(1.0, 1.5, 0.1);
        glTranslatef(-0.5, 0, -0.5);
            if(!redSignal) cube(1.0, 1.000, 1.000, 1.0);
            else cube(1.000, 1.000, 1.000, 1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 26.0, -2.0);
        glScalef(1.0, 1.5, 0.1);
        glTranslatef(-0.5, 0, -0.5);
            if(!redSignal) cube(0.0, 1.000, 0.000, 1.0);
            else cube(1.000, 1.000, 1.000, 1.0);
    glPopMatrix();
    ///back in z
    glPushMatrix();
        glTranslatef(0, 30.0, 2.0);
        glScalef(1.0, 1.5, 0.1);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.0, 0.000, 0.000, 1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 28.0, 2.0);
        glScalef(1.0, 1.5, 0.1);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.000, 1.000, 0.000, 1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 26.0, 2.0);
        glScalef(1.0, 1.5, 0.1);
        glTranslatef(-0.5, 0, -0.5);
        cube(0.0, 1.000, 0.000, 1.0);
    glPopMatrix();
    ///left in x
    glPushMatrix();
        glTranslatef(2.0, 30.0, 0);
        glScalef(0.1, 1.5, 1.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.0, 0.000, 0.000, 1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0, 28.0, 0);
        glScalef(0.1, 1.5, 1.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.000, 1.000, 0.000, 1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0, 26.0, 0);
        glScalef(0.1, 1.5, 1.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(0.0, 1.000, 0.000, 1.0);
    glPopMatrix();
    ///right in x
    glPushMatrix();
        glTranslatef(-2.0, 30.0, 0);
        glScalef(0.1, 1.5, 1.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.0, 0.000, 0.000, 1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-2.0, 28.0, 0);
        glScalef(0.1, 1.5, 1.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.000, 1.000, 0.000, 1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-2.0, 26.0, 0);
        glScalef(0.1, 1.5, 1.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(0.0, 1.000, 0.000, 1.0);
    glPopMatrix();
}

void trafficSystemPositioned()
{
    glPushMatrix();
        ///glTranslatef(0, 0, 35.0);
        trafficSystem();
    glPopMatrix();
}


/// Curves & Surfaces
const double PI = 3.14159265389;
const int L = 20;
const int nt = 40;				//number of slices along x-direction
const int ntheta = 20;

GLfloat ctrlpoints[L+1][3] =
{
    {-0.0359765, 0.0234949, 0},
    {-0.223935, 0.187959, 0},
    {-0.29442, 0.422907, 0},
    {-0.200441, 0.610866, 0},
    {0.0814978, 0.610866, 0},
    {0.48091, 0.657856, 0},
    {0.739354, 0.657856, 0},
    {1.04479, 0.657856, 0},
    {1.30323, 0.657856, 0},
    {1.46769, 0.446402, 0},
    {1.4207, 0.234949, 0},
    {1.32673, 0.117474, 0}
};

void setNormal(GLfloat x1, GLfloat y1,GLfloat z1, GLfloat x2, GLfloat y2,GLfloat z2, GLfloat x3, GLfloat y3,GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2-x1;
    Uy = y2-y1;
    Uz = z2-z1;

    Vx = x3-x1;
    Vy = y3-y1;
    Vz = z3-z1;

    Nx = Uy*Vz - Uz*Vy;
    Ny = Uz*Vx - Ux*Vz;
    Nz = Ux*Vy - Uy*Vx;

    glNormal3f(-Nx,-Ny,-Nz);
}

long long nCr(int n, int r)
{
    if(r > n / 2) r = n - r; // because C(n, r) == C(n, n - r)
    long long ans = 1;
    int i;

    for(i = 1; i <= r; i++)
    {
        ans *= n - r + i;
        ans /= i;
    }

    return ans;
}

void BezierCurve ( double t,  float xy[2])
{
    double y=0;
    double x=0;
    t=t>1.0?1.0:t;
    for(int i=0; i<=L; i++)
    {
        int ncr=nCr(L,i);
        double oneMinusTpow=pow(1-t,double(L-i));
        double tPow=pow(t,double(i));
        double coef=oneMinusTpow*tPow*ncr;
        x+=coef*ctrlpoints[i][0];
        y+=coef*ctrlpoints[i][1];

    }
    xy[0] = float(x);
    xy[1] = float(y);
}

void wheel()
{
    GLfloat m_amb[] = {0.184, 0.310, 0.310, 1};
    GLfloat m_diff[] = {0.184, 0.310, 0.310, 1};
    GLfloat m_spec[] = {0.184, 0.310, 0.310, 1};
    GLfloat m_sh[] = {30};

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    int i, j;
    float x, y, z, r;				//current coordinates
    float x1, y1, z1, r1;			//next coordinates
    float theta;

    const float startx = 0, endx = ctrlpoints[L][0];
    //number of angular slices
    const float dx = (endx - startx) / nt;	//x step size
    const float dtheta = 2*PI / ntheta;		//angular step size

    float t=0;
    float dt=1.0/nt;
    float xy[2];
    BezierCurve( t,  xy);
    x = xy[0];
    r = xy[1];
    //rotate about z-axis
    float p1x,p1y,p1z,p2x,p2y,p2z;
    for ( i = 0; i < nt; ++i )  			//step through x
    {
        theta = 0;
        t+=dt;
        BezierCurve( t,  xy);
        x1 = xy[0];
        r1 = xy[1];

        //draw the surface composed of quadrilaterals by sweeping theta
        glBegin( GL_QUAD_STRIP );
        for ( j = 0; j <= ntheta; ++j )
        {
            theta += dtheta;
            double cosa = cos( theta );
            double sina = sin ( theta );
            y = r * cosa;
            y1 = r1 * cosa;	//current and next y
            z = r * sina;
            z1 = r1 * sina;	//current and next z

            //edge from point at x to point at next x
            glVertex3f (x, y, z);

            if(j>0)
            {
                setNormal(p1x,p1y,p1z,p2x,p2y,p2z,x, y, z);
            }
            else
            {
                p1x=x;
                p1y=y;
                p1z=z;
                p2x=x1;
                p2y=y1;
                p2z=z1;

            }
            glVertex3f (x1, y1, z1);

            //forms quad with next pair of points with incremented theta value
        }
        glEnd();
        x = x1;
        r = r1;
    } //for i
}

void wheelPositioned(){
    /// right wheels
    glPushMatrix();
        glTranslatef(-5, 1.0, -18.0);
        glScalef(1.0, 2.0, 1.5);
        wheel();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-5, 1.0, -21.0);
        glScalef(1.0, 2.0, 1.5);
        wheel();
    glPopMatrix();

    /// left wheels
    glPushMatrix();
        glTranslatef(-1.75, 1.0, -18.0);
        glScalef(1.0, 2.0, 1.5);
        wheel();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-1.75, 1.0, -21.0);
        glScalef(1.0, 2.0, 1.5);
        wheel();
    glPopMatrix();
}

void carBodyRaw()
{
    /// main body
    glPushMatrix();
        glTranslatef(-3.0, 1.5, -19.5);
        glScalef(3.5, 2.0, 5.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.0, 0.0, 1.0, 1.0);
    glPopMatrix();
    /// upper body
    glPushMatrix();
        glTranslatef(-3.0, 3.5, -20.0);
        glScalef(1.75, 1.0, 2.5);
        glTranslatef(-0.5, 0, -0.5);
        cube(0.0, 1.0, 1.0, 1.0);
    glPopMatrix();
}

void car()
{
    /// car 1
    glPushMatrix();
        if(!redSignal){
            glTranslatef(-1.0, 0, car1Initial += carStep);
        }else{
            glTranslatef(-1.0, 0, car1Initial);
        }

        if(car1Initial > 40.0){
            car1Initial = 0.0;
        }

        wheelPositioned();
        carBodyRaw();
    glPopMatrix();

    /// car 2
    glPushMatrix();
       if(!redSignal){
            glTranslatef(1.0, 0, car2Initial -= carStep);
        }else{
            glTranslatef(1.0, 0, car2Initial);
        }

        if(car2Initial < -40.0){
            car2Initial = 0.0;
        }

        glRotated(-180, 0, 1, 0);
        wheelPositioned();
        carBodyRaw();
    glPopMatrix();

    glPushMatrix();
        glRotated(-90, 0, 1, 0);
        glTranslatef(-1.0, 0, 4);
        wheelPositioned();
        carBodyRaw();
    glPopMatrix();
}


/// Objects to display
void objects()
{
    axes();

    flr();
    ground();
    road();
    car();

    //buildings();
    //Clock();
    //jatriChaonePositioned();

    leftLight();
    rightLight();
    spotLight();

                ///leftLightIndicator();
                ///rightLightIndicator();
                ///spotLightIndicator();

                ///eyeIndicator();
                ///lookAtPointIndicator();

    //sky();
    //clouds();
    //sun();
    //plane();

    //lamPosts();
    trees();

    trafficSystemPositioned();

    ///cout<<"X : "<<lookX<<" "<<"Y : "<<lookY<<" "<<"Z : "<<lookZ<<endl;
}


static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-5, 5, -5, 5, 3, 100.0);
    gluPerspective(0,0,0,0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ,  lookX, lookY, lookZ,  ux, uy, uz);

    glRotatef(rotation, 0, 1, 0);

    objects();

    glutSwapBuffers();
}


void Yaw_y1(){
    GLfloat tx, tz;

    tx = lookX - eyeX;
    tz = lookZ - eyeZ;

    dx = tx*cs_angle+tz*sn_angle;
    dz = -tx*sn_angle+tz*cs_angle;

    lookX = dx + eyeX;
    lookZ = dz + eyeZ;

}

void Yaw_y2(){
    GLfloat tx, tz;

    tx = lookX - eyeX;
    tz = lookZ - eyeZ;

    dx = tx*cs_angle-tz*sn_angle;
    dz = tx*sn_angle+tz*cs_angle;

    lookX = dx + eyeX;
    lookZ = dz + eyeZ;

}
void Pitch_y1(){
    GLfloat ty, tz;

    ty = lookY - eyeY;
    tz = lookZ - eyeZ;

    dy = ty*cs_angle-tz*sn_angle;
    dz = ty*sn_angle+tz*cs_angle;

    lookY = dy + eyeY;
    lookZ = dz + eyeZ;

}
void Pitch_y2(){
    GLfloat ty, tz;

    ty = lookY - eyeY;
    tz = lookZ - eyeZ;

    dy = ty*cs_angle+tz*sn_angle;
    dz = -ty*sn_angle+tz*cs_angle;

    lookY = dy + eyeY;
    lookZ = dz + eyeZ;

}

void Roll_y1(){
    dx = ux*cs_angle-uy*sn_angle;
    dy = ux*sn_angle+uy*cs_angle;

    ux = dx;
    uy = dy;
}

void Roll_y2(){
    dx = ux*cs_angle+uy*sn_angle;
    dy = -ux*sn_angle+uy*cs_angle;

    ux = dx;
    uy = dy;
}
static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27 :
        exit(0);
        break;
    case ',':
        rotation += 5;
        break;
    case '.':
        rotation -= 5;
        break;

    /// amb, dif & spe control
    case 'a':
        amb_on =! amb_on;
        break;
    case 'd':
        dif_on =! dif_on;
        break;
    case 's':
        spe_on =! spe_on;
        break;

    /// zoom in, zoom out & left-right
    case 'g':
        //if(lookX <22) lookX++;
        //else lookX;
        lookY++;
        break;
    case 'h':
        //if(lookX > -22) lookX--;
        //else lookX;
        lookY--;
        break;

    case 'j':
        eyeX++;
        break;
    case 'k':
        eyeX--;
        break;


    case 'u':
        eyeY++;
        lookY++;
        break;
    case 'i':
        if(lookY > 5){
            lookY--;
            eyeY--;
        }else{
            lookY;
            eyeY;
        }
        //eyeY--;
        //lookY--;
        break;

    case '=':
        if(lookZ < 123){
            lookZ++;
            eyeZ++;
        }else{
            lookZ;
            eyeZ;
        }
        //eyeZ++;
        //lookZ++;
        break;
    case '-':
        if(lookZ > 29){
            lookZ--;
            eyeZ--;
        }else{
            lookZ;
            eyeZ;
        }
        //eyeZ--;
        //lookZ--;
        break;

    ///lighting on off
    case '1':
        leftLight_on =!leftLight_on;
        break;
    case '2':
        rightLight_on =!rightLight_on;
        break;
    case '3':
        spotLight_on =! spotLight_on;
        break;

          case '8':
        Pitch_y1();
        break;
    case '9':
        Pitch_y2();
        break;
    case '6':
        Yaw_y1();
        break;
    case '7':
        Yaw_y2();
        break;
    case '4':
        Roll_y1();
        break;
    case '5':
        Roll_y2();
        break;

    /// moving objects control
    case 't':
        redSignal =!redSignal;
        break;

    }
    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Scenario");

    glutReshapeFunc(res);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);

    glEnable(GL_LIGHTING);
    GLfloat globalAmbient[] = {0.0, 0.0, 0.0, 1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    texture_image();

    printf("\nPlease, Follow this instructions for zooming & lighting: \n\n");

    printf("\tZoom In -> +\n");
    printf("\tZoom Out -> -\n");
    printf("\tLook Left -> g\n");
    printf("\ttLook Right -> h\n");
    printf("\tMove Camera Left -> j\n");
    printf("\tMove Camera Right -> k\n");
    printf("\tUp -> u\n");
    printf("\tDown -> i\n\n");

    printf("\tPoint Light #1 On/Off -> 1\n");
    printf("\tPoint Light #2 On/Off -> 2\n");
    printf("\tSpot Light On/Off -> 3\n\n");

    printf("\tAmbient Light On/Off -> a\n");
    printf("\tDiffuse Light On/Off -> d\n");
    printf("\tSpecular Light On/Off -> s\n\n");

    printf("\tTraffic System control -> t\n");


    glutMainLoop();

    return EXIT_SUCCESS;
}
