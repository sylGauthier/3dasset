#ifndef TDASSET_H
#define TDASSET_H

#include <3dmr/material/pbr.h>
#include <3dmr/material/phong.h>
#include <3dmr/material/solid.h>
#include <3dmr/scene/node.h>

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
void asset_mat_solid_texid(struct MaterialConfig* config, GLuint tex);

void asset_mat_phong_color(struct MaterialConfig* config,
                           float r, float g, float b, float shiny);
void asset_mat_phong_texid(struct MaterialConfig* config,
                           GLuint tex, float shiny);

void asset_mat_pbr_color(struct MaterialConfig* config,
                         float r, float g, float b, float metal, float rough);
void asset_mat_pbr_texid(struct MaterialConfig* config,
                         GLuint tex, float metal, float rough);

struct Node* asset_box(struct MaterialConfig* config,
                       float w, float h, float d);
struct Node* asset_quad(struct MaterialConfig* config, float w, float h);
struct Node* asset_icosphere(struct MaterialConfig* config,
                             float radius, unsigned int numSplits);

#endif
