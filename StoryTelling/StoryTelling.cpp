#include "StoryTelling.h"
#include <sstream>
#include "HeaderOnlyCsv.hpp"

using namespace NSStoryTelling;

static std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim))
    {
        result.push_back(item);
    }

    return result;
}

void StoryTelling::Init(
    IFont* font,
    ISoundEffect* SE,
    ISprite* sprTextBack,
    ISprite* sprFade,
    const std::vector<Page>& pageList)
{
    m_font = font;
    m_SE = SE;
    m_sprTextBack = sprTextBack;
    m_sprFade = sprFade;
    m_pageList = pageList;
    m_isFadeIn = true;
}

void NSStoryTelling::StoryTelling::Init(IFont* font,
                                        ISoundEffect* SE,
                                        ISprite* sprTextBack,
                                        ISprite* sprFade,
                                        const std::string& csvFile,
                                        ISprite* sprImage)
{
    m_font = font;
    m_SE = SE;
    m_sprTextBack = sprTextBack;
    m_sprFade = sprFade;
    m_sprImage = sprImage;

    std::vector<std::vector<std::string>> vvs = csv::Read(csvFile);
    std::vector<Page> pageList;
    Page page;
    int pageNum = 0;
    std::vector<std::vector<std::string>> textList;
    for (size_t i = 1; i < vvs.size(); ++i)
    {
        std::vector<std::string> line = vvs.at(i);
        int pageNumTemp = std::stoi(line.at(0));

        // 新しいページ
        if (pageNum != pageNumTemp)
        {
            // 古いページを登録
            {
                // １行目だったら古いページはない
                if (page.GetSprite() == nullptr)
                {
                    // Do nothing
                }
                else
                {
                    page.SetTextList(textList);
                    pageList.push_back(page);

                    page.SetSprite(nullptr);
                    textList.clear();
                }
            }
            pageNum = pageNumTemp;
            std::string imagePath = line.at(1);

            ISprite* sprite = sprImage->Create();
            sprite->Load(imagePath);
            page.SetSprite(sprite);

            std::vector<std::string> texts = split(line.at(2), '\n');

            for (size_t j = 0; j < texts.size(); ++j)
            {
                texts.at(j).erase(std::remove(texts.at(j).begin(), texts.at(j).end(), '"'),
                                  texts.at(j).end());
            }
            textList.push_back(texts);
        }
        else
        {
            std::vector<std::string> texts = split(line.at(2), '\n');
            for (size_t j = 0; j < texts.size(); ++j)
            {
                texts.at(j).erase(std::remove(texts.at(j).begin(), texts.at(j).end(), '"'),
                                  texts.at(j).end());
            }
            textList.push_back(texts);
        }
    }
    page.SetTextList(textList);
    pageList.push_back(page);

    m_pageList = pageList;

    m_isFadeIn = true;
}

void StoryTelling::Next()
{
    if (m_waitNextCount < WAIT_NEXT_FRAME)
    {
        return;
    }
    int textIndex = m_pageList.at(m_pageIndex).GetTextIndex();
    int textIndexMax = m_pageList.at(m_pageIndex).GetTextList().size();
    if (textIndex < textIndexMax - 1)
    {
        textIndex++;
    }
    else
    {
        m_FadeOutCount = 0;
        m_isFadeOut = true;
    }
    m_pageList.at(m_pageIndex).SetTextIndex(textIndex);
    m_SE->PlayMove();
    m_waitNextCount = 0;
}

bool StoryTelling::Update()
{
    bool isFinish = false;
    if (m_isFadeIn)
    {
        if (m_FadeInCount < FADE_FRAME_MAX)
        {
            m_FadeInCount++;
        }
        else
        {
            m_isFadeIn = false;
            m_FadeInCount = 0;
        }
    }
    else if (m_isFadeOut)
    {
        if (m_FadeOutCount < FADE_FRAME_MAX)
        {
            m_FadeOutCount++;
        }
        else
        {
            m_isFadeOut = false;
            m_FadeOutCount = 0;
            m_isFadeIn = true;
            m_FadeInCount = 0;
            if (m_pageIndex <= (int)m_pageList.size() - 2)
            {
                m_pageIndex++;
                m_pageList.at(m_pageIndex).SetTextIndex(0);
            }
            else
            {
                isFinish = true;
            }
        }
    }
    else
    {
        m_waitNextCount++;
    }
    return isFinish;
}

void StoryTelling::Render()
{
    m_pageList.at(m_pageIndex).GetSprite()->DrawImage(0, 0);
    m_sprTextBack->DrawImage(0, 0);
    std::vector<std::vector<std::string>> vss = m_pageList.at(m_pageIndex).GetTextList();
    int textIndex = m_pageList.at(m_pageIndex).GetTextIndex();
    if (vss.at(textIndex).size() >= 1)
    {
        m_font->DrawText_(vss.at(textIndex).at(0), 100, 730);
    }

    if (vss.at(textIndex).size() >= 2)
    {
        m_font->DrawText_(vss.at(textIndex).at(1), 100, 780);
    }

    if (vss.at(textIndex).size() >= 3)
    {
        m_font->DrawText_(vss.at(textIndex).at(2), 100, 830);
    }

    if (m_isFadeIn)
    {
        m_sprFade->DrawImage(0, 0, 255 - m_FadeInCount*255/FADE_FRAME_MAX);
    }
    if (m_isFadeOut)
    {
        m_sprFade->DrawImage(0, 0, m_FadeOutCount*255/FADE_FRAME_MAX);
    }
}

void StoryTelling::Finalize()
{
    delete m_sprTextBack;
    m_sprTextBack = nullptr;
    delete m_sprFade;
    m_sprFade = nullptr;
    delete m_font;
    m_font = nullptr;
    delete m_SE;
    m_SE = nullptr;
    for (std::size_t i = 0; i < m_pageList.size(); ++i)
    {
        delete m_pageList.at(i).GetSprite();
        m_pageList.at(i).SetSprite(nullptr);
    }
    delete m_sprImage;
    m_sprImage = nullptr;
}

ISprite* Page::GetSprite() const
{
    return m_sprite;
}

void Page::SetSprite(ISprite* sprite)
{
    m_sprite = sprite;
}

std::vector<std::vector<std::string>> Page::GetTextList() const
{
    return m_textList;
}

void Page::SetTextList(const std::vector<std::vector<std::string>>& textList)
{
    m_textList = textList;
}

int Page::GetTextIndex() const
{
    return m_textIndex;
}

void Page::SetTextIndex(const int index)
{
    m_textIndex = index;
}

