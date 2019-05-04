//
//  Window.cpp
//  gtf
//
//  Created by David Gallardo on 23/05/16.
//  Copyright (c) 2016 GTF Development Group. All rights reserved.
//

//stl
#include <functional>
#include <iostream>

//thirdparty
#include <imgui.h>
#include <imgui_internal.h>
#include <glad.h>
#include <glfw/glfw3.h>

//gtf
#include "gtf/Window.h"
#include "gtf/App.h"
#include "gtf/OpenGLRHI.h"

//gtf private
#include "ImGuiSetup.h"

namespace gtf 
{
struct GTFNativeWindow
{
    GLFWwindow* glfw_window;
    Window* gtf_window;
	ImGuiLocalGLProperties imgui_GLProperties;

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        /*if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }*/
    }
};


Window::Window(const char* title, unsigned int width, unsigned int height)
{
    using namespace std::placeholders;
    
    m_title = title;
    
    m_windowWidth = width;
    m_windowHeight = height;
    
    m_nativeWindow = new GTFNativeWindow();
    m_nativeWindow->gtf_window = this;
    
    
    
    GLFWwindow* parentNative = nullptr;
    
    if(GAPP->getMainWindow() != nullptr)
    {
        parentNative = GAPP->getMainWindow()->m_nativeWindow->glfw_window;
    }
    
    glfwWindowHint(GLFW_VISIBLE, false);
    
    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL,  parentNative);
    
    //glfwSetKeyCallback(window, *key_callback.target<GLFWkeyfun>());
    
    glfwSetWindowUserPointer(window, this);
    
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods){
        Window* mainWinRef = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        mainWinRef->keyEvent(key, scancode, action, mods);
    });
    
    glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int c){
        Window* mainWinRef = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        mainWinRef->charInputEvent(c);
    });
    
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos){
        Window* mainWinRef = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        mainWinRef->mouseMove(xpos, ypos);
    });
    
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods){
        Window* mainWinRef = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        mainWinRef->mouseButton(button, action == GLFW_PRESS);
    });
    
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset){
        Window* mainWinRef = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        mainWinRef->mouseScroll(xoffset, yoffset);
    });
    
    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height){
        Window* mainWinRef = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        mainWinRef->resize(width, height);
    });
    
    glfwSetDropCallback(window, [](GLFWwindow* window, int count, const char** paths){
        Window* mainWinRef = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        mainWinRef->fileDrop(count, paths);
    });
    
    glfwMakeContextCurrent(window);
    
    //gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    
    if(!GRHI)
    {
        GRHI = new OpenGLRHI();
        
        if(GRHI->init())
        {
            std::cout << "ERROR: MPMainWindow::run : Can't initalize RHI!";
        }
    }
    
	ImGuiGL3_CreateLocalDeviceObjects(m_nativeWindow->imgui_GLProperties);

    glfwSwapInterval(1);
    
    
    m_nativeWindow->glfw_window = window;
	ImFontAtlas* atlas = new ImFontAtlas();
	m_imguiContext = ImGui::CreateContext(atlas);
    ImGui::SetCurrentContext(m_imguiContext);
    GTFIMGUI_InitImGui();
    ImGuiGL3_CreateFontsTexture();
}

Window::~Window()
{
    ImGui::SetCurrentContext(m_imguiContext);
    
    if(GAPP->getMainWindow() != this)
        m_imguiContext->IO.Fonts = nullptr;
    
    ImGui::DestroyContext(m_imguiContext);
    //delete m_imguiContext;
    glfwDestroyWindow(m_nativeWindow->glfw_window);
    delete m_nativeWindow;
}

bool Window::wantToClose()
{
    return glfwWindowShouldClose(m_nativeWindow->glfw_window) ? true : false;
}

void Window::stopClosing()
{
    glfwSetWindowShouldClose(m_nativeWindow->glfw_window, false);
}

void Window::mouseMove(double x, double y)
{
    m_mouseCoords[0] = (float)x;
    m_mouseCoords[1] = (float)y;
}

void Window::mouseButton(unsigned int button, bool pressed)
{
    m_mousePressed[button] = pressed;
}

void Window::mouseScroll(double xoffset, double yoffset)
{
    ImGuiIO& io = m_imguiContext->IO;
    io.MouseWheel = (float)yoffset;
}

void Window::keyEvent(int key, int scancode, int action, int mods)
{
    ImGuiIO& io = m_imguiContext->IO;
    if (action == GLFW_PRESS)
        io.KeysDown[key] = true;
    if (action == GLFW_RELEASE)
        io.KeysDown[key] = false;
    
    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
}

void Window::charInputEvent(unsigned int charCode)
{
    ImGuiIO& io = m_imguiContext->IO;
    if (charCode > 0 && charCode < 0x10000)
        io.AddInputCharacter((unsigned short)charCode);
}

void Window::resize(int newWidth, int newHeight)
{
    m_windowWidth = newWidth;
    m_windowHeight = newHeight;
}

void Window::fileDrop(int count, const char** paths)
{
}

void Window::preFrame(double deltaTime)
{
    glfwMakeContextCurrent(m_nativeWindow->glfw_window);
    ImGui::SetCurrentContext(m_imguiContext);
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)m_windowWidth, (float)m_windowHeight);
    io.DeltaTime = (float)deltaTime;
    io.MousePos = ImVec2((float)m_mouseCoords[0], (float)m_mouseCoords[1]);
    io.MouseDown[0] = m_mousePressed[0];
    io.MouseDown[1] = m_mousePressed[1];
    io.MouseDown[2] = m_mousePressed[2];
    ImGui::NewFrame();
}

void Window::frame(double deltaTime)
{
   
}

void Window::postFrame(double deltaTime)
{

    ImGui::Render();
	ImGuiGL3_RenderDrawLists(ImGui::GetDrawData(), m_nativeWindow->imgui_GLProperties);

    glfwSwapBuffers(m_nativeWindow->glfw_window);
    
}

void Window::setVisible(bool visible)
{
    if(visible)
        glfwShowWindow(m_nativeWindow->glfw_window);
    else
        glfwHideWindow(m_nativeWindow->glfw_window);
}

void Window::invalidateLocalRenderContextObjects()
{
	ImGuiGL3_InvalidateLocalDeviceObjects(m_nativeWindow->imgui_GLProperties);
}
} //namespace gtf