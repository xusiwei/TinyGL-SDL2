// OpenGL 1.1 API 三角形示例程序
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui.h"
#include <GL/gl.h>

#ifndef M_PI
#define M_PI 3.14159265
#endif

// 初始化OpenGL渲染器状态
void init() {
  glEnable(GL_DEPTH_TEST); // 启用深度测试
}

// 绘制一个三角形
void draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色和深度缓冲区
  glLoadIdentity(); // 重置当前的模型观察矩阵

  // 移动到屏幕中间
  glTranslatef(0.0f, 0.0f, -5.0f);

  // 绘制三角形
  glBegin(GL_TRIANGLES);
  glColor3f(1.0f, 0.0f, 0.0f);    // 红色
  glVertex3f(-1.0f, -1.0f, 0.0f); // 第一个顶点

  glColor3f(0.0f, 1.0f, 0.0f);   // 绿色
  glVertex3f(1.0f, -1.0f, 0.0f); // 第二个顶点

  glColor3f(0.0f, 0.0f, 1.0f);  // 蓝色
  glVertex3f(0.0f, 1.0f, 0.0f); // 第三个顶点
  glEnd();

  tkSwapBuffers(); // 交换前后缓冲区
}

// 窗口大小改变时的回调函数
void reshape(int w, int h) {
  glViewport(0, 0, w, h);      // 设置视口大小
  glMatrixMode(GL_PROJECTION); // 切换到投影矩阵
  glLoadIdentity();            // 重置投影矩阵
  gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.1, 100.0); // 设置透视投影
  glMatrixMode(GL_MODELVIEW); // 切换回模型观察矩阵
  glLoadIdentity();           // 重置模型观察矩阵
}

void idle(void) { draw(); }

GLenum key(int k, GLenum mask) {}

int main(int argc, char **argv) {
  // glutInit(&argc, argv); // 初始化GLUT
  // glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // 设置显示模式
  // glutInitWindowSize(640, 480); // 设置窗口大小
  // glutInitWindowPosition(100, 100); // 设置窗口位置
  // glutCreateWindow("OpenGL Triangle"); // 创建窗口

  // init(); // 初始化OpenGL渲染器状态
  // glutDisplayFunc(display); // 设置显示回调函数
  // glutReshapeFunc(reshape); // 设置窗口大小改变的回调函数

  // glutMainLoop(); // 进入GLUT事件处理循环

  return ui_loop(argc, argv, "OpenGL Triangle");
}
