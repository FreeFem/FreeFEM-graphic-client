#ifndef LABEL_TABLE_H_
#define LABEL_TABLE_H_

#include <vector>

namespace ffGraph {

struct Color {
    float r, g, b, a;
};

typedef unsigned long int Label;

struct LabelTable {
    std::vector<Label> UniqueLabels;
    std::vector<Color> UniqueColors;
};

void AddLabelToTable(LabelTable& Table, Label l);
void GenerateColorFromLabels(LabelTable& Table);
void ClearLabelTable(LabelTable& Table);

Color NewColor(float H, float V, float Opacity);
Color GetColor(const LabelTable& Table, const Label l);

}    // namespace ffGraph

#endif    // LABEL_TABLE_H_