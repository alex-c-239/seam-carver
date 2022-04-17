#include "SeamCarver.h"

#include <cmath>
#include <limits>

SeamCarver::SeamCarver(Image image)
    : m_image(std::move(image))
{
}

const Image & SeamCarver::GetImage() const
{
    return m_image;
}

size_t SeamCarver::GetImageWidth() const
{
    return m_image.width();
}

size_t SeamCarver::GetImageHeight() const
{
    return m_image.height();
}

Image::Pixel SeamCarver::getPixel(std::size_t columnId, std::size_t rowId) const
{
    return m_image.GetPixel(columnId, rowId);
}

std::size_t SeamCarver::nextColumn(std::size_t columnId) const
{
    return (columnId + 1 >= m_image.width()) ? (columnId + 1 - m_image.width()) : (columnId + 1);
}

std::size_t SeamCarver::prevColumn(std::size_t columnId) const
{
    return (columnId == 0) ? (m_image.width() - 1) : (columnId - 1);
}

std::size_t SeamCarver::nextRow(std::size_t rowId) const
{
    return (rowId + 1 >= m_image.height()) ? (rowId + 1 - m_image.height()) : (rowId + 1);
}

std::size_t SeamCarver::prevRow(std::size_t rowId) const
{
    return (rowId == 0) ? (m_image.height() - 1) : (rowId - 1);
}

double SeamCarver::GetPixelEnergy(size_t columnId, size_t rowId) const
{
    int delta_x, delta_y, delta_red, delta_green, delta_blue;

    delta_red = getPixel(nextColumn(columnId), rowId).m_red - getPixel(prevColumn(columnId), rowId).m_red;
    delta_green = getPixel(nextColumn(columnId), rowId).m_green - getPixel(prevColumn(columnId), rowId).m_green;
    delta_blue = getPixel(nextColumn(columnId), rowId).m_blue - getPixel(prevColumn(columnId), rowId).m_blue;

    delta_x = delta_red * delta_red + delta_green * delta_green + delta_blue * delta_blue;

    delta_red = getPixel(columnId, nextRow(rowId)).m_red - getPixel(columnId, prevRow(rowId)).m_red;
    delta_green = getPixel(columnId, nextRow(rowId)).m_green - getPixel(columnId, prevRow(rowId)).m_green;
    delta_blue = getPixel(columnId, nextRow(rowId)).m_blue - getPixel(columnId, prevRow(rowId)).m_blue;

    delta_y = delta_red * delta_red + delta_green * delta_green + delta_blue * delta_blue;

    return sqrt(delta_x + delta_y);
}

SeamCarver::Seam SeamCarver::find_seam(bool is_vertical) const
{
    if (m_image.empty()) {
        return {};
    }

    std::vector<std::vector<Node>> map(
            (is_vertical) ? m_image.height() : m_image.width(),
            std::vector<Node>((is_vertical) ? m_image.width() : m_image.height()));

    for (std::size_t x = 0; x < map.size(); ++x) {
        for (std::size_t y = 0; y < map[x].size(); ++y) {
            map[x][y].energy = (is_vertical) ? GetPixelEnergy(y, x) : GetPixelEnergy(x, y);
            map[x][y].previous = y;
            if (x > 0) {
                double min_energy = map[x - 1][y].energy;
                if (y > 0 && map[x - 1][y - 1].energy < min_energy) {
                    min_energy = map[x - 1][y - 1].energy;
                    map[x][y].previous = y - 1;
                }
                if (y + 1 < map[x - 1].size() && map[x - 1][y + 1].energy < min_energy) {
                    min_energy = map[x - 1][y + 1].energy;
                    map[x][y].previous = y + 1;
                }
                map[x][y].energy += min_energy;
            }
        }
    }

    std::size_t from = 0;
    double min_energy = map[map.size() - 1][0].energy;
    for (std::size_t i = 1; i < map[map.size() - 1].size(); ++i) {
        if (map[map.size() - 1][i].energy < min_energy) {
            min_energy = map[map.size() - 1][i].energy;
            from = i;
        }
    }

    Seam result(map.size());
    for (std::size_t i = 0; i < map.size(); ++i) {
        result[result.size() - 1 - i] = from;
        from = map[map.size() - 1 - i][from].previous;
    }
    return result;
}

SeamCarver::Seam SeamCarver::FindHorizontalSeam() const
{
    return find_seam(false);
}

SeamCarver::Seam SeamCarver::FindVerticalSeam() const
{
    return find_seam(true);
}

void SeamCarver::RemoveHorizontalSeam(const Seam & seam)
{
    for (std::size_t x = 0; x < m_image.width(); ++x) {
        m_image.m_table[x].erase(m_image.m_table[x].begin() + seam[x]);
    }
}

void SeamCarver::RemoveVerticalSeam(const Seam & seam)
{
    for (std::size_t y = 0; y < m_image.height(); ++y) {
        for (std::size_t x = seam[y]; x < m_image.width() - 1; ++x) {
            m_image.m_table[x][y] = m_image.m_table[x + 1][y];
        }
    }
    m_image.m_table.resize(m_image.width() - 1);
}
