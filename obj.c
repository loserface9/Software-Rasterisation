#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "obj.h"


struct import_obj_Arena {
    FILE *fptr;
    bool free_fptr;
};
struct fline_Output {
    char *contents;
    bool end_of_file;
};


const ObjFace ObjFace_NULL = {NULL, NULL, NULL, -1};
const Obj Obj_NULL = {NULL, NULL, NULL, NULL, -1, -1, -1, -1};


static struct fline_Output get_fline(FILE *stream);
static int string_to_Vec3 (Vec3 *restrict output, const char *restrict string);
static int string_to_ObjFace (ObjFace *restrict output, const char *restrict string);


Obj import_obj (const char *filepath) {
    struct import_obj_Arena import_obj_arena = {NULL, false};
    const int initial_num_v = 100, initial_num_vt = 100, initial_num_vn = 100, initial_num_f = 100;
    int v_buffer_len = initial_num_v, vt_buffer_len = initial_num_v,
        vn_buffer_len = initial_num_v, f_buffer_len = initial_num_f;
    Obj object = {
        malloc(initial_num_v * sizeof(Vec3)),
        malloc(initial_num_vt * sizeof(Vec3)),
        malloc(initial_num_vn * sizeof(Vec3)),
        malloc(initial_num_f * sizeof(ObjFace)),
        0, 0, 0, 0
    };

    FILE *fptr = fopen(filepath, "r");
    if (fptr == NULL) {
        printf("file obj.c, function import_obj():\n\tFailed to open file.\n");
        goto error;
    }
    import_obj_arena.fptr = fptr;
    import_obj_arena.free_fptr = true;

    struct fline_Output curr_fline_output = get_fline(fptr);
    while (!curr_fline_output.end_of_file) {
        const char *curr_line = curr_fline_output.contents;
        const int curr_line_len = strlen(curr_line);

        // instruction is normally a string like "v" or "vn" or "f"
        const int instruction_str_len = curr_line_len;
        char instruction[instruction_str_len];

        if (curr_line_len < 3) goto iterate_lines_cleanup;

        for (int i = 0; i < instruction_str_len; i++) instruction[i] = '\0';

        int instruction_str_idx = 0;
        while ((curr_line[instruction_str_idx] != ' ') & (curr_line[instruction_str_idx] != '\0')) {
            if (instruction_str_idx == (instruction_str_len-1)) {
                printf("file obj.c, function import_obj():\n\tOverflow in instruction buffer.\n");
                goto error;
            }

            instruction[instruction_str_idx] = curr_line[instruction_str_idx];
            instruction_str_idx++;
        }

        // curr_line now points to the start of the numeric data
        curr_line += instruction_str_idx + 1;

        if ((instruction[0] == 'v') & (instruction[1] == '\0')) {
            // Vertex
            // Check whether to increase the size of the buffer
            if (object.num_v == v_buffer_len) {
                v_buffer_len += initial_num_v;
                object.vertices = realloc(object.vertices, v_buffer_len * sizeof(Vec3));
            }

            Vec3 *vertex_ptr = &object.vertices[object.num_v];
            string_to_Vec3(vertex_ptr, curr_line);
            object.num_v++;
        } else if ((instruction[0] == 'v') & (instruction[1] == 't') & (instruction[2] == '\0')) {
            // Vertex texture
            // Check whether to increase the size of the buffer
            if (object.num_vt == vt_buffer_len) {
                vt_buffer_len += initial_num_vt;
                object.vertex_textures = realloc(object.vertex_textures, vt_buffer_len * sizeof(Vec3));
            }

            Vec3 *vertex_texture_ptr = &object.vertex_textures[object.num_vt];
            string_to_Vec3(vertex_texture_ptr, curr_line);
            object.num_vt++;
        } else if ((instruction[0] == 'v') & (instruction[1] == 'n') & (instruction[2] == '\0')) {
            // Vertex normal
            // Check whether to increase the size of the buffer
            if (object.num_vn == vn_buffer_len) {
                vn_buffer_len += initial_num_vn;
                object.vertex_normals = realloc(object.vertex_normals, vn_buffer_len * sizeof(Vec3));
            }

            Vec3 *vertex_normal_ptr = &object.vertex_normals[object.num_vn];
            string_to_Vec3(vertex_normal_ptr, curr_line);
            object.num_vn++;
        } else if ((instruction[0] == 'f') & (instruction[1] == '\0')) {
            // Face
            // Check whether to increase the size of the buffer
            if (object.num_f == f_buffer_len) {
                f_buffer_len += initial_num_f;
                object.faces = realloc(object.faces, f_buffer_len * sizeof(ObjFace));
            }

            ObjFace *face_ptr = &object.faces[object.num_f];
            string_to_ObjFace(face_ptr, curr_line);
            object.num_f++;
        }

        iterate_lines_cleanup:
            free(curr_fline_output.contents);
            curr_fline_output = get_fline(fptr);
    }

    if (import_obj_arena.free_fptr) fclose(import_obj_arena.fptr);
    return object;

    error:
        if (import_obj_arena.free_fptr) fclose(import_obj_arena.fptr);
        Obj err_output = Obj_NULL;
        return err_output;
}


static struct fline_Output get_fline(FILE *stream) {
/* Take a file pointer as input
 * Output contents of file until next newline or end of file.
 * Does not include newline character in the output.
 * File pointer will be incremented to the beginning of the next line
 */

    const int initial_buffer_length = 100;

