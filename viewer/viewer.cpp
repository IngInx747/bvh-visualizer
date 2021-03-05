#include "viewer.h"

#include <chrono>

#ifdef MAC_OS
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif // MAC_OS

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <stb.h>
#include <imgui.h>

#include "ArcBall.h"
#include "Mesh.h"
#include "UI.h"

#include "collider.h"
#include "bvh.h"

using namespace OpenMesh;
using M = TheMesh;
using Point = M::Point;

// window width and height
static int g_win_width, g_win_height;
static int g_button;
static int g_startx, g_starty;
static int g_pick_x, g_pick_y;
static int g_shade_flag = 0;

// rotation quaternion and translation vector for the object
static glm::quat g_obj_rot(1, 0, 0, 0);
static glm::vec3 g_obj_trans(0, 0, 0);

// arc ball
static ArcBall g_arcball;

// mesh
static TheMesh g_mesh;

// method
static TheMethod g_method(&g_mesh);
static Collider g_rc(&g_mesh);

// Bvh
static Bvh g_bvh;

// Bvh debug
static std::vector<Aabb> g_bboxes;

inline double When()
{
#ifdef _WIN32
    SYSTEMTIME tp; GetSystemTime(&tp);
    return double(tp.wSecond) + double(tp.wMilliseconds) / 1E3;
#else
    struct timeval tp; gettimeofday(&tp,nullptr);
    return double(tp.tv_sec) + double(tp.tv_usec)/1E6;
#endif
}

// helper function to remind the user about commands, hot keys
void print_usage_message()
{
    printf("w  -  Wireframe Display\n");
    printf("f  -  Flat Shading \n");
    printf("s  -  Smooth Shading\n");
    printf("?  -  Help Information\n");
    printf("esc - Quit\n");
}

// setup the object, transform from the world to the object coordinate system
void transform_world2object(void)
{
    glTranslatef(g_obj_trans[0], g_obj_trans[1], g_obj_trans[2]);
    glm::mat4 rot = glm::toMat4(g_obj_rot);
    glMultMatrixf(glm::value_ptr(rot));
}

// the eye is always fixed at world z = +5
void setup_camera(void)
{
    glLoadIdentity();
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
}

// setup light
void setup_lights()
{
    GLfloat lightOnePosition[4] = { 0, 0, 1, 0 };
    GLfloat lightTwoPosition[4] = { 0, 0, -1, 0 };
    glLightfv(GL_LIGHT1, GL_POSITION, lightOnePosition);
    glLightfv(GL_LIGHT2, GL_POSITION, lightTwoPosition);
}

void draw_axis()
{
    glPushMatrix();
    // move the axes to the screen corner
    glTranslatef(0.0, 0.0, 0.0);
    // draw our axes
    glDisable(GL_LIGHTING);
    glLineWidth(5.0);
    glBegin(GL_LINES);
    // draw line for x axis
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    // draw line for y axis
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    // draw line for Z axis
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
    glEnd();
    // load the previous matrix
    glPopMatrix();
}

void draw_unit_box()
{
    glPushMatrix();
    // move the axes to the screen corner
    glTranslatef(0.0, 0.0, 0.0);
    // draw our axes
    glDisable(GL_LIGHTING);
    glLineWidth(1.0);
    glBegin(GL_LINES);
    glColor3f(1.0, 1.0, 1.0);
    // draw lines
    glVertex3f(-1.0, -1.0, -1.0); glVertex3f( 1.0, -1.0, -1.0);
    glVertex3f(-1.0,  1.0, -1.0); glVertex3f( 1.0,  1.0, -1.0);
    glVertex3f(-1.0, -1.0,  1.0); glVertex3f( 1.0, -1.0,  1.0);
    glVertex3f(-1.0,  1.0,  1.0); glVertex3f( 1.0,  1.0,  1.0);
    glVertex3f(-1.0, -1.0, -1.0); glVertex3f(-1.0,  1.0, -1.0);
    glVertex3f( 1.0, -1.0, -1.0); glVertex3f( 1.0,  1.0, -1.0);
    glVertex3f(-1.0, -1.0,  1.0); glVertex3f(-1.0,  1.0,  1.0);
    glVertex3f( 1.0, -1.0,  1.0); glVertex3f( 1.0,  1.0,  1.0);
    glVertex3f(-1.0, -1.0, -1.0); glVertex3f(-1.0, -1.0,  1.0);
    glVertex3f( 1.0, -1.0, -1.0); glVertex3f( 1.0, -1.0,  1.0);
    glVertex3f(-1.0,  1.0, -1.0); glVertex3f(-1.0,  1.0,  1.0);
    glVertex3f( 1.0,  1.0, -1.0); glVertex3f( 1.0,  1.0,  1.0);
    glEnd();
    // load the previous matrix
    glPopMatrix();
}

