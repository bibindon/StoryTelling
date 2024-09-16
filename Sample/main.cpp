#pragma comment( lib, "d3d9.lib" )
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx9d.lib" )
#else
#pragma comment( lib, "d3dx9.lib" )
#endif

#pragma comment (lib, "winmm.lib")

#pragma comment( lib, "StoryTelling.lib" )

#include "..\StoryTelling\StoryTelling.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <vector>

#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }

class Sprite : public ISprite
{
public:

    Sprite(LPDIRECT3DDEVICE9 dev)
        : m_pD3DDevice(dev)
    {
    }

    void DrawImage(const int x, const int y, const int transparency) override
    {
        D3DXVECTOR3 pos {(float)x, (float)y, 0.f};
        m_D3DSprite->Begin(D3DXSPRITE_ALPHABLEND);
        RECT rect = {
            0,
            0,
            static_cast<LONG>(m_width),
            static_cast<LONG>(m_height) };
        D3DXVECTOR3 center { 0, 0, 0 };
        m_D3DSprite->Draw(
            m_pD3DTexture,
            &rect,
            &center,
            &pos,
            D3DCOLOR_ARGB(transparency, 255, 255, 255));
        m_D3DSprite->End();

    }

    void Load(const std::string& filepath) override
    {
        LPD3DXSPRITE tempSprite { nullptr };
        if (FAILED(D3DXCreateSprite(m_pD3DDevice, &m_D3DSprite)))
        {
            throw std::exception("Failed to create a sprite.");
        }

        if (FAILED(D3DXCreateTextureFromFile(
            m_pD3DDevice,
            filepath.c_str(),
            &m_pD3DTexture)))
        {
            throw std::exception("Failed to create a texture.");
        }

        D3DSURFACE_DESC desc { };
        if (FAILED(m_pD3DTexture->GetLevelDesc(0, &desc)))
        {
            throw std::exception("Failed to create a texture.");
        }
        m_width = desc.Width;
        m_height = desc.Height;
    }

    ~Sprite()
    {
        m_D3DSprite->Release();
        m_pD3DTexture->Release();
    }

private:

    LPDIRECT3DDEVICE9 m_pD3DDevice = NULL;
    LPD3DXSPRITE m_D3DSprite = NULL;
    LPDIRECT3DTEXTURE9 m_pD3DTexture = NULL;
    UINT m_width { 0 };
    UINT m_height { 0 };
};

class Font : public IFont
{
public:

    Font(LPDIRECT3DDEVICE9 pD3DDevice)
        : m_pD3DDevice(pD3DDevice)
    {
    }

    void Init()
    {
        HRESULT hr = D3DXCreateFont(
            m_pD3DDevice,
            24,
            0,
            FW_NORMAL,
            1,
            false,
            SHIFTJIS_CHARSET,
            OUT_TT_ONLY_PRECIS,
            ANTIALIASED_QUALITY,
            FF_DONTCARE,
            "‚l‚r –¾’©",
            &m_pFont);
    }

    virtual void DrawText_(const std::string& msg, const int x, const int y)
    {
        RECT rect = { x, y, 0, 0 };
        m_pFont->DrawText(NULL, msg.c_str(), -1, &rect, DT_LEFT | DT_NOCLIP,
            D3DCOLOR_ARGB(255, 255, 255, 255));
    }

    ~Font()
    {
        m_pFont->Release();
    }

private:

    LPDIRECT3DDEVICE9 m_pD3DDevice = NULL;
    LPD3DXFONT m_pFont = NULL;
};


class SoundEffect : public ISoundEffect
{
    virtual void PlayMove() override
    {
        PlaySound("cursor_move.wav", NULL, SND_FILENAME | SND_ASYNC);
    }
    virtual void Init() override
    {

    }
};

LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
LPD3DXFONT g_pFont = NULL;
LPD3DXMESH pMesh = NULL;
D3DMATERIAL9* pMaterials = NULL;
LPDIRECT3DTEXTURE9* pTextures = NULL;
DWORD dwNumMaterials = 0;
LPD3DXEFFECT pEffect = NULL;
D3DXMATERIAL* d3dxMaterials = NULL;
float f = 0.0f;
bool bFinish = false;

StoryTelling* story = nullptr;

void TextDraw(LPD3DXFONT pFont, char* text, int X, int Y)
{
    RECT rect = { X,Y,0,0 };
    pFont->DrawText(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));
}

