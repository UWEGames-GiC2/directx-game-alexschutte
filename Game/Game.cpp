//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <time.h>

#include <iostream>

//Scarle Headers
#include "GameData.h"
#include "GameState.h"
#include "DrawData.h"
#include "DrawData2D.h"
#include "ObjectList.h"
#include "Projectile.h"

#include "CMOGO.h"
#include <DirectXCollision.h>
#include "Collision.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept :
    m_window(nullptr),
    m_outputWidth(800),
    m_outputHeight(600),
    m_featureLevel(D3D_FEATURE_LEVEL_11_0)
{
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND _window, int _width, int _height)
{
    m_window = _window;
    m_outputWidth = std::max(_width, 1);
    m_outputHeight = std::max(_height, 1);

    CreateDevice();

    CreateResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

    //seed the random number generator
    srand((UINT)time(NULL));

    //set up keyboard and mouse system
    //documentation here: https://github.com/microsoft/DirectXTK/wiki/Mouse-and-keyboard-input
    m_keyboard = std::make_unique<Keyboard>();
    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(_window);
    m_mouse->SetMode(Mouse::MODE_RELATIVE);
    //Hide the mouse pointer
    ShowCursor(false);

    //create GameData struct and populate its pointers
    m_GD = new GameData;
    m_GD->m_GS = GS_PLAY_MAIN_CAM;

    //set up systems for 2D rendering
    m_DD2D = new DrawData2D();
    m_DD2D->m_Sprites.reset(new SpriteBatch(m_d3dContext.Get()));
    m_DD2D->m_Font.reset(new SpriteFont(m_d3dDevice.Get(), L"..\\Assets\\italic.spritefont"));
    m_states = new CommonStates(m_d3dDevice.Get());

    //set up DirectXTK Effects system
    m_fxFactory = new EffectFactory(m_d3dDevice.Get());
    //Tell the fxFactory to look to the correct build directory to pull stuff in from
    ((EffectFactory*)m_fxFactory)->SetDirectory(L"..\\Assets");
    //init render system for VBGOs
    VBGO::Init(m_d3dDevice.Get());

    //set audio system
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif
    m_audioEngine = std::make_unique<AudioEngine>(eflags);

    //create a set of dummy things to show off the engine

    //create a base light
    m_light = new Light(Vector3(0.0f, 100.0f, 160.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.4f, 0.1f, 0.1f, 1.0f));
    m_GameObjects.push_back(m_light);

    //find how big my window is to correctly calculate my aspect ratio
    float AR = (float)_width / (float)_height;

    //terrain
    Terrain* terrain = new Terrain("cubeDRed", m_d3dDevice.Get(), m_fxFactory, Vector3(-5000.0f, -50.0f, -5000.0f), 0.0f, 0.0f, 0.0f, 250.0f * Vector3(100.0f, 1.0f, 100.0f));
    m_GameObjects.push_back(terrain);
    m_ColliderObjects.push_back(terrain);

    Terrain* terrain2 = new Terrain("cubeDRed", m_d3dDevice.Get(), m_fxFactory, Vector3(-80.0f, -0.75f, 0.0f), 0.0f, 0.0f, 0.0f, Vector3(500.0f, 200.0f, 60.0f));
    m_GameObjects.push_back(terrain2);
    m_ColliderObjects.push_back(terrain2);

    Terrain* terrain3 = new Terrain("cubeDRed", m_d3dDevice.Get(), m_fxFactory, Vector3(-80.0f, -0.75f, -100.0f), 0.0f, 0.0f, 0.0f, Vector3(500.0f, 200.0f, 60.0f));
    m_GameObjects.push_back(terrain3);
    m_ColliderObjects.push_back(terrain3);

    Terrain* terrain4 = new Terrain("cubeDRed", m_d3dDevice.Get(), m_fxFactory, Vector3(10.0f, -0.75f, -140.0f), 0.0f, 0.0f, 0.0f, Vector3(60.0f, 200.0f, 500.0f));
    m_GameObjects.push_back(terrain4);
    m_ColliderObjects.push_back(terrain4);

    Terrain* terrain5 = new Terrain("cubeDRed", m_d3dDevice.Get(), m_fxFactory, Vector3(-90.0f, -0.75f, -70.0f), 0.0f, 0.0f, 0.0f, Vector3(60.0f, 200.0f, 500.0f));
    m_GameObjects.push_back(terrain5);
    m_ColliderObjects.push_back(terrain5);

    Terrain* terrain6 = new Terrain("cubeDRed", m_d3dDevice.Get(), m_fxFactory, Vector3(-90.0f, -0.75f, -210.0f), 0.0f, 0.0f, 0.0f, Vector3(60.0f, 200.0f, 500.0f));
    m_GameObjects.push_back(terrain6);
    m_ColliderObjects.push_back(terrain6);

    Terrain* terrain7 = new Terrain("cubeDRed", m_d3dDevice.Get(), m_fxFactory, Vector3(-190.0f, -0.75f, 0.0f), 0.0f, 0.0f, 0.0f, Vector3(500.0f, 200.0f, 60.0f));
    m_GameObjects.push_back(terrain7);
    m_ColliderObjects.push_back(terrain7);

    Terrain* terrain8 = new Terrain("cubeDRed", m_d3dDevice.Get(), m_fxFactory, Vector3(-190.0f, -0.75f, -100.0f), 0.0f, 0.0f, 0.0f, Vector3(500.0f, 200.0f, 60.0f));
    m_GameObjects.push_back(terrain8);
    m_ColliderObjects.push_back(terrain8);

    Terrain* terrain10 = new Terrain("cubeDRed", m_d3dDevice.Get(), m_fxFactory, Vector3(-200.0f, -0.75f, -70.0f), 0.0f, 0.0f, 0.0f, Vector3(60.0f, 200.0f, 500.0f));
    m_GameObjects.push_back(terrain10);
    m_ColliderObjects.push_back(terrain10);

    Terrain* terrain11 = new Terrain("cubeDRed", m_d3dDevice.Get(), m_fxFactory, Vector3(-200.0f, -0.75f, -210.0f), 0.0f, 0.0f, 0.0f, Vector3(60.0f, 200.0f, 500.0f));
    m_GameObjects.push_back(terrain11);
    m_ColliderObjects.push_back(terrain11);

    Terrain* terrain12 = new Terrain("cubeDRed", m_d3dDevice.Get(), m_fxFactory, Vector3(-300.0f, -0.75f, 0.0f), 0.0f, 0.0f, 0.0f, Vector3(500.0f, 200.0f, 60.0f));
    m_GameObjects.push_back(terrain12);
    m_ColliderObjects.push_back(terrain12);

    Terrain* terrain13 = new Terrain("cubeDRed", m_d3dDevice.Get(), m_fxFactory, Vector3(-300.0f, -0.75f, -100.0f), 0.0f, 0.0f, 0.0f, Vector3(500.0f, 200.0f, 60.0f));
    m_GameObjects.push_back(terrain13);
    m_ColliderObjects.push_back(terrain13);

    Terrain* terrain14 = new Terrain("cubeDRed", m_d3dDevice.Get(), m_fxFactory, Vector3(-310.0f, -0.75f, -70.0f), 0.0f, 0.0f, 0.0f, Vector3(60.0f, 200.0f, 500.0f));
    m_GameObjects.push_back(terrain14);
    m_ColliderObjects.push_back(terrain14);

    Terrain* terrain15 = new Terrain("cubeDRed", m_d3dDevice.Get(), m_fxFactory, Vector3(-310.0f, -0.75f, -210.0f), 0.0f, 0.0f, 0.0f, Vector3(60.0f, 200.0f, 500.0f));
    m_GameObjects.push_back(terrain15);
    m_ColliderObjects.push_back(terrain15);

    //projectiles
    for (size_t i = 0; i < 5; i++)
    {
        pProjectile[i] = new Projectile("sphereWhite", m_d3dDevice.Get(), m_fxFactory);
        pProjectile[i]->setExistence(false);
        pProjectile[i]->SetScale(5.0f);
        m_GameObjects.push_back(pProjectile[i]);
        m_PlayerProjectiles.push_back(pProjectile[i]);
        m_PhysicsObjects.push_back(pProjectile[i]);
    }

    //targets
    pTarget = new Target("target", m_d3dDevice.Get(), m_fxFactory);
    pTarget->SetPos(Vector3(-40.0f, 20.0f, -50.0f));
    pTarget->SetScale(20.0f);
    m_GameObjects.push_back(pTarget);
    m_ColliderObjects.push_back(pTarget);
    m_Targets.push_back(pTarget);

    pTarget1 = new Target("target", m_d3dDevice.Get(), m_fxFactory);
    pTarget1->SetPos(Vector3(-60.0f, 30.0f, -30.0f));
    pTarget1->SetScale(20.0f);
    m_GameObjects.push_back(pTarget1);
    m_ColliderObjects.push_back(pTarget1);
    m_Targets.push_back(pTarget1);

    pTarget2 = new Target("target", m_d3dDevice.Get(), m_fxFactory);
    pTarget2->SetPos(Vector3(-20.0f, 10.0f, -40.0f));
    pTarget2->SetScale(20.0f);
    m_GameObjects.push_back(pTarget2);
    m_ColliderObjects.push_back(pTarget2);
    m_Targets.push_back(pTarget2);

    pTarget3 = new Target("target", m_d3dDevice.Get(), m_fxFactory);
    pTarget3->SetPos(Vector3(-130.0f, 20.0f, -30.0f));
    pTarget3->SetScale(20.0f);
    m_GameObjects.push_back(pTarget3);
    m_ColliderObjects.push_back(pTarget3);
    m_Targets.push_back(pTarget3);

    pTarget4 = new Target("target", m_d3dDevice.Get(), m_fxFactory);
    pTarget4->SetPos(Vector3(-150.0f, 35.0f, -40.0f));
    pTarget4->SetScale(20.0f);
    m_GameObjects.push_back(pTarget4);
    m_ColliderObjects.push_back(pTarget4);
    m_Targets.push_back(pTarget4);

    pTarget5 = new Target("target", m_d3dDevice.Get(), m_fxFactory);
    pTarget5->SetPos(Vector3(-150.0f, 10.0f, -50.0f));
    pTarget5->SetScale(20.0f);
    m_GameObjects.push_back(pTarget5);
    m_ColliderObjects.push_back(pTarget5);
    m_Targets.push_back(pTarget5);

    pTarget6 = new Target("target", m_d3dDevice.Get(), m_fxFactory);
    pTarget6->SetPos(Vector3(-250.0f, 10.0f, -50.0f));
    pTarget6->SetScale(20.0f);
    m_GameObjects.push_back(pTarget6);
    m_ColliderObjects.push_back(pTarget6);
    m_Targets.push_back(pTarget6);

    pTarget7 = new Target("target", m_d3dDevice.Get(), m_fxFactory);
    pTarget7->SetPos(Vector3(-240.0f, 25.0f, -30.0f));
    pTarget7->SetScale(20.0f);
    m_GameObjects.push_back(pTarget7);
    m_ColliderObjects.push_back(pTarget7);
    m_Targets.push_back(pTarget7);

    pTarget8 = new Target("target", m_d3dDevice.Get(), m_fxFactory);
    pTarget8->SetPos(Vector3(-230.0f, 15.0f, -40.0f));
    pTarget8->SetScale(20.0f);
    m_GameObjects.push_back(pTarget8);
    m_ColliderObjects.push_back(pTarget8);
    m_Targets.push_back(pTarget8);

    //doors

    pDoor = new Door("longCube", m_d3dDevice.Get(), m_fxFactory, 3);
    pDoor->SetPos(Vector3(-90.0f, 30.75f, -30.0f));
    pDoor->SetScale(Vector3(400.0f, 70.0f, 300.0f));
    m_GameObjects.push_back(pDoor);
    m_ColliderObjects.push_back(pDoor);
    
    pDoor2 = new Door("longCube", m_d3dDevice.Get(), m_fxFactory, 6);
    pDoor2->SetPos(Vector3(-190.0f, 30.75f, -30.0f));
    pDoor2->SetScale(Vector3(400.0f, 70.0f, 300.0f));
    m_GameObjects.push_back(pDoor2);
    m_ColliderObjects.push_back(pDoor2);

    pDoor3 = new Door("longCube", m_d3dDevice.Get(), m_fxFactory, 9);
    pDoor3->SetPos(Vector3(-290.0f, 30.75f, -30.0f));
    pDoor3->SetScale(Vector3(400.0f, 70.0f, 300.0f));
    m_GameObjects.push_back(pDoor3);
    m_ColliderObjects.push_back(pDoor3);

    //create a base camera
    m_cam = new Camera(0.25f * XM_PI, AR, 1.0f, 10000.0f, Vector3::UnitY, Vector3::Zero);
    m_cam->SetPos(Vector3(0.0f, 200.0f, 200.0f));
    m_GameObjects.push_back(m_cam);

    //add Player
    pPlayer = new Player("longCube", m_d3dDevice.Get(), m_fxFactory);
    pPlayer->SetScale(Vector3(60.0f, 60.0f, 60.0f));
    m_GameObjects.push_back(pPlayer);
    m_PhysicsObjects.push_back(pPlayer);
    pPlayer->projectiles = m_PlayerProjectiles;

    //add a secondary camera
    m_TPScam = new TPSCamera(0.25f * XM_PI, AR, 0.01f, 10000.0f, pPlayer, Vector3::UnitY, Vector3(0.0f, 0.0f, 0.01f));
    m_GameObjects.push_back(m_TPScam);

    //create DrawData struct and populate its pointers
    m_DD = new DrawData;
    m_DD->m_pd3dImmediateContext = nullptr;
    m_DD->m_states = m_states;
    m_DD->m_cam = m_cam;
    m_DD->m_light = m_light;

    //example basic 2D stuff

    TextGO2D* crosshair = new TextGO2D("+");
    crosshair->SetPos(Vector2(380.0f, 250.0f));
    crosshair->SetColour(Color((float*)&Colors::LimeGreen));
    m_GameObjects2D.push_back(crosshair);

    TextGO2D* menu1 = new TextGO2D("Target Practice!");
    menu1->SetPos(Vector2(m_outputWidth / 6, m_outputHeight / 2 - 100));
    menu1->SetColour(Color((float*)&Colors::LimeGreen));
    m_MenuUIObjects.push_back(menu1);

    TextGO2D* menu2 = new TextGO2D("PRESS ENTER KEY TO PLAY");
    menu2->SetPos(Vector2(m_outputWidth / 6, m_outputHeight / 2));
    menu2->SetColour(Color((float*)&Colors::LimeGreen));
    m_MenuUIObjects.push_back(menu2);

    TextGO2D* menu3 = new TextGO2D("PRESS SPACE FOR INSTRUCTIONS");
    menu3->SetPos(Vector2(m_outputWidth / 6, m_outputHeight / 2 + 100));
    menu3->SetColour(Color((float*)&Colors::LimeGreen));
    m_MenuUIObjects.push_back(menu3);
    
    //ImageGO2D* logo = new ImageGO2D("logo_small", m_d3dDevice.Get());
    //logo->SetPos(200.0f * Vector2::One);
    //m_GameObjects2D.push_back(logo);
    //ImageGO2D* bug_test = new ImageGO2D("bug_test", m_d3dDevice.Get());
    //bug_test->SetPos(300.0f * Vector2::One);
    //m_GameObjects2D.push_back(bug_test);

    //TextGO2D* text = new TextGO2D("Test Text");
    //text->SetPos(Vector2(100, 10));
    //text->SetColour(Color((float*)&Colors::Yellow));
    //m_GameObjects2D.push_back(text);

    //Test Sounds
    //Loop* loop = new Loop(m_audioEngine.get(), "NightAmbienceSimple_02");
    //loop->SetVolume(0.1f);
    //loop->Play();
    //m_Sounds.push_back(loop);

    /*TestSound* TS = new TestSound(m_audioEngine.get(), "Explo1");
    m_Sounds.push_back(TS);*/
}

// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
        {
            Update(m_timer);
        });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& _timer)
{
    float elapsedTime = float(_timer.GetElapsedSeconds());
    m_GD->m_dt = elapsedTime;

    //this will update the audio engine but give us chance to do somehting else if that isn't working
    if (!m_audioEngine->Update())
    {
        if (m_audioEngine->IsCriticalError())
        {
            // We lost the audio device!
        }
    }
    else
    {
        //update sounds playing
        for (list<Sound*>::iterator it = m_Sounds.begin(); it != m_Sounds.end(); it++)
        {
            (*it)->Tick(m_GD);
        }
    }

    ReadInput();
    //upon space bar switch camera state
    //see docs here for what's going on: https://github.com/Microsoft/DirectXTK/wiki/Keyboard

    switch (current)
    {
        case GAMEPLAY:
            //update all objects
            for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
            {
                (*it)->Tick(m_GD);
            }
            for (list<GameObject2D*>::iterator it = m_GameObjects2D.begin(); it != m_GameObjects2D.end(); it++)
            {
                (*it)->Tick(m_GD);
            }

            CheckCollision();

        break;

        case MENU:
            //update menus
            for (list<GameObject2D*>::iterator it = m_MenuUIObjects.begin(); it != m_MenuUIObjects.end(); it++)
            {
                (*it)->Tick(m_GD);
            }
            break;
        case INSTRUCTIONS:
            //TODO text update
            break;
        case GAME_OVER:
            break;
        case GAME_WON:
            break;
            
    }
}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    //set immediate context of the graphics device
    m_DD->m_pd3dImmediateContext = m_d3dContext.Get();

    //set which camera to be used
    m_DD->m_cam = m_TPScam;


    //update the constant buffer for the rendering of VBGOs
    VBGO::UpdateConstantBuffer(m_DD);

    m_DD2D->m_Sprites->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied());

    switch (current)
    {
    case GAMEPLAY:
        //Draw 3D Game Obejects
        for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
        {
            if ((*it)->DoesExist())
            {
                (*it)->Draw(m_DD);
            }
        }

        // Draw sprite batch stuff 
        for (list<GameObject2D*>::iterator it = m_GameObjects2D.begin(); it != m_GameObjects2D.end(); it++)
        {
            (*it)->Draw(m_DD2D);
        }
        break;

    case MENU:
    case INSTRUCTIONS:
    case GAME_OVER:
    case GAME_WON:

        for (list<GameObject2D*>::iterator it = m_MenuUIObjects.begin(); it != m_MenuUIObjects.end(); it++)
        {
            (*it)->Draw(m_DD2D);
        }
        break;
    }
    m_DD2D->m_Sprites->End();

    //drawing text screws up the Depth Stencil State, this puts it back again!
    m_d3dContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);

    Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::CornflowerBlue);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int _width, int _height)
{
    m_outputWidth = std::max(_width, 1);
    m_outputHeight = std::max(_height, 1);

    CreateResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& _width, int& _height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    _width = 800;
    _height = 600;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
    UINT creationFlags = 0;

#ifdef _DEBUG
    //creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
    //something missing on the machines in 2Q28
    //this should work!
#endif

    static const D3D_FEATURE_LEVEL featureLevels[] =
    {
        // TODO: Modify for supported Direct3D feature levels
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    // Create the DX11 API device object, and get a corresponding context.
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    DX::ThrowIfFailed(D3D11CreateDevice(
        nullptr,                            // specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        static_cast<UINT>(std::size(featureLevels)),
        D3D11_SDK_VERSION,
        device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
        &m_featureLevel,                    // returns feature level of device created
        context.ReleaseAndGetAddressOf()    // returns the device immediate context
    ));

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(device.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide[] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    DX::ThrowIfFailed(device.As(&m_d3dDevice));
    DX::ThrowIfFailed(context.As(&m_d3dContext));

    // TODO: Initialize device dependent objects here (independent of window size).
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews[] = { nullptr };
    m_d3dContext->OMSetRenderTargets(static_cast<UINT>(std::size(nullViews)), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    const UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    const UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
    const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    constexpr UINT backBufferCount = 2;

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory2> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = backBufferCount;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a SwapChain from a Win32 window.
        DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            m_d3dDevice.Get(),
            m_window,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            m_swapChain.ReleaseAndGetAddressOf()
        ));

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain.Reset();
    m_d3dContext.Reset();
    m_d3dDevice.Reset();

    CreateDevice();

    CreateResources();
}

void Game::ReadInput()
{
    m_GD->m_KBS = m_keyboard->GetState();
    m_GD->m_KBS_tracker.Update(m_GD->m_KBS);
    //quit game on hitting escape
    if (m_GD->m_KBS.Escape)
    {
        ExitGame();
    }

    m_GD->m_MS = m_mouse->GetState();

    //lock the cursor to the centre of the window
    RECT window;
    GetWindowRect(m_window, &window);
    SetCursorPos((window.left + window.right) >> 1, (window.bottom + window.top) >> 1);

    switch (current)
    {
    case MENU:
        // TODO menu functionality here
        if (m_GD->m_KBS.Enter)
        {
            current = GAMEPLAY;
        }
        break;
    case INSTRUCTIONS:
    case GAME_OVER:
    case GAME_WON:
        //TODO menu functionality here
        break;
    }
}

void Game::CheckProjectileCollision()
{
    for (int i = 0; i < m_PlayerProjectiles.size(); i++) for (int j = 0; j < m_Targets.size(); j++)
    {
        if (m_PlayerProjectiles[i]->Intersects(*m_Targets[j]))
        {
            if (m_PlayerProjectiles[i]->DoesExist() && m_Targets[j]->DoesExist())
            {
                m_Targets[j]->targetHit();
                std::cout << "Hit" << endl;
                targets_hit++;
                std::cout << targets_hit << endl;
                if (targets_hit == pDoor->target_req)
                {
                    pDoor->Open();
                    std::cout << "Door 1 open" << endl;
                }
                if (targets_hit == pDoor2->target_req)
                {
                    pDoor2->Open();
                    std::cout << "Door 2 open" << endl;

                }
                if (targets_hit == pDoor3->target_req)
                {
                    pDoor3->Open();
                    std::cout << "Door 3 open" << endl;

                }
            }
            m_PlayerProjectiles[i]->setExistence(false);
        }
    }
}

void Game::CheckCollision()
{
    for (int i = 0; i < m_PhysicsObjects.size(); i++) for (int j = 0; j < m_ColliderObjects.size(); j++)
    {
        CheckProjectileCollision();
        if (m_PhysicsObjects[i]->Intersects(*m_ColliderObjects[j])) //std::cout << "Collision Detected!" << std::endl;
        {
            if (m_PhysicsObjects[i]->DoesExist() && m_ColliderObjects[j]->DoesExist())
            {
                XMFLOAT3 eject_vect = Collision::ejectionCMOGO(*m_PhysicsObjects[i], *m_ColliderObjects[j]);
                auto pos = m_PhysicsObjects[i]->GetPos();
                m_PhysicsObjects[i]->SetPos(pos - eject_vect);
            }
        }
    }
}