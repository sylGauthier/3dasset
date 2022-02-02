#ifndef TDASSET_H
#define TDASSET_H

#include <3dmr/material/pbr.h>
#include <3dmr/material/phong.h>
#include <3dmr/material/solid.h>

struct MaterialConfig {
    enum MaterialType {
        MAT_SOLID,
        MAT_PHONG,
        MAT_PBR
    } type;

    union MaterialParam {
        struct SolidMaterialParams solid;
        struct PhongMaterialParams phong;
        struct PBRMaterialParams pbr;
    } params;
};

void asset_mat_solid_color(struct MaterialConfig* config,
                           float r, float g, float b);
void asset_mat_phong_color(struct MaterialConfig* config,
                           float r, float g, float b, float shiny);
void asset_mat_pbr_color(struct MaterialConfig* config,
                         float r, float g, float b, float metal, float rough);

struct Node* asset_box(struct MaterialConfig* config,
                       float w, float h, float d);

#endif