HRESULT InitD3D(HWND hWnd)
{
    if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
    {
        return E_FAIL;
    }

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.BackBufferCount = 1;
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    d3dpp.MultiSampleQuality = 0;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.Flags = 0;
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
    {
        if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
        {
            return(E_FAIL);
        }
    }

    HRESULT hr = D3DXCreateFont(
        g_pd3dDevice,
        20,
        0,
        FW_HEAVY,
        1,
        false,
        SHIFTJIS_CHARSET,
        OUT_TT_ONLY_PRECIS,
        ANTIALIASED_QUALITY,
        FF_DONTCARE,
        "‚l‚r ƒSƒVƒbƒN",
        &g_pFont);
    if FAILED(hr)
    {
        return(E_FAIL);
    }

    LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

    if (FAILED(D3DXLoadMeshFromX("cube.x", D3DXMESH_SYSTEMMEM,
        g_pd3dDevice, NULL, &pD3DXMtrlBuffer, NULL,
        &dwNumMaterials, &pMesh)))
    {
        MessageBox(NULL, "Xƒtƒ@ƒCƒ‹‚Ì“Ç‚Ýž‚Ý‚ÉŽ¸”s‚µ‚Ü‚µ‚½", NULL, MB_OK);
        return E_FAIL;
    }
    d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
    pMaterials = new D3DMATERIAL9[dwNumMaterials];
    pTextures = new LPDIRECT3DTEXTURE9[dwNumMaterials];

    for (DWORD i = 0; i < dwNumMaterials; i++)
    {
        pMaterials[i] = d3dxMaterials[i].MatD3D;
        pMaterials[i].Ambient = pMaterials[i].Diffuse;
        pTextures[i] = NULL;
        if (d3dxMaterials[i].pTextureFilename != NULL &&
            lstrlen(d3dxMaterials[i].pTextureFilename) > 0)
        {
            if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice,
                d3dxMaterials[i].pTextureFilename,
                &pTextures[i])))
            {
                MessageBox(NULL, "ƒeƒNƒXƒ`ƒƒ‚Ì“Ç‚Ýž‚Ý‚ÉŽ¸”s‚µ‚Ü‚µ‚½", NULL, MB_OK);
            }
        }
    }
    pD3DXMtrlBuffer->Release();

    D3DXCreateEffectFromFile(
        g_pd3dDevice,
        "simple.fx",
        NULL,
        NULL,
        D3DXSHADER_DEBUG,
        NULL,
        &pEffect,
        NULL
    );

    return S_OK;
}

void InitStory()
{
    // new‚Íƒ‰ƒCƒuƒ‰ƒŠ‚ÌŽg—pŽÒ‚ª‚·‚é‚ªAdelete‚Íƒ‰ƒCƒuƒ‰ƒŠ“à‚Ås‚í‚ê‚éB
    // ‚¿‚å‚Á‚Æ—Ç‚­‚È‚¢‚¯‚ÇEEE‚Ü‚Ÿ‚æ‚µI
    ISoundEffect* pSE = new SoundEffect();

    Sprite* sprTextBack = new Sprite(g_pd3dDevice);
    sprTextBack->Load("textBack.png");

    IFont* pFont = new Font(g_pd3dDevice);
    pFont->Init();

    std::vector<Page> pageList;
    {
        Page page;
        Sprite* sprite = new Sprite(g_pd3dDevice);
        sprite->Load("opening01.png");
        page.SetSprite(sprite);
        std::vector<std::vector<std::string> > vss;
        std::vector<std::string> vs;
        vs.push_back("ƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚P");
        vs.push_back("ƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚Q");
        vs.push_back("ƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚R");
        vss.push_back(vs);
        vs.clear();
        vs.push_back("ƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚SƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚SƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚S");
        vs.push_back("ƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚TƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚TƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚T");
        vs.push_back("ƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚UƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚UƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚U");
        vss.push_back(vs);
        vs.clear();
        vs.push_back("ƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚VƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚VƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚VƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚VƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚V");
        vs.push_back("ƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚WƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚WƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚WƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚WƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚W");
        vs.push_back("ƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚XƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚XƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚XƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚XƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚X");
        vss.push_back(vs);
        page.SetTextList(vss);
        pageList.push_back(page);
    }
    {
        Page page;
        Sprite* sprite = new Sprite(g_pd3dDevice);
        sprite->Load("opening02.png");
        page.SetSprite(sprite);
        std::vector<std::vector<std::string> > vss;
        std::vector<std::string> vs;
        vs.push_back("ƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚`");
        vs.push_back("ƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚a");
        vs.push_back("ƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚b");
        vss.push_back(vs);
        vs.clear();
        vs.push_back("ƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚cƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚cƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚c");
        vs.push_back("ƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚dƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚d");
        vs.push_back("ƒTƒ“ƒvƒ‹ƒeƒLƒXƒg‚e");
        vss.push_back(vs);
        page.SetTextList(vss);
        pageList.push_back(page);
    }
    {
        Page page;
        Sprite* sprite = new Sprite(g_pd3dDevice);
        sprite->Load("opening03.png");
        page.SetSprite(sprite);
        std::vector<std::vector<std::string> > vss;
        std::vector<std::string> vs;
        vs.push_back("‚P‚P‚P‚P‚P‚P‚P‚P‚P‚P‚P");
        vs.push_back("‚Q‚Q‚Q‚Q‚Q‚Q‚Q‚Q‚Q‚Q‚Q‚Q‚Q");
        vs.push_back("‚R‚R‚R‚R‚R‚R‚R‚R‚R‚R‚R‚R‚R‚R‚R‚R‚R");
        vss.push_back(vs);
        vs.clear();
        vs.push_back("‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S‚S");
        vs.push_back("");
        vss.push_back(vs);
        vs.clear();
        vs.push_back("‚T‚T‚T‚T‚T‚T‚T‚T‚T‚T‚T‚T‚T‚T‚T‚T‚T");
        vss.push_back(vs);
        page.SetTextList(vss);
        pageList.push_back(page);
    }

    story->Init(pFont, pSE, sprTextBack, pageList);
}

