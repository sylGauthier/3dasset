#include <stdio.h>
#include <stdlib.h>

#include <3dmr/mesh/box.h>
#include <3dmr/scene/node.h>

#include <3dasset.h>

void asset_mat_solid_color(struct MaterialConfig* config,
                           float r, float g, float b) {
    struct SolidMaterialParams* p;

    config->type = MAT_SOLID;
    p = &config->params.solid;

    solid_material_params_init(p);
    material_param_set_vec3_elems(&p->color, r, g, b);
}

void asset_mat_phong_color(struct MaterialConfig* config,
                           float r, float g, float b, float shiny) {
    struct PhongMaterialParams* p;

    config->type = MAT_PHONG;
    p = &config->params.phong;

    phong_material_params_init(p);
    material_param_set_vec3_elems(&p->ambient, r, g, b);
    material_param_set_vec3_elems(&p->diffuse, r, g, b);
    material_param_set_vec3_elems(&p->specular, r, g, b);
    material_param_set_float_constant(&p->shininess, shiny);
}

void asset_mat_pbr_color(struct MaterialConfig* config,
                         float r, float g, float b, float metal, float rough) {
    struct PBRMaterialParams* p;

    config->type = MAT_PBR;
    p = &config->params.pbr;

    pbr_material_params_init(p);
    material_param_set_vec3_elems(&p->albedo, r, g, b);
    material_param_set_float_constant(&p->metalness, metal);
    material_param_set_float_constant(&p->roughness, rough);
}

static struct Material* make_material(struct MaterialConfig* conf,
                                      enum MeshFlags flags) {
    struct Material* new = NULL;

    switch (conf->type) {
        case MAT_SOLID:
            if (!(new = solid_material_new(flags, &conf->params.solid))) {
                fprintf(stderr, "Error: solid_material_new failed\n");
            }
            break;
        case MAT_PHONG:
            if (!(new = phong_material_new(flags, &conf->params.phong))) {
                fprintf(stderr, "Error: phong_material_new failed\n");
            }
            break;
        case MAT_PBR:
            if (!(new = pbr_material_new(flags, &conf->params.pbr))) {
                fprintf(stderr, "Error: pbr_material_new failed\n");
            }
            break;
    }
    return new;
}

static struct Node* make_asset(struct Mesh* mesh, struct MaterialConfig* conf) {
    struct Geometry* geom = NULL;
    struct Node* node = NULL;

    if (       !(geom = calloc(1, sizeof(*geom)))
            || !(node = malloc(sizeof(*node)))) {
        fprintf(stderr, "Error: asset_box: malloc failed\n");
    } else if (!(geom->vertexArray = vertex_array_new(mesh))) {
        fprintf(stderr, "Error: asset_box: vertex_array_new failed\n");
    } else if (!(geom->material = make_material(conf, mesh->flags))) {
        fprintf(stderr, "Error: asset_box: make_material failed\n");
    } else {
        node_init(node);
        node_set_geometry(node, geom);
        return node;
    }
    free(node);
    if (geom) {
        free(geom->material);
        vertex_array_free(geom->vertexArray);
        free(geom->vertexArray);
    }
    return NULL;
}

struct Node* asset_box(struct MaterialConfig* config,
                       float w, float h, float d) {
    struct Mesh m;
    struct Node* n = NULL;

    if (!make_box(&m, w, h, d)) {
        fprintf(stderr, "Error: asset_box: make_box failed\n");
    } else {
        n = make_asset(&m, config);
    }
    mesh_free(&m);
    return n;
}
