#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <3dmr/mesh/box.h>
#include <3dmr/mesh/quad.h>
#include <3dmr/mesh/icosphere.h>
#include <3dmr/scene/node.h>
#include <3dmr/render/texture.h>

#include <3dasset.h>

void asset_mat_solid_color(struct MaterialConfig* config,
                           float r, float g, float b) {
    struct SolidMaterialParams* p;

    config->type = MAT_SOLID;
    p = &config->params.solid;

    solid_material_params_init(p);
    material_param_set_vec3_elems(&p->color, r, g, b);
}

void asset_mat_solid_texid(struct MaterialConfig* config, GLuint tex) {
    struct SolidMaterialParams* p;

    config->type = MAT_SOLID;
    p = &config->params.solid;

    solid_material_params_init(p);
    material_param_set_vec3_texture(&p->color, tex);
}

void asset_mat_overlay_color(struct MaterialConfig* config,
                             float r, float g, float b) {
    struct SolidMaterialParams* p;

    config->type = MAT_SOLID_OVERLAY;
    p = &config->params.solid;

    solid_material_params_init(p);
    material_param_set_vec3_elems(&p->color, r, g, b);
}

void asset_mat_overlay_texid(struct MaterialConfig* config, GLuint tex) {
    struct SolidMaterialParams* p;

    config->type = MAT_SOLID_OVERLAY;
    p = &config->params.solid;

    solid_material_params_init(p);
    material_param_set_vec3_texture(&p->color, tex);
}

int asset_mat_overlay_texpath(struct MaterialConfig* config, const char* tex) {
    GLuint texid;
    const char* ext;

    if (!(ext = strrchr(tex, '.'))) {
        fprintf(stderr, "Error: asset_mat_overlay_texid: missing extension\n");
        return 0;
    }
    if (!strcmp(ext, ".png")) {
        if (!(texid = texture_load_from_png(tex))) {
            fprintf(stderr, "Error: asset_mat_overlay_texpath: "
                            "texture_load_from_png failed\n");
            return 0;
        }
    } else if (!strcmp(ext, ".jpg") || !strcmp(ext, ".jpeg")) {
        if (!(texid = texture_load_from_jpeg(tex))) {
            fprintf(stderr, "Error: asset_mat_overlay_texpath: "
                            "texture_load_from_jpeg failed\n");
            return 0;
        }
    } else {
        fprintf(stderr, "Error: asset_mat_overlay_texpath: "
                        "unkown file extension: %s\n", ext);
        return 0;
    }
    asset_mat_overlay_texid(config, texid);
    return 1;
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

void asset_mat_phong_texid(struct MaterialConfig* config,
                           GLuint tex, float shiny) {
    struct PhongMaterialParams* p;

    config->type = MAT_PHONG;
    p = &config->params.phong;

    phong_material_params_init(p);
    material_param_set_vec3_texture(&p->ambient, tex);
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

void asset_mat_pbr_texid(struct MaterialConfig* config,
                         GLuint tex, float metal, float rough) {
    struct PBRMaterialParams* p;

    config->type = MAT_PBR;
    p = &config->params.pbr;

    pbr_material_params_init(p);
    material_param_set_vec3_texture(&p->albedo, tex);
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
        case MAT_SOLID_OVERLAY:
            if (!(new = solid_overlay_material_new(flags,
                                                   &conf->params.solid))) {
                fprintf(stderr, "Error: solid_overlay_material_new failed\n");
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

struct Node* asset_quad(struct MaterialConfig* config, float w, float h) {
    struct Mesh m;
    struct Node* n = NULL;

    if (!make_quad(&m, w, h)) {
        fprintf(stderr, "Error: asset_quad: make_quad failed\n");
    } else {
        n = make_asset(&m, config);
    }
    mesh_free(&m);
    return n;
}

struct Node* asset_icosphere(struct MaterialConfig* config,
                             float r, unsigned int numSplits) {
    struct Mesh m;
    struct Node* n = NULL;

    if (!make_icosphere(&m, r, numSplits)) {
        fprintf(stderr, "Error: asset_icosphere: make_icosphere failed\n");
    } else {
        n = make_asset(&m, config);
    }
    mesh_free(&m);
    return n;
}

void asset_free(struct Node* node) {
    if (!node) return;
    if (node->type == NODE_GEOMETRY) {
        struct Geometry* g = node->data.geometry;
        vertex_array_free(g->vertexArray);
        free(g->material);
        free(g);
    }
    free(node);
}
