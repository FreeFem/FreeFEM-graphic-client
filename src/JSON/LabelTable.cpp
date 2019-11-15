#include <cmath>
#include <iostream>
#include "LabelTable.h"

namespace ffGraph {

Color NewColor(float H, float V, float Opacity) {
    float S = 1.f;

    float C = V * S;
    float X = C * (1 - fabs(fmod(H / 60.f, 2.f) - 1));
    float m = V - C;

    float R, G, B = 0.f;
    if (H >= 0 && H < 60) {
        R = C;
        G = X;
        B = 0.f;
    } else if (H >= 60 && H < 120) {
        R = X;
        G = C;
        B = 0.f;
    } else if (H >= 120 && H < 180) {
        R = 0.f;
        G = C;
        B = X;
    } else if (H >= 180 && H < 240) {
        R = 0.f;
        G = X;
        B = C;
    } else if (H >= 240 && H < 300) {
        R = X;
        G = 0.f;
        B = C;
    } else if (H >= 300 && H < 360) {
        R = C;
        G = 0.f;
        B = X;
    }
    return {fabsf32((R + m)), fabsf32((G + m)), fabsf32((B + m)), Opacity};
}

void AddLabelToTable(LabelTable& Table, Label l) {
    for (size_t i = 0; i < Table.UniqueLabels.size( ); ++i) {
        if (Table.UniqueLabels[i] == l) return;
    }
    Table.UniqueLabels.push_back(l);
}

void GenerateColorFromLabels(LabelTable& Table) {
    Table.UniqueColors.resize(Table.UniqueLabels.size( ));
    float Delta = 359.f / (float)Table.UniqueLabels.size( );
    float V = 1.f;

    float count = 0.f;
    for (size_t i = 0; i < Table.UniqueLabels.size( ); ++i) {
        if (i % 100 == 0 && i != 0) V -= 0.25f;
        Table.UniqueColors[i] = NewColor(Delta * count, 1.f, 1.f);
        count += 1.f;
    }
}

void ClearLabelTable(LabelTable& Table) {
    Table.UniqueColors.clear( );
    Table.UniqueLabels.clear( );
}

Color GetColor(const LabelTable& Table, const Label l) {
    for (size_t i = 0; i < Table.UniqueLabels.size( ); ++i) {
        if (Table.UniqueLabels[i] == l) return Table.UniqueColors[i];
    }
    return (Color){0.f, 0.f, 0.f, 1.f};
}

}    // namespace ffGraph