// draw bounding box
void draw_aabb(const Aabb& bbox, const vec3& color, float lineWidth = 1.0f)
{
    glPushMatrix();
    // move the axes to the screen corner
    glTranslatef(0.0, 0.0, 0.0);
    // draw our axes
    glDisable(GL_LIGHTING);
    glLineWidth(lineWidth);
    glBegin(GL_LINES);
    glColor3f(color.r, color.g, color.b);
    const vec3& a = bbox.pMin;
    const vec3& b = bbox.pMax;
    // draw lines
    glVertex3f(a.x, a.y, a.z); glVertex3f(b.x, a.y, a.z);
    glVertex3f(a.x, a.y, b.z); glVertex3f(b.x, a.y, b.z);
    glVertex3f(a.x, b.y, a.z); glVertex3f(b.x, b.y, a.z);
    glVertex3f(a.x, b.y, b.z); glVertex3f(b.x, b.y, b.z);
    glVertex3f(a.x, a.y, a.z); glVertex3f(a.x, b.y, a.z);
    glVertex3f(a.x, a.y, b.z); glVertex3f(a.x, b.y, b.z);
    glVertex3f(b.x, a.y, a.z); glVertex3f(b.x, b.y, a.z);
    glVertex3f(b.x, a.y, b.z); glVertex3f(b.x, b.y, b.z);
    glVertex3f(a.x, a.y, a.z); glVertex3f(a.x, a.y, b.z);
    glVertex3f(a.x, b.y, a.z); glVertex3f(a.x, b.y, b.z);
    glVertex3f(b.x, a.y, a.z); glVertex3f(b.x, a.y, b.z);
    glVertex3f(b.x, b.y, a.z); glVertex3f(b.x, b.y, b.z);
    glEnd();
    // load the previous matrix
    glPopMatrix();
}

void draw_mesh()
{
    glEnable(GL_LIGHTING);

    glLineWidth(1.0);
    glColor3f(220.f / 255.f, 220.f / 255.f, 220.f / 255.f);
    for (M::FaceIter fiter = g_mesh.faces_begin(); fiter != g_mesh.faces_end(); ++fiter)
    {
        glBegin(GL_POLYGON);
        FaceHandle hF = *fiter;

        for (M::FaceVertexIter fviter = g_mesh.fv_iter(hF); fviter != g_mesh.fv_end(hF); ++fviter)
        {
            VertexHandle hV = *fviter;
            const Point& p = g_mesh.point(hV);
            Point n;
            switch (g_shade_flag)
            {
            case 0:
                n = g_mesh.normal(hF);
                break;
            case 1:
                n = g_mesh.normal(hV);
                break;
            }
            glNormal3d(n[0], n[1], n[2]);
            glVertex3d(p[0], p[1], p[2]);
        }
        glEnd();
    }
}

