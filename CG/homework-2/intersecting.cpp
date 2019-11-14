#include <iostream>
#include <fstream>
#include <vector>
#include "GL/glut.h"

using namespace std;

class Edge
{
public:
	int ymax;
	float x;
	float dx;
	float z;
	float dz;
	float r;
	float dr;
	float g;
	float dg;
	float b;
	float db;
	Edge* next;
};

class Point
{
public:
	int x;
	int y;
	int z;
	int r;
	int g;
	int b;
	Point(int x, int y, int z, int r, int g, int b)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->r = r;
		this->g = g;
		this->b = b;
	}
};

const int windowWidth = 600;
const int windowHeight = 400;

void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
}

void polygonScan()
{
	fstream file;
	file.open("intersecting.tri");

	// 深度缓冲
	int zBuf[windowHeight][windowWidth];

	for (int i = 0; i < windowHeight; i++)
		for (int j = 0; j < windowWidth; j++)
			zBuf[i][j] = -1;

	glBegin(GL_POINTS);
	for (int i = 1; i <= 2; i++) {
		vector<Point> vertices;
		int x[3], y[3], z[3], r[3], g[3], b[3];
		file >> x[0] >> y[0] >> z[0] >> x[1] >> y[1] >> z[1] >> x[2] >> y[2] >> z[2]
			>> r[0] >> g[0] >> b[0] >> r[1] >> g[1] >> b[1] >> r[2] >> g[2] >> b[2];
		vertices.push_back(Point(x[0] + windowWidth / 2, y[0] + windowHeight / 2, z[0], r[0], g[0], b[0]));
		vertices.push_back(Point(x[1] + windowWidth / 2, y[1] + windowHeight / 2, z[1], r[1], g[1], b[1]));
		vertices.push_back(Point(x[2] + windowWidth / 2, y[2] + windowHeight / 2, z[2], r[2], g[2], b[2]));

		// 计算最高点的y坐标
		int maxY = 0;
		for (int i = 0; i < vertices.size(); i++)
			if (vertices[i].y > maxY)
				maxY = vertices[i].y;
			
		// 初始化 ET 和 AET
		Edge *pET[windowHeight];
		Edge *AET;
		for (int i = 0; i < maxY; i++)
		{
			pET[i] = new Edge();
			pET[i]->next = nullptr;
		}
		AET = new Edge();
		AET->next = nullptr;

		// 建立边表ET
		for (int i = 0; i < vertices.size(); i++)
		{
			// 取出当前点1前后相邻的共4个点，点1与点2的连线作为本次循环处理的边，另外两个点点0和点3用于计算奇点
			int x0 = vertices[(i - 1 + vertices.size()) % vertices.size()].x;
			int x1 = vertices[i].x;
			int x2 = vertices[(i + 1) % vertices.size()].x;
			int x3 = vertices[(i + 2) % vertices.size()].x;
			int y0 = vertices[(i - 1 + vertices.size()) % vertices.size()].y;
			int y1 = vertices[i].y;
			int y2 = vertices[(i + 1) % vertices.size()].y;
			int y3 = vertices[(i + 2) % vertices.size()].y;
			int z1 = vertices[i].z;
			int z2 = vertices[(i + 1) % vertices.size()].z;
			int r1 = vertices[i].r;
			int r2 = vertices[(i + 1) % vertices.size()].r;
			int g1 = vertices[i].g;
			int g2 = vertices[(i + 1) % vertices.size()].g;
			int b1 = vertices[i].b;
			int b2 = vertices[(i + 1) % vertices.size()].b;
			// 水平线直接舍弃
			if (y1 == y2)
				continue;
			// 分别计算下端点y坐标、上端点y坐标、下端点x坐标和斜率倒数
			int ymin = y1 > y2 ? y2 : y1;
			int ymax = y1 > y2 ? y1 : y2;
			float x = y1 > y2 ? x2 : x1;
			float z = y1 > y2 ? z2 : z1;
			float r = y1 > y2 ? r2 : r1;
			float g = y1 > y2 ? g2 : g1;
			float b = y1 > y2 ? b2 : b1;
			float dx = (x1 - x2) * 1.0f / (y1 - y2);
			float dz = (z1 - z2) * 1.0f / (y1 - y2);
			float dr = (r1 - r2) * 1.0f / (y1 - y2);
			float dg = (g1 - g2) * 1.0f / (y1 - y2);
			float db = (b1 - b2) * 1.0f / (y1 - y2);
			// 奇点特殊处理
			if (((y0 < y1) && (y1 < y2)) || ((y0 > y1) && (y1 > y2)) || ((y1 < y2) && (y2 < y3)) || ((y1 > y2) && (y2 > y3)))
			{
				ymin++;
				x += dx;
				z += dz;
				r += dr;
				g += dg;
				b += db;
			}
			// 创建新边，插入边表ET
			Edge *p = new Edge();
			p->ymax = ymax;
			p->x = x;
			p->dx = dx;
			p->z = z;
			p->dz = dz;
			p->r = r;
			p->dr = dr;
			p->g = g;
			p->dg = dg;
			p->b = b;
			p->db = db;
			p->next = pET[ymin]->next;
			pET[ymin]->next = p;
		}

		// 扫描线从下往上扫描，y坐标每次加1
		for (int i = 0; i < maxY; i++)
		{
			// 取出ET中当前扫描行的所有边并按x的递增顺序（若x相等则按dx的递增顺序）插入AET
			while (pET[i]->next)
			{
				// 取出ET中当前扫描行表头位置的边
				Edge *pInsert = pET[i]->next;
				Edge *p = AET;
				// 在AET中搜索合适的插入位置
				while (p->next)
				{
					if (pInsert->x > p->next->x)
					{
						p = p->next;
						continue;
					}
					if (pInsert->x == p->next->x && pInsert->dx > p->next->dx)
					{
						p = p->next;
						continue;
					}
					// 找到位置
					break;
				}
				// 将pInsert从ET中删除，并插入AET的当前位置
				pET[i]->next = pInsert->next;
				pInsert->next = p->next;
				p->next = pInsert;
			}

			// AET中的边两两配对并填色
			Edge *p = AET;

			while (p->next && p->next->next)
			{
				float x1 = p->next->x;
				float x2 = p->next->next->x;
				float z1 = p->next->z;
				float z2 = p->next->next->z;
				float r1 = p->next->r;
				float r2 = p->next->next->r;
				float g1 = p->next->g;
				float g2 = p->next->next->g;
				float b1 = p->next->b;
				float b2 = p->next->next->b;
				float ddz = (z1 - z2) * 1.0f / (x1 - x2);
				float ddr = (r1 - r2) * 1.0f / (x1 - x2);
				float ddg = (g1 - g2) * 1.0f / (x1 - x2);
				float ddb = (b1 - b2) * 1.0f / (x1 - x2);
				float pz = z1;
				float pr = r1;
				float pg = g1;
				float pb = b1;
				for (int x = x1; x < x2; x++)
				{
					if (pz > zBuf[x][i]) {
						glColor3f(pr, pg, pb);
						glVertex2i(x, i);
						zBuf[x][i] = pz;
					}
					pz += ddz;
					pr += ddr;
					pg += ddg;
					pb += ddb;
				}
				p = p->next->next;
			}
			cout << endl;

			//删除AET中满足y=ymax的边
			p = AET;
			while (p->next)
			{
				if (p->next->ymax == i)
				{
					Edge *pDelete = p->next;
					p->next = pDelete->next;
					pDelete->next = nullptr;
					delete pDelete;
				}
				else
				{
					p = p->next;
				}
			}

			// 更新AET中边的x值，进入下一循环
			p = AET;
			while (p->next)
			{
				p->next->x += p->next->dx;
				p->next->z += p->next->dz;
				p->next->r += p->next->dr;
				p->next->g += p->next->dg;
				p->next->b += p->next->db;
				p = p->next;
			}
		}
	}
	glEnd();
	glFlush();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(50, 100);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("hw2");
	init();
	glutDisplayFunc(polygonScan);
	glutMainLoop();
	return 0;
}