VOID Cleanup()
{
    SAFE_RELEASE(pMesh);
    SAFE_RELEASE(g_pFont);
    SAFE_RELEASE(g_pd3dDevice);
    SAFE_RELEASE(g_pD3D);
}

VOID Render()
{
    if (NULL == g_pd3dDevice)
    {
        return;
    }
    f += 0.010f;

    D3DXMATRIX mat;
    D3DXMATRIX View, Proj;
    D3DXMatrixPerspectiveFovLH(&Proj, D3DXToRadian(45), 1600.0f / 900.0f, 1.0f, 10000.0f);
    D3DXVECTOR3 vec1(3 * sinf(f), 3, -3 * cosf(f));
    D3DXVECTOR3 vec2(0, 0, 0);
    D3DXVECTOR3 vec3(0, 1, 0);
    D3DXMatrixLookAtLH(&View, &vec1, &vec2, &vec3);
    D3DXMatrixIdentity(&mat);
    mat = mat * View * Proj;
    pEffect->SetMatrix("matWorldViewProj", &mat);

    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0);

    if (SUCCEEDED(g_pd3dDevice->BeginScene()))
    {
        char msg[128];
        strcpy_s(msg, 128, "‚lƒL[‚ÅŽ†ŽÅ‹ŠJŽn");
        TextDraw(g_pFont, msg, 0, 0);

        pEffect->SetTechnique("BasicTec");
        UINT numPass;
        pEffect->Begin(&numPass, 0);
        pEffect->BeginPass(0);
        for (DWORD i = 0; i < dwNumMaterials; i++)
        {
            pEffect->SetTexture("texture1", pTextures[i]);
            pMesh->DrawSubset(i);
        }
        if (story != nullptr)
        {
            story->Render();
        }
        pEffect->EndPass();
        pEffect->End();
        g_pd3dDevice->EndScene();
    }

    g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        Cleanup();
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        Render();
        return 0;
    case WM_SIZE:
        InvalidateRect(hWnd, NULL, true);
        return 0;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case 'M':
        {
            if (story != nullptr)
            {
                story->Finalize();
                delete story;
            }
            story = new StoryTelling();
            InitStory();
            break;
        }
        case VK_RETURN:
        {
            if (story != nullptr)
            {
                bFinish = story->Next();
                if (bFinish)
                {
                    story->Finalize();
                    delete story;
                    story = nullptr;
                }
            }
            break;
        }
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_LBUTTONDOWN:
    {
        bFinish = story->Next();
        if (bFinish)
        {
            story->Finalize();
            delete story;
            story = nullptr;
        }
        break;
    }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

INT WINAPI wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ INT)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "Window1", NULL };
    RegisterClassEx(&wc);

    RECT rect;
    SetRect(&rect, 0, 0, 1600, 900);
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    rect.right = rect.right - rect.left;
    rect.bottom = rect.bottom - rect.top;
    rect.top = 0;
    rect.left = 0;

    HWND hWnd = CreateWindow("Window1", "Hello DirectX9 World !!",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rect.right, rect.bottom,
        NULL, NULL, wc.hInstance, NULL);

    if (SUCCEEDED(InitD3D(hWnd)))
    {
        ShowWindow(hWnd, SW_SHOWDEFAULT);
        UpdateWindow(hWnd);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    UnregisterClass("Window1", wc.hInstance);
    return 0;
}