void draw_selected_vertices()
{
    glDisable(GL_LIGHTING);

    glPointSize(15.0f);
    glColor3f(0.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
    for (M::VertexIter viter = g_mesh.vertices_begin(); viter != g_mesh.vertices_end(); ++viter)
    {
        VertexHandle hV = *viter;
        if (g_mesh.status(hV).selected())
        {
            Point p = g_mesh.point(hV);
            glVertex3f(p[0], p[1], p[2]);
        }
    }
    glEnd();
}

void draw_selected_edges()
{
    glDisable(GL_LIGHTING);

    glLineWidth(5.);
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    for (M::EdgeIter eiter = g_mesh.edges_begin(); eiter != g_mesh.edges_end(); ++eiter)
    {
        EdgeHandle hE = *eiter;
        if (g_mesh.status(hE).selected())
        {
            EdgeHandle hE = *eiter;
            HalfedgeHandle hH = g_mesh.halfedge_handle(hE, 0);
            VertexHandle hV0 = g_mesh.from_vertex_handle(hH);
            VertexHandle hV1 = g_mesh.to_vertex_handle(hH);
            Point p0 = g_mesh.point(hV0);
            Point p1 = g_mesh.point(hV1);
            Point n0 = g_mesh.normal(hV0);
            Point n1 = g_mesh.normal(hV1);
            float offset = 0.001f;
            p0 = p0 + n0 * offset;
            p1 = p1 + n1 * offset;
            glVertex3f(p0[0], p0[1], p0[2]);
            glVertex3f(p1[0], p1[1], p1[2]);
        }
    }
    glEnd();
}

void draw_selected_faces()
{
    PrimitiveBound bound(g_mesh);

    glEnable(GL_LIGHTING);
    for (M::FaceIter fiter = g_mesh.faces_begin(); fiter != g_mesh.faces_end(); ++fiter)
    {
        FaceHandle hF = *fiter;

        if (g_mesh.status(hF).selected())
        {
            glLineWidth(1.0);
            glColor3f(1, 0, 1);
            glBegin(GL_POLYGON);
            for (M::FaceVertexIter fviter = g_mesh.fv_iter(hF); fviter != g_mesh.fv_end(hF); ++fviter)
            {
                VertexHandle hV = *fviter;
                Point p = g_mesh.point(hV);
                Point n;
                switch (g_shade_flag)
                {
                case 0:
                    n = g_mesh.normal(hF);
                    break;
                case 1:
                    n = g_mesh.normal(hV);
                    break;
                }
                float offset = 0.001f;
                p += n * offset;
                glNormal3d(n[0], n[1], n[2]);
                glVertex3d(p[0], p[1], p[2]);
            }
            glEnd();

            //draw_aabb(bound(hF), { 1,1,1 }, 5.f);
        }
    }
}

void draw_bvh(const Bvh& bvh)
{
    //for (const NodePtr& node : bvh.GetNodes())
    //    if (IsLeaf(node))
    //        draw_aabb(node->bbox, { 1,1,1 });
    for (const BvhNode& node : bvh.GetNodes())
        if (IsLeaf(node))
            draw_aabb(node.bbox, { 1,1,1 });
}

void draw_aabb(const Aabb& bbox)
{
    g_bboxes.push_back(bbox);
}

void pick_attribute(int x, int y)
{
    double modelViewMatrix[16];
    double projectionMatrix[16];
    int viewport[4];

    setup_camera();
    glPushMatrix();
    transform_world2object();

    // Get transformation matrix in OpenGL
    glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);

    glPopMatrix();

    // Un-project from 2D screen point to real 3D position.
    double nearPlaneLocation[3];
    gluUnProject(x, y, 0.0, modelViewMatrix, projectionMatrix,
        viewport, &nearPlaneLocation[0], &nearPlaneLocation[1],
        &nearPlaneLocation[2]);

    double farPlaneLocation[3];
    gluUnProject(x, y, 1.0, modelViewMatrix, projectionMatrix,
        viewport, &farPlaneLocation[0], &farPlaneLocation[1],
        &farPlaneLocation[2]);

    vec3 ro(nearPlaneLocation[0], nearPlaneLocation[1], nearPlaneLocation[2]);

    vec3 rd(farPlaneLocation[0] - nearPlaneLocation[0],
        farPlaneLocation[1] - nearPlaneLocation[1],
        farPlaneLocation[2] - nearPlaneLocation[2]);

    // Bvh debug only
    g_bboxes.clear();

    float dist = 1e10f;
    FaceHandle hFs;
    //double dt = When();
    auto start = std::chrono::steady_clock::now();

    if (UIOption::accel_mode)
        hFs = g_rc.collide(g_bvh, ro, rd, dist);
    else
        hFs = g_rc.collide(ro, rd, dist);

    //dt = When() - dt;
    //printf("Using time = %lf\n", dt);
    auto end = std::chrono::steady_clock::now();
    printf("Elapsed time = %zd us\n", std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

    if (hFs.is_valid())
    {
        Point hit = g2o(ro + rd * dist);

        if (UIOption::select_mode == UIOption::SELECT_EDGE)
        {
            EdgeHandle hEs;
            float minD = 1e20f; // distance from hit point to each edge

            for (M::FaceEdgeIter eiter = g_mesh.fe_begin(hFs); eiter != g_mesh.fe_end(hFs); ++eiter)
            {
                EdgeHandle hE = *eiter;
                HalfedgeHandle hH = g_mesh.halfedge_handle(hE, 0);
                VertexHandle hV0 = g_mesh.from_vertex_handle(hH);
                VertexHandle hV1 = g_mesh.to_vertex_handle(hH);
                Point p0 = g_mesh.point(hV0);
                Point p1 = g_mesh.point(hV1);
                float a = (p0 - hit).norm();
                float b = (p1 - hit).norm();
                float d = (p1 - p0).norm();
                float p = (a + b + d) * 0.5f;
                float s = std::sqrtf(p * (p - a) * (p - b) * (p - d));
                float h = 2 * s / d;

                if (h < minD)
                {
                    hEs = hE;
                    minD = h;
                }
            }

            if (g_mesh.status(hEs).selected())
                g_mesh.status(hEs).set_selected(false);
            else
                g_mesh.status(hEs).set_selected(true);
        }

        if (UIOption::select_mode == UIOption::SELECT_VERT)
        {
            VertexHandle hVs;
            float minD = 1e20f;

            for (M::FaceVertexIter viter = g_mesh.fv_begin(hFs); viter != g_mesh.fv_end(hFs); ++viter)
            {
                VertexHandle hV = *viter;
                Point p = g_mesh.point(hV);
                float d = (p - hit).norm();

                if (d < minD)
                {
                    hVs = hV;
                    minD = d;
                }
            }

            if (g_mesh.status(hVs).selected())
                g_mesh.status(hVs).set_selected(false);
            else
                g_mesh.status(hVs).set_selected(true);
        }

        if (UIOption::select_mode == UIOption::SELECT_FACE)
        {
            if (g_mesh.status(hFs).selected())
                g_mesh.status(hFs).set_selected(false);
            else
                g_mesh.status(hFs).set_selected(true);
        }
    }
}

// display call back function
void display()
{
    // clear frame buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setup_lights();
    // transform from the eye coordinate system to the world system
    setup_camera();
    glPushMatrix();
    // transform from the world to the ojbect coordinate system
    transform_world2object();

    // plot aligned box of object
    draw_axis();
    //draw_unit_box();

    // draw selected attributes
    draw_selected_vertices();
    draw_selected_edges();
    draw_selected_faces();

    // draw mesh
    draw_mesh();

    // bvh debug
    //draw_bvh(g_bvh);
    if (UIOption::show_bvh_bbox)
        for (const Aabb& bbox : g_bboxes)
            draw_aabb(bbox, { 1,1,1 }, 2);

    // ui
    UI::render();

    glPopMatrix();
    glutSwapBuffers();
    glutPostRedisplay();
}

// Called when a "resize" event is received by the window.
void reshape(int w, int h)
{
    float ar;

    g_win_width = w;
    g_win_height = h;

    ar = (float)(w) / h;
    glViewport(0, 0, w, h); // Set Viewport
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(
        45.0, // field of view in degrees
        ar,   // aspect ratio
        0.1,  // Z near
        100.0 // Z far
    );

    glMatrixMode(GL_MODELVIEW);

    UI::reshape(w, h);

    glutPostRedisplay();
}

// Keyboard call back function
void keyBoard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'f':
        // Flat Shading
        glPolygonMode(GL_FRONT, GL_FILL);
        g_shade_flag = 0;
        break;
    case 's':
        // Smooth Shading
        glPolygonMode(GL_FRONT, GL_FILL);
        g_shade_flag = 1;
        break;
    case 'w':
        // Wireframe mode
        glPolygonMode(GL_FRONT, GL_LINE);
        break;
    case '?':
        print_usage_message();
        break;
    case 27:
        exit(0);
        break;
    }

    UI::keyboard(key, x, y);

    glutPostRedisplay();
}

