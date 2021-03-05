#include "UI.h"

#include <imgui.h>
#include <backends/imgui_impl_glut.h>
#include <backends/imgui_impl_opengl2.h>

int UIOption::select_mode = UIOption::SELECT_NONE;
bool UIOption::accel_mode = 1;
bool UIOption::show_bvh_bbox = 1;

void UI::initialize()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGLUT_Init();
    //ImGui_ImplGLUT_InstallFuncs();
    ImGui_ImplOpenGL2_Init();
}

void UI::shutdown()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
}

void UI::render()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGLUT_NewFrame();

    // display callback function
    _display_func();

    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    ImGui::EndFrame();
}

void UI::reshape(int w, int h)
{
    ImGui_ImplGLUT_ReshapeFunc(w, h);
}

void UI::keyboard(unsigned char key, int x, int y)
{
    ImGui_ImplGLUT_KeyboardFunc(key, x, y);
}

void UI::mouse_move(int x, int y)
{
    ImGui_ImplGLUT_MotionFunc(x, y);
}

void UI::mouse_click(int button, int state, int x, int y)
{
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);
}

void UI::_display_func()
{
    ImGui::Begin("Menu");

    ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);

    if (ImGui::CollapsingHeader("Select Mode"))
    {
        if (ImGui::Button("None"))
        {
            UIOption::select_mode = UIOption::SELECT_NONE;
        }
        ImGui::SameLine();
        if (ImGui::Button("Vert"))
        {
            UIOption::select_mode = UIOption::SELECT_VERT;
        }

        if (ImGui::Button("Edge"))
        {
            UIOption::select_mode = UIOption::SELECT_EDGE;
        }
        ImGui::SameLine();
        if (ImGui::Button("Face"))
        {
            UIOption::select_mode = UIOption::SELECT_FACE;
        }

        ImGui::Checkbox("Accel", &UIOption::accel_mode);
        ImGui::SameLine();
        ImGui::Checkbox("BBox", &UIOption::show_bvh_bbox);
    }

    ImGui::End();
}
