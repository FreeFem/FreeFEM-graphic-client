#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include "Logger.h"
#include "Import.h"

namespace ffGraph {
namespace JSON {

#define ISOLINE_NBR 20

static glm::vec2 IsoValue(glm::vec2 T[3], glm::vec2 BarycentricPoint)
{
    glm::mat2 mat(glm::vec2(T[1].x - T[0].x, T[1].y - T[0].y), glm::vec2(T[2].x - T[0].x, T[2].y - T[0].y));

    return mat * BarycentricPoint + T[0];
}

static float norme2(glm::vec2 a, glm::vec2 b)
{
    return a.x * b.x + a.y * b.y;
}

Geometry ConstructIsoLines(std::vector<float>& Vertices, std::vector<uint32_t>& Indices, std::vector<float> Values, std::vector<float>& RefTriangle, std::vector<float>& KSub, float min, float max) {
    size_t nsubT = KSub.size() / 3;
    size_t nsubV = RefTriangle.size() / 2;
    size_t nK = Values.size() / (Indices.size() / 3);
    std::vector<glm::vec2> Pn(nsubV);
    size_t count = 0;
    int o = 0;

    Geometry n;
    n.Data = ffNewArray(ISOLINE_NBR * (Indices.size() / 3) * nsubT * 2, sizeof(Vertex));
    std::cout << ISOLINE_NBR * (Indices.size() / 3) * 2 << "\n";

    Vertex *ptr = (Vertex *)n.Data.Data;
    std::vector<float> Viso(ISOLINE_NBR);

    for (size_t i = 0; i < ISOLINE_NBR; ++i) {
        Viso[i] = ((max - min) / (float)ISOLINE_NBR) * (float)i + min;
        std::cout << "Viso[" << i << "] = " << Viso[i] << "\n";

    }

    for (size_t i = 0; i < (Indices.size() / 3); ++i) {
        std::cout << "Triangle N" << i << "\n";
        glm::vec2 triangle[3] = {
            glm::vec2(Vertices[Indices[i * 3] * 3 + 0], Vertices[Indices[i * 3] * 3 + 1]),
            glm::vec2(Vertices[Indices[i * 3 + 1] * 3 + 0], Vertices[Indices[i * 3 + 1] * 3 + 1]),
            glm::vec2(Vertices[Indices[i * 3 + 2] * 3 + 0], Vertices[Indices[i * 3 + 2] * 3 + 1])
        };
        for (size_t j = 0; j < nsubV; ++j) {
            Pn[j] = IsoValue(triangle, glm::vec2(RefTriangle[j * 2], RefTriangle[j * 2 + 1]));
        }
        for (size_t sk = 0; sk < nsubT; ++sk) {
            std::cout << "\tSubTriangle N" << sk << " / " << nsubT << "\n";
            int i0 = KSub[sk * 3 + 0];
            int i1 = KSub[sk * 3 + 1];
            int i2 = KSub[sk * 3 + 2];

            glm::vec3 ff = glm::vec3(Values[o + i0], Values[o + i1], Values[o + i2]);
            glm::vec2 Pt[3] = {
                Pn[i0], Pn[i1], Pn[i2]
            };

            glm::vec2 PQ[5];
            float eps2 = std::min(std::min(norme2(Pt[0], Pt[1]), norme2(Pt[0], Pt[2])), norme2(Pt[1], Pt[2])) * 1e-8;
            for (size_t l = 0; l < ISOLINE_NBR; ++l) {
                float xf = Viso[l];
                int im = 0;
                std::cout << "Iteration " << l << "\n";
                for (size_t m = 0; m < 3; ++m) {
                    int a = (m + 1) % 3;
                    float fi = ff[m];
                    float fj = ff[a];

                    if ((fi <= xf && fj >= xf) || (fi >= xf && fj <= xf)) {
                        if (std::abs(fi - fj) <= 0.1e-10) {
                            std::cout << "Placing point from the loop.\n";
                            ptr[count].x = Pt[m].x;
                            ptr[count].y = Pt[m].y;
                            ptr[count].z = 0.f;
                            ptr[count].r = 1.f;
                            ptr[count].g = 0.f;
                            ptr[count].b = (xf - min) / (max - min);
                            ptr[count].a = 1.f;
                            count += 1;

                            ptr[count].x = Pt[a].x;
                            ptr[count].y = Pt[a].y;
                            ptr[count].z = 0.f;
                            ptr[count].r = 1.f;
                            ptr[count].g = 0.f;
                            ptr[count].b = (xf - min) / (max - min);
                            ptr[count].a = 1.f;
                            count += 1;
                        } else {
                            float xlam = (fi - xf) / (fi - fj);
                            if (im != 0 && PQ[im - 1].x == (Pt[m] * (1.f - xlam) + Pt[a] * xlam).x && PQ[im - 1].y == (Pt[m] * (1.f - xlam) + Pt[a] * xlam).y)
                                continue;
                            PQ[im] = Pt[m] * (1.f - xlam) + Pt[a] * xlam;
                            std::cout << PQ[im].x << ", " << PQ[im].y << "\n";
                            im += 1;
                        }
                    }
                }
                if (im >= 2) {
                    if (norme2(PQ[0], PQ[1]) > eps2) {
                            std::cout << "\tPlacing point from outside the loop.\n";
                            std::cout << "\t\tPQ[0] : { " << PQ[0].x << ", " << PQ[0].y << " }\n";
                            std::cout << "\t\tPQ[1] : { " << PQ[1].x << ", " << PQ[1].y << " }\n";
                            ptr[count].x = PQ[0].x;
                            ptr[count].y = PQ[0].y;
                            ptr[count].z = 0.f;
                            ptr[count].r = 1.f;
                            ptr[count].g = 0.f;
                            ptr[count].b = (xf - min) / (max - min);
                            ptr[count].a = 1.f;
                            count += 1;

                            ptr[count].x = PQ[1].x;
                            ptr[count].y = PQ[1].y;
                            ptr[count].z = 0.f;
                            ptr[count].r = 1.f;
                            ptr[count].g = 0.f;
                            ptr[count].b = (xf - min) / (max - min);
                            ptr[count].a = 1.f;
                            count += 1;
                    }
                }
            }
        }
        o += nK;
    }
    std::cout << "Count : " << count << "\n";
    return n;
}

}
}