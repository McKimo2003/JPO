#pragma once
#include <wx/wx.h>
#include <vector>

class Graph : public wxPanel
{

private:
    bool m_hasData = false;
    std::vector<double> m_xData;
    std::vector<double> m_yData;

public:
    Graph(wxWindow* parent);

    void SetData(const std::vector<double>& x, const std::vector<double>& y);
    void OnPaint(wxPaintEvent& event);

    const std::vector<double>& getY()const{
        return m_yData;
    }
    const std::vector<double>& getX()const{
        return m_xData;
    }
};

