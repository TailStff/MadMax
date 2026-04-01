#include "mmLinear.h"

mmLinear::mmLinear(ExecutionEnv *_executionEnv)
{
    executionEnv = _executionEnv;
}

mmLinear::~mmLinear()
{
}

float mmLinear::Evaluate(const PointXY *table, size_t size, float x, bool clamp)
{
    // Cas particulier : table vide
    if (size == 0)
        return 0.0f;

    // Cas particulier : table à un point
    if (size == 1)
        return table[0].y;

    // x en dessous de la table
    if (x <= table[0].x)
    {
        // Clamp à la valeur minimale de la table
        if (clamp)
            return table[0].y;

        // Extrapolation avec les 2 premiers points
        const float x0 = table[0].x;
        const float y0 = table[0].y;
        const float x1 = table[1].x;
        const float y1 = table[1].y;

        if (x1 == x0)
            return y0;

        return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
    }

    // x au dessus de la table
    if (x >= table[size - 1].x)
    {
        // Clamp à la valeur maximale de la table
        if (clamp)
            return table[size - 1].y;

        // Extrapolation avec les 2 derniers points
        const float x0 = table[size - 2].x;
        const float y0 = table[size - 2].y;
        const float x1 = table[size - 1].x;
        const float y1 = table[size - 1].y;

        if (x1 == x0)
            return y1;

        return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
    }

    // x dans la table → interpolation classique
    for (size_t i = 0; i < size - 1; i++)
    {
        if (x >= table[i].x && x <= table[i + 1].x)
        {
            const float x0 = table[i].x;
            const float y0 = table[i].y;
            const float x1 = table[i + 1].x;
            const float y1 = table[i + 1].y;

            if (x1 == x0)
                return y0;

            return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
        }
    }

    // fallback ultra safe
    return table[size - 1].y;
}
