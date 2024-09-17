// CSVファイル読み込みは保留。このクラスを使う機会が3回くらいしかないなら、作っても意味ない。
#pragma once
#include <string>
#include <vector>

class ISprite
{
public:
    virtual void DrawImage(const int x, const int y, const int transparency = 255) = 0;
    virtual void Load(const std::string& filepath) = 0;
    virtual ~ISprite() {};
};

class IFont
{
public:
    virtual void DrawText_(const std::string& msg, const int x, const int y) = 0;
    virtual void Init() = 0;
    virtual ~IFont() {};
};

class ISoundEffect
{
public:
    virtual void PlayMove() = 0;
    virtual void Init() = 0;
    virtual ~ISoundEffect() {};
};

class Page
{
public:
    ISprite* GetSprite() const;
    void SetSprite(ISprite* sprite);

    std::vector<std::vector<std::string>> GetTextList() const;
    void SetTextList(const std::vector<std::vector<std::string>>& textList);

    int GetTextIndex() const;
    void SetTextIndex(const int index);

private:

    ISprite* m_sprite = nullptr;
    std::vector<std::vector<std::string>> m_textList;
    int m_textIndex = 0;
};

class StoryTelling
{
public:

    void Init(
        IFont* font,
        ISoundEffect* SE,
        ISprite* sprTextBack,
        ISprite* sprFade,
        const std::vector<Page>& pageList);

    void Next();
    bool Update();
    void Render();

    void Finalize();

private:

    ISprite* m_sprTextBack;
    IFont* m_font;
    ISoundEffect* m_SE;
    std::vector<Page> m_pageList;
    int m_pageIndex = 0;

    ISprite* m_sprFade;
    const int FADE_FRAME_MAX = 30;
    bool m_isFadeIn = false;
    int m_FadeInCount = 0;
    bool m_isFadeOut = false;
    int m_FadeOutCount = 0;

    const int WAIT_NEXT_FRAME = 60;
    int m_waitNextCount = 0;
};

