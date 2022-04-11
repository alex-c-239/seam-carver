#include "SeamCarver.h"

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
    return 0;
}

size_t SeamCarver::GetImageHeight() const
{
    return 0;
}

double SeamCarver::GetPixelEnergy(size_t, size_t) const
{
    return 0.0;
}

SeamCarver::Seam SeamCarver::FindHorizontalSeam() const
{
    return Seam();
}

SeamCarver::Seam SeamCarver::FindVerticalSeam() const
{
    return Seam();
}

void SeamCarver::RemoveHorizontalSeam(const Seam &)
{
}

void SeamCarver::RemoveVerticalSeam(const Seam &)
{
}