// mouse click call back function
void mouseClick(int button, int state, int x, int y)
{
    /* set up an arcball around the Eye's center
    switch y coordinates to right handed system  */

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        g_button = GLUT_LEFT_BUTTON;
        g_pick_x = x;
        g_pick_y = y;
        g_arcball.set(
            (g_win_width < g_win_height) ? g_win_width / 2 : g_win_height / 2,
            x - g_win_width / 2,
            g_win_height / 2 - y);
    }
    
    if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
    {
        g_startx = x;
        g_starty = y;
        g_button = GLUT_MIDDLE_BUTTON;
    }
    
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        g_startx = x;
        g_starty = y;
        g_button = GLUT_RIGHT_BUTTON;
    }
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        if (g_pick_x == x && g_pick_y == y)
        {
            pick_attribute(x, g_win_height - y);
        }
    }

    UI::mouse_click(button, state, x, y);

    return;
}

// mouse motion call back function
void mouseMove(int x, int y)
{
    glm::vec3 trans;
    glm::quat rot;
    
    // rotation, call g_arcball
    if (g_button == GLUT_LEFT_BUTTON)
    {
        rot = g_arcball.update_quat(x - g_win_width / 2, g_win_height / 2 - y);
        g_obj_rot = rot * g_obj_rot;
        glutPostRedisplay();
    }
    
    // xy translation
    if (g_button == GLUT_MIDDLE_BUTTON)
    {
        double scale = 10. / g_win_height;
        trans = glm::vec3(scale * (x - g_startx), scale * (g_starty - y), 0);
        g_startx = x;
        g_starty = y;
        g_obj_trans = g_obj_trans + trans;
        glutPostRedisplay();
    }
    
    // zoom in and out
    if (g_button == GLUT_RIGHT_BUTTON)
    {
        double scale = 10. / g_win_height;
        trans = glm::vec3(0, 0, scale * (g_starty - y));
        g_startx = x;
        g_starty = y;
        g_obj_trans = g_obj_trans + trans;
        glutPostRedisplay();
    }

    UI::mouse_move(x, y);
}

