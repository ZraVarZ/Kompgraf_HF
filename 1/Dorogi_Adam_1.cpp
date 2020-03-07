#include <GL/glut.h>
#include <bevgrafmath2017.h>
#include <math.h>
#include <vector>
#include <iostream>

using namespace std;

GLsizei winWidth = 1024, winHeight = 768;
GLint dragged = -1;
GLint keyStates[256];
vector<vec2> points;
mat24 G;
mat4 Mm = { {-1.0f, 3.0f, -3.0f, 1.0f},
			{3.0f, -6.0f, 0.0f, 4.0f},
			{-3.0f, 3.0f, 3.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 0.0f} };
mat4 M = 1.0f / 6.0f * Mm;
vec4 T;
vec2 S, E, Pm1, Pnp1;

void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, winWidth, 0.0, winHeight);
	glShadeModel(GL_FLAT);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(7.0);
	glLineWidth(3.0);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < points.size(); i++)
	{
		glVertex2f(points[i].x, points[i].y);
	}
	glEnd();

	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < points.size(); i++)
	{
		glVertex2f(points[i].x, points[i].y);
	}
	glEnd();

	if (points.size() >= 2)
	{
		Pm1 = points[0] * 2 - points[1];
		S = 1 / 6 * (Pm1 + points[0] * 4 + points[1]);
	}
	if (points.size() > 3)
	{
		Pnp1 = points[(points.size() - 1)] * 2 - points[(points.size() - 2)];
		E = 1 / 6 * (points[(points.size() - 1)] + points[(points.size() - 1)] * 4 + Pnp1);
	}

	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINE_STRIP);
	if (points.size() > 3)
	{
		G = { Pm1, points[0], points[1], points[2] };
		for (float t = 0.0; t < 1.0; t += 0.01)
		{
			T = { t * t * t, t * t, t, 1 };
			glVertex2f((G * M * T).x, (G * M * T).y);
		}

		for (int i = 0; i < points.size() - 3; i++)
		{
			G = { points[i], points[i + 1], points[i + 2], points[i + 3] };
			for (float t = 0.0; t < 1.0; t += 0.01)
			{
				T = { t * t * t, t * t, t, 1 };
				glVertex2f((G * M * T).x, (G * M * T).y);
			}
		}

		G = { points[points.size() - 3], points[points.size() - 2], points[points.size() - 1], Pnp1 };
		for (float t = 0.0; t < 1.0; t += 0.01)
		{
			T = { t * t * t, t * t, t, 1 };
			glVertex2f((G * M * T).x, (G * M * T).y);
		}
	}
	glEnd();

	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_POINTS);
	if (points.size() > 3)
	{
		float t = 1;
		T = { t * t * t, t * t, t, 1 };

		G = { Pm1, points[0], points[1], points[2] };
		glVertex2f((G * M * T).x, (G * M * T).y);

		for (int i = 0; i < points.size() - 3; i++)
		{
			G = { points[i], points[i + 1], points[i + 2], points[i + 3] };
			T = { t * t * t, t * t, t, 1 };
			glVertex2f((G * M * T).x, (G * M * T).y);
		}
	}
	glEnd();

	glutSwapBuffers();
}

void keyPressed(unsigned char key, int x, int y)
{
	keyStates[key] = 1;
}

void keyUp(unsigned char key, int x, int y)
{
	keyStates[key] = 0;
}

void keyOperations()
{
	if (keyStates[27]) { exit(0); }
}

void update(int e)
{
	keyOperations();
	glutPostRedisplay();
	glutTimerFunc(5, update, 0);
}

GLint getActivePoint1(vector<vec2> p /*vec2 p[]*/, GLint size, GLint sens, GLint x, GLint y)
{
	GLint i, s = sens * sens;
	vec2 P = { (float)x, (float)y };

	for (i = 0; i < size; i++)
		if (dist2(p[i], P) < s)
			return i;
	return -1;
}

void processMouse(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
	GLint i;
	if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN)
		if ((i = getActivePoint1(points, points.size(), 8, xMouse, winHeight - yMouse)) != -1)
			dragged = i;
		else
		{
			points.push_back(vec2(xMouse, winHeight - yMouse));
		}
	if (button == GLUT_LEFT_BUTTON && action == GLUT_UP)
		dragged = -1;
	if (button == GLUT_RIGHT_BUTTON && action == GLUT_DOWN)
		if ((i = getActivePoint1(points, points.size(), 8, xMouse, winHeight - yMouse)) != -1)
		{
			points.erase(points.begin() + i);
		}
}

void processMouseActiveMotion(GLint xMouse, GLint yMouse)
{
	GLint i;
	if (dragged >= 0) {
		points[dragged].x = xMouse;
		points[dragged].y = winHeight - yMouse;
		glutPostRedisplay();
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Dorogi Adam 1");
	init();
	glutDisplayFunc(display);
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyUp);
	glutTimerFunc(5, update, 0);
	glutMainLoop();
	return 0;
}