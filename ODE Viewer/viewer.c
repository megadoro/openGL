#include <GL/freeglut.h>
#include "ode.h"
#include "color.h"

bool ACTIVE = true;

plist *list = NULL;

float scalef = 1.5;
float ds;
float a;
plist b;

void drawLine(plist *trail, rgb_t color)
{
    glBegin(GL_LINE_STRIP);
    glColor3d(color.r,color.g,color.b);
    while(trail != NULL){
        glVertex2d(trail->x,trail->y);
        trail = trail->next;
    }
    glEnd();
}

void drawCircle(plist head, double r, rgb_t color)
{
    glColor3d(color.r,color.g,color.b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++)
        glVertex2d(head.x+r*cos(i*3.14159265/180), head.y+r*sin(i*3.14159265/180));
    glEnd();
}

void displayF()
{
    glClear(GL_COLOR_BUFFER_BIT);
    rgb_t color;
    color.r = 0;
    color.g = 96/255.0;
    color.b = 173/255.0;
    drawLine(list, color);
    drawCircle(*list, 0.02, color);
    glutSwapBuffers();
}

void reshapeF(int w,int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w*a/h+b.x,w*a/h+b.x,-a+b.y,a+b.y,1,-1);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1,1,1,0);
}

void init()
{
    plist_add_front(&list,0,0,0);

    ACTIVE = true;
    b.x = b.y = 0;
    ds = 0.05;
    a = 1;

    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    reshapeF(viewport[2],viewport[3]);
}

void keyboardF(unsigned char key, int x, int y)
{
    switch(key)
    {
        case '+':
            glTranslatef(b.x,b.y,0);
            glScalef(scalef,scalef,1);
            glTranslatef(-b.x,-b.y,0);
            ds/=scalef;
            a/=scalef;
            break;
        case '-':
            glTranslatef(b.x,b.y,0);
            glScalef(1/scalef,1/scalef,1);
            glTranslatef(-b.x,-b.y,0);
            ds*=scalef;
            a*=scalef;
            break;
        case 'p': case 'P': case ' ':
            ACTIVE=!ACTIVE;
            break;
        case 'f': case 'F':
            glutFullScreenToggle();
            break;
        case 'r': case 'R':
            init();
            break;
        case 'q': case 'Q': case 27:
            exit(0);
            break;
    }
}

void specialKeyboardF(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_F11:
            glutFullScreenToggle();
            break;
        case GLUT_KEY_UP:
            glTranslatef(0,-ds,0);
            b.y+=ds;
            break;
        case GLUT_KEY_DOWN:
            glTranslatef(0,ds,0);
            b.y-=ds;
            break;
        case GLUT_KEY_LEFT:
            glTranslatef(ds,0,0);
            b.x-=ds;
            break;
        case GLUT_KEY_RIGHT:
            glTranslatef(-ds,0,0);
            b.x+=ds;
            break;
    }
}

plist getPosition(int x, int y)
{
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    y = viewport[3]-y;
    plist tmp;
    tmp.x = ((x*2.0/viewport[2]-1)*viewport[2]/viewport[3]) * a + b.x;
    tmp.y = (2.0*y/viewport[3]-1) * a + b.y;
    tmp.t = 0;
    return tmp;
}

void mouseF(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        plist tmp = getPosition(x,y);
        plist_erase(&list);
        plist_add_front(&list, tmp.x, tmp.y, tmp.t);
    }
}

void idleF(void)
{
    if(ACTIVE)
        plist_evolve_ode(&list, NULL);
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("ODE Viewer");

    init();

    glutDisplayFunc(displayF);
    glutIdleFunc(idleF);
    glutKeyboardFunc(keyboardF);
    glutSpecialFunc(specialKeyboardF);
    glutMouseFunc(mouseF);
    glutReshapeFunc(reshapeF);

    glutMainLoop();
    return 0;
}

