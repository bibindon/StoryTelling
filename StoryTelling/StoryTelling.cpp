// TODO フェードインアウト
// TODO 連打できないようにする
#include "StoryTelling.h"
#include <sstream>

std::vector<std::string> split(const std::string& s, char delim)
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

void StoryTelling::Init(IFont* font, ISoundEffect* SE, ISprite* sprTextBack, const std::vector<Page>& pageList)
{
    m_font = font;
    m_SE = SE;
    m_sprTextBack = sprTextBack;
    m_pageList = pageList;
}

bool StoryTelling::Next()
{
    int textIndex = m_pageList.at(m_pageIndex).GetTextIndex();
    int textIndexMax = m_pageList.at(m_pageIndex).GetTextList().size();
    if (textIndex < textIndexMax - 1)
    {
        textIndex++;
    }
    else
    {
        if (m_pageIndex < (int)m_pageList.size() - 1)
        {
            textIndex = 0;
            m_pageIndex++;
        }
        else
        {
            return true;
        }
    }
    m_pageList.at(m_pageIndex).SetTextIndex(textIndex);
    m_SE->PlayMove();

    return false;
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
}

void StoryTelling::Finalize()
{
    delete m_sprTextBack;
    m_sprTextBack = nullptr;
    delete m_font;
    m_font = nullptr;
    delete m_SE;
    m_SE = nullptr;
    for (std::size_t i = 0; i < m_pageList.size(); ++i)
    {
        delete m_pageList.at(i).GetSprite();
        m_pageList.at(i).SetSprite(nullptr);
    }
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