// setup GL states
void setupGLstate()
{
    GLfloat lightOneColor[] = { 1, 1, 1, 1.f };
    GLfloat globalAmb[] = { 0.1f, 0.1f, 0.1f, 1 };
    GLfloat lightOnePosition[] = { 0.f, 0.f, 1.f, 1.f };
    GLfloat lightTwoPosition[] = { 0.f, 0.f, -1.f, 1.f };

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.17f, 0.17f, 0.41f, 0);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightOneColor);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightOneColor);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glLightfv(GL_LIGHT1, GL_POSITION, lightOnePosition);
    glLightfv(GL_LIGHT2, GL_POSITION, lightTwoPosition);

    const GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64.0f);

    GLfloat mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_diffuse[] = { 0.01f, 0.01f, 0.01f, 1.0f };
    GLfloat mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat mat_shininess[] = { 32 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

// initialize window
void initOpenGL(int argc, char* argv[])
{
    // glut stuff
    glutInit(&argc, argv); // Initialize GLUT
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Mesh Viewer"); // Create window with given title
    glViewport(0, 0, 600, 600);

    glutDisplayFunc(display); // Set-up callback functions
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMove);
    glutKeyboardFunc(keyBoard);
    setupGLstate();
}

// build Bvh of mesh
void initBvh(Bvh& bvh, TheMesh& mesh)
{
    PrimitiveBound bound(mesh);
    PrimitiveSplit split(bound);
    std::vector<Primitive> primitives;

    for (auto& fh : mesh.faces())
    {
        primitives.push_back(fh);
    }

    bvh.Build(primitives, bound, split, 1);
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [mesh name]\n", argv[0]);
        return 1;
    }

    OpenMesh::IO::Options opt;
    if (!OpenMesh::IO::read_mesh(g_mesh, argv[1], opt))
    {
        fprintf(stderr, "ERROR: Cannot load mesh: %s\n", argv[1]);
        return 1;
    }

    resize_unit_box(g_mesh);
    g_mesh.update_normals();

    initBvh(g_bvh, g_mesh);

    int numInnrNode = 0, numLeafNode = 0;
    for (const auto& node : g_bvh.GetNodes())
    {
        if (IsLeaf(node))
        {
            ++numLeafNode;
            //printf("offset = %d, length = %d\n", Offset(node), Length(node));
        }
        else ++numInnrNode;
    }
    printf("Total node num = %zd\n", g_bvh.GetNodes().size());
    printf("Leaf  node num = %zd\n", numLeafNode);
    printf("Inter node num = %zd\n", numInnrNode);

    print_usage_message();

    initOpenGL(argc, argv);

    UI::initialize();

    glutMainLoop(); // Start GLUT event-processing loop

    UI::shutdown();

	return 0;
}