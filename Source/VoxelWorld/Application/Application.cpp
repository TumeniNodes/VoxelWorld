/*================================================================
Filename: Application.cpp
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <Windows.h>

#include <Utility/Clock.h>
#include <Utility/ConfigFile.h>
#include <Utility/FPSCounter.h>
#include <Utility/HelperFunctions.h>

#include <D3DObject/Sampler.h>
#include <Input/InputManager.h>
#include <Chunk/BasicRenderer.h>
#include <Resource/ResourceName.h>
#include <Screen/Crosshair.h>
#include <Screen/GUISystem.h>
#include <Texture/Texture2D.h>
#include <Window/Window.h>
#include <World/World.h>
#include "Application.h"

#ifdef GUI_SYSTEM_NK
#include <Screen/nuklear/nuklear.h>
#endif

#define PRINT_FPS 1

namespace
{
    bool LoadAppConf(Application::AppConf &conf)
    {
        ConfigFile file(APPLICATION_CONFIGURE_FILE);
        if(!file)
            return false;

        try
        {
            conf.winWidth  = std::stoi(file("Initialize", "WindowWidth"));
            conf.winHeight = std::stoi(file("Initialize", "WindowHeight"));
            conf.MSAA      = std::stoi(file("Initialize", "MSAA"));

            conf.unloadDistance  = std::stoi(file("World", "UnloadDistance"));
            conf.preloadDistance = std::stoi(file("World", "PreloadDistance"));
            conf.renderDistance  = std::stoi(file("World", "RenderDistance"));
            conf.loaderCount     = std::stoi(file("World", "LoaderCount"));

            conf.maxFogStart = std::stof(file("Fog", "Start"));
            conf.maxFogRange = std::stof(file("Fog", "Range"));
        }
        catch(const std::invalid_argument&)
        {
            return false;
        }

        return true;
    }
}

Application::Application(void)
    : win_(Window::GetInstance()),
      input_(InputManager::GetInstance()),
      gui_(GUISystem::GetInstance())
{
    dev_ = nullptr;
    DC_ = nullptr;

    fogStart_ = 0.0f;
    fogRange_ = 1.0f;
}

bool Application::Initialize(std::string &errMsg)
{
    errMsg = "";

    if(!LoadAppConf(appConf_))
    {
        errMsg = "Failed to load configure file";
        return false;
    }

    const std::vector<GUISystem::FontSpecifier> fonts =
    {
        { "Bin/Fonts/DroidSans.ttf", 18.0f }
    };

    if(!win_.InitWindow(appConf_.winWidth, appConf_.winHeight,
        L"VoxelWorld", errMsg) ||
       !win_.InitD3D(appConf_.MSAA, 0, errMsg) ||
       !win_.InitGUI(fonts, errMsg))
    {
        return false;
    }
    dev_ = win_.GetD3DDevice();
    DC_ = win_.GetD3DDeviceContext();

    if(!ckRendererMgr_.Initialize(errMsg))
        return false;

    if(!immScr2D_.Initialize(errMsg))
        return false;
    if(!crosshair_.Initialize())
    {
        errMsg = "Failed to initialize crosshair";
        return false;
    }

    world_ = std::make_unique<World>(appConf_.preloadDistance,
                                     appConf_.renderDistance,
                                     appConf_.unloadDistance);
    if(!world_->Initialize(appConf_.loaderCount, errMsg))
        return false;

    return true;
}

void Application::Run(void)
{
    ChunkSectionRenderQueue renderQueue;

    input_.LockCursor(true, win_.ClientCentreX(), win_.ClientCentreY());
    input_.ShowCursor(false);

    Clock clock;
    clock.Restart();

#if PRINT_FPS
    FPSCounter fps;
    float lastFPS = 0.0f;
    fps.Restart();
#endif

    float daynightT = 0.0f;
    
    while(!input_.IsKeyDown(VK_ESCAPE))
    {
        gui_.NewFrame();

#if PRINT_FPS
        {
            fps.Tick();
            if(fps.GetFPS() != lastFPS)
                lastFPS = fps.GetFPS();
            ImGui::SetNextWindowSize(ImVec2(200, 200));
            ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoResize |
                                           ImGuiWindowFlags_NoMove |
                                           ImGuiWindowFlags_NoCollapse);
            {
                gui_.PushFont(0);
                ImGui::Text(("WindowSize: " + std::to_string(win_.GetClientWidth()) +
                             ", " + std::to_string(win_.GetClientHeight())).c_str());
                gui_.PopFont();
                ImGui::Text(("FPS: " + std::to_string(lastFPS)).c_str());
            }
            ImGui::End();
        }
#endif

        //����ʱ��������ͱ���ɫ
        daynightT += input_.IsKeyDown('T') ? 0.01f : 0.0001f;
        float absdnt = 0.5f * (std::max)((std::min)(2.0f * std::cos(daynightT), 1.0f), -1.0f) + 0.5f;
        Vector3 sunlight = { absdnt, absdnt, absdnt };
        win_.SetBackgroundColor(0.0f, absdnt, absdnt, 0.0f);

        win_.ClearRenderTarget();
        win_.ClearDepthStencil();

        //���ǿ������
        ckRendererMgr_.SetSunlight(DC_, sunlight);

        //������
        fogStart_ = (std::min)(fogStart_ + 0.12f, appConf_.maxFogStart);
        fogRange_ = (std::min)(fogRange_ + 0.12f, appConf_.maxFogRange);
        ckRendererMgr_.SetFog(DC_, fogStart_, fogRange_, { 0.0f, absdnt, absdnt }, world_->GetActor().GetCameraPosition());

        //�������
        world_->Update(16.6667f);

        //�ύ������Ⱦ����
        world_->Render(&renderQueue);

        //��Ⱦ����
        ckRendererMgr_.SetTrans(DC_, world_->GetActor().GetCamera().GetViewProjMatrix().Transpose());
        ckRendererMgr_.Render(DC_, renderQueue);

        //��Ⱦ��ɫ
        world_->GetActor().Render();

        //����׼��
        crosshair_.Draw(&immScr2D_);

        //����GUI
        gui_.Render();

        //win_.SetVsync(false);
        win_.Present();
        win_.DoEvents();
        clock.Tick();
    }
}