    char *contents = malloc(initial_buffer_length * sizeof(char));
    int buffer_length = initial_buffer_length;
    int curr_char = fgetc(stream);
    int char_idx = 0;
    while ((curr_char != '\n') & (curr_char != EOF)) {
        // Check whether to increase the size of the buffer
        if (char_idx == buffer_length) {
            buffer_length += initial_buffer_length;
            contents = realloc(contents, buffer_length * sizeof(char));
        }

        // Assign the char to the string
        contents[char_idx] = (char) curr_char;

        // Fetch next char
        curr_char = fgetc(stream);
        char_idx++;
    }

    const int contents_length = char_idx;
    contents = realloc(contents, (contents_length + 1) * sizeof(char));
    contents[contents_length] = '\0';

    const struct fline_Output output = {
        contents, curr_char == EOF
    };
    return output;
}


static int string_to_Vec3 (Vec3 *restrict output, const char *restrict string) {
// string should initially point to the first float, or whitespace before the first float

    const char *curr_float = string;

    for (int dim = 0; dim < 3; dim++) {
        while (curr_float[0] == ' ') curr_float++;
        bool is_negative = curr_float[0] == '-';
        curr_float += is_negative;

        // Position of DP and space
        const int dp_pos = strcspn(curr_float, "."), space_pos = strcspn(curr_float, " ");
        // Length of each part of the float
        const int before_dp_len = dp_pos,
                  after_dp_len = space_pos - dp_pos - 1;
        char before_dp[before_dp_len+1], after_dp[after_dp_len+1];

        // Fetch digits before DP
        for (int before_dp_idx = 0; before_dp_idx < before_dp_len; before_dp_idx++) {
            before_dp[before_dp_idx] = curr_float[before_dp_idx];
        }
        // Fetch digits after DP
        for (int after_dp_idx = 0; after_dp_idx < after_dp_len; after_dp_idx++) {
            after_dp[after_dp_idx] = curr_float[dp_pos + 1 + after_dp_idx];
        }


        (*output)[dim] = 0;
        // Add digits before DP
        for (int before_dp_idx = 0; before_dp_idx < before_dp_len; before_dp_idx++) {
            double power = (before_dp_len - 1) - before_dp_idx;
            double mul = pow(10, power);
            (*output)[dim] += (before_dp[before_dp_idx] - '0') * mul;
        }
        // Add digits after DP
        for (int after_dp_idx = 0; after_dp_idx < after_dp_len; after_dp_idx++) {
            double power = -after_dp_idx - 1;
            double mul = pow(10, power);
            (*output)[dim] += (after_dp[after_dp_idx] - '0') * mul;
        }

        if (is_negative) {
            (*output)[dim] = -(*output)[dim];
        }

        curr_float += space_pos + 1;
    }

    return 0;
}


static int string_to_ObjFace (ObjFace *restrict output, const char *restrict string) {
// string should initially point to the first number, or whitespace before the first number

    int slash_count = 0;
    for (int i = 0; string[i] != '\0'; i++) {
        slash_count += string[i] == '/';
    }
    const int num_vertices = slash_count / 2;
    output->vertices        = calloc(num_vertices, sizeof(int));
    output->vertex_textures = calloc(num_vertices, sizeof(int));
    output->vertex_normals  = calloc(num_vertices, sizeof(int));
    output->num_vertices    = num_vertices;

    const char *curr_vertex = string;
    for (int vertex_idx = 0; vertex_idx < num_vertices; vertex_idx++) {
        while (curr_vertex[0] == ' ') curr_vertex++;

        const int slash_1_pos = strcspn(curr_vertex,                   "/");
        const int slash_2_pos = strcspn(curr_vertex + slash_1_pos + 1, "/") + slash_1_pos + 1;
        const int space_pos   = strcspn(curr_vertex,                   " ");
        const int v_len  = slash_1_pos,
                  vt_len = slash_2_pos - slash_1_pos - 1,
                  vn_len = space_pos - slash_2_pos - 1;


        // Add digits for vertices
        for (int v_idx = 0; v_idx < v_len; v_idx++) {
            const char digit = curr_vertex[v_idx];
            const double power = (v_len - 1) - v_idx;
            const double mul = pow(10, power);
            output->vertices[vertex_idx] += (digit - '0') * mul;
        }
        // Add digits for vertex textures
        if (vt_len == 0) output->vertex_textures[vertex_idx] = -1;
        for (int vt_idx = 0; vt_idx < vt_len; vt_idx++) {
            const char digit = curr_vertex[slash_1_pos + 1 + vt_idx];
            const double power = (vt_len - 1) - vt_idx;
            const double mul = pow(10, power);
            output->vertex_textures[vertex_idx] += (digit - '0') * mul;
        }
        // Add digits for vertex normals
        if (vn_len == 0) output->vertex_normals[vertex_idx] = -1;
        for (int vn_idx = 0; vn_idx < vn_len; vn_idx++) {
            const char digit = curr_vertex[slash_2_pos + 1 + vn_idx];
            const double power = (vn_len - 1) - vn_idx;
            const double mul = pow(10, power);
            output->vertex_normals[vertex_idx] += (digit - '0') * mul;
        }

        // Remove 1 from each to give zero-indexing
        output->vertices[vertex_idx]        -= 1;
        output->vertex_textures[vertex_idx] -= 1;
        output->vertex_normals[vertex_idx]  -= 1;

        curr_vertex += space_pos + 1;
    }

    return 0;
}
