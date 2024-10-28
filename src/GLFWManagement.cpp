#include "GLFWManagement.h"

#include "Enums.h"
#include "OpenGLManager.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string_view>

namespace GLFWglobals
{
    extern OpenGLManager* openGLManager = nullptr;
    extern GLFWwindow* mainWindow = nullptr;

    extern int mainWindowWidth = 0;
    extern int mainWindowHeight = 0;

    extern float deltaTime = 0.0f;
    extern float lastFrameTime = 0.0f;

    extern bool firstMousePositionChange = true;

    extern float lastMousePositionX = 0.0f;
    extern float lastMousePositionY = 0.0f;
}

namespace GLFW
{
    void init(std::string_view mainWinTitle,
        int mainWinWidth, int mainWinHeight,
        int majorGLVersion, int minorGLVersion,
        std::string_view executablePath)
    {
        GLFWglobals::lastMousePositionX = mainWinWidth / 2.0f;
        GLFWglobals::lastMousePositionY = mainWinHeight / 2.0f;

        GLFWglobals::mainWindowWidth = mainWinWidth;
        GLFWglobals::mainWindowHeight = mainWinHeight;

        if (!glfwInit()) { std::exit(EXIT_FAILURE); }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorGLVersion);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorGLVersion);

        GLFWglobals::mainWindow = glfwCreateWindow(mainWinWidth, mainWinHeight, mainWinTitle.data(), NULL, NULL);

        glfwMakeContextCurrent(GLFWglobals::mainWindow);

        glfwSetCursorPosCallback(GLFWglobals::mainWindow, processCursorPosition);
        glfwSetKeyCallback(GLFWglobals::mainWindow, processKeysClick);
        glfwSetScrollCallback(GLFWglobals::mainWindow, processScroll);
        glfwSetMouseButtonCallback(GLFWglobals::mainWindow, processMouseClick);
        glfwSetWindowSizeCallback(GLFWglobals::mainWindow, processWindowResize);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::exit(EXIT_FAILURE); }

        GLFWglobals::openGLManager = new OpenGLManager(executablePath, GLFWglobals::mainWindowWidth, GLFWglobals::mainWindowHeight);
        GLFWglobals::openGLManager->init(GLFWglobals::mainWindow);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplGlfw_InitForOpenGL(GLFWglobals::mainWindow, true);
        ImGui_ImplOpenGL3_Init();

        ImGui::StyleColorsLight();
    }

    void renderLoop()
    {
        while (!glfwWindowShouldClose(GLFWglobals::mainWindow))
        {
            calcDeltaTimePerFrame();

            GLFWglobals::openGLManager->display(GLFWglobals::mainWindow, glfwGetTime());
            renderMenu();

            glfwSwapBuffers(GLFWglobals::mainWindow);
            glfwPollEvents();
        }
    }

    void destroy()
    {
        if (GLFWglobals::openGLManager) delete GLFWglobals::openGLManager;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(GLFWglobals::mainWindow);
        glfwTerminate();
    }

    void processCursorPosition(GLFWwindow* window, double xposIn, double yposIn)
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            float xpos = static_cast<float>(xposIn);
            float ypos = static_cast<float>(yposIn);

            if (GLFWglobals::firstMousePositionChange)
            {
                GLFWglobals::lastMousePositionX = xpos;
                GLFWglobals::lastMousePositionY = ypos;
                GLFWglobals::firstMousePositionChange = false;
            }

            float xoffset = xpos - GLFWglobals::lastMousePositionX;
            float yoffset = GLFWglobals::lastMousePositionY - ypos; // reversed since y-coordinates go from bottom to top

            GLFWglobals::lastMousePositionX = xpos;
            GLFWglobals::lastMousePositionY = ypos;

            GLFWglobals::openGLManager->mouseMovement(xoffset, yoffset);
        }
    }

    void renderMenu()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Display mode"))
            {
                if (ImGui::MenuItem("Trajectory"))
                {
                    GLFWglobals::openGLManager->setDisplayMode(Trajectory);
                }
                if (ImGui::BeginMenu("Trajectory and cuts"))
                {
                    if (ImGui::MenuItem("Filled cuts"))
                    {
                        GLFWglobals::openGLManager->setDisplayMode(Trajectory_and_filled_cuts);
                    }
                    if (ImGui::MenuItem("Frame cuts"))
                    {
                        GLFWglobals::openGLManager->setDisplayMode(Trajectory_and_frame_cuts);
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Replicated cut"))
                {
                    if (ImGui::BeginMenu("Filled surface"))
                    {
                        if (ImGui::MenuItem("Smoothing normals light"))
                        {
                            GLFWglobals::openGLManager->setDisplayMode(Replicated_cut_smoothing_normals_filled_surface);
                        }
                        if (ImGui::MenuItem("No smoothing normals light"))
                        {
                            GLFWglobals::openGLManager->setDisplayMode(Replicated_cut_no_smoothing_normals_filled_surface);
                        }
                        if (ImGui::MenuItem("Smoothing normals light with normals display"))
                        {
                            GLFWglobals::openGLManager->setDisplayMode(Replicated_cut_smoothing_normals_display_filled_surface);
                        }
                        if (ImGui::MenuItem("No smoothing normals light with normals display"))
                        {
                            GLFWglobals::openGLManager->setDisplayMode(Replicated_cut_no_smoothing_normals_display_filled_surface);
                        }
                        if (ImGui::MenuItem("No light model"))
                        {
                            GLFWglobals::openGLManager->setDisplayMode(Replicated_cut_no_light_filled_surface);
                        }

                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Frame surface"))
                    {
                        if (ImGui::MenuItem("Simple frame"))
                        {
                            GLFWglobals::openGLManager->setDisplayMode(Replicated_cut_simple_frame_surface);
                        }
                        if (ImGui::MenuItem("Frame with trajectory"))
                        {
                            GLFWglobals::openGLManager->setDisplayMode(Replicated_cut_trajectory_frame_surface);
                        }
                        if (ImGui::MenuItem("Frame with trajectory and cuts"))
                        {
                            GLFWglobals::openGLManager->setDisplayMode(Replicated_cut_trajectory_and_cuts_frame_surface);
                        }

                        ImGui::EndMenu();
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Light"))
            {
                int size = GLFWglobals::openGLManager->getPointLightSourceCounts();

                for (int i = 0; i < size; ++i)
                {
                    if (ImGui::BeginMenu(GLFWglobals::openGLManager->getPointLightSourceName(i).c_str()))
                    {
                        if (ImGui::MenuItem("Set position"))
                        {
                            GLFWglobals::openGLManager->setSelectedPointLight(i);
                        }

                        if (ImGui::BeginMenu("Set ambient component"))
                        {
                            glm::vec3 amb = GLFWglobals::openGLManager->getAmbientComponent(i);
                            float ambient[3]{ amb.x, amb.y, amb.z };

                            bool isChanged = ImGui::ColorPicker3("Ambient", ambient);

                            if (isChanged)
                                GLFWglobals::openGLManager->setAmbientComponent(i, ambient);

                            ImGui::EndMenu();
                        }

                        if (ImGui::BeginMenu("Set diffuse component"))
                        {
                            glm::vec3 dif = GLFWglobals::openGLManager->getDiffuseComponent(i);
                            float diffuse[3]{ dif.x, dif.y, dif.z };

                            bool isChanged = ImGui::ColorPicker3("Diffuse", diffuse);

                            if (isChanged)
                                GLFWglobals::openGLManager->setDiffuseComponent(i, diffuse);

                            ImGui::EndMenu();
                        }

                        if (ImGui::BeginMenu("Set specular component"))
                        {
                            glm::vec3 spec = GLFWglobals::openGLManager->getSpecularComponent(i);
                            float specular[3]{ spec.x, spec.y, spec.z };

                            bool isChanged = ImGui::ColorPicker3("Specular", specular);

                            if (isChanged)
                                GLFWglobals::openGLManager->setSpecularComponent(i, specular);

                            ImGui::EndMenu();
                        }

                        if (ImGui::MenuItem("Delete"))
                        {
                            GLFWglobals::openGLManager->deletePointLightSource(i);
                            size--;
                        }

                        ImGui::EndMenu();
                    }
                }

                if (ImGui::MenuItem("Add point light source"))
                {
                    GLFWglobals::openGLManager->addPointLightSource();
                }

                if (ImGui::MenuItem("Unselect"))
                {
                    GLFWglobals::openGLManager->setSelectedPointLight(-1);
                }

                if (ImGui::MenuItem("Switch global ambient light"))
                {
                    GLFWglobals::openGLManager->switchGlobalAmbientLight();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Material"))
            {
                auto& materialsNames = GLFWglobals::openGLManager->getNaturalMaterialsNames();
                int size = materialsNames.size();

                for (int i = 0; i < size; ++i)
                {
                    if (ImGui::MenuItem(materialsNames[i].c_str()))
                    {
                        GLFWglobals::openGLManager->setReplicatedCutMaterial(materialsNames[i]);
                    }
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Texture"))
            {
                auto& texturesNames = GLFWglobals::openGLManager->getTexturesNames();
                int size = texturesNames.size();

                for (int i = 0; i < size; ++i)
                {
                    if (ImGui::MenuItem(texturesNames[i].c_str()))
                    {
                        GLFWglobals::openGLManager->setReplicatedCutTexture(texturesNames[i]);
                    }
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Projection"))
            {
                if (ImGui::MenuItem("Perspective"))
                {
                    GLFWglobals::openGLManager->setProjectionMode(true);
                }
                if (ImGui::MenuItem("Orthographic"))
                {
                    GLFWglobals::openGLManager->setProjectionMode(false);
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void processKeysClick(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            GLFWglobals::openGLManager->frontMovement(GLFWglobals::deltaTime);
        }
        else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            GLFWglobals::openGLManager->backMovement(GLFWglobals::deltaTime);
        }
        else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            GLFWglobals::openGLManager->rightMovement(GLFWglobals::deltaTime);
        }
        else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            GLFWglobals::openGLManager->leftMovement(GLFWglobals::deltaTime);
        }
        else if (key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT) && mods == GLFW_MOD_SHIFT)
        {
            GLFWglobals::openGLManager->moveSelectedPointLight(-1, 0, 0, GLFWglobals::deltaTime);
        }
        else if (key == GLFW_KEY_Y && (action == GLFW_PRESS || action == GLFW_REPEAT) && mods == GLFW_MOD_SHIFT)
        {
            GLFWglobals::openGLManager->moveSelectedPointLight(0, -1, 0, GLFWglobals::deltaTime);
        }
        else if (key == GLFW_KEY_Z && (action == GLFW_PRESS || action == GLFW_REPEAT) && mods == GLFW_MOD_SHIFT)
        {
            GLFWglobals::openGLManager->moveSelectedPointLight(0, 0, -1, GLFWglobals::deltaTime);
        }
        else if (key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            GLFWglobals::openGLManager->moveSelectedPointLight(1, 0, 0, GLFWglobals::deltaTime);
        }
        else if (key == GLFW_KEY_Y && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            GLFWglobals::openGLManager->moveSelectedPointLight(0, 1, 0, GLFWglobals::deltaTime);
        }
        else if (key == GLFW_KEY_Z && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            GLFWglobals::openGLManager->moveSelectedPointLight(0, 0, 1, GLFWglobals::deltaTime);
        }
    }

    void processScroll(GLFWwindow* window, double xoffset, double yoffset)
    {
        GLFWglobals::openGLManager->mouseScroll(static_cast<float>(yoffset));
    }

    void processMouseClick(GLFWwindow* window, int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            GLFWglobals::firstMousePositionChange = true;
        }
    }

    void processWindowResize(GLFWwindow* window, int width, int height)
    {
        GLFWglobals::mainWindowWidth = width;
        GLFWglobals::mainWindowHeight = height;

        glViewport(0, 0, width, height);

        GLFWglobals::openGLManager->windowResize(width, height);
    }

    void calcDeltaTimePerFrame()
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        GLFWglobals::deltaTime = currentFrame - GLFWglobals::lastFrameTime;
        GLFWglobals::lastFrameTime = currentFrame;
    }
}