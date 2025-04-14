#include "Graph.h"
#include <wx/wx.h>

Graph::Graph(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1400, 400)) {
    Bind(wxEVT_PAINT, &Graph::OnPaint, this);
}

void Graph::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);

    //Wyj¹tek
    try {
        dc.Clear();

        int width, height;
        GetClientSize(&width, &height); //wysokosc i szerokosc wykresu


        //Marginesy osi XY
        int margin = 50;
        int axisX0 = margin;
        int axisY0 = height - margin;
        int axisX1 = width - margin;
        int axisY1 = margin;


        //Rysowanie osi XY
        dc.DrawLine(axisX0, axisY0, axisX1, axisY0);
        dc.DrawLine(axisX0, axisY0, axisX0, axisY1);

        //Sprawdzanie czy jest to uruchomienie programu, pierwszy raz w celu wykrycia b³êdów
        if (!m_hasData || m_xData.empty() || m_yData.empty()) {
            return;
        }

        //Szukanie najmniejszej i najwiekszej wartosci w celu rysowania osi (zakresów)
        auto [minY, maxY] = std::minmax_element(m_yData.begin(), m_yData.end());
        double yMin = *minY;
        double yMax = *maxY;
        double xMax = m_xData.size() - 1;


        if (yMin == yMax) yMax += 1;

        //Kolory punktow i lini na wykresie
        dc.SetPen(wxPen(*wxBLUE, 2));
        dc.SetBrush(wxBrush(*wxRED));

        //Rysowanie linii na wykresie (skaluje dane do XY i rysuje koleczka w punktach
        wxPoint prev;
        for (size_t i = 0; i < m_xData.size(); ++i) {
            double xVal = m_xData[i];
            double yVal = m_yData[i];

            int x = axisX0 + (int)((xVal / xMax) * (axisX1 - axisX0));
            int y = axisY0 - (int)(((yVal - yMin) / (yMax - yMin)) * (axisY0 - axisY1));

            dc.DrawCircle(wxPoint(x, y), 3);

            //£¹czy obecny punkt z poprzednim
            if (i > 0)
                dc.DrawLine(prev.x, prev.y, x, y);

            //Zapamiêtuje obecny punkt, zeby moc z kolejnym go polaczyc
            prev = wxPoint(x, y);
        }

        //rysownie wartosci na osi X
        dc.SetFont(wxFontInfo(8));
        for (size_t i = 0; i < m_xData.size(); i += 2) {
            int x = axisX0 + (int)((m_xData[i] / xMax) * (axisX1 - axisX0));
            dc.DrawText(wxString::Format("%d", (int)m_xData[i]), x - 5, axisY0 + 10);
        }

        for (int j = 0; j <= 5; ++j) {
            double yVal = yMin + j * (yMax - yMin) / 5.0;
            int y = axisY0 - (int)(((yVal - yMin) / (yMax - yMin)) * (axisY0 - axisY1));
            dc.DrawText(wxString::Format("%.1f", yVal), 5, y - 5);
            dc.DrawLine(axisX0 - 5, y, axisX0 + 5, y);
        }
    } //B³êdy z wykresem
    catch (const std::exception& e) {
        wxLogError("B³¹d podczas rysowania wykresu: %s", e.what());
    }
    catch (...) {
        wxLogError("Nieznany b³¹d podczas rysowania wykresu.");
    }
}

// Funkcja do ustawienia danych wykresu
void Graph::SetData(const std::vector<double>& x, const std::vector<double>& y) {
    m_xData = x;
    m_yData = y;
    m_hasData = true;
    Refresh();